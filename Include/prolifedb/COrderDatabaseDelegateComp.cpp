#include <prolifedb/COrderDatabaseDelegateComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>
#include <istd/CCrcCalculator.h>
#include <iprm/ITextParam.h>
#include <iprm/IIdParam.h>
#include <iprm/IEnableableParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtlic/CHardwareInstanceInfo.h>

// ProLife includes
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/TOrderedWrap.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray COrderDatabaseDelegateComp::GetSelectionQuery(
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


QByteArray COrderDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			bool useExternDelegate) const
{
	QByteArray retVal;

	if (useExternDelegate){
		QByteArrayList oldOrderedDeviceIds;
		imtbase::IObjectCollection::DataPtr objectPtr;
		if (collection.GetObjectData(objectId, objectPtr)){
			prolifedata::COrderInfo* oldOrderInfoPtr = dynamic_cast<prolifedata::COrderInfo*>(objectPtr.GetPtr());
			if (oldOrderInfoPtr != nullptr){
				oldOrderedDeviceIds << GetDeviceIdsFromOrder(oldOrderInfoPtr);
			}
		}

		QByteArrayList newOrderedDeviceIDs;
		prolifedata::COrderInfo* newOrderInfoPtr = const_cast<prolifedata::COrderInfo*>(dynamic_cast<const prolifedata::COrderInfo*>(&object));
		if (newOrderInfoPtr != nullptr){
			newOrderedDeviceIDs << GetDeviceIdsFromOrder(newOrderInfoPtr);
		}

		// Calculate removed devices
		QByteArrayList removedDeviceIds;
		for (const QByteArray& deviceId : oldOrderedDeviceIds){
			if (!newOrderedDeviceIDs.contains(deviceId)){
				removedDeviceIds << deviceId;
			}
		}

		for (const QByteArray& deviceId : removedDeviceIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_deviceCollectionCompPtr->GetObjectData(deviceId, dataPtr)){
				prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(dataPtr.GetPtr());
				if (deviceInfoPtr != nullptr){
					deviceInfoPtr->SetOrderId("");

					retVal += m_deviceDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_deviceCollectionCompPtr, deviceId, *deviceInfoPtr, operationContextPtr, false);
				}
			}
		}
	}

	QByteArray documentContent;
	if (WriteDataToMemory("DocumentInfo", object, documentContent)){
		quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

		QString queryStr;
		if (*m_isMultiTypeAttrPtr){
			queryStr = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\" (\"DocumentId\", \"Document\", \"LastModified\", \"Checksum\", \"IsActive\", \"RevisionNumber\", \"TypeId\") VALUES('%2', '%3', '%4', '%5', true, "
			" (SELECT COUNT(\"Id\") FROM \"%1\" WHERE \"DocumentId\" = '%2') + 1 ),"
			" (SELECT \"TypeId\" FROM \"%1\" WHERE \"DocumentId\" = '%2' LIMIT 1) )," );

		}
		else{
			queryStr = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"DocumentId\" = '%2'; INSERT INTO \"%1\" (\"DocumentId\", \"Document\", \"LastModified\", \"Checksum\", \"IsActive\", \"RevisionNumber\") VALUES('%2', '%3', '%4', '%5', true, (SELECT COUNT(\"Id\") FROM \"%1\" WHERE \"DocumentId\" = '%2') + 1 );");
		}
		retVal += queryStr
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(objectId))
					.arg(SqlEncode(documentContent))
					.arg(QDateTime::currentDateTimeUtc().toString(Qt::ISODate))
					.arg(checksum)
					.toUtf8();

		retVal += CreateOperationDescriptionQuery(objectId, operationContextPtr);
	}

	return retVal;
}


