#include <prolifedb/CSoftwareProductDatabaseDelegateComp.h>


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

		QByteArray selectionQuery = QString("AND \"Product\"->'Data'->>'Uuid' = '%1' ")
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
	return QString("SELECT \"Product\"->'Data'->>'Uuid' as \"DocumentId\", \"DocumentId\" as \"OrderUuid\", \"Document\"->>'OrderId' as \"OrderId\", \"Product\"->'Data' as \"Document\" "
					"FROM \"Orders\", jsonb_array_elements(\"Document\"->'Products'->'ObjectsList') as \"Product\" "
					"WHERE \"Product\"->>'TypeId' = 'Software' AND \"IsActive\" = true ");
}


bool CSoftwareProductDatabaseDelegateComp::CreateObjectFilterQuery(
			const iprm::IParamsSet& filterParams,
			QString& filterQuery) const
{
	return BaseClass::CreateObjectFilterQuery(filterParams, filterQuery);
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
		if (columnId == "OrderId"){
			sortQuery = QString("ORDER BY \"%1\" %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
		else{
			sortQuery = QString("ORDER BY \"Product\"->'Data'->>'%1' %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
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
				textFilterQuery += QString("\"Document\"->>'OrderId' ILIKE '%%1%'").arg(textFilter);
			}
			else{
				textFilterQuery += QString("\"Product\"->'Data'->>'%1' ILIKE '%%2%'").arg(qPrintable(columnId)).arg(textFilter);
			}
		}
	}

	return true;
}


} // namespace prolifedb


