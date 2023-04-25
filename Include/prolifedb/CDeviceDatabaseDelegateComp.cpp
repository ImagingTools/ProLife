#include <prolifedb/CDeviceDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>
#include <iprm/ISelectionParam.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

QString CDeviceDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	return QString("SELECT \"Id\", \"%1\", \"Document\", \"RevisionNumber\", \"LastModified\","
					"(SELECT \"LastModified\" FROM \"%2\" as t1 WHERE \"RevisionNumber\" = 1 AND t2.\"%1\" = t1.\"%1\" LIMIT 1) as \"Added\","
					"(SELECT \"Document\"->>'OrderId' FROM \"Orders\" as t3 WHERE t3.\"IsActive\" = true AND t3.\"DocumentId\" = t2.\"Document\"->>'OrderId' LIMIT 1) as \"OrderId\""
					" FROM \"%2\""
					" as t2 WHERE \"IsActive\" = true")
			.arg(qPrintable(*m_objectIdColumnAttrPtr))
			.arg(qPrintable(*m_tableNameAttrPtr));
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
	}

	if (!columnId.isEmpty() && !sortOrder.isEmpty()){
		if (columnId == "LastModified" || columnId == "Added" || columnId == "OrderId"){
			sortQuery = QString("ORDER BY \"%1\" %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
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
		QByteArrayList idsList(paramIds.cbegin(), paramIds.cend());
		for (int i = 0; i < idsList.size(); i++){
			QByteArray key = idsList[i];

			if (i > 0){
				filterQuery += " AND ";
			}

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
							ordersFilterQuery += QString("\"Document\"->>'OrderId' = '%1'").arg(qPrintable(orderId));
						}

						if (!ordersFilterQuery.isEmpty()){
							ordersFilterQuery += ')';
						}

						filterQuery += ordersFilterQuery;
					}
				}
			}
			else{
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				filterQuery += QString("\"Document\"->>'%1' = '%2'").arg(qPrintable(key)).arg(value);
			}
		}

		if (!filterQuery.isEmpty()){
			filterQuery = '(' + filterQuery + ')';
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

			if (filteringColumnIds[i] == "OrderId"){
				textFilterQuery += QString("(SELECT \"Document\"->>'%1' FROM \"Orders\" as t3 WHERE t3.\"IsActive\" = true AND t3.\"DocumentId\" = t2.\"Document\"->>'%1' LIMIT 1) ILIKE '%%2%'")
						.arg(qPrintable(filteringColumnIds[i]))
						.arg(textFilter);;
			}
			else{
				textFilterQuery += QString("\"Document\"->>'%1' ILIKE '%%2%'").arg(qPrintable(filteringColumnIds[i])).arg(textFilter);
			}
		}
	}

	return true;
}


} // namespace prolifedb


