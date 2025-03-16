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
			filterQuery = QString("root.\"Document\"->'Groups' = '[]'");
		}
		else {
			for (int i = 0; i < groupIds.size(); i++){
				if (i > 0){
					filterQuery += " OR ";
				}
				
				filterQuery += QString("root.\"Document\"->'Groups' ? '%1'").arg(qPrintable(groupIds[i]));
			}
			
			filterQuery = QString(R"((%0) OR users."Document"->>'Id' = '%1')").arg(filterQuery, qPrintable(userId));
		}
	}
	
	return filterQuery;
}


QByteArray CAccountDatabaseDelegateComp::CreateJoinTablesQuery() const
{
	return QByteArray(R"(
			LEFT JOIN "Users" AS users
				ON users."DocumentId"::text = root."RevisionInfo"->>'OwnerId'
	)");
}


} // namespace prolifedb


