#include <prolifedb/CDeviceDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtlic/IHardwareInstanceInfo.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/TOrderedWrap.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

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
	return QString("SELECT \"Id\", \"%1\", \"Document\", \"RevisionNumber\", \"LastModified\","
					"(SELECT \"LastModified\" FROM \"%2\" as t1 WHERE \"RevisionNumber\" = 1 AND t2.\"%1\" = t1.\"%1\" LIMIT 1) as \"Added\","
					"(SELECT \"Document\"->>'OrderId' FROM \"Orders\" as t3 WHERE t3.\"IsActive\" = true AND t3.\"DocumentId\" = t2.\"Document\"->>'OrderId') as \"OrderId\","
					" (SELECT jsonb_array_length(\"Document\"->'SoftwareIds')  FROM \"BindingProducts\" as t3 "
					" WHERE t3.\"IsActive\" = true AND t3.\"DocumentId\" = t2.\"DocumentId\" ) as \"SoftwareLinksCount\" "
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
	case imtbase::ICollectionFilter::SO_NO_ORDER:
		sortOrder = "ASC";
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
#if QT_VERSION >= 0x051500
		QByteArrayList idsList(paramIds.cbegin(), paramIds.cend());
#else
		QByteArrayList idsList = paramIds.toList();
#endif
		for (int i = 0; i < idsList.size(); i++){
			QByteArray key = idsList[i];

			if (key == "LicenseStatus"){
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

//				if (i > 0){
//					filterQuery += " AND ";
//				}

				QByteArray countLicenseSql = "(SELECT jsonb_array_length(\"Document\"->'SoftwareIds') FROM \"BindingProducts\" as t3  WHERE t3.\"IsActive\" = true AND t3.\"DocumentId\" = t2.\"DocumentId\" )";

				if (value == "WithoutLicense"){
					filterQuery += "(" + countLicenseSql + " IS NULL OR " + countLicenseSql + " = 0)";
				}
				else{
					filterQuery += countLicenseSql + " > 0";
				}
				continue;
			}
			else if (key == "Orders"){
				const iprm::ISelectionParam* selectionPtr = dynamic_cast<const iprm::ISelectionParam*>(filterParams.GetParameter(key));
				if (selectionPtr != nullptr){
					const iprm::IOptionsList* optionsListPtr = selectionPtr->GetSelectionConstraints();
					if (optionsListPtr != nullptr){
						QString ordersFilterQuery;
						if (optionsListPtr->GetOptionsCount() > 0){
							ordersFilterQuery += "(";
						}

						for (int j = 0; j < optionsListPtr->GetOptionsCount(); j++){
							if (j > 0){
								ordersFilterQuery += " OR ";
							}
							QByteArray orderId = optionsListPtr->GetOptionId(j);
							ordersFilterQuery += QString("\"Document\"->>'OrderId' = '%1'").arg(qPrintable(orderId));
						}

						if (!ordersFilterQuery.isEmpty()){
							ordersFilterQuery += ')';
						}

//						if (i > 0){
//							filterQuery += " AND ";
//						}

						if (!filterQuery.isEmpty()){
							filterQuery += " AND ";
						}

						filterQuery += ordersFilterQuery;
					}
				}
			}
			else if (key == "Status"){
				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(filterParams.GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

//				if (i > 0){
//					filterQuery += " AND ";
//				}

				if (!filterQuery.isEmpty()){
					filterQuery += " AND ";
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


