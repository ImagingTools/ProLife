#include <prolifedb/COrderDatabaseDelegateComp.h>


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <iprm/TParamsPtr.h>
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

	QByteArray selectionQuery = BaseClass::GetSelectionQuery(objectId, offset, count, paramsPtr);

	return selectionQuery;
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

			int revisionVersion = 1;

			retVal.query = QString("UPDATE \"%1\" SET IsActive = false WHERE OrderId = '%2'; INSERT INTO \"%1\"(OrderId, AccountId, Document, RevisionNumber, LastModified, Checksum, IsActive) VALUES('%2', '%3', '%4', '%5', '%6', '%7', true);")
						.arg(qPrintable(*m_tableNameAttrPtr))
						.arg(qPrintable(objectId))
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
		documentContent = documentContent.replace("'", "''");
		quint32 checksum = istd::CCrcCalculator::GetCrcFromData((const quint8*)documentContent.constData(), documentContent.size());
		const prolifedata::IOrderInfo* orderInfoPtr = dynamic_cast<const prolifedata::IOrderInfo*>(&object);
		Q_ASSERT(orderInfoPtr != nullptr);
		if (orderInfoPtr == nullptr){
			return QByteArray();
		}
		QByteArray accountId = orderInfoPtr->GetCustomerId();
		QByteArray orderId = orderInfoPtr->GetOrderId();

		retVal = QString("UPDATE \"%1\" SET IsActive = false WHERE OrderId = '%2'; INSERT INTO \"%1\" (OrderId, AccountId, Document, LastModified, Checksum, IsActive, RevisionNumber) VALUES('%2', '%3', '%4', '%5', '%6', true, (Select count(Id) from \"%1\" where OrderId = '%2') + 1 );")
					.arg(qPrintable(*m_tableNameAttrPtr))
					.arg(qPrintable(objectId))
					.arg(qPrintable(accountId))
					.arg(qPrintable(documentContent))
					.arg(QDateTime::currentDateTime().toString(Qt::ISODate))
					.arg(checksum).toLocal8Bit();
	}

	return retVal;
}


// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

QString COrderDatabaseDelegateComp::GetBaseSelectionQuery() const
{
	return QString("SELECT * FROM \"%1\" WHERE IsActive = true").arg(qPrintable(*m_tableNameAttrPtr));
}


bool COrderDatabaseDelegateComp::CreateSortQuery(const imtbase::ICollectionFilter& collectionFilter, QString& sortQuery) const
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


bool COrderDatabaseDelegateComp::CreateFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
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


bool COrderDatabaseDelegateComp::CreateTextFilterQuery(
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


