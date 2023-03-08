#pragma once


// ACF includes
#include <ifile/IFilePersistence.h>

// ImtCore includes
#include <imtdb/IMetaInfoTableDelegate.h>
//#include <imtdb/CSqlDatabaseDocumentDelegateComp.h>
#include <imtdb/CSqlJsonDatabaseDelegateComp.h>


namespace prolifedb
{


class CDeviceDatabaseDelegateComp: public imtdb::CSqlJsonDatabaseDelegateComp
{
public:
	typedef imtdb::CSqlJsonDatabaseDelegateComp BaseClass;

	I_BEGIN_COMPONENT(CDeviceDatabaseDelegateComp)
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual istd::IChangeable* CreateObjectFromRecord(const QSqlRecord& record) const override;
	virtual NewObjectQuery CreateNewObjectQuery(
				const QByteArray& typeId,
				const QByteArray& proposedObjectId,
				const QString& objectName,
				const QString& objectDescription,
				const istd::IChangeable* valuePtr) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object) const override;
};


} // namespace prolifedb


