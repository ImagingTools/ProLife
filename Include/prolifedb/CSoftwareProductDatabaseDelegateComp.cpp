#include <prolifedb/CSoftwareProductDatabaseDelegateComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/IIdParam.h>
#include <iprm/IEnableableParam.h>
#include <iprm/TParamsPtr.h>

//ImtCore includes
#include <imtlic/CProductInstanceInfo.h>
#include <imtauth/IUserInfo.h>

//ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CSoftwareProductDatabaseDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		return QString("SELECT * FROM \"%1\" WHERE \"IsActive\" = true AND \"%2\" = '%3'")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(*m_objectIdColumnAttrPtr))
				.arg(qPrintable(objectId)).toUtf8();
	}

	QByteArray beforeSelectionQuery;

	beforeSelectionQuery += R"(DROP TABLE IF EXISTS "UsersTemp";)";
	beforeSelectionQuery += R"(CREATE TEMP TABLE "UsersTemp"("UserId" varchar, "Groups" varchar);)";

	if (m_userCollectionCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids userCollectionIds = m_userCollectionCompPtr->GetElementIds();

		for (const imtbase::ICollectionInfo::Id& userCollectionId : userCollectionIds){
			idoc::MetaInfoPtr dataMetaInfo = m_userCollectionCompPtr->GetDataMetaInfo(userCollectionId);
			if (dataMetaInfo.IsValid()){
				QString groups = dataMetaInfo->GetMetaInfo(imtauth::IUserInfo::MIT_GROUPS).toString();
				QString userId = dataMetaInfo->GetMetaInfo(imtauth::IUserInfo::MIT_ID).toString();

				beforeSelectionQuery += QString(R"(INSERT INTO "UsersTemp" ("UserId", "Groups") VALUES('%1', '%2');)")
						.arg(userId)
						.arg(groups).toUtf8();
			}
		}
	}

	beforeSelectionQuery += R"(DROP TABLE IF EXISTS "LicensesTemp";)";
	beforeSelectionQuery += R"(DROP TABLE IF EXISTS "ProductsTemp";)";

	beforeSelectionQuery += R"(CREATE TEMP TABLE "LicensesTemp"("DocumentId" varchar, "LicenseId" varchar, "LicenseName" varchar);)";
	beforeSelectionQuery += R"(CREATE TEMP TABLE "ProductsTemp"("DocumentId" varchar, "ProductId" varchar, "ProductName" varchar);)";

	if (m_licenseCollectionCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids licenseCollectionIds = m_licenseCollectionCompPtr->GetElementIds();

		for (const imtbase::ICollectionInfo::Id& licenseCollectionId : licenseCollectionIds){
			idoc::MetaInfoPtr dataMetaInfo = m_licenseCollectionCompPtr->GetDataMetaInfo(licenseCollectionId);
			if (dataMetaInfo.IsValid()){
				QByteArray licenseId = dataMetaInfo->GetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_ID).toByteArray();
				QString licenseName = dataMetaInfo->GetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_NAME).toString();

				beforeSelectionQuery += QString(R"(INSERT INTO "LicensesTemp" ("DocumentId", "LicenseId", "LicenseName") VALUES('%1', '%2', '%3');)")
						.arg(qPrintable(licenseCollectionId))
						.arg(qPrintable(licenseId))
						.arg(licenseName)
						.toUtf8();
			}
		}
	}

	if (m_productCollectionCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids productCollectionIds = m_productCollectionCompPtr->GetElementIds();

		for (const imtbase::ICollectionInfo::Id& productCollectionId : productCollectionIds){
			idoc::MetaInfoPtr dataMetaInfo = m_productCollectionCompPtr->GetDataMetaInfo(productCollectionId);
			if (dataMetaInfo.IsValid()){
				QByteArray productId = dataMetaInfo->GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_ID).toByteArray();
				QString productName = dataMetaInfo->GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_NAME).toString();

				beforeSelectionQuery += QString(R"(INSERT INTO "ProductsTemp" ("DocumentId", "ProductId", "ProductName") VALUES('%1', '%2', '%3');)")
						.arg(qPrintable(productCollectionId))
						.arg(qPrintable(productId))
						.arg(productName)
						.toUtf8();
			}
		}
	}

	if (m_databaseEngineCompPtr.IsValid()){
		QSqlError sqlError;
		m_databaseEngineCompPtr->ExecSqlQuery(beforeSelectionQuery, &sqlError);
		if (sqlError.type() != QSqlError::NoError){
			SendErrorMessage(0, sqlError.text(), "CDeviceDatabaseDelegateComp");

			qDebug() << "SQL-error" << beforeSelectionQuery;

			return QByteArray();
		}
	}

	QByteArray selectionQuery = BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);

	return selectionQuery;
}


