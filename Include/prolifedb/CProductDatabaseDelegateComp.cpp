#include <prolifedb/CProductDatabaseDelegateComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ProLife includes
#include <prolifedata/CProductInfoMetaInfo.h>


namespace prolifedb
{


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

istd::IChangeable* CProductDatabaseDelegateComp::CreateObjectFromRecord(const QByteArray& /*typeId*/, const QSqlRecord& record) const
{
	if (!m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_productInfoFactCompPtr.IsValid()){
		return nullptr;
	}

    istd::TDelPtr<prolifedata::IProductInfo> productInfoPtr = m_productInfoFactCompPtr.CreateInstance();
	if (!productInfoPtr.IsValid()){
		return nullptr;
	}

	QByteArray productId;
	if (record.contains("Id")){
		productId = record.value("Id").toByteArray();
	}

	if (record.contains("Name")){
		QString productName = record.value("Name").toString();

		productInfoPtr->SetProductName(productName);
	}

	if (record.contains("Description")){
		QString productDescription = record.value("Description").toString();

		productInfoPtr->SetProductDescription(productDescription);
	}

	if (record.contains("Manufacturer")){
		QString productManufacturer = record.value("Manufacturer").toString();

		productInfoPtr->SetProductManufacturer(productManufacturer);
	}

	return productInfoPtr.PopPtr();
}


imtdb::IDatabaseObjectDelegate::NewObjectQuery CProductDatabaseDelegateComp::CreateNewObjectQuery(
		const QByteArray& /*typeId*/,
		const QByteArray& /*proposedObjectId*/,
		const QString& objectName,
		const QString& /*objectDescription*/,
		const istd::IChangeable* valuePtr) const
{
    const prolifedata::IProductInfo* productInfoPtr = dynamic_cast<const prolifedata::IProductInfo*>(valuePtr);
	if (productInfoPtr == nullptr){
		return NewObjectQuery();
	}

	QString productName = productInfoPtr->GetProductName();
	if (productName.isEmpty()){
		productName = objectName;
	}

	if (productName.isEmpty()){
		return NewObjectQuery();
	}

	QByteArray productId = productName.toUtf8();

	QString productDescription = productInfoPtr->GetProductDescription();

	QString productManufacturer = productInfoPtr->GetProductManufacturer();

	NewObjectQuery retVal;

	retVal.query = QString("INSERT INTO \"Products\" (Id, Name, Description,  Manufacturer) VALUES('%1', '%2', '%3', '%4');")
			.arg(qPrintable(productId))
			.arg(productName)
			.arg(productDescription)
			.arg(productManufacturer)
			.toLocal8Bit();

	retVal.objectName = productName;

	return retVal;
}


QByteArray CProductDatabaseDelegateComp::CreateDeleteObjectQuery(
			const imtbase::IObjectCollection& collection,
			const QByteArray& objectId) const
{
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
        const prolifedata::IProductInfo* productInfoPtr = dynamic_cast<const prolifedata::IProductInfo*>(objectPtr.GetPtr());
		if (productInfoPtr == nullptr){
			return QByteArray();
		}

		QByteArray retVal = QString("DELETE FROM \"Products\" WHERE Id = '%1';").arg(qPrintable(objectId)).toLocal8Bit();

		return retVal;
	}

	return QByteArray();
}


QByteArray CProductDatabaseDelegateComp::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& objectId,
			const istd::IChangeable& object) const
{
    const prolifedata::IProductInfo* productInfoPtr = dynamic_cast<const prolifedata::IProductInfo*>(&object);
	if (productInfoPtr == nullptr){
		return QByteArray();
	}

	QString productName = productInfoPtr->GetProductName();
	if (productName.isEmpty()){
		return QByteArray();
	}

	QByteArray productId = productName.toUtf8();

	QString productDescription = productInfoPtr->GetProductDescription();

	QString productManufacturer = productInfoPtr->GetProductManufacturer();

	QByteArray retVal = QString("UPDATE \"Products\" SET Id ='%1', Name = '%2', Description = '%3', Manufacturer = '%4' WHERE Id ='%5';")
				.arg(qPrintable(productId))
				.arg(productName)
				.arg(productDescription)
				.arg(productManufacturer)
				.arg(qPrintable(objectId))
				.toLocal8Bit();

	return retVal;
}


QByteArray CProductDatabaseDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const QString& newObjectName) const
{
    const prolifedata::IProductInfo* productInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
        productInfoPtr = dynamic_cast<const prolifedata::IProductInfo*>(objectPtr.GetPtr());
	}

	if (productInfoPtr == nullptr){
		return QByteArray();
	}

	if (objectId.isEmpty()){
		return QByteArray();
	}

	QByteArray newId = newObjectName.toUtf8();
	QByteArray retVal = QString("UPDATE \"Products\" SET Id = '%1', Name = '%2' WHERE Id = '%3';")
				.arg(qPrintable(newId))
				.arg(newObjectName)
				.arg(qPrintable(objectId))
				.toLocal8Bit();

	return retVal;
}


QByteArray CProductDatabaseDelegateComp::CreateDescriptionObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const QString& description) const
{
    const prolifedata::IProductInfo* productInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
        productInfoPtr = dynamic_cast<const prolifedata::IProductInfo*>(objectPtr.GetPtr());
	}

	if (productInfoPtr == nullptr){
		return QByteArray();
	}

	QByteArray retVal = QString("UPDATE \"Products\" SET Description = '%1' WHERE Id ='%2';").arg(description).arg(qPrintable(objectId)).toLocal8Bit();

	return retVal;
}


// protected methods

// reimplemented (imtdb::CSqlDatabaseObjectDelegateCompBase)

idoc::MetaInfoPtr CProductDatabaseDelegateComp::CreateObjectMetaInfo(const QByteArray& /*typeId*/) const
{
    return idoc::MetaInfoPtr(new imod::TModelWrap<prolifedata::CProductInfoMetaInfo>);
}


bool CProductDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("Name")){
		QString productName = record.value("Name").toString();

        metaInfo.SetMetaInfo(prolifedata::IProductInfo::MIT_PRODUCT_NAME, productName);
	}

	if (record.contains("Description")){
		QString productDescription = record.value("Description").toString();

        metaInfo.SetMetaInfo(prolifedata::IProductInfo::MIT_PRODUCT_DESCRIPTION, productDescription);
	}

	if (record.contains("Manufacturer")){
		QString productManufacturer = record.value("Manufacturer").toString();

        metaInfo.SetMetaInfo(prolifedata::IProductInfo::MIT_PRODUCT_MANUFACTURER, productManufacturer);
	}

	return true;
}


} // namespace prolifelicdb


