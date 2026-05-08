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
#include <prolifedata/COrderCustomerRole.h>
#include <prolifedata/prolifedata.h>


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

	// Track customer role changes
	const imtbase::IObjectCollection* oldRolesPtr = oldOrderInfoPtr->GetCustomerRoles();
	const imtbase::IObjectCollection* newRolesPtr = newOrderInfoPtr->GetCustomerRoles();

	if (oldRolesPtr != nullptr && newRolesPtr != nullptr){
		imtbase::ICollectionInfo::Ids oldRoleIds = oldRolesPtr->GetElementIds();
		imtbase::ICollectionInfo::Ids newRoleIds = newRolesPtr->GetElementIds();

		// Detect removed roles (present in old, absent in new by roleType+customerId pair)
		for (const imtbase::ICollectionInfo::Id& oldRoleId : oldRoleIds){
			imtbase::IObjectCollection::DataPtr oldRoleData;
			if (oldRolesPtr->GetObjectData(oldRoleId, oldRoleData)){
				const prolifedata::COrderCustomerRole* oldRole = dynamic_cast<const prolifedata::COrderCustomerRole*>(oldRoleData.GetPtr());
				if (oldRole == nullptr){
					continue;
				}

				bool found = false;
				for (const imtbase::ICollectionInfo::Id& newRoleId : newRoleIds){
					imtbase::IObjectCollection::DataPtr newRoleData;
					if (newRolesPtr->GetObjectData(newRoleId, newRoleData)){
						const prolifedata::COrderCustomerRole* newRole = dynamic_cast<const prolifedata::COrderCustomerRole*>(newRoleData.GetPtr());
						if (newRole != nullptr &&
							newRole->GetRoleType() == oldRole->GetRoleType() &&
							newRole->GetCustomerId() == oldRole->GetCustomerId()){
							found = true;
							break;
						}
					}
				}

				if (!found){
					QByteArray roleTypeId = prolifedata::GetIdFromCustomerRoleType(oldRole->GetRoleType());
					InsertOperationDescription(documentChangeCollection, "RemoveCustomerRole", roleTypeId, QT_TRANSLATE_NOOP("Attribute", "Customer Role"), oldRole->GetCustomerId(), "");
				}
			}
		}

		// Detect added roles (present in new, absent in old by roleType+customerId pair)
		for (const imtbase::ICollectionInfo::Id& newRoleId : newRoleIds){
			imtbase::IObjectCollection::DataPtr newRoleData;
			if (newRolesPtr->GetObjectData(newRoleId, newRoleData)){
				const prolifedata::COrderCustomerRole* newRole = dynamic_cast<const prolifedata::COrderCustomerRole*>(newRoleData.GetPtr());
				if (newRole == nullptr){
					continue;
				}

				bool found = false;
				for (const imtbase::ICollectionInfo::Id& oldRoleId : oldRoleIds){
					imtbase::IObjectCollection::DataPtr oldRoleData;
					if (oldRolesPtr->GetObjectData(oldRoleId, oldRoleData)){
						const prolifedata::COrderCustomerRole* oldRole = dynamic_cast<const prolifedata::COrderCustomerRole*>(oldRoleData.GetPtr());
						if (oldRole != nullptr &&
							oldRole->GetRoleType() == newRole->GetRoleType() &&
							oldRole->GetCustomerId() == newRole->GetCustomerId()){
							found = true;
							break;
						}
					}
				}

				if (!found){
					QByteArray roleTypeId = prolifedata::GetIdFromCustomerRoleType(newRole->GetRoleType());
					InsertOperationDescription(documentChangeCollection, "AddCustomerRole", roleTypeId, QT_TRANSLATE_NOOP("Attribute", "Customer Role"), "", newRole->GetCustomerId());
				}
			}
		}
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
	else if (type == "AddCustomerRole"){
		QString change = iqt::GetTranslation(
			m_translationManagerCompPtr.GetPtr(),
			QString(QT_TR_NOOP("Added customer role '%1' for customer '%2'")).toUtf8(),
			languageId,
			"prolifegql::COrderChangeGeneratorComp");

		change = change.arg(QString::fromUtf8(keyName.toUtf8()), GetAccountName(newValue));

		retVal += change + "\n";
	}
	else if (type == "RemoveCustomerRole"){
		QString change = iqt::GetTranslation(
			m_translationManagerCompPtr.GetPtr(),
			QString(QT_TR_NOOP("Removed customer role '%1' for customer '%2'")).toUtf8(),
			languageId,
			"prolifegql::COrderChangeGeneratorComp");

		change = change.arg(QString::fromUtf8(keyName.toUtf8()), GetAccountName(oldValue));

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
	if (!IsUuid(accountId)){
		return accountId;
	}

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
	if (!IsUuid(productId)){
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

	if (!productDataPtr.IsValid()){
		return productId;
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

	if (lisaProductName.isEmpty()){
		return productId;
	}

	QString retVal = lisaProductName;
	if (!productName.isEmpty()){
		retVal =  retVal + " (" + productName + ")";
	}

	return retVal;
}


} // namespace prolifegql


