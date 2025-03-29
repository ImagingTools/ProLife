#include <prolifegql/COrderChangeGeneratorComp.h>


// ImtCore includes
#include <imtbase/CObjectLink.h>
#include <iqt/iqt.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/ILicenseDefinition.h>
#include <imtlic/CHardwareInstanceInfo.h>
#include <imtauth/CCompanyInfo.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/IDeviceInfo.h>


namespace prolifegql
{


// protected methods

bool COrderChangeGeneratorComp::CompareDocuments(
				const istd::IChangeable& oldDocument,
				const istd::IChangeable& newDocument,
				imtbase::CObjectCollection& documentChangeCollection,
				QString& errorMessage)
{
	prolifedata::CIdentifiableOrderInfo* oldOrderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(const_cast<istd::IChangeable*>(&oldDocument));
	if (oldOrderInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. Old document is invalid");
		return false;
	}

	prolifedata::CIdentifiableOrderInfo* newOrderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(const_cast<istd::IChangeable*>(&newDocument));
	if (newOrderInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. New document is invalid");
		return false;
	}

	QByteArray oldOrderId = oldOrderInfoPtr->GetOrderId();
	QByteArray newOrderId = newOrderInfoPtr->GetOrderId();
	if (oldOrderId != newOrderId){
		InsertOperationDescription(documentChangeCollection, "", "OrderId", QT_TRANSLATE_NOOP("Attribute", "Order-ID"), oldOrderId, newOrderId);
	}

	QByteArray oldPurchaseOrderId = oldOrderInfoPtr->GetPurchaseOrderId();
	QByteArray newPurchaseOrderId = newOrderInfoPtr->GetPurchaseOrderId();
	if (oldPurchaseOrderId != newPurchaseOrderId){
		InsertOperationDescription(documentChangeCollection, "", "PurchaseId", QT_TRANSLATE_NOOP("Attribute", "Purchase Order-ID"), oldPurchaseOrderId, newPurchaseOrderId);
	}

	QByteArray oldCustomerId = oldOrderInfoPtr->GetCustomerId();
	QByteArray newCustomerId = newOrderInfoPtr->GetCustomerId();
	if (oldCustomerId != newCustomerId){
		InsertOperationDescription(documentChangeCollection, "", "OrderCustomer", QT_TRANSLATE_NOOP("Attribute", "Order Customer"), oldCustomerId, newCustomerId);
	}

	prolifedata::IOrderInfo::OrderStatus oldStatus = oldOrderInfoPtr->GetOrderStatus();
	prolifedata::IOrderInfo::OrderStatus newStatus = newOrderInfoPtr->GetOrderStatus();
	if (oldStatus != newStatus){
		QStringList statuses = oldOrderInfoPtr->OrderStatusGetStrings();
		InsertOperationDescription(documentChangeCollection, "", "Status", QT_TRANSLATE_NOOP("Attribute", "Status"), statuses[oldStatus].toUtf8(), statuses[newStatus].toUtf8());
	}

	QString oldDescription = oldOrderInfoPtr->GetDescription();
	QString newDescription = newOrderInfoPtr->GetDescription();
	if (oldDescription != newDescription){
		InsertOperationDescription(documentChangeCollection, "", "Description", QT_TRANSLATE_NOOP("Attribute", "Description"), oldDescription.toUtf8(), newDescription.toUtf8());
	}

	QByteArrayList addedProducts;
	QByteArrayList removedProducts;
	QByteArrayList updatedProducts;

	imtbase::IObjectCollection* oldProductCollectionPtr = oldOrderInfoPtr->GetProducts();
	imtbase::IObjectCollection* newProductCollectionPtr = newOrderInfoPtr->GetProducts();

	if (oldProductCollectionPtr != nullptr && newProductCollectionPtr != nullptr){
		GenerateDifferences(*oldProductCollectionPtr, *newProductCollectionPtr, addedProducts, removedProducts, updatedProducts);
	}

	for (const QByteArray& productObjectId : std::as_const(addedProducts)){
		InsertOperationDescription(documentChangeCollection, "AddProduct", "ProductId", "Product-ID", "", productObjectId);
	}

	for (const QByteArray& productObjectId : std::as_const(removedProducts)){
		InsertOperationDescription(documentChangeCollection, "RemoveProduct", "ProductId", "Product-ID", productObjectId, "");
	}

	return true;
}


QString COrderChangeGeneratorComp::CreateCustomOperationDescription(
	const imtbase::COperationDescription& operationDescription,
	const QByteArray& languageId) const
{
	QString retVal;

	QByteArray type = operationDescription.GetOperationTypeId();
	QByteArray oldValue = operationDescription.GetOldValue();
	QByteArray newValue = operationDescription.GetNewValue();
	QString keyName = operationDescription.GetKeyName();
	keyName = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), keyName.toUtf8(), languageId, "Attribute");

	if (type == "AddProduct"){
		QString change = iqt::GetTranslation(
			m_translationManagerCompPtr.GetPtr(),
			QString(QT_TR_NOOP("Added the product '%1'")).toUtf8(),
			languageId,
			"prolifegql::COrderChangeGeneratorComp");
		
		change = change.arg(GetProductName(newValue));

		retVal += change + "\n";
	}
	else if (type == "RemoveProduct"){
		QString change = iqt::GetTranslation(
			m_translationManagerCompPtr.GetPtr(),
			QString(QT_TR_NOOP("Removed the product '%1'")).toUtf8(),
			languageId,
			"prolifegql::COrderChangeGeneratorComp");

		change = change.arg(GetProductName(oldValue));

		retVal += change + "\n";
	}

	return retVal;
}