QByteArray COrderDatabaseDelegateComp::CreateDeleteObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const imtbase::IOperationContext* operationContextPtr) const
{
	QByteArray retVal;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		prolifedata::COrderInfo* orderInfoPtr = dynamic_cast<prolifedata::COrderInfo*>(objectPtr.GetPtr());
		if (orderInfoPtr != nullptr){
			QByteArrayList deviceIDs = GetDeviceIdsFromOrder(orderInfoPtr);
			for (const QByteArray& deviceId : deviceIDs){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_deviceCollectionCompPtr->GetObjectData(deviceId, dataPtr)){
					prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(dataPtr.GetPtr());
					if (deviceInfoPtr != nullptr){
						deviceInfoPtr->SetOrderId("");

						retVal += m_deviceDatabaseDelegateCompPtr->CreateUpdateObjectQuery(*m_deviceCollectionCompPtr, deviceId, *deviceInfoPtr, operationContextPtr, false);
					}
				}
			}
		}
	}
	retVal += QString("DELETE FROM \"%1\" WHERE \"%2\" = '%3';").arg(qPrintable(*m_tableNameAttrPtr)).arg(qPrintable(*m_objectIdColumnAttrPtr)).arg(qPrintable(objectId)).toUtf8();

	return retVal;
}


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QString COrderDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	return QString("SELECT *"
					 "FROM ("
						"SELECT "
							"(SELECT \"Document\"->'Groups' FROM \"Accounts\" as acc WHERE acc.\"DocumentId\" = t1.\"Document\"->>'OrderCustomer' AND acc.\"IsActive\" = true) as \"Groups\","
							"\"DocumentId\", "
							"\"Document\", "
							"\"LastModified\", "
							"("
								"SELECT \"LastModified\" "
								"FROM \"Orders\" as root "
								"WHERE \"RevisionNumber\" = 1 AND t1.\"DocumentId\" = root.\"DocumentId\" LIMIT 1"
							") as \"Added\", "
							"("
								"SELECT \"Document\"->>'Name' "
								"FROM \"Accounts\" as t3 "
								"WHERE t3.\"IsActive\" = true AND t3.\"DocumentId\" = t1.\"Document\"->>'OrderCustomer'"
							") as \"OrderCustomer\", "
							"("
								"SELECT array_to_string(ARRAY(SELECT "
									"(SELECT dev.\"Document\"->>'MacAddress' "
									"FROM \"Devices\" as dev "
									"WHERE dev.\"IsActive\" = true AND dev.\"DocumentId\" = item_object->'Data'->>'DeviceId') "
								"FROM \"Orders\" as ord, jsonb_array_elements(\"Document\"->'Products'->'ObjectsList') with ordinality arr(item_object, position) "
								"WHERE ord.\"IsActive\" = true AND ord.\"DocumentId\" = t1.\"DocumentId\" AND item_object->'Data'->'DeviceId' IS NOT NULL), '')"
							") as \"MacAddress\", "
							"\"IsActive\""
							"FROM \"Orders\" as t1"
						") as root "
					"WHERE \"IsActive\" = true");
}


bool COrderDatabaseDelegateComp::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (!paramIds.isEmpty()){
#if QT_VERSION >= 0x051500
		QByteArrayList ids(paramIds.cbegin(), paramIds.cend());
#else
		QByteArrayList ids = paramIds.toList();
#endif
		for (const QByteArray& id : ids){
			if (id == "CustomerUuid"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(id));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();

				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
				}

				filterQuery += QString("(\"Document\"->>'OrderCustomer' = '%1')").arg(value);
			}
			else if (id == "Groups"){
				iprm::TParamsPtr<iprm::IParamsSet> filterParamPtr(&filterParams, id);
				if (filterParamPtr.IsValid()){
					QByteArray userId;
					iprm::TParamsPtr<iprm::ITextParam> userParamPtr(filterParamPtr.GetPtr(), "UserParam");
					if (userParamPtr.IsValid()){
						userId = userParamPtr->GetText().toUtf8();
					}

					iprm::TParamsPtr<iprm::ITextParam> textParamPtr(filterParamPtr.GetPtr(), "GroupParam");
					if (textParamPtr.IsValid()){
						QByteArray groups = textParamPtr->GetText().toUtf8();
						QByteArrayList groupIds;
						if (!groups.isEmpty()){
							groupIds = groups.split(';');
						}

						if (!groupIds.isEmpty()){
							QString array = "array[";

							for (int i = 0; i < groupIds.size(); i++){
								if (i > 0){
									array += ",";
								}

								array += "'" + groupIds[i] + "'";
							}

							array += "]";

							if (!filterQuery.isEmpty()){
								filterQuery += " AND ";
							}

							filterQuery += QString(R"((root."Groups" ?| %1))").arg(array);
						}
						else{
							if (!filterQuery.isEmpty()){
								filterQuery += " AND ";
							}

							filterQuery += QString(R"((SELECT ord."OwnerId" FROM "Orders" as ord WHERE ord."DocumentId" = root."DocumentId" AND ord."RevisionNumber" = 1 LIMIT 1) = '%1')").arg(userId);
						}
					}
				}
			}
			else if (id == "OrderId" || id == "PurchaseId"){
				iprm::TParamsPtr<iprm::IParamsSet> filterParamPtr(&filterParams, id);
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
						filterQuery += QString("(\"Document\"->>'%1' = '%2')").arg(qPrintable(id)).arg(value);
					}
					else{
						filterQuery += QString("(\"Document\"->>'%1' != '%1')").arg(qPrintable(id)).arg(value);
					}
				}
			}
		}
	}

	return true;
}