QByteArray CSoftwareProductDatabaseDelegateComp::GetObjectIdFromRecord(const QSqlRecord& record) const
{
	if (record.contains("DocumentId")){
		QByteArray documentId = record.value(qPrintable("DocumentId")).toByteArray();

		return documentId;
	}

	return QByteArray();
}


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QString CSoftwareProductDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	return R"(
			SELECT
				acc."Document"->'Groups' as "Groups",
				root."DocumentId",
				root."Document"->>'SerialNumber' as "SerialNumber",
				root."Document"->>'OrderId' as "OrderUuid",
				root."Document"->>'Project' as "Project",
				bp."Document"->>'HardwareId'  as "DeviceUuid",
				ord."Document"->>'OrderId' as "OrderId",
				ord."Document"->>'PurchaseId' as "PurchaseOrderId",
				acc."Document"->>'Name' as "Customer",
				acc."DocumentId" as "CustomerUuid",
				dev."Document"->>'MacAddress' as "DeviceId",
				root."Document"->'Licenses'->0->'LicenseData'->>'LicenseId' as "LicenseUuid",
				(SELECT lic."LicenseId" FROM "LicensesTemp" as lic WHERE lic."DocumentId" = root."Document"->'Licenses'->0->'LicenseData'->>'LicenseId') as "LicenseId",
				(SELECT lic."LicenseName" FROM "LicensesTemp" as lic WHERE lic."DocumentId" = root."Document"->'Licenses'->0->'LicenseData'->>'LicenseId') as "LicenseName",
				root."Document"->>'InUse' as "InUse",
				root."Document"->>'ProductId' as "ProductUuid",
				(SELECT prod."ProductId" FROM "ProductsTemp" as prod WHERE prod."DocumentId" = root."Document"->>'ProductId') as "ProductId",
				(SELECT prod."ProductName" FROM "ProductsTemp" as prod WHERE prod."DocumentId" = root."Document"->>'ProductId') as "ProductName",
				root."Document",
				root."LastModified",
				(
					SELECT "LastModified"
					FROM "SoftwareInstances" as t2
					WHERE "RevisionNumber" = 1 AND root."DocumentId" = t2."DocumentId" LIMIT 1
				) as "Added",
				(SELECT COUNT(*) FROM "Orders" WHERE "DocumentId" = root."Document"->>'OrderId' AND "IsActive" = true LIMIT 1) AS "OrderCount"
			FROM "SoftwareInstances" as root
			LEFT JOIN "BindingProducts" as bp  ON bp."Document"->'SoftwareIds' ? root."DocumentId" AND bp."IsActive"=true
			LEFT JOIN "Devices" as dev ON  dev."IsActive" = true AND dev."DocumentId" = bp."Document"->>'HardwareId'
			LEFT JOIN "Orders" as ord ON ord."DocumentId"=root."Document"->>'OrderId' AND ord."IsActive"=true
			LEFT JOIN "Accounts" as acc ON acc."IsActive" = true AND acc."DocumentId" = ord."Document"->>'OrderCustomer'
			WHERE root."IsActive"=true)";
}


