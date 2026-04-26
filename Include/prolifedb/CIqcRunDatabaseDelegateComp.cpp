#include <prolifedb/CIqcRunDatabaseDelegateComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IIdParam.h>


namespace prolifedb
{


// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QString CIqcRunDatabaseDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QString filterQuery;

	auto appendFilter = [&filterQuery](const QString& clause)
	{
		if (clause.isEmpty()){
			return;
		}

		if (!filterQuery.isEmpty()){
			filterQuery += " AND ";
		}

		filterQuery += clause;
	};

	iprm::IParamsSet::Ids paramIds = filterParams.GetParamIds();

	if (paramIds.contains("BatchUuid")){
		iprm::TParamsPtr<iprm::IIdParam> batchUuidParamPtr(&filterParams, "BatchUuid");
		if (batchUuidParamPtr.IsValid()){
			QByteArray batchUuid = batchUuidParamPtr->GetId();
			if (!batchUuid.isEmpty()){
				appendFilter(QString(R"(root."Document"->>'BatchUuid' = '%1')").arg(qPrintable(batchUuid)));
			}
		}
	}

	if (paramIds.contains("SystemId")){
		iprm::TParamsPtr<iprm::IIdParam> systemIdParamPtr(&filterParams, "SystemId");
		if (systemIdParamPtr.IsValid()){
			QByteArray systemId = systemIdParamPtr->GetId();
			if (!systemId.isEmpty()){
				appendFilter(QString(R"(root."Document"->>'SystemId' = '%1')").arg(qPrintable(systemId)));
			}
		}
	}

	if (paramIds.contains("ExternalRunId")){
		iprm::TParamsPtr<iprm::IIdParam> externalRunIdParamPtr(&filterParams, "ExternalRunId");
		if (externalRunIdParamPtr.IsValid()){
			QByteArray externalRunId = externalRunIdParamPtr->GetId();
			if (!externalRunId.isEmpty()){
				appendFilter(QString(R"(root."Document"->>'ExternalRunId' = '%1')").arg(qPrintable(externalRunId)));
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
