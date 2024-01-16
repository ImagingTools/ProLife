#include <prolifedb/COrderDatabaseDelegateComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>
#include <istd/CCrcCalculator.h>
#include <iprm/ITextParam.h>
#include <iprm/IEnableableParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtlic/CHardwareInstanceInfo.h>

// ProLife includes
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/TOrderedWrap.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray COrderDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			const imtbase::IOperationContext* operationContextPtr,
			bool useExternDelegate) const
{
	QByteArray retVal;

	if (useExternDelegate){
		QByteArrayList oldOrderedDeviceIDs;
		imtbase::IObjectCollection::DataPtr objectPtr;
		if (collection.GetObjectData(objectId, objectPtr)){
			prolifedata::COrderInfo* oldOrderInfoPtr = dynamic_cast<prolifedata::COrderInfo*>(objectPtr.GetPtr());
			if (oldOrderInfoPtr != nullptr){
				oldOrderedDeviceIDs << GetDeviceIdsFromOrder(oldOrderInfoPtr);
			}
		}

		QByteArrayList newOrderedDeviceIDs;
		prolifedata::COrderInfo* newOrderInfoPtr = const_cast<prolifedata::COrderInfo*>(dynamic_cast<const prolifedata::COrderInfo*>(&object));
		if (newOrderInfoPtr != nullptr){
			newOrderedDeviceIDs << GetDeviceIdsFromOrder(newOrderInfoPtr);
		}

		// Calculate removed devices
		QByteArrayList removedDeviceIDs;
		for (const QByteArray& deviceId : oldOrderedDeviceIDs){
			if (!newOrderedDeviceIDs.contains(deviceId)){
				removedDeviceIDs << deviceId;
			}
		}

		for (const QByteArray& deviceId : removedDeviceIDs){
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
							"\"DocumentId\", "
							"\"Document\", "
							"\"LastModified\", "
							"("
								"SELECT \"LastModified\" "
								"FROM \"Orders\" as t2 "
								"WHERE \"RevisionNumber\" = 1 AND t1.\"DocumentId\" = t2.\"DocumentId\" LIMIT 1"
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
						") u "
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
			else if (id == "OrderCustomers"){
				const iprm::ISelectionParam* selectionPtr = dynamic_cast<const iprm::ISelectionParam*>(filterParams.GetParameter(id));
				if (selectionPtr != nullptr){
					const iprm::IOptionsList* optionsListPtr = selectionPtr->GetSelectionConstraints();
					if (optionsListPtr != nullptr){

						QString filter;

						for (int i = 0; i < optionsListPtr->GetOptionsCount(); i++){
							if (i > 0){
								filter += " OR ";
							}
							QByteArray accountId = optionsListPtr->GetOptionId(i);
							filter += QString("\"Document\"->>'OrderCustomer' = '%1'").arg(qPrintable(accountId));
						}

						if (!filter.isEmpty()){
							if (!filterQuery.isEmpty()){
								filterQuery += " AND ";
							}
						}

						filter = "(" + filter + ")";

						filterQuery += filter;
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

		qDebug() << "filterQuery" << filterQuery;
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


