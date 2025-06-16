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

	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (paramIds.contains("GroupFilter")){
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

				filterQuery += QString(R"((acc."Document"->'Groups' ?| %1))").arg(array);
			}
			else{
				filterQuery += QString(R"(users."Document"->>'Id' = '%1')").arg(qPrintable(userId));
			}
		}
	}

	return filterQuery;
}


QByteArray COrderDatabaseDelegateComp::CreateJoinTablesQuery() const
{
	return QByteArray(R"(
			LEFT JOIN "Accounts" AS acc
				ON acc."DocumentId"::text = root."Document"->>'OrderCustomer'
				AND acc."State" = 'Active'
			LEFT JOIN "Users" AS users
				ON users."Document"->>'Id'::text = root1."RevisionInfo"->>'OwnerId'
				AND users."State" = 'Active'
	)");
}


} // namespace prolifedb


