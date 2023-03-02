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

QByteArray CDeviceDatabaseDelegateComp::GetSelectionQuery(const QByteArray& objectId, int offset, int count, const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		return QString("SELECT * FROM \"%1\" WHERE IsActive = true AND %2 = '%3'")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(s_documentIdColumn))
				.arg(qPrintable(objectId)).toLocal8Bit();
	}

	QByteArray selectionQuery = BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);

	return selectionQuery;
}


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


QByteArray CDeviceDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId) const
{
	QByteArray retVal = QString("DELETE FROM \"%1\" WHERE %2 = '%3';").arg(qPrintable(*m_tableNameAttrPtr)).arg(qPrintable(s_documentIdColumn)).arg(qPrintable(objectId)).toLocal8Bit();

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


QByteArray CDeviceDatabaseDelegateComp::GetCountQuery(const iprm::IParamsSet* paramsPtr) const
{
	return QString("SELECT COUNT(*) FROM \"%1\" WHERE IsActive = true").arg(qPrintable(*m_tableNameAttrPtr)).toLocal8Bit();
}


// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

QString CDeviceDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	return QString("SELECT * FROM \"%1\" WHERE IsActive = true").arg(qPrintable(*m_tableNameAttrPtr));
}


bool CDeviceDatabaseDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray objectId;
	if (record.contains("DocumentId")){
		objectId = record.value("DocumentId").toByteArray();
	}

	if (!objectId.isEmpty()){
		QByteArray query = QString("SELECT * from \"Devices\" WHERE DocumentId = '%1' AND RevisionNumber = '%2';")
				.arg(qPrintable(objectId))
				.arg(1).toUtf8();

		QSqlError error;
		QSqlQuery sqlQuery = m_databaseEngineCompPtr->ExecSqlQuery(query, &error);

		if (sqlQuery.next()){
			QSqlRecord orderRecord = sqlQuery.record();

			if (orderRecord.contains("LastModified")){
				QDateTime insertionTime = orderRecord.value("LastModified").toDateTime();

				metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME, insertionTime);
				metaInfo.SetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME, insertionTime);
			}
		}
	}

	if (record.contains("LastModified")){
		QDateTime lastModificationTime = record.value("LastModified").toDateTime();

		metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME, lastModificationTime);
		metaInfo.SetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME, lastModificationTime);
	}

	if (record.contains("RevisionNumber")){
		qlonglong revisionNumber = record.value("RevisionNumber").toLongLong();

		metaInfo.SetMetaInfo(imtbase::ICollectionInfo::MIT_REVISION, revisionNumber);
	}

	return true;
}


bool CDeviceDatabaseDelegateComp::CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const
{
	QByteArray columnId;
	QByteArray sortOrder;

	if (!collectionFilter.GetSortingInfoIds().isEmpty()){
		columnId = collectionFilter.GetSortingInfoIds().first();
	}

	switch (collectionFilter.GetSortingOrder()){
	case imtbase::ICollectionFilter::SO_ASC:
		sortOrder = "ASC";
		break;
	case imtbase::ICollectionFilter::SO_DESC:
		sortOrder = "DESC";
		break;
	}

	if (!columnId.isEmpty() && !sortOrder.isEmpty()){
		sortQuery = QString("ORDER BY document->>'%1' %2").arg(qPrintable(columnId)).arg(qPrintable(sortOrder));
	}

	return true;
}


bool CDeviceDatabaseDelegateComp::CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	bool retVal = true;

	QString textFilterQuery;
	iprm::TParamsPtr<imtbase::ICollectionFilter> collectionFilterParamPtr(&filterParams, "Filter");
	if (collectionFilterParamPtr.IsValid()){
		retVal = CreateTextFilterQuery(*collectionFilterParamPtr, textFilterQuery);
		if (!retVal){
			return false;
		}
	}

	if (!textFilterQuery.isEmpty()){
		filterQuery += "AND (" + textFilterQuery + ")";
	}

	return true;
}


bool CDeviceDatabaseDelegateComp::CreateTextFilterQuery(
			const imtbase::ICollectionFilter& collectionFilter,
			QString& textFilterQuery) const
{
	QByteArrayList filteringColumnIds = collectionFilter.GetFilteringInfoIds();
	if (filteringColumnIds.isEmpty()){
		return true;
	}

	QString textFilter = collectionFilter.GetTextFilter();
	if (!textFilter.isEmpty()){
		textFilterQuery = QString("document->>'%1' ILIKE '%%2%'").arg(qPrintable(filteringColumnIds.first())).arg(textFilter);

		for (int i = 1; i < filteringColumnIds.count(); ++i){
			textFilterQuery += " OR ";

			textFilterQuery += QString("document->>'%1' ILIKE '%%2%'").arg(qPrintable(filteringColumnIds[i])).arg(textFilter);
		}
	}

	return true;
}


} // namespace prolifedb