bool COrderDatabaseDelegateComp::CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const
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
	}

	if (!columnId.isEmpty() && !sortOrder.isEmpty()){
		if (columnId == "LastModified" || columnId == "Added" || columnId == "OrderCustomer"){
			sortQuery = QString("ORDER BY \"%1\" %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
		else if (columnId == "Status"){
			sortQuery = QString(R"(ORDER BY CASE
						WHEN "Document"->>'Status' = 'none' THEN 0
						WHEN "Document"->>'Status' = 'created' THEN 1
						WHEN "Document"->>'Status' = 'inProgress' THEN 2
						WHEN "Document"->>'Status' = 'canceled' THEN 3
						WHEN "Document"->>'Status' = 'onHold' THEN 4
						WHEN "Document"->>'Status' = 'finished' THEN 5
						WHEN "Document"->>'Status' = 'closed' THEN 6
						ELSE 7 END %1)")
					.arg(qPrintable(sortOrder));
		}
		else{
			sortQuery = QString("ORDER BY \"Document\"->>'%1' %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
	}

	return true;
}


bool COrderDatabaseDelegateComp::CreateTextFilterQuery(
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

			if (filteringColumnIds[i] == "OrderCustomer"){
				textFilterQuery += QString("\"OrderCustomer\" ILIKE '%%1%'")
										.arg(textFilter);
			}
			else if (filteringColumnIds[i] == "MacAddress"){
				textFilterQuery += QString("\"MacAddress\" ILIKE '%%1%'")
										.arg(textFilter);
			}
			else{
				textFilterQuery += QString("\"Document\"->>'%1' ILIKE '%%2%'").arg(qPrintable(filteringColumnIds[i])).arg(textFilter);
			}
		}
	}

	return true;
}


bool COrderDatabaseDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	BaseClass::SetCollectionItemMetaInfoFromRecord(record, metaInfo);

	if (record.contains("Document")){
		QByteArray json = record.value("Document").toByteArray();
		QJsonDocument jsonDocument = QJsonDocument::fromJson(json);

		if (!jsonDocument.isNull()){
			QString description = jsonDocument["Description"].toString();
			metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION, description);

			QString name = jsonDocument["OrderId"].toString();
			metaInfo.SetMetaInfo(imtbase::ICollectionInfo::EIT_NAME, name);
		}
	}

	return true;
}


// protected methods

QByteArrayList COrderDatabaseDelegateComp::GetDeviceIdsFromOrder(prolifedata::COrderInfo* orderInfoPtr) const
{
	QByteArrayList result;

	if (orderInfoPtr != nullptr){
		imtbase::IObjectCollection* productCollectionPtr = orderInfoPtr->GetProducts();
		if (productCollectionPtr != nullptr){
			imtbase::ICollectionInfo::Ids orderedProductsIds = productCollectionPtr->GetElementIds();
			for(const QByteArray& objectId : orderedProductsIds){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (productCollectionPtr->GetObjectData(objectId, dataPtr)){
					const imtlic::CIdentifiableHardwareInstanceInfo* hardwareProductPtr = dynamic_cast<const imtlic::CIdentifiableHardwareInstanceInfo*>(dataPtr.GetPtr());
					if (hardwareProductPtr != nullptr){
						QByteArray deviceId = hardwareProductPtr->GetObjectUuid();
						result << deviceId;
					}
				}
			}
		}
	}

	return result;
}


} // namespace prolifedb