QString COrderChangeGeneratorComp::GetKeyNameForOperation(const QByteArray& key, const QByteArray& value) const
{
	if (key == "OrderCustomer"){
		return GetAccountName(value);
	}
	else{
		return BaseClass::GetKeyNameForOperation(key, value);
	}
}


void COrderChangeGeneratorComp::GenerateDifferences(
			imtbase::IObjectCollection& prevOrderProducts,
			imtbase::IObjectCollection& currentOrderProducts,
			QByteArrayList& addProducts,
			QByteArrayList& removedProducts,
			QByteArrayList& /*updatedProducts*/) const
{
	imtbase::ICollectionInfo::Ids prevProductIds = prevOrderProducts.GetElementIds();
	imtbase::ICollectionInfo::Ids currentProductIds = currentOrderProducts.GetElementIds();

	QByteArrayList oldProductUuids;
	QByteArrayList newProductUuids;

	for (const imtbase::ICollectionInfo::Id& productId : currentProductIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (currentOrderProducts.GetObjectData(productId, dataPtr)){
			const imtbase::CObjectLink* objectLinkPtr = dynamic_cast<const imtbase::CObjectLink*>(dataPtr.GetPtr());
			if (objectLinkPtr != nullptr){
				newProductUuids << objectLinkPtr->GetObjectUuid();
			}
		}
	}

	for (const imtbase::ICollectionInfo::Id& productId : prevProductIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (prevOrderProducts.GetObjectData(productId, dataPtr)){
			const imtbase::CObjectLink* objectLinkPtr = dynamic_cast<const imtbase::CObjectLink*>(dataPtr.GetPtr());
			if (objectLinkPtr != nullptr){
				oldProductUuids << objectLinkPtr->GetObjectUuid();
			}
		}
	}

	for (const imtbase::ICollectionInfo::Id& productId : newProductUuids){
		if (!oldProductUuids.contains(productId)){
			addProducts << productId;
		}
	}

	for (const imtbase::ICollectionInfo::Id& productId : oldProductUuids){
		if (!newProductUuids.contains(productId)){
			removedProducts << productId;
		}
	}
}


// private methods

QString COrderChangeGeneratorComp::GetAccountName(const QByteArray& accountId) const
{
	imtbase::IObjectCollection::DataPtr newDataPtr;
	if (m_accountCollectionCompPtr->GetObjectData(accountId, newDataPtr)){
		const imtauth::CIdentifiableCompanyInfo* companyInfoPtr = dynamic_cast<const imtauth::CIdentifiableCompanyInfo*>(newDataPtr.GetPtr());
		if (companyInfoPtr != nullptr){
			return companyInfoPtr->GetName();
		}
	}

	return accountId;
}


QString COrderChangeGeneratorComp::GetProductName(const QByteArray& productId) const
{
	QUuid uuid = QUuid::fromRfc4122(productId);
	if (uuid.isNull()){
		return productId;
	}
	
	QByteArray lisaProductId;
	QByteArray productName;

	imtbase::IObjectCollection::DataPtr productDataPtr;
	if (m_softwareCollectionCompPtr->GetObjectData(productId, productDataPtr)){
		const imtlic::IProductInstanceInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
		if (productInfoPtr != nullptr){
			lisaProductId = productInfoPtr->GetProductId();

			productName = productInfoPtr->GetSerialNumber();
		}
	}
	else{
		if (m_deviceCollectionCompPtr->GetObjectData(productId, productDataPtr)){
			const prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::IDeviceInfo*>(productDataPtr.GetPtr());
			if (deviceInfoPtr != nullptr){
				lisaProductId = deviceInfoPtr->GetDeviceType();

				productName = deviceInfoPtr->GetMacAddress();
			}
		}
	}

	QString lisaProductName;

	if (m_productCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_productCollectionCompPtr->GetObjectData(lisaProductId, dataPtr)){
			const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(dataPtr.GetPtr());
			if (productInfoPtr != nullptr){
				lisaProductName = productInfoPtr->GetName().toUtf8();
			}
		}
	}

	if (!lisaProductName.isEmpty() && !productName.isEmpty()){
		return lisaProductName + " (" + productName + ")";
	}


	return productId;
}


} // namespace prolifegql


