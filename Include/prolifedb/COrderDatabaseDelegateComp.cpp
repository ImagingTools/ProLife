#include <prolifedb/COrderDatabaseDelegateComp.h>


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <istd/CSystem.h>
#include <istd/CCrcCalculator.h>

// ProLife includes
#include <prolifedata/IOrderInfo.h>


namespace prolifedb
{


static const QByteArray s_documentIdColumn = "OrderId";
static const QByteArray s_idColumn = "Id";


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

QByteArray COrderDatabaseDelegateComp::GetSelectionQuery(const QByteArray& objectId, int offset, int count, const iprm::IParamsSet* paramsPtr) const
{
	if (!objectId.isEmpty()){
		return QString("SELECT * FROM \"%1\" WHERE IsActive = true AND %2 = '%3'")
				.arg(qPrintable(*m_tableNameAttrPtr))
				.arg(qPrintable(s_documentIdColumn))
				.arg(qPrintable(objectId)).toLocal8Bit();
	}

	return  QString("SELECT * FROM \"%1\" WHERE IsActive = true")
			.arg(qPrintable(*m_tableNameAttrPtr)).toLocal8Bit();
}


istd::IChangeable* COrderDatabaseDelegateComp::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_documentFactoriesCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<istd::IChangeable> documentPtr;

//	if (record.contains("TypeId")){
//		QByteArray typeId = record.value("TypeId").toByteArray();

//		int index = m_documentFactoriesCompPtr.FindValue(typeId);
//		if (index >= 0){
//			documentPtr.SetPtr(m_documentFactoriesCompPtr.CreateInstance(index));
//		}
//	}

	if (m_documentFactoriesCompPtr.GetCount() > 0){
		documentPtr.SetPtr(m_documentFactoriesCompPtr.CreateInstance(0));
	}


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
	else{
//		if (m_documentFactCompPtr.IsValid()){
//			workingDocumentPtr.SetPtr(m_documentFactCompPtr.CreateInstance());
//		}
	}

	if (workingDocumentPtr.IsValid()){
		QByteArray documentContent;
		if (WriteDataToMemory(*workingDocumentPtr, documentContent)){

			const prolifedata::IOrderInfo* orderInfoPtr = dynamic_cast<const prolifedata::IOrderInfo*>(workingDocumentPtr.GetPtr());
			Q_ASSERT(orderInfoPtr != nullptr);
			if (orderInfoPtr == nullptr){
				return NewObjectQuery();
			}

			QByteArray objectId = proposedObjectId.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8() : proposedObjectId;
//			QByteArray revisionUuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
			quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());

			QByteArray accountId = orderInfoPtr->GetCustomerId();
			QByteArray orderId = orderInfoPtr->GetOrderId();

			int revisionVersion = 0;

			retVal.query = QString("UPDATE \"%1\" SET IsActive = false WHERE OrderId = '%2'; INSERT INTO \"%1\"(OrderId, AccountId, Document, RevisionNumber, LastModified, Checksum, IsActive) VALUES('%2', '%3', '%4', '%5', '%6', '%7', true);")
						.arg(qPrintable(*m_tableNameAttrPtr))
						.arg(qPrintable(orderId))
						.arg(qPrintable(accountId))
						.arg(qPrintable(documentContent))
						.arg(revisionVersion)
						.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
						.arg(checksum).toLocal8Bit();

			retVal.objectName = objectName;
		}
	}

	return retVal;
}


QByteArray COrderDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId) const
{
	QByteArray retVal = QString("DELETE FROM \"%1\" WHERE %2 = '%3';").arg(qPrintable(*m_tableNameAttrPtr)).arg(qPrintable(s_documentIdColumn)).arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


QByteArray COrderDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId,
			const istd::IChangeable& object) const
{
	QByteArray retVal;

	QByteArray documentContent;
	if (WriteDataToMemory(object, documentContent)){
		quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());
		QByteArray revisionUuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();

		retVal = QString("UPDATE \"%1\" SET IsActive = false where OrderId = '%2'; INSERT INTO \"%1\"(OrderId, RevisionId, LastModified, Checksum, IsActive, Order) VALUES('%2', '%3', '%4', '%5', true, '%6');")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(objectId))
					.arg(qPrintable(revisionUuid))
					.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
					.arg(checksum)
					.arg(qPrintable(documentContent.toBase64()))
					.toLocal8Bit();

	}

	return retVal;
}



} // namespace imtdb


