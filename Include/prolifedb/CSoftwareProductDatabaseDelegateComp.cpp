#include <prolifedb/CSoftwareProductDatabaseDelegateComp.h>


// ACF includes
#include <iprm/ITextParam.h>
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

//	if (!TableIsExists("UsersTemp")){
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
//	}

	beforeSelectionQuery += R"(DROP TABLE IF EXISTS "LicensesTemp";)";
	beforeSelectionQuery += R"(DROP TABLE IF EXISTS "ProductsTemp";)";

	beforeSelectionQuery += R"(CREATE TEMP TABLE "LicensesTemp"("DocumentId" varchar, "LicenseId" varchar, "LicenseName" varchar);)";
	beforeSelectionQuery += R"(CREATE TEMP TABLE "ProductsTemp"("DocumentId" varchar, "ProductId" varchar, "ProductName" varchar);)";

	if (m_licenseCollectionCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids licenseCollectionIds = m_licenseCollectionCompPtr->GetElementIds();

		for (const imtbase::ICollectionInfo::Id& licenseCollectionId : licenseCollectionIds){
			idoc::MetaInfoPtr dataMetaInfo = m_licenseCollectionCompPtr->GetDataMetaInfo(licenseCollectionId);
			QByteArray licenseId = dataMetaInfo->GetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_ID).toByteArray();
			QString licenseName = dataMetaInfo->GetMetaInfo(imtlic::ILicenseDefinition::MIT_LICENSE_NAME).toString();

			beforeSelectionQuery += QString(R"(INSERT INTO "LicensesTemp" ("DocumentId", "LicenseId", "LicenseName") VALUES('%1', '%2', '%3');)")
						.arg(qPrintable(licenseCollectionId))
						.arg(qPrintable(licenseId))
						.arg(licenseName)
						.toUtf8();
		}
	}

	if (m_productCollectionCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids productCollectionIds = m_productCollectionCompPtr->GetElementIds();

		for (const imtbase::ICollectionInfo::Id& productCollectionId : productCollectionIds){
			idoc::MetaInfoPtr dataMetaInfo = m_productCollectionCompPtr->GetDataMetaInfo(productCollectionId);

			QByteArray productId = dataMetaInfo->GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_ID).toByteArray();
			QString productName = dataMetaInfo->GetMetaInfo(imtlic::IProductInfo::MIT_PRODUCT_NAME).toString();

			beforeSelectionQuery += QString(R"(INSERT INTO "ProductsTemp" ("DocumentId", "ProductId", "ProductName") VALUES('%1', '%2', '%3');)")
					.arg(qPrintable(productCollectionId))
					.arg(qPrintable(productId))
					.arg(productName)
					.toUtf8();
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
				si."DocumentId",
				si."Document"->>'SerialNumber' as "SerialNumber",
				si."Document"->>'OrderId' as "OrderUuid",
				si."Document"->>'Project' as "Project",
				bp."Document"->>'HardwareId'  as "DeviceUuid",
				ord."Document"->>'OrderId' as "OrderId",
				acc."Document"->>'Name' as "Customer",
				acc."DocumentId" as "CustomerUuid",
				dev."Document"->>'MacAddress' as "DeviceId",
				si."Document"->'Licenses'->0->'LicenseData'->>'LicenseId' as "LicenseUuid",
				(SELECT lic."LicenseId" FROM "LicensesTemp" as lic WHERE lic."DocumentId" = si."Document"->'Licenses'->0->'LicenseData'->>'LicenseId') as "LicenseId",
				(SELECT lic."LicenseName" FROM "LicensesTemp" as lic WHERE lic."DocumentId" = si."Document"->'Licenses'->0->'LicenseData'->>'LicenseId') as "LicenseName",
				si."Document"->>'InUse' as "InUse",
				si."Document"->>'ProductId' as "ProductUuid",
				(SELECT prod."ProductId" FROM "ProductsTemp" as prod WHERE prod."DocumentId" = si."Document"->>'ProductId') as "ProductId",
				(SELECT prod."ProductName" FROM "ProductsTemp" as prod WHERE prod."DocumentId" = si."Document"->>'ProductId') as "ProductName",
				si."Document",
				si."LastModified",
				(
					SELECT "LastModified"
					FROM "SoftwareInstances" as t2
					WHERE "RevisionNumber" = 1 AND si."DocumentId" = t2."DocumentId" LIMIT 1
				) as "Added",
				(SELECT COUNT(*) FROM "Orders" WHERE "DocumentId" = si."Document"->>'OrderId' AND "IsActive" = true LIMIT 1) AS "OrderCount"
			FROM "SoftwareInstances" as si
			LEFT JOIN "BindingProducts" as bp  ON bp."Document"->'SoftwareIds' ? si."DocumentId" AND bp."IsActive"=true
			LEFT JOIN "Devices" as dev ON  dev."IsActive" = true AND dev."DocumentId" = bp."Document"->>'HardwareId'
			LEFT JOIN "Orders" as ord ON ord."DocumentId"=si."Document"->>'OrderId' AND ord."IsActive"=true
			LEFT JOIN "Accounts" as acc ON acc."IsActive" = true AND acc."DocumentId" = ord."Document"->>'OrderCustomer'
			WHERE si."IsActive"=true)";
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
				iprm::TParamsPtr<iprm::ITextParam> hardwareUuidParamPtr(bindingFilterParamPtr.GetPtr(), "HardwareUuid");
				if (hardwareUuidParamPtr.IsValid()){
					QString value = hardwareUuidParamPtr->GetText();
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
					QString productFilter = QString(R"((si."Document"->>'ProductId' = '%1'))").arg(value);

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

						QString excludeFilter = QString(R"((si."DocumentId" NOT IN (%1)))").arg(resultUuids.join(','));

						if (!filterQuery.isEmpty()){
							filterQuery += " AND ";
						}

						filterQuery += excludeFilter;
					}
				}
			}
		}

		if (paramIdsList.contains("CustomerUuid")){
			iprm::TParamsPtr<iprm::ITextParam> filterParamPtr(&filterParams, "CustomerUuid");
			if (filterParamPtr.IsValid()){
				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				QString value = filterParamPtr->GetText();

				filterQuery += QString(R"((acc."DocumentId" = '%1'))").arg(value);
			}
		}

		if (paramIdsList.contains("LicenseFilter")){
			iprm::TParamsPtr<iprm::ITextParam> filterParamPtr(&filterParams, "LicenseFilter");
			if (filterParamPtr.IsValid()){
				QString value = filterParamPtr->GetText();

				QString filter;
				if (value == "OnlyPaired"){
					filter = QString(R"(((dev."Document"->>'MacAddress' != '') AND ((si."Document"->>'InUse')::boolean = false)))");
				}
				else if (value == "OnlyUnpaired"){
					filter = QString(R"(((dev."Document"->>'MacAddress' = '' OR dev."Document"->>'MacAddress' IS NULL) AND ((si."Document"->>'InUse')::boolean = false)))");
				}
				else if (value == "OnlyInUse"){
					filter = QString(R"(((si."Document"->>'InUse')::boolean = true))");
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
				QString filter = QString(R"((si."Document"->>'SerialNumber' = '%1'))").arg(value);

				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				filterQuery += filter;
			}
		}

		if (paramIdsList.contains("Orders")){
			const iprm::ISelectionParam* selectionPtr = dynamic_cast<const iprm::ISelectionParam*>(filterParams.GetParameter("Orders"));
			if (selectionPtr != nullptr){
				const iprm::IOptionsList* optionsListPtr = selectionPtr->GetSelectionConstraints();
				if (optionsListPtr != nullptr){
					QString ordersFilterQuery;
					if (optionsListPtr->GetOptionsCount() > 0){
						ordersFilterQuery += "(";
					}

					for (int i = 0; i < optionsListPtr->GetOptionsCount(); i++){
						if (i > 0){
							ordersFilterQuery += " OR ";
						}
						QByteArray optionId = optionsListPtr->GetOptionId(i);
						QString optionName = optionsListPtr->GetOptionName(i);

						if (!optionName.isEmpty()){
							ordersFilterQuery += QString("si.\"Document\"->>'OrderId' = '%1'").arg(optionName);
						}
						else{
							ordersFilterQuery += QString("(si.\"Document\"->>'OrderId' = '' AND ((SELECT string_to_array('%1', ';') && string_to_array(%2, ';')) OR %3))")
										.arg(qPrintable(optionId))
										.arg("(SELECT \"Groups\" FROM \"UsersTemp\" WHERE \"UserId\" = (SELECT \"OwnerId\" FROM \"SoftwareInstances\" WHERE \"DocumentId\" = si.\"DocumentId\" AND \"RevisionNumber\" = 1 LIMIT 1))")
										.arg("((SELECT \"OwnerId\" FROM \"SoftwareInstances\" WHERE \"DocumentId\" = si.\"DocumentId\" AND \"RevisionNumber\" = 1 LIMIT 1) = 'su')");
						}
					}

					if (!ordersFilterQuery.isEmpty()){
						ordersFilterQuery += ')';
					}

					if (!ordersFilterQuery.isEmpty()){
						if (!filterQuery.isEmpty()){
							filterQuery += " AND ";
						}

						filterQuery += ordersFilterQuery;
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
			sortQuery =  QString(R"(ORDER BY (SELECT lic."%1" FROM "LicensesTemp" as lic WHERE lic."DocumentId" = si."Document"->'Licenses'->0->'LicenseData'->>'LicenseId') %2)")
					.arg(qPrintable(columnId))
					.arg(qPrintable(sortOrder));
		}
		else if (columnId == "ProductId"){
			sortQuery =  QString(R"(ORDER BY (SELECT prod."ProductId" FROM "ProductsTemp" as prod WHERE prod."DocumentId" = si."Document"->>'ProductId') %1)")
					.arg(qPrintable(sortOrder));
		}
		else if (columnId == "ProductName"){
			sortQuery =  QString(R"(ORDER BY (SELECT prod."ProductName" FROM "ProductsTemp" as prod WHERE prod."DocumentId" = si."Document"->>'ProductId') %1)")
					.arg(qPrintable(sortOrder));
		}
		else if (columnId == "OrderId" || columnId == "DeviceId" || columnId == "Customer" || columnId == "LastModified" || columnId == "Added"){
			sortQuery = QString("ORDER BY \"%1\" %2")
					.arg(qPrintable(columnId))
					.arg(qPrintable(sortOrder));
		}
		else{
			sortQuery = QString("ORDER BY si.\"Document\"->>'%1' %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
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
				textFilterQuery += QString("(SELECT lic.\"%1\" FROM \"LicensesTemp\" as lic WHERE lic.\"DocumentId\" = si.\"Document\"->'Licenses'->0->'LicenseData'->>'LicenseId') ILIKE '%%2%'").arg(qPrintable(columnId)).arg(textFilter);
			}
			else if (columnId == "ProductId"){
				textFilterQuery += QString("(SELECT prod.\"ProductId\" FROM \"ProductsTemp\" as prod WHERE prod.\"DocumentId\" = si.\"Document\"->>'ProductId') ILIKE '%%1%'").arg(textFilter);
			}
			else if (columnId == "ProductName"){
				textFilterQuery += QString("(SELECT prod.\"ProductName\" FROM \"ProductsTemp\" as prod WHERE prod.\"DocumentId\" = si.\"Document\"->>'ProductId') ILIKE '%%1%'").arg(textFilter);
			}
			else{
				textFilterQuery += QString("si.\"Document\"->>'%1' ILIKE '%%2%'").arg(qPrintable(columnId)).arg(textFilter);
			}
		}
	}

	return true;
}


} // namespace prolifedb


