#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>

// ImtCore includes
#include <imtdb/IMetaInfoTableDelegate.h>
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>


namespace prolifedb
{


class CDeviceDatabaseDelegateComp: public imtdb::CSqlJsonDatabaseDelegateComp
{
public:
	typedef imtdb::CSqlJsonDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CDeviceDatabaseDelegateComp)
	I_END_COMPONENT

	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)
	virtual QString GetBaseSelectionQuery() const override;
	virtual bool CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const override;
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual bool CreateTextFilterQuery(const imtbase::ICollectionFilter& collectionFilter, QString& textFilterQuery) const override;
};


} // namespace prolifedb


