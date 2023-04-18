#include <prolifedb/COrderDatabaseDelegateComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QString COrderDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	return QString("SELECT \"Id\", \"%1\", \"Document\", \"RevisionNumber\", \"LastModified\","
					"(SELECT \"LastModified\" FROM \"%2\" as t1 WHERE \"RevisionNumber\" = 1 AND t2.\"%1\" = t1.\"%1\" LIMIT 1) as \"Added\","
					"(SELECT \"Document\"->>'AccountName' FROM \"Accounts\" as t3 WHERE t3.\"IsActive\" = true AND t3.\"DocumentId\" = t2.\"Document\"->>'OrderCustomer' LIMIT 1) as \"OrderCustomer\""
					" FROM \"%2\""
					" as t2 WHERE \"IsActive\" = true")
			.arg(qPrintable(*m_objectIdColumnAttrPtr))
			.arg(qPrintable(*m_tableNameAttrPtr));
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

						filterQuery = '(' + filterQuery + ')';
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
		else{
			sortQuery = QString("ORDER BY \"Document\"->>'%1' %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
		}
	}

	return true;
}


} // namespace prolifedb


