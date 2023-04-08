#include <prolifedb/COrderDatabaseDelegateComp.h>


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <istd/TOptDelPtr.h>
#include <istd/CSystem.h>
#include <istd/CCrcCalculator.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>


namespace prolifedb
{


static const QByteArray s_documentIdColumn = "OrderId";
static const QByteArray s_idColumn = "Id";


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* COrderDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_documentFactoriesCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<istd::IChangeable> documentPtr;
	documentPtr.SetPtr(new prolifedata::CIdentifiableOrderInfo());
	if (!documentPtr.IsValid()){
		return nullptr;
	}

	if (record.contains(*m_documentContentColumnIdAttrPtr)){
		QByteArray documentContent = record.value(qPrintable(*m_documentContentColumnIdAttrPtr)).toByteArray();

		if (ReadDataFromMemory("Software", documentContent, *documentPtr)){
			return documentPtr.PopPtr();
		}
	}

	return nullptr;
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery COrderDatabaseDelegateComp::CreateNewObjectQuery(
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
		if (WriteDataToMemory("Software", *workingDocumentPtr, documentContent)){
			const prolifedata::IOrderInfo* orderInfoPtr = dynamic_cast<const prolifedata::IOrderInfo*>(workingDocumentPtr.GetPtr());
			Q_ASSERT(orderInfoPtr != nullptr);
			if (orderInfoPtr == nullptr){
				return NewObjectQuery();
			}

			QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8() : proposedObjectId;
			quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

			QByteArray accountId = orderInfoPtr->GetCustomerId();
			QByteArray orderId = orderInfoPtr->GetOrderId();

			int revisionVersion = 1;
			retVal.query = QString("UPDATE \"%1\" SET \"IsActive\" = false WHERE \"OrderId\" = '%2'; INSERT INTO \"%1\"(\"OrderId\", \"AccountId\", \"Document\", \"RevisionNumber\", \"LastModified\", \"Checksum\", \"IsActive\") VALUES('%2', '%3', '%4', '%5', '%6', '%7', true);")
						.arg(qPrintable(*m_tableNameAttrPtr))
						.arg(qPrintable(objectId))
						.arg(qPrintable(accountId))
						.arg(SqlEncode(documentContent))
						.arg(revisionVersion)
						.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
						.arg(checksum).toLocal8Bit();

			retVal.objectName = objectName;
		}
	}

	return retVal;
}


QByteArray COrderDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object) const
{
	QByteArray retVal;

	QByteArray documentContent;
	if (WriteDataToMemory("Software", object, documentContent)){
		quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());
		const prolifedata::CIdentifiableOrderInfo* orderInfoPtr = dynamic_cast<const prolifedata::CIdentifiableOrderInfo*>(&object);
		Q_ASSERT(orderInfoPtr != nullptr);
		if (orderInfoPtr == nullptr){
			return QByteArray();
		}
		QByteArray accountId = orderInfoPtr->GetCustomerId();
		QByteArray orderId = orderInfoPtr->GetOrderId();
		QByteArray objectUuid = orderInfoPtr->GetObjectUuid();

		retVal = QString("UPDATE \"%1\" SET \"IsActive\" = false, \"OrderId\" = '%2' WHERE \"OrderId\" = '%3'; INSERT INTO \"%1\" (\"OrderId\", \"AccountId\", \"Document\", \"LastModified\", \"Checksum\", \"IsActive\", \"RevisionNumber\") VALUES('%2', '%4', '%5', '%6', '%7', true, (SELECT COUNT(\"Id\") FROM \"%1\" WHERE \"OrderId\" = '%3') + 1 );")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(objectUuid))
					.arg(qPrintable(objectId))
					.arg(qPrintable(accountId))
					.arg(SqlEncode(documentContent))
					.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
					.arg(checksum).toLocal8Bit();
	}

	return retVal;
}


// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

bool COrderDatabaseDelegateComp::SetCollectionItemMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	bool retVal = BaseClass::SetCollectionItemMetaInfoFromRecord(record, metaInfo);

	if (record.contains("Document")){
		QByteArray json = record.value("Document").toByteArray();
		QJsonDocument jsonDocument = QJsonDocument::fromJson(json);
		if (!jsonDocument.isNull()){
			if (jsonDocument.isObject()){
				QJsonObject jsonObject = jsonDocument.object();
				metaInfo.SetMetaInfo(idoc::IDocumentMetaInfo::MIT_TITLE, jsonObject["OrderId"].toString());
			}
		}
	}

	return retVal;
}


} // namespace prolifedb