bool CSoftwareProductDatabaseDelegateComp::CreateObjectFilterQuery(
			const iprm::IParamsSet& filterParams,
			QString& filterQuery) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	if (!paramIds.isEmpty()){
#if QT_VERSION >= 0x051500
		QByteArrayList paramIdsList(paramIds.cbegin(), paramIds.cend());
#else
		QByteArrayList paramIdsList = paramIds.toList();
#endif

		if (paramIdsList.contains("BindingFilter")){
			iprm::TParamsPtr<iprm::IParamsSet> bindingFilterParamPtr(&filterParams, "BindingFilter");
			if (bindingFilterParamPtr.IsValid()){
				iprm::TParamsPtr<iprm::IIdParam> hardwareUuidParamPtr(bindingFilterParamPtr.GetPtr(), "HardwareUuid");
				if (hardwareUuidParamPtr.IsValid()){
					QString value = hardwareUuidParamPtr->GetId();
					QString filter = QString(R"((bp."Document"->>'HardwareId' = '%1'))").arg(value);

					if (!filterQuery.isEmpty()){
						filterQuery += " AND ";
					}

					filterQuery += filter;
				}

				iprm::TParamsPtr<iprm::ITextParam> hardwareUuidFilterParamPtr(bindingFilterParamPtr.GetPtr(), "HardwareUuidFilter");
				if (hardwareUuidFilterParamPtr.IsValid()){
					QString value = hardwareUuidFilterParamPtr->GetText();
					QString elementFilter = QString(R"((bp."Document"->>'HardwareId' = '' OR bp."Document"->>'HardwareId' IS NULL OR bp."Document"->>'HardwareId' = '%1'))").arg(value);

					if (!filterQuery.isEmpty()){
						filterQuery += " AND ";
					}

					filterQuery += elementFilter;
				}

				iprm::TParamsPtr<iprm::ITextParam> deviceIdFilterParamPtr(bindingFilterParamPtr.GetPtr(), "DeviceId");
				if (deviceIdFilterParamPtr.IsValid()){
					QString value = deviceIdFilterParamPtr->GetText();

					QString elementFilter = QString(R"(((dev."Document"->>'MacAddress' = '' OR dev."Document"->>'MacAddress' IS NULL)))");

					if (!filterQuery.isEmpty()){
						filterQuery += " AND ";
					}

					filterQuery += elementFilter;
				}

				iprm::TParamsPtr<iprm::ITextParam> productUuidFilterParamPtr(bindingFilterParamPtr.GetPtr(), "ProductUuid");
				if (productUuidFilterParamPtr.IsValid()){
					QString value = productUuidFilterParamPtr->GetText();
					QString productFilter = QString(R"((root."Document"->>'ProductId' = '%1'))").arg(value);

					if (!filterQuery.isEmpty()){
						filterQuery += " AND ";
					}

					filterQuery += productFilter;
				}

				iprm::TParamsPtr<iprm::ITextParam> customerUuidFilterParamPtr(bindingFilterParamPtr.GetPtr(), "CustomerUuid");
				if (customerUuidFilterParamPtr.IsValid()){
					QString value = customerUuidFilterParamPtr->GetText();
					QString accountFilter = QString(R"((acc."DocumentId" = '%1'))").arg(value);

					if (!filterQuery.isEmpty()){
						filterQuery += " AND ";
					}

					filterQuery += accountFilter;
				}

				iprm::TParamsPtr<iprm::ITextParam> excludeFilterParamPtr(bindingFilterParamPtr.GetPtr(), "ExcludeUuids");
				if (excludeFilterParamPtr.IsValid()){
					QString value = excludeFilterParamPtr->GetText();

					if (!value.isEmpty()){
						QStringList uuids = value.split(';');

						QStringList resultUuids;
						for (const QString& uuid : uuids){
							QString result = "'" + uuid + "'";
							resultUuids << result;
						}

						QString excludeFilter = QString(R"((root."DocumentId" NOT IN (%1)))").arg(resultUuids.join(','));

						if (!filterQuery.isEmpty()){
							filterQuery += " AND ";
						}

						filterQuery += excludeFilter;
					}
				}

				iprm::TParamsPtr<iprm::ITextParam> licenseIdsFilterParamPtr(bindingFilterParamPtr.GetPtr(), "LicenseIds");
				if (licenseIdsFilterParamPtr.IsValid()){
					QString value = licenseIdsFilterParamPtr->GetText();

					if (!value.isEmpty()){
						QStringList uuids = value.split(';');

						QStringList resultUuids;
						for (const QString& uuid : uuids){
							QString result = "'" + uuid + "'";
							resultUuids << result;
						}

						QString excludeFilter = QString(R"(((SELECT lic."LicenseId" FROM "LicensesTemp" as lic WHERE lic."DocumentId" = root."Document"->'Licenses'->0->'LicenseData'->>'LicenseId') NOT IN (%1)))").arg(resultUuids.join(','));

						if (!filterQuery.isEmpty()){
							filterQuery += " AND ";
						}

						filterQuery += excludeFilter;
					}
				}
			}
		}

		if (paramIdsList.contains("CustomerUuid")){
			iprm::TParamsPtr<iprm::IIdParam> filterParamPtr(&filterParams, "CustomerUuid");
			if (filterParamPtr.IsValid()){
				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				QString value = filterParamPtr->GetId();

				filterQuery += QString(R"((acc."DocumentId" = '%1'))").arg(value);
			}
		}

		if (paramIdsList.contains("LicenseFilter")){
			iprm::TParamsPtr<iprm::IIdParam> filterParamPtr(&filterParams, "LicenseFilter");
			if (filterParamPtr.IsValid()){
				QString value = filterParamPtr->GetId();

				QString filter;
				if (value == "OnlyPaired"){
					filter = QString(R"(((dev."Document"->>'MacAddress' != '') AND ((root."Document"->>'InUse')::boolean = false)))");
				}
				else if (value == "OnlyUnpaired"){
					filter = QString(R"(((dev."Document"->>'MacAddress' = '' OR dev."Document"->>'MacAddress' IS NULL) AND ((root."Document"->>'InUse')::boolean = false)))");
				}
				else if (value == "OnlyInUse"){
					filter = QString(R"(((root."Document"->>'InUse')::boolean = true))");
				}

				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				filterQuery += filter;
			}
		}

		if (paramIdsList.contains("SerialNumber")){
			iprm::TParamsPtr<iprm::ITextParam> filterParamPtr(&filterParams, "SerialNumber");
			if (filterParamPtr.IsValid()){
				QString value = filterParamPtr->GetText();
				QString filter = QString(R"((root."Document"->>'SerialNumber' = '%1'))").arg(value);

				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				filterQuery += filter;
			}
		}

		if (paramIdsList.contains("Groups")){
			iprm::TParamsPtr<iprm::IParamsSet> filterParamPtr(&filterParams, "Groups");
			if (filterParamPtr.IsValid()){
				QByteArray userId;
				iprm::TParamsPtr<iprm::ITextParam> userParamPtr(filterParamPtr.GetPtr(), "UserParam");
				if (userParamPtr.IsValid()){
					userId = userParamPtr->GetText().toUtf8();
				}

				iprm::TParamsPtr<iprm::ITextParam> textParamPtr(filterParamPtr.GetPtr(), "GroupParam");
				QString groupFilter;

				if (textParamPtr.IsValid()){
					QByteArray groups = textParamPtr->GetText().toUtf8();
					QByteArrayList groupIds;
					if (!groups.isEmpty()){
						groupIds = groups.split(';');
					}

					QString ownerSubquery = QString(R"((SELECT sof."OwnerId" FROM "SoftwareInstances" as sof WHERE sof."DocumentId" = root."DocumentId" AND sof."RevisionNumber" = 1 LIMIT 1))");

					if (!groupIds.isEmpty()){
						QString array = "array[";

						for (int i = 0; i < groupIds.size(); i++){
							if (i > 0){
								array += ",";
							}

							array += "'" + groupIds[i] + "'";
						}

						array += "]";

						QString groupsQuery = QString(R"((SELECT "Groups" FROM "UsersTemp" WHERE "UserId" = %1))").arg(ownerSubquery);
						groupFilter += QString(R"((acc."Document"->'Groups' ?| %1) OR (root."Document"->>'OrderId' = '' AND (%2 ?| %1)))").arg(array).arg(QString(R"((to_jsonb(string_to_array((%1), ';'))))").arg(groupsQuery));
					}
					else{
						groupFilter += QString(R"(%1 = '%2')").arg(ownerSubquery).arg(userId);
					}

					if (!groupFilter.isEmpty()){
						if (!filterQuery.isEmpty()){
							filterQuery += " AND ";
						}

						filterQuery += "(" + groupFilter + ")";
					}
				}
			}
		}

		if (!filterQuery.isEmpty()){
			filterQuery = "(" + filterQuery + ")";
		}
	}

	return true;
}


