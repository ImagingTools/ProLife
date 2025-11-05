#include <prolifedb/CAccountDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserGroupFilter.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

QString CAccountDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QString filterQuery;

	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (paramIds.contains("GroupFilter")){
		iprm::TParamsPtr<imtauth::IUserGroupFilter> filterParamPtr(&filterParams, "GroupFilter");
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
	}

	return filterQuery;
}


QByteArray CAccountDatabaseDelegateComp::CreateJoinTablesQuery() const
{
	return QByteArray(R"(
			LEFT JOIN "Users" AS users
				ON (
					(users."Document"->>'Id' = root1."RevisionInfo"->>'OwnerId'
					OR users."DocumentId"::text = root1."RevisionInfo"->>'OwnerId')
					AND users."State" = 'Active'
				)
	)");
}


} // namespace prolifedb


