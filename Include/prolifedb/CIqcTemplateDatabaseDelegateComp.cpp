#include <prolifedb/CIqcTemplateDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>


namespace prolifedb
{


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QString CIqcTemplateDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QString filterQuery;

	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	if (paramIds.contains("SupplierId")){
		iprm::TParamsPtr<iprm::IIdParam> supplierIdParamPtr(&filterParams, "SupplierId");
		if (supplierIdParamPtr.IsValid()){
			QByteArray supplierId = supplierIdParamPtr->GetId();
			if (!supplierId.isEmpty()){
				filterQuery = QString(R"(root."Document"->>'SupplierId' = '%1')").arg(qPrintable(supplierId));
			}
		}
	}

	return filterQuery;
}


QByteArray CIqcTemplateDatabaseDelegateComp::CreateJoinTablesQuery() const
{
	return QByteArray();
}


} // namespace prolifedb
