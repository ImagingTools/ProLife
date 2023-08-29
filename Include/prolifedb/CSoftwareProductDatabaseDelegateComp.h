#pragma once


// ImtCore includes
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>


namespace prolifedb
{


class CSoftwareProductDatabaseDelegateComp: public imtdb::CSqlJsonDatabaseDelegateComp
{
public:
	typedef imtdb::CSqlJsonDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CSoftwareProductDatabaseDelegateComp)
		I_ASSIGN(m_orderDatabaseDelegateCompPtr, "OrderDatabaseSqlDelegate", "Order database sql delegate", true, "OrderDatabaseSqlDelegate");
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual QByteArray GetSelectionQuery(
				const QByteArray& objectId = QByteArray(),
				int offset = 0,
				int count = -1,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual QByteArray GetObjectIdFromRecord(const QSqlRecord& record) const override;

	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)
	virtual QString GetBaseSelectionQuery() const override;
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;
	virtual bool CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const override;
	virtual bool CreateTextFilterQuery(const imtbase::ICollectionFilter& collectionFilter, QString& textFilterQuery) const override;

private:
	I_REF(imtdb::ISqlDatabaseObjectDelegate, m_orderDatabaseDelegateCompPtr);
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
};


} // namespace prolifedb


