#include <prolifelicdb/CProductDatabaseDelegateComp.h>

// ACF includes
#include <imod/TModelWrap.h>

#include <prolifeauth/CProductInfoMetaInfo.h>


namespace prolifelicdb
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

	istd::TDelPtr<prolifeauth::IProductInfo> productInfoPtr = m_productInfoFactCompPtr.CreateInstance();
	if (!productInfoPtr.IsValid()){
		return nullptr;
	}

	QByteArray ProductId;
	if (record.contains("Id")){
		ProductId = record.value("Id").toByteArray();
	}

	if (record.contains("Name")){
		QString ProductName = record.value("Name").toString();

		productInfoPtr->SetProductName(ProductName);
	}

	if (record.contains("Description")){
		QString ProductDescription = record.value("Description").toString();

		productInfoPtr->SetProductDescription(ProductDescription);
	}

	if (record.contains("Manufacturer")){
		QString ProductManufacturer = record.value("Manufacturer").toString();

		productInfoPtr->SetProductManufacturer(ProductManufacturer);
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
	const prolifeauth::IProductInfo* productInfoPtr = dynamic_cast<const prolifeauth::IProductInfo*>(valuePtr);
	if (productInfoPtr == nullptr){
		return NewObjectQuery();
	}

	QString ProductName = productInfoPtr->GetProductName();
	if (ProductName.isEmpty()){
		ProductName = objectName;
	}

	if (ProductName.isEmpty()){
		return NewObjectQuery();
	}

	QByteArray ProductId = ProductName.toUtf8();

	QString ProductDescription = productInfoPtr->GetProductDescription();

	QString ProductManufacturer = productInfoPtr->GetProductManufacturer();

	NewObjectQuery retVal;

	retVal.query = QString("INSERT INTO \"Products\" (Id, Name, Description,  Manufacturer) VALUES('%1', '%2', '%3', '%4');")
			.arg(qPrintable(ProductId))
			.arg(ProductName)
			.arg(ProductDescription)
			.arg(ProductManufacturer)
			.toLocal8Bit();

	retVal.objectName = ProductName;

	return retVal;
}


QByteArray CProductDatabaseDelegateComp::CreateDeleteObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId) const
{
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		const prolifeauth::IProductInfo* productInfoPtr = dynamic_cast<const prolifeauth::IProductInfo*>(objectPtr.GetPtr());
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
	const prolifeauth::IProductInfo* productInfoPtr = dynamic_cast<const prolifeauth::IProductInfo*>(&object);
	if (productInfoPtr == nullptr){
		return QByteArray();
	}

	QString ProductName = productInfoPtr->GetProductName();

	if (ProductName.isEmpty()){
		return QByteArray();
	}

	QByteArray ProductId = ProductName.toUtf8();

	QString ProductDescription = productInfoPtr->GetProductDescription();

	QString ProductManufacturer = productInfoPtr->GetProductManufacturer();

	QByteArray retVal = QString("UPDATE \"Products\" SET Id ='%1', Name = '%2', Description = '%3', Manufacturer = '%4' WHERE Id ='%5';")
			.arg(qPrintable(ProductId))
			.arg(ProductName)
			.arg(ProductDescription)
			.arg(ProductManufacturer)
			.arg(qPrintable(objectId))
			.toLocal8Bit();

	return retVal;
}


QByteArray CProductDatabaseDelegateComp::CreateRenameObjectQuery(
		const imtbase::IObjectCollection& collection,
		const QByteArray& objectId,
		const QString& newObjectName) const
{
	const prolifeauth::IProductInfo* productInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		productInfoPtr = dynamic_cast<const prolifeauth::IProductInfo*>(objectPtr.GetPtr());
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
	const prolifeauth::IProductInfo* productInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (collection.GetObjectData(objectId, objectPtr)){
		productInfoPtr = dynamic_cast<const prolifeauth::IProductInfo*>(objectPtr.GetPtr());
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
	return idoc::MetaInfoPtr(new imod::TModelWrap<prolifeauth::CProductInfoMetaInfo>);
}


bool CProductDatabaseDelegateComp::SetObjectMetaInfoFromRecord(const QSqlRecord& record, idoc::IDocumentMetaInfo& metaInfo) const
{
	if (record.contains("Name")){
		QString ProductName = record.value("Name").toString();

		metaInfo.SetMetaInfo(prolifeauth::IProductInfo::MIT_PRODUCT_NAME, ProductName);
	}

	if (record.contains("Description")){
		QString ProductDescription = record.value("Description").toString();

		metaInfo.SetMetaInfo(prolifeauth::IProductInfo::MIT_PRODUCT_DESCRIPTION, ProductDescription);
	}

	if (record.contains("Manufacturer")){
		QString ProductManufacturer = record.value("Manufacturer").toString();

		metaInfo.SetMetaInfo(prolifeauth::IProductInfo::MIT_PRODUCT_MANUFACTURER, ProductManufacturer);
	}



	return true;
}


}// namespace prolifelicdb






