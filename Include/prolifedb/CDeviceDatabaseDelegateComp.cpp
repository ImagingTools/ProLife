#include <prolifedb/CDeviceDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>
#include <iprm/IIdParam.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IEnableableParam.h>

// ImtCore includes
#include <imtlic/IHardwareInstanceInfo.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/ILicenseDefinition.h>
#include <imtauth/IUserInfo.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/TOrderedWrap.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray CDeviceDatabaseDelegateComp::GetSelectionQuery(
			const QByteArray& objectId,
			int offset,
			int count,
			const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		return GetObjectSelectionQuery(objectId, paramsPtr);
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

	if (!TableExists("LicensesTemp")){
		beforeSelectionQuery += R"(DROP TABLE IF EXISTS "LicensesTemp";)";
		beforeSelectionQuery += R"(CREATE TEMP TABLE "LicensesTemp"("DocumentId" varchar, "LicenseId" varchar, "LicenseName" varchar);)";

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
	}

	if (!TableExists("ProductsTemp")){
		beforeSelectionQuery += R"(DROP TABLE IF EXISTS "ProductsTemp";)";
		beforeSelectionQuery += R"(CREATE TEMP TABLE "ProductsTemp"("DocumentId" varchar, "ProductId" varchar, "ProductName" varchar);)";

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
	}

	if (m_databaseEngineCompPtr.IsValid()){
		if (!beforeSelectionQuery.isEmpty()){
			QSqlError sqlError;
			m_databaseEngineCompPtr->ExecSqlQuery(beforeSelectionQuery, &sqlError);
			if (sqlError.type() != QSqlError::NoError){
				SendErrorMessage(0, sqlError.text(), "CDeviceDatabaseDelegateComp");

				qDebug() << "SQL-error" << beforeSelectionQuery;

				return QByteArray();
			}
		}
	}

	QByteArray selectionQuery = BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);

	return selectionQuery;
}


QByteArray CDeviceDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			bool /*useExternDelegate*/) const
{
	QByteArray retVal = BaseClass::CreateUpdateObjectQuery(collection, objectId, object, operationContextPtr, false);

	return retVal;
}


QByteArray CDeviceDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	QByteArray retVal;

	retVal += QString("DELETE FROM \"%1\" WHERE \"%2\" = '%3';").arg(qPrintable(*m_tableNameAttrPtr)).arg(qPrintable(*m_objectIdColumnAttrPtr)).arg(qPrintable(objectId)).toUtf8();

	return retVal;
}


// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

QString CDeviceDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	QString retVal;

	retVal += R"(
				SELECT *
				FROM
				(
					SELECT
						(SELECT "Document"->'Groups' FROM "Accounts" as acc WHERE acc."DocumentId" = (SELECT "Document"->>'OrderCustomer' FROM "Orders" as orders WHERE orders."IsActive" = true AND orders."DocumentId" = root."Document"->>'OrderId') AND acc."IsActive" = true) as "Groups",
						"Id",
						"DocumentId",
						"Document",
						('s' || replace("Document"->>'MacAddress', ':', '')) as "MacAddress",
						"Document"->>'OrderId' as "OrderUuid",
						"Document"->>'DeviceType' as "ProductUuid",
						"Document"->>'Project' as "Project",
						"Document"->>'ConfigurationType' as "LicenseUuid",
						(SELECT lic."LicenseName" FROM "LicensesTemp" as lic WHERE lic."DocumentId" = root."Document"->>'ConfigurationType') as "LicenseName",
						(SELECT lic."LicenseId" FROM "LicensesTemp" as lic WHERE lic."DocumentId" = root."Document"->>'ConfigurationType') as "LicenseId",
						(SELECT prod."ProductName" FROM "ProductsTemp" as prod WHERE prod."DocumentId" = root."Document"->>'DeviceType') as "DeviceType",
						"OwnerId",
						"RevisionNumber",
						"LastModified",
						(SELECT "LastModified" FROM "Devices" as t1 WHERE "RevisionNumber" = 1 AND root."DocumentId" = t1."DocumentId" LIMIT 1) as "Added",
						(SELECT "Document"->>'OrderCustomer' FROM "Orders" as orders WHERE orders."IsActive" = true AND orders."DocumentId" = root."Document"->>'OrderId') as "CustomerUuid",
						(SELECT "Document"->>'Name' FROM "Accounts" as acc WHERE acc."IsActive" = true AND acc."DocumentId" = ((SELECT "Document"->>'OrderCustomer' FROM "Orders" as orders WHERE orders."IsActive" = true AND orders."DocumentId" = root."Document"->>'OrderId' LIMIT 1))) as "Customer",
						(SELECT "Document"->>'OrderId' FROM "Orders" as t3 WHERE t3."IsActive" = true AND t3."DocumentId" = root."Document"->>'OrderId') as "OrderId",
						(SELECT "Document"->>'PurchaseId' FROM "Orders" as t3 WHERE t3."IsActive" = true AND t3."DocumentId" = root."Document"->>'OrderId') as "PurchaseOrderId",
						(SELECT jsonb_array_length("Document"->'SoftwareIds')  FROM "BindingProducts" as t3 WHERE t3."IsActive" = true AND t3."DocumentId" = root."DocumentId" ) as "SoftwareLinksCount",
						"IsActive"
					FROM "Devices" as root WHERE "IsActive" = true
				) as root WHERE "IsActive" = true )";
	return retVal;
}


