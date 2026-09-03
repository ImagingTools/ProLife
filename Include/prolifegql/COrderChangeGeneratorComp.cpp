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

	InsertChange(documentChangeCollection, "OrderId", QT_TRANSLATE_NOOP("Attribute", "Order-ID"), oldOrderInfoPtr->GetOrderId(), newOrderInfoPtr->GetOrderId());
	InsertChange(documentChangeCollection, "PurchaseId", QT_TRANSLATE_NOOP("Attribute", "Purchase Order-ID"), oldOrderInfoPtr->GetPurchaseOrderId(), newOrderInfoPtr->GetPurchaseOrderId());
	InsertChange(documentChangeCollection, "OrderCustomer", QT_TRANSLATE_NOOP("Attribute", "Order Customer"), oldOrderInfoPtr->GetCustomerId(), newOrderInfoPtr->GetCustomerId());
	InsertTextChange(documentChangeCollection, "Description", QT_TRANSLATE_NOOP("Attribute", "Description"), oldOrderInfoPtr->GetDescription(), newOrderInfoPtr->GetDescription());
	InsertEnumChange(
				documentChangeCollection,
				"Status",
				QT_TRANSLATE_NOOP("Attribute", "Status"),
				oldOrderInfoPtr->GetOrderStatus(),
				newOrderInfoPtr->GetOrderStatus(),
				oldOrderInfoPtr->OrderStatusGetStrings());

	CompareCustomerRoles(oldOrderInfoPtr->GetCustomerRoles(), newOrderInfoPtr->GetCustomerRoles(), documentChangeCollection);

	InsertListChanges(
				documentChangeCollection,
				"AddProduct",
				"RemoveProduct",
				"ProductId",
				QT_TRANSLATE_NOOP("Attribute", "Product-ID"),
				GetProductUuids(oldOrderInfoPtr->GetProducts()),
				GetProductUuids(newOrderInfoPtr->GetProducts()));

	return true;
}


void COrderChangeGeneratorComp::CompareCustomerRoles(
			const imtbase::IObjectCollection* oldRolesPtr,
			const imtbase::IObjectCollection* newRolesPtr,
			imtbase::CObjectCollection& documentChangeCollection)
{
	// Roles are identified by the role type / customer pair, the element ID of the entry is not stable.
	const CustomerRoles oldRoles = GetCustomerRoles(oldRolesPtr);
	const CustomerRoles newRoles = GetCustomerRoles(newRolesPtr);

	for (const CustomerRole& role : oldRoles){
		if (!newRoles.contains(role)){
			InsertOperationDescription(
						documentChangeCollection,
						"RemoveCustomerRole",
						role.first,
						QT_TRANSLATE_NOOP("Attribute", "Customer Role"),
						role.second,
						QByteArray());
		}
	}

	for (const CustomerRole& role : newRoles){
		if (!oldRoles.contains(role)){
			InsertOperationDescription(
						documentChangeCollection,
						"AddCustomerRole",
						role.first,
						QT_TRANSLATE_NOOP("Attribute", "Customer Role"),
						QByteArray(),
						role.second);
		}
	}
}


COrderChangeGeneratorComp::CustomerRoles COrderChangeGeneratorComp::GetCustomerRoles(const imtbase::IObjectCollection* rolesPtr)
{
	CustomerRoles retVal;

	if (rolesPtr == nullptr){
		return retVal;
	}

	const imtbase::ICollectionInfo::Ids roleIds = rolesPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& roleId : roleIds){
		imtbase::IObjectCollection::DataPtr roleDataPtr;
		if (!rolesPtr->GetObjectData(roleId, roleDataPtr)){
			continue;
		}

		const prolifedata::COrderCustomerRole* rolePtr = dynamic_cast<const prolifedata::COrderCustomerRole*>(roleDataPtr.GetPtr());
		if (rolePtr == nullptr){
			continue;
		}

		retVal << CustomerRole(prolifedata::GetIdFromCustomerRoleType(rolePtr->GetRoleType()), rolePtr->GetCustomerId());
	}

	return retVal;
}


QByteArrayList COrderChangeGeneratorComp::GetProductUuids(const imtbase::IObjectCollection* productCollectionPtr)
{
	QByteArrayList retVal;

	if (productCollectionPtr == nullptr){
		return retVal;
	}

	const imtbase::ICollectionInfo::Ids productIds = productCollectionPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& productId : productIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!productCollectionPtr->GetObjectData(productId, dataPtr)){
			continue;
		}

		const imtbase::CObjectLink* objectLinkPtr = dynamic_cast<const imtbase::CObjectLink*>(dataPtr.GetPtr());
		if (objectLinkPtr != nullptr){
			retVal << objectLinkPtr->GetObjectUuid();
		}
	}

	return retVal;
}


QString COrderChangeGeneratorComp::CreateCustomOperationDescription(
			const imtbase::COperationDescription& operationDescription,
			const QByteArray& languageId) const
{
	static const QByteArray translationContext = QByteArrayLiteral("prolifegql::COrderChangeGeneratorComp");

	const QByteArray type = operationDescription.GetOperationTypeId();
	const QByteArray oldValue = operationDescription.GetOldValue();
	const QByteArray newValue = operationDescription.GetNewValue();

	// For customer roles the key holds the role type, which is what the user has to see.
	const QString roleName = Translate(operationDescription.GetKey(), languageId, "Attribute").toHtmlEscaped();

	if (type == "AddProduct"){
		return Translate(QT_TR_NOOP("Added the product '%1'"), languageId, translationContext).arg(GetProductName(newValue).toHtmlEscaped());
	}

	if (type == "RemoveProduct"){
		return Translate(QT_TR_NOOP("Removed the product '%1'"), languageId, translationContext).arg(GetProductName(oldValue).toHtmlEscaped());
	}

	if (type == "AddCustomerRole"){
		return Translate(QT_TR_NOOP("Added customer role '%1' for customer '%2'"), languageId, translationContext).arg(roleName, GetAccountName(newValue).toHtmlEscaped());
	}

	if (type == "RemoveCustomerRole"){
		return Translate(QT_TR_NOOP("Removed customer role '%1' for customer '%2'"), languageId, translationContext).arg(roleName, GetAccountName(oldValue).toHtmlEscaped());
	}

	return QString();
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


// private methods

QString COrderChangeGeneratorComp::GetAccountName(const QByteArray& accountId) const
{
	if (!IsUuid(accountId) || !m_accountCollectionCompPtr.IsValid()){
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
	if (m_softwareCollectionCompPtr.IsValid() && m_softwareCollectionCompPtr->GetObjectData(productId, productDataPtr)){
		const imtlic::IProductInstanceInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
		if (productInfoPtr != nullptr){
			lisaProductId = productInfoPtr->GetProductId();
			productName = productInfoPtr->GetSerialNumber();
		}
	}
	else if (m_deviceCollectionCompPtr.IsValid() && m_deviceCollectionCompPtr->GetObjectData(productId, productDataPtr)){
		const prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::IDeviceInfo*>(productDataPtr.GetPtr());
		if (deviceInfoPtr != nullptr){
			lisaProductId = deviceInfoPtr->GetDeviceType();
			productName = deviceInfoPtr->GetMacAddress();
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

	// Without the product catalogue entry the serial number resp. MAC-address still identifies the product.
	if (lisaProductName.isEmpty()){
		return !productName.isEmpty() ? QString::fromUtf8(productName) : QString::fromUtf8(productId);
	}

	QString retVal = lisaProductName;
	if (!productName.isEmpty()){
		retVal =  retVal + " (" + productName + ")";
	}

	return retVal;
}


} // namespace prolifegql


