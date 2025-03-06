#include <prolifedb/CAccountDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ProLife includes
#include <prolifedata/IGroupFilter.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

QString CAccountDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QString filterQuery;
	
	iprm::TParamsPtr<prolifedata::IGroupFilter> filterParamPtr(&filterParams, "GroupFilter");
	if (filterParamPtr.IsValid()){
		QByteArray userId = filterParamPtr->GetUserId();
		QByteArrayList groupIds = filterParamPtr->GetGroupIds();
		
		if (groupIds.isEmpty()){
			filterQuery = QString("\"Document\"->'Groups' = '[]'");
		}
		else {
			for (int i = 0; i < groupIds.size(); i++){
				if (i > 0){
					filterQuery += " OR ";
				}
				
				filterQuery += QString("\"Document\"->'Groups' ? '%1'").arg(qPrintable(groupIds[i]));
			}
			
			QString ownerSubquery = QString(R"((SELECT acc."RevisionInfo"->>'OwnerId' FROM "Accounts" as acc WHERE acc."DocumentId" = root."DocumentId" AND (acc."RevisionInfo"->>'RevisionNumber')::int = 1 LIMIT 1))");
			
			filterQuery += QString(R"( OR (%1 = '%2'))").arg(ownerSubquery, qPrintable(userId));
		}
	}
	
	return filterQuery;
}


} // namespace prolifedb


