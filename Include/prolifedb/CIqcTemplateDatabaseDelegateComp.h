#pragma once


// ImtCore includes
#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>


namespace prolifedb
{


class CIqcTemplateDatabaseDelegateComp: public imtdb::CSqlDatabaseDocumentDelegateComp
{
public:
	typedef imtdb::CSqlDatabaseDocumentDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CIqcTemplateDatabaseDelegateComp)
	I_END_COMPONENT

	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)
	virtual QString CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const override;
	virtual QByteArray CreateJoinTablesQuery() const override;
};


} // namespace prolifedb
