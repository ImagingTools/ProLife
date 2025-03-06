#include <prolifedb/COrderDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ProLife includes
#include <prolifedata/IGroupFilter.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QString COrderDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QString filterQuery;
	
	iprm::TParamsPtr<prolifedata::IGroupFilter> filterParamPtr(&filterParams, "GroupFilter");
	if (filterParamPtr.IsValid()){
		QByteArray userId = filterParamPtr->GetUserId();
		QByteArrayList groupIds = filterParamPtr->GetGroupIds();
		
		if (!groupIds.isEmpty()){
			QString array = "array[";
			
			for (int i = 0; i < groupIds.size(); i++){
				if (i > 0){
					array += ",";
				}
				
				array += "'" + groupIds[i] + "'";
			}
			
			array += "]";
			
			filterQuery += QString(R"(((SELECT \"Document\"->'Groups' FROM \"Accounts\" as acc WHERE acc.\"DocumentId\"::text = t1.\"Document\"->>'OrderCustomer' AND acc.\"State\" = 'Active') ?| %1))").arg(array);
		}
		else{
			filterQuery += QString(R"((SELECT ord."RevisionInfo"->>'OwnerId' FROM "Orders" as ord WHERE ord."DocumentId" = root."DocumentId" AND (ord."RevisionInfo"->>'RevisionNumber')::int = 1 LIMIT 1) = '%1')").arg(qPrintable(userId));
		}
	}
	
	return filterQuery;
}


} // namespace prolifedb


