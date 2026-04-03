#include <prolifedb/CIqcRunDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>


namespace prolifedb
{


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QString CIqcRunDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QString filterQuery;

	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();
	if (paramIds.contains("BatchUuid")){
		iprm::TParamsPtr<iprm::IIdParam> batchUuidParamPtr(&filterParams, "BatchUuid");
		if (batchUuidParamPtr.IsValid()){
			QByteArray batchUuid = batchUuidParamPtr->GetId();
			if (!batchUuid.isEmpty()){
				filterQuery += QString(R"(root."Document"->>'BatchUuid' = '%1')").arg(qPrintable(batchUuid));
			}
		}
	}

	return filterQuery;
}


QByteArray CIqcRunDatabaseDelegateComp::CreateJoinTablesQuery() const
{
	return QByteArray();
}


} // namespace prolifedb