bool CDeviceDatabaseDelegateComp::CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const
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
		sortOrder = "ASC";
		break;
	}

	if (!columnId.isEmpty() && !sortOrder.isEmpty()){
		if (	columnId == "LastModified" ||
				columnId == "Added" ||
				columnId == "OrderId" ||
				columnId == "LicenseId" ||
				columnId == "LicenseName" ||
				columnId == "DeviceType" ||
				columnId == "Customer" ||
				columnId == "PurchaseOrderId"){
			sortQuery = QString("ORDER BY \"%1\" %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
		else if (columnId == "Status"){
			sortQuery = QString(R"(ORDER BY CASE
						WHEN "Document"->>'Status' = 'none' THEN 0
						WHEN "Document"->>'Status' = 'accepted' THEN 1
						WHEN "Document"->>'Status' = 'inProgress' THEN 2
						WHEN "Document"->>'Status' = 'canceled' THEN 3
						WHEN "Document"->>'Status' = 'onHold' THEN 4
						WHEN "Document"->>'Status' = 'finished' THEN 5
						ELSE 6 END %1)")
					.arg(qPrintable(sortOrder));
		}
		else{
			sortQuery = QString("ORDER BY \"Document\"->>'%1' %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
	}

	return true;
}


bool CDeviceDatabaseDelegateComp::CreateObjectFilterQuery(
			const iprm::IParamsSet& filterParams,
			QString& filterQuery) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (!paramIds.isEmpty()){
#if QT_VERSION >= 0x051500
		QByteArrayList idsList(paramIds.cbegin(), paramIds.cend());
#else
		QByteArrayList idsList = paramIds.toList();
#endif

		if (idsList.contains("Status")){
			const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter("Status"));
			if (textParamPtr == nullptr){
				return false;
			}

			if (!filterQuery.isEmpty()){
				filterQuery += " AND ";
			}

			QString value = textParamPtr->GetText();
			filterQuery += QString("\"Document\"->>'Status' = '%1'").arg(value);
		}

		for (int i = 0; i < idsList.size(); i++){
			QByteArray key = idsList[i];
			if (key == "ExcludeIds"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				QStringList excludeIds = value.split(';');

				QStringList resultUuids;
				for (const QString& uuid : excludeIds){
					QString result = "'" + uuid + "'";
					resultUuids << result;
				}

				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				filterQuery += QString(R"((root."DocumentId" NOT IN (%1)))").arg(resultUuids.join(','));
			}
			else if (key == "ProductUuid"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();

				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				filterQuery += QString(R"((root."ProductUuid" = '%1'))").arg(value);
			}
			else if (key == "CustomerUuid"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();

				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				filterQuery += QString(R"((root."CustomerUuid" = '%1'))").arg(value);
			}
			else if (key == "LicenseStatus"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();

				if (value == "None"){
					continue;
				}

				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				QByteArray countLicenseSql = "(SELECT jsonb_array_length(\"Document\"->'SoftwareIds') FROM \"BindingProducts\" as t3  WHERE t3.\"IsActive\" = true AND t3.\"DocumentId\" = root.\"DocumentId\" )";

				if (value == "WithoutLicense"){
					filterQuery += "(" + countLicenseSql + " IS NULL OR " + countLicenseSql + " = 0)";
				}
				else{
					filterQuery += countLicenseSql + " > 0";
				}
				continue;
			}
			else if (key == "Groups"){
				iprm::TParamsPtr<iprm::IParamsSet> filterParamPtr(&filterParams, key);
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

						QString ownerSubquery = QString(R"((SELECT dev."OwnerId" FROM "Devices" as dev WHERE dev."DocumentId" = root."DocumentId" AND dev."RevisionNumber" = 1 LIMIT 1))");

						if (!groupIds.isEmpty()){
							QString array = "array[";

							for (int j = 0; j < groupIds.size(); j++){
								if (j > 0){
									array += ",";
								}

								array += "'" + groupIds[j] + "'";
							}

							array += "]";

							QString groupsQuery = QString(R"((SELECT "Groups" FROM "UsersTemp" WHERE "UserId" = %1))").arg(ownerSubquery);
							groupFilter += QString(R"((root."Groups" ?| %1) OR (root."Document"->>'OrderId' = '' AND (%2 ?| %1)))").arg(array).arg(QString(R"((to_jsonb(string_to_array((%1), ';'))))").arg(groupsQuery));
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
			else if (key == "MacAddress" || key == "SerialNumber"){
				iprm::TParamsPtr<iprm::IParamsSet> filterParamPtr(&filterParams, key);
				if (filterParamPtr.IsValid()){
					QString value;
					iprm::TParamsPtr<iprm::ITextParam> valueParamPtr(filterParamPtr.GetPtr(), "Value");
					if (valueParamPtr.IsValid()){
						value = valueParamPtr->GetText();
					}

					bool isEqual = true;
					iprm::TParamsPtr<iprm::IEnableableParam> enableableParamPtr(filterParamPtr.GetPtr(), "IsEqual");
					if (enableableParamPtr.IsValid()){
						isEqual = enableableParamPtr->IsEnabled();
					}

					if (isEqual){
						filterQuery += QString("(\"Document\"->>'%1' = '%2')").arg(qPrintable(key)).arg(value);
					}
					else{
						filterQuery += QString("(\"Document\"->>'%1' != '%1')").arg(qPrintable(key)).arg(value);
					}
				}
			}
		}
	}

	return true;
}


bool CDeviceDatabaseDelegateComp::CreateTextFilterQuery(
			const imtbase::ICollectionFilter& collectionFilter,
			QString& textFilterQuery) const
{
	QByteArrayList filteringColumnIds = collectionFilter.GetFilteringInfoIds();
	if (filteringColumnIds.isEmpty()){
		return true;
	}

	QString textFilter = collectionFilter.GetTextFilter();
	if (!textFilter.isEmpty()){

		for (int i = 0; i < filteringColumnIds.size(); i++){
			if (i > 0){
				textFilterQuery += " OR ";
			}

			if (filteringColumnIds[i] == "MacAddress"){
				textFilter = textFilter.toUtf8().replace(":", "");
			}

			if (	filteringColumnIds[i] == "OrderId" ||
					filteringColumnIds[i] == "MacAddress" ||
					filteringColumnIds[i] == "LicenseId" ||
					filteringColumnIds[i] == "PurchaseOrderId" ||
					filteringColumnIds[i] == "Customer" ||
					filteringColumnIds[i] == "LicenseName" ||
					filteringColumnIds[i] == "DeviceType"){
				textFilterQuery += QString("\"%1\" ILIKE '%%2%'").arg(qPrintable(filteringColumnIds[i])).arg(textFilter);
			}
			else{
				textFilterQuery += QString("\"Document\"->>'%1' ILIKE '%%2%'").arg(qPrintable(filteringColumnIds[i])).arg(textFilter);
			}
		}
	}

	return true;
}


bool CDeviceDatabaseDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	BaseClass::SetCollectionItemMetaInfoFromRecord(record, metaInfo);

	if (record.contains("Document")){
		QByteArray json = record.value("Document").toByteArray();
		QJsonDocument jsonDocument = QJsonDocument::fromJson(json);

		if (!jsonDocument.isNull()){
			QString description = jsonDocument["Description"].toString();
			metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION, description);

			QString name = jsonDocument["MacAddress"].toString();
			metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_NAME, name);
		}
	}

	return true;
}


} // namespace prolifedb


