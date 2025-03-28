#include <prolifedb/CDeviceDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilterHelper.h>
#include <imtauth/IUserInfo.h>

// ProLife includes
#include <prolifedata/IGroupFilter.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

QString CDeviceDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QString filterQuery;
	
	iprm::TParamsPtr<prolifedata::IGroupFilter> filterParamPtr(&filterParams, "GroupFilter");
	if (filterParamPtr.IsValid()){
		QByteArray userId = filterParamPtr->GetUserId();
		QByteArrayList groupIds = filterParamPtr->GetGroupIds();
		
		if (!groupIds.isEmpty()){
			QString array = "array[";
			
			for (int j = 0; j < groupIds.size(); j++){
				if (j > 0){
					array += ",";
				}
				
				array += "'" + groupIds[j] + "'";
			}
			
			array += "]";
			
			filterQuery += QString(R"((acc."Document"->'Groups' ?| %0) OR (root."DataMetaInfo"->>'OrderId' = '' AND users."Document"->'Groups' ?| %0))").arg(array);
		}
		else{
			filterQuery += QString(R"(users."Document"->>'Id' = '%1')").arg(qPrintable(userId));
		}
	}
	
	return filterQuery;
}


bool CDeviceDatabaseDelegateComp::CreateTextFilterQuery(
	const imtbase::IComplexCollectionFilter& collectionFilter,
	QString& textFilterQuery) const
{
	bool retVal = BaseClass::CreateTextFilterQuery(collectionFilter, textFilterQuery);
	if (retVal){
		QSet<QByteArray> filteringFieldIds =
			imtbase::CComplexCollectionFilterHelper::GetFilteringFieldIds(collectionFilter.GetFieldsFilter());
		
		QString textFilter = imtbase::CComplexCollectionFilterHelper::GetTextFilter(collectionFilter.GetFieldsFilter()).replace(":", "");
		if (!textFilter.isEmpty() && filteringFieldIds.contains("MacAddress")){
			QString macAddressTextFilter = QString(R"(('s' || replace("MacAddress", ':', '')) ILIKE '%%1%')").arg(textFilter);
			if (textFilterQuery.isEmpty()){
				textFilterQuery = macAddressTextFilter;
			}
			else{
				textFilterQuery = "(" + textFilterQuery + ") OR" + "(" + macAddressTextFilter + ")";
			}
		}
	}
	
	return retVal;
}


QByteArray CDeviceDatabaseDelegateComp::CreateJoinTablesQuery() const
{
	return QByteArray(R"(
			LEFT JOIN "Orders" AS orders
				ON orders."DocumentId"::text = root."DataMetaInfo"->>'OrderId'
				AND orders."State" = 'Active'
			LEFT JOIN "Accounts" AS acc
				ON acc."DocumentId"::text = orders."Document"->>'OrderCustomer'
				AND acc."State" = 'Active'
			LEFT JOIN "Users" AS users
				ON users."Document"->>'Id'::text = root1."RevisionInfo"->>'OwnerId'
				AND users."State" = 'Active'
	)");
}


} // namespace prolifedb


