#pragma once


// ImtCore includes
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>


namespace prolifedb
{


class CAccountDatabaseDelegateComp: public imtdb::CSqlJsonDatabaseDelegateComp
{
public:
	typedef imtdb::CSqlJsonDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CAccountDatabaseDelegateComp)
	I_END_COMPONENT

	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual bool SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const override;
};


} // namespace prolifedb


