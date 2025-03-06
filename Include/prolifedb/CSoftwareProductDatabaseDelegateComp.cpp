#include <prolifedb/CSoftwareProductDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>

// ProLife includes
#include <prolifedata/IGroupFilter.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QString CSoftwareProductDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QString filterQuery;
	
	iprm::TParamsPtr<prolifedata::IGroupFilter> filterParamPtr(&filterParams, "GroupFilter");
	if (filterParamPtr.IsValid()){
		QByteArray userId = filterParamPtr->GetUserId();
		QByteArrayList groupIds = filterParamPtr->GetGroupIds();
		
		QString accountGroupsQuery = QString(R"((SELECT "Document"->'Groups' FROM "Accounts" as acc WHERE acc."DocumentId"::text = (SELECT "Document"->>'OrderCustomer' FROM "Orders" as orders WHERE orders."State" = 'Active' AND orders."DocumentId"::text = root."Document"->>'OrderId') AND acc."State" = 'Active'))");
		QString ownerSubquery = QString(R"((SELECT "RevisionInfo"->>'OwnerId' FROM "SoftwareInstances" as dev WHERE dev."DocumentId"::text = root."DocumentId"::text AND (dev."RevisionInfo"->>'RevisionNumber')::int = 1 LIMIT 1))");
		
		if (!groupIds.isEmpty()){
			QString array = "array[";
			
			for (int j = 0; j < groupIds.size(); j++){
				if (j > 0){
					array += ",";
				}
				
				array += "'" + groupIds[j] + "'";
			}
			
			array += "]";
			
			QString groupsQuery = QString(R"((SELECT "Document"->'Groups' FROM "Users" WHERE "DocumentId"::text = %1))").arg(ownerSubquery);
			filterQuery += QString(R"((%0 ?| %1) OR (root."Document"->>'OrderId' = '' AND (%2 ?| %1)))").arg(accountGroupsQuery, array, QString(R"((to_jsonb(string_to_array((%1), ';'))))").arg(groupsQuery));
		}
		else{
			filterQuery += QString(R"(%1 = '%2')").arg(ownerSubquery, qPrintable(userId));
		}
	}
	
	return filterQuery;
}


} // namespace prolifedb


