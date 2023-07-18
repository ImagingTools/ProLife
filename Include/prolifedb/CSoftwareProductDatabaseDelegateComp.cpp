#include <prolifedb/CSoftwareProductDatabaseDelegateComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/IEnableableParam.h>
#include <iprm/TParamsPtr.h>

//ImtCore includes
#include <imtlic/CProductInstanceInfo.h>

//ProLife includes
#include <prolifedata/COrderInfo.h>


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
		QByteArray baseQuery = GetBaseSelectionQuery().toUtf8();

		QByteArray selectionQuery = QString("AND \"DocumentId\" = '%1' ")
				.arg(qPrintable(objectId)).toLocal8Bit();

		selectionQuery = baseQuery + selectionQuery;

		return selectionQuery;
	}

	return BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);
}


QByteArray CSoftwareProductDatabaseDelegateComp::GetObjectIdFromRecord(const QSqlRecord& record) const
{
	if (record.contains("DocumentId")){
		QByteArray documentId = record.value(qPrintable("DocumentId")).toByteArray();

		return documentId;
	}

	return QByteArray();
}


istd::IChangeable* CSoftwareProductDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	istd::TDelPtr<prolifedata::CIdentifiableOrderInfo> orderInfoPtr;
	orderInfoPtr.SetPtr(new prolifedata::CIdentifiableOrderInfo());

	istd::TDelPtr<imtlic::CIdentifiableSoftwareInstanceInfo> productInstancePtr;
	productInstancePtr.SetPtr(new imtlic::CIdentifiableSoftwareInstanceInfo());

	if (record.contains("Document")){
		QByteArray productJson = record.value(qPrintable("Document")).toByteArray();

		if (!ReadDataFromMemory("Software", productJson, *productInstancePtr)){
			return nullptr;
		}
	}

	if (record.contains("OrderUuid")){
		QByteArray orderUuid = record.value(qPrintable("OrderUuid")).toByteArray();

		orderInfoPtr->SetObjectUuid(orderUuid);
	}

	if (record.contains("OrderId")){
		QByteArray orderId = record.value(qPrintable("OrderId")).toByteArray();

		orderInfoPtr->SetOrderId(orderId);
	}

	if (record.contains("Customer")){
		QByteArray customer = record.value(qPrintable("Customer")).toByteArray();

		orderInfoPtr->SetCustomerId(customer);
	}

	QByteArray productObjectUuid = productInstancePtr->GetObjectUuid();

	imtbase::IObjectCollection* productCollection = orderInfoPtr->GetProducts();
	if (productCollection != nullptr){
		productCollection->InsertNewObject(QByteArray("Software"), "", "", productInstancePtr.GetPtr(), productObjectUuid);
	}

	return orderInfoPtr.PopPtr();
}


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QString CSoftwareProductDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	// COALESCE (NULLIF()) - Replacing all null values with an empty string
	return R"(SELECT * FROM
				(SELECT
					"Product"->'Data'->>'SerialNumber' as "SerialNumber",
					"Product"->'Data'->>'Uuid' as "DocumentId",
					"DocumentId" as "OrderUuid",
					"Document"->>'OrderId' as "OrderId",
					(SELECT "Document"->>'Name' FROM "Accounts" WHERE "Accounts"."IsActive" = true AND "Accounts"."DocumentId" = orders."Document"->>'OrderCustomer') as "Customer",
					"Product"->'Data' as "Document",
					COALESCE (
						NULLIF ((SELECT "Document"->>'MacAddress' FROM "Devices" WHERE "Devices"."IsActive" = true AND "Devices"."DocumentId" =
							(SELECT "HardwareProduct"->'Data'->>'DeviceId' as "HardwareId" FROM "Orders" as "HardwareOrders", jsonb_array_elements("Document"->'Products'->'ObjectsList') as "HardwareProduct"
								WHERE "HardwareProduct"->>'TypeId' = 'Hardware' AND "HardwareProduct"->'Data'->>'SoftwareId' = "Product"->'Data'->>'Uuid' AND "HardwareOrders"."IsActive" = true LIMIT 1)), ''), '') as "DeviceId",
					"IsActive"
					FROM "Orders" as orders, jsonb_array_elements("Document"->'Products'->'ObjectsList') as "Product"
				WHERE "Product"->>'TypeId' = 'Software' AND "IsActive" = true
				) t
			WHERE "IsActive" = true )";
}


bool CSoftwareProductDatabaseDelegateComp::CreateObjectFilterQuery(
			const iprm::IParamsSet& filterParams,
			QString& filterQuery) const
{
	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	if (!paramIds.isEmpty()){
		QByteArrayList paramIdsList(paramIds.cbegin(), paramIds.cend());

		int index = 0;
		for (const QByteArray& key : paramIdsList){
			if (key.contains('/')){
				continue;
			}

			if (index > 0){
				filterQuery += " AND ";
			}

			index++;

			if (key == "Orders"){
				const iprm::ISelectionParam* selectionPtr = dynamic_cast<const iprm::ISelectionParam*>(filterParams.GetParameter(key));
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
							QByteArray orderId = optionsListPtr->GetOptionId(i);
							ordersFilterQuery += QString("\"OrderUuid\" = '%1'").arg(qPrintable(orderId));
						}

						if (!ordersFilterQuery.isEmpty()){
							ordersFilterQuery += ')';
						}

						filterQuery += ordersFilterQuery;
					}
				}
			}
			else{
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
						filterQuery += QString("\"%1\" = '%2'").arg(qPrintable(key)).arg(value);
					}
					else{
						filterQuery += QString("\"%1\" != '%2'").arg(qPrintable(key)).arg(value);
					}
				}
			}
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
	}

	if (!columnId.isEmpty() && !sortOrder.isEmpty()){
		if (columnId == "OrderId" || columnId == "DeviceId" || columnId == "Customer"){
			sortQuery = QString("ORDER BY \"%1\" %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
		else{
			sortQuery = QString("ORDER BY \"Document\"->>'%1' %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
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
			if (columnId == "OrderId" || columnId == "DeviceId" || columnId == "Customer"){
				textFilterQuery += QString("\"%1\" ILIKE '%%2%'").arg(qPrintable(columnId)).arg(textFilter);
			}
			else{
				textFilterQuery += QString("\"Document\"->>'%1' ILIKE '%%2%'").arg(qPrintable(columnId)).arg(textFilter);
			}
		}
	}

	return true;
}


} // namespace prolifedb


