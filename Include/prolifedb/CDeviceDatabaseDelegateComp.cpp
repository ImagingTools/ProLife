#include <prolifedb/CDeviceDatabaseDelegateComp.h>


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <istd/TOptDelPtr.h>
#include <istd/CSystem.h>
#include <istd/CCrcCalculator.h>

// ProLife includes
#include <prolifedata/TOrderedWrap.h>
#include <prolifedata/CDeviceInfo.h>


namespace prolifedb
{


static const QByteArray s_documentIdColumn = "DocumentId";
static const QByteArray s_idColumn = "Id";


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CDeviceDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_documentFactoriesCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<istd::IChangeable> documentPtr;

	documentPtr.SetPtr(new prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>());

	if (!documentPtr.IsValid()){
		return nullptr;
	}

	if (record.contains(*m_documentContentColumnIdAttrPtr)){
		QByteArray documentContent = record.value(qPrintable(*m_documentContentColumnIdAttrPtr)).toByteArray();

		if (ReadDataFromMemory(documentContent, *documentPtr)){
			return documentPtr.PopPtr();
		}
	}

	return nullptr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CDeviceDatabaseDelegateComp::CreateNewObjectQuery(
			const QByteArray& typeId,
			const QByteArray& proposedObjectId,
			const QString& objectName,
			const QString& objectDescription,
			const istd::IChangeable* valuePtr) const
{
	NewObjectQuery retVal;

	istd::TOptDelPtr<const istd::IChangeable> workingDocumentPtr;
	if (valuePtr != nullptr){
		workingDocumentPtr.SetPtr(valuePtr, false);
	}

	if (workingDocumentPtr.IsValid()){
		QByteArray documentContent;
		if (WriteDataToMemory(*workingDocumentPtr, documentContent)){
			const prolifedata::CIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::CIdentifiableDeviceInfo*>(workingDocumentPtr.GetPtr());
			Q_ASSERT(deviceInfoPtr != nullptr);
			if (deviceInfoPtr == nullptr){
				return NewObjectQuery();
			}

			QByteArray objectId = deviceInfoPtr->GetObjectUuid();
			quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

			int revisionVersion = 1;

			retVal.query = QString("UPDATE \"%1\" SET IsActive = false WHERE DocumentId = '%2'; INSERT INTO \"%1\"(DocumentId, AccountId, Document, RevisionNumber, LastModified, Checksum, IsActive) VALUES('%2', '%3', '%4', '%5', '%6', '%7', true);")
						.arg(qPrintable(*m_tableNameAttrPtr))
						.arg(qPrintable(objectId))
						.arg(qPrintable(""))
						.arg(SqlEncode(documentContent))
						.arg(revisionVersion)
						.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
						.arg(checksum).toLocal8Bit();

			retVal.objectName = objectName;
		}
	}

	return retVal;
}


QByteArray CDeviceDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object) const
{
	QByteArray retVal;

	QByteArray documentContent;
	if (WriteDataToMemory(object, documentContent)){
		quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());
		const prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::IDeviceInfo*>(&object);
		Q_ASSERT(deviceInfoPtr != nullptr);
		if (deviceInfoPtr == nullptr){
			return QByteArray();
		}

		retVal = QString("UPDATE \"%1\" SET IsActive = false WHERE DocumentId = '%2'; INSERT INTO \"%1\" (DocumentId, AccountId, Document, LastModified, Checksum, IsActive, RevisionNumber) VALUES('%2', '%3', '%4', '%5', '%6', true, (Select count(Id) from \"%1\" where DocumentId = '%2') + 1 );")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(objectId))
					.arg(qPrintable(""))
					.arg(SqlEncode(documentContent))
					.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
					.arg(checksum).toLocal8Bit();
	}

	return retVal;
}


} // namespace prolifedb


