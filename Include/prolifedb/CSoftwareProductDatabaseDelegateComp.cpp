#include <prolifedb/CSoftwareProductDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserGroupFilter.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QString CSoftwareProductDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QString filterQuery;

	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (paramIds.contains("GroupFilter")){
		iprm::TParamsPtr<imtauth::IUserGroupFilter> filterParamPtr(&filterParams, "GroupFilter");
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
	}

	return filterQuery;
}


QByteArray CSoftwareProductDatabaseDelegateComp::CreateJoinTablesQuery() const
{
	return QByteArray(R"(
			LEFT JOIN "Orders" AS orders
				ON orders."DocumentId"::text = root."DataMetaInfo"->>'OrderId'
				AND orders."State" = 'Active'
			LEFT JOIN "Accounts" AS acc
				ON acc."DocumentId"::text = orders."Document"->>'OrderCustomer'
				AND acc."State" = 'Active'
			LEFT JOIN "Users" AS users
				ON (
					(users."Document"->>'Id' = root1."RevisionInfo"->>'OwnerId'
					OR users."DocumentId"::text = root1."RevisionInfo"->>'OwnerId')
					AND users."State" = 'Active'
				)
	)");
}


} // namespace prolifedb