bool CSoftwareProductDatabaseDelegateComp::CreateSortQuery(
			const imtbase::ICollectionFilter& collectionFilter,
			QString& sortQuery) const
{
	QByteArray columnId;
	QByteArray sortOrder;

	if (!collectionFilter.GetSortingInfoIds().isEmpty()){
		columnId = collectionFilter.GetSortingInfoIds().first();
	}

	switch (collectionFilter.GetSortingOrder()){
	case imtbase::ICollectionFilter::SO_ASC:
		sortOrder = "ASC";
		break;
	case imtbase::ICollectionFilter::SO_DESC:
		sortOrder = "DESC";
		break;
	case imtbase::ICollectionFilter::SO_NO_ORDER:
		break;
	}

	if (!columnId.isEmpty() && !sortOrder.isEmpty()){
		if (columnId == "LicenseId" || columnId == "LicenseName"){
			sortQuery =  QString(R"(ORDER BY (SELECT lic."%1" FROM "LicensesTemp" as lic WHERE lic."DocumentId" = root."Document"->'Licenses'->0->'LicenseData'->>'LicenseId') %2)")
					.arg(qPrintable(columnId))
					.arg(qPrintable(sortOrder));
		}
		else if (columnId == "ProductId"){
			sortQuery =  QString(R"(ORDER BY (SELECT prod."ProductId" FROM "ProductsTemp" as prod WHERE prod."DocumentId" = root."Document"->>'ProductId') %1)")
					.arg(qPrintable(sortOrder));
		}
		else if (columnId == "ProductName"){
			sortQuery =  QString(R"(ORDER BY (SELECT prod."ProductName" FROM "ProductsTemp" as prod WHERE prod."DocumentId" = root."Document"->>'ProductId') %1)")
					.arg(qPrintable(sortOrder));
		}
		else if (columnId == "OrderId" ||
				 columnId == "PurchaseOrderId" ||
				 columnId == "DeviceId" ||
				 columnId == "Customer" ||
				 columnId == "LastModified" ||
				 columnId == "Added"){
			sortQuery = QString("ORDER BY \"%1\" %2")
					.arg(qPrintable(columnId))
					.arg(qPrintable(sortOrder));
		}
		else{
			sortQuery = QString("ORDER BY root.\"Document\"->>'%1' %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
	}

	return true;
}


bool CSoftwareProductDatabaseDelegateComp::CreateTextFilterQuery(
			const imtbase::ICollectionFilter& collectionFilter,
			QString& textFilterQuery) const
{
	QByteArrayList filteringColumnIds = collectionFilter.GetFilteringInfoIds();
	if (filteringColumnIds.isEmpty()){
		return true;
	}

	QString textFilter = collectionFilter.GetTextFilter();
	if (!textFilter.isEmpty()){
		for (int i = 0; i < filteringColumnIds.count(); i++){
			if (i > 0){
				textFilterQuery += " OR ";
			}

			QByteArray columnId = filteringColumnIds[i];

			if (columnId == "OrderId"){
				textFilterQuery += QString("ord.\"Document\"->>'%1' ILIKE '%%2%'").arg(qPrintable(columnId)).arg(textFilter);
			}
			else if (columnId == "PurchaseOrderId"){
				textFilterQuery += QString("ord.\"Document\"->>'PurchaseId' ILIKE '%%1%'").arg(textFilter);
			}
			else if (columnId == "DeviceId"){
				textFilterQuery += QString("dev.\"Document\"->>'MacAddress' ILIKE '%%1%'").arg(textFilter);
			}
			else if (columnId == "Customer"){
				textFilterQuery += QString("acc.\"Document\"->>'Name' ILIKE '%%1%'").arg(textFilter);
			}
			else if (columnId == "DeviceUuid"){
				textFilterQuery += QString("bp.\"Document\"->>'%1' ILIKE '%%2%'").arg(qPrintable(columnId)).arg(textFilter);
			}
			else if (columnId == "LicenseId" || columnId == "LicenseName"){
				textFilterQuery += QString("(SELECT lic.\"%1\" FROM \"LicensesTemp\" as lic WHERE lic.\"DocumentId\" = root.\"Document\"->'Licenses'->0->'LicenseData'->>'LicenseId') ILIKE '%%2%'").arg(qPrintable(columnId)).arg(textFilter);
			}
			else if (columnId == "ProductId"){
				textFilterQuery += QString("(SELECT prod.\"ProductId\" FROM \"ProductsTemp\" as prod WHERE prod.\"DocumentId\" = root.\"Document\"->>'ProductId') ILIKE '%%1%'").arg(textFilter);
			}
			else if (columnId == "ProductName"){
				textFilterQuery += QString("(SELECT prod.\"ProductName\" FROM \"ProductsTemp\" as prod WHERE prod.\"DocumentId\" = root.\"Document\"->>'ProductId') ILIKE '%%1%'").arg(textFilter);
			}
			else{
				textFilterQuery += QString("root.\"Document\"->>'%1' ILIKE '%%2%'").arg(qPrintable(columnId)).arg(textFilter);
			}
		}
	}

	return true;
}


} // namespace prolifedb


