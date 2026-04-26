#include <prolifedb/CProcurementOrderDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserGroupFilter.h>


namespace prolifedb
{


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QString CProcurementOrderDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QString filterQuery;

	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (paramIds.contains("GroupFilter")){
		iprm::TParamsPtr<imtauth::IUserGroupFilter> filterParamPtr(&filterParams, "GroupFilter");
		if (filterParamPtr.IsValid()){
			QByteArrayList groupIds = filterParamPtr->GetGroupIds();
			QByteArray userId = filterParamPtr->GetUserId();

			if (!groupIds.isEmpty()){
				QString array = "array[";

				for (int i = 0; i < groupIds.size(); i++){
					if (i > 0){
						array += ",";
					}

					array += "'" + groupIds[i] + "'";
				}

				array += "]";

				filterQuery += QString(R"((root."Document"->'Groups' ?| %1))").arg(array);
			}
			else{
				filterQuery += QString(R"(users."Document"->>'Id' = '%1')").arg(qPrintable(userId));
			}
		}
	}

	return filterQuery;
}


QByteArray CProcurementOrderDatabaseDelegateComp::CreateJoinTablesQuery() const
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
