#include <prolifedb/CIqcRunDatabaseDelegateComp.h>


namespace prolifedb
{


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QString CIqcRunDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& /*filterParams*/) const
{
	return QString();
}


QByteArray CIqcRunDatabaseDelegateComp::CreateJoinTablesQuery() const
{
	return QByteArray();
}


} // namespace prolifedb
