#include <prolifedb/COrderDatabaseDelegateComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

//QString COrderDatabaseDelegateComp::GetBaseSelectionQuery() const
//{
//	return QString("SELECT \"Id\", \"%1\", \"Document\", \"RevisionNumber\", \"LastModified\","
//					"(SELECT \"LastModified\" FROM \"%2\" as t1 WHERE \"RevisionNumber\" = 1 AND t2.\"%1\" = t1.\"%1\" LIMIT 1) as \"Added\","
//					"(SELECT \"Document\"->>'AccountName' FROM \"Accounts\" as t3 WHERE t3.\"IsActive\" = true AND t3.\"DocumentId\" = t2.\"Document\"->>'OrderCustomer' LIMIT 1) as \"OrderCustomer\""
//					" FROM \"%2\""
//					" as t2 WHERE \"IsActive\" = true")
//			.arg(qPrintable(*m_objectIdColumnAttrPtr))
//			.arg(qPrintable(*m_tableNameAttrPtr));
//}


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
		QByteArrayList ids(paramIds.cbegin(), paramIds.cend());

		for (const QByteArray& id : ids){
			if (id == "OrderCustomers"){
				const iprm::ISelectionParam* selectionPtr = dynamic_cast<const iprm::ISelectionParam*>(filterParams.GetParameter(id));
				if (selectionPtr != nullptr){
					const iprm::IOptionsList* optionsListPtr = selectionPtr->GetSelectionConstraints();
					if (optionsListPtr != nullptr){
						for (int i = 0; i < optionsListPtr->GetOptionsCount(); i++){
							if (i > 0){
								filterQuery += " OR ";
							}
							QByteArray accountId = optionsListPtr->GetOptionId(i);
							filterQuery += QString("\"Document\"->>'OrderCustomer' = '%1'").arg(qPrintable(accountId));
						}

						if (!filterQuery.isEmpty()){
							filterQuery = '(' + filterQuery + ')';
						}
					}
				}
			}
		}
	}

	return !filterQuery.isEmpty();
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
//			else if (filteringColumnIds[i] == "OrderCustomer"){
//				textFilterQuery += QString("(SELECT \"Document\"->>'AccountName' FROM \"Accounts\" as t3 WHERE t3.\"IsActive\" = true AND t3.\"DocumentId\" = t2.\"Document\"->>'%1' LIMIT 1) ILIKE '%%2%'")
//										.arg(qPrintable(filteringColumnIds[i]))
//										.arg(textFilter);
//			}
			else{
				textFilterQuery += QString("\"Document\"->>'%1' ILIKE '%%2%'").arg(qPrintable(filteringColumnIds[i])).arg(textFilter);
			}
		}
	}

	return true;
}


} // namespace prolifedb


