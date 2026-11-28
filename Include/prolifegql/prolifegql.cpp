#include <prolifegql/prolifegql.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Sensors.h>


// STL includes
#include <algorithm>

// Qt includes
#include <QtCore/QDate>
#include <QtCore/QMap>

// ImtCore includes
#include <imtauth/imtauth.h>
#include <imtauth/IUserInfo.h>
#include <imtaccount/IAddress.h>
#include <imtaccount/IAddressProvider.h>
#include <imtgql/IGqlContext.h>
#include <imtlic/ILicenseInstance.h>
#include <imtservergql/DocumentSharing.h>
#include <imtservergql/imtservergql.h>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/IOrdered.h>


namespace prolifegql
{


namespace
{


/**
	A field of a document, whether the request changes it and the permission which allows the change.
*/
struct FieldChange
{
	bool isChanged;
	QByteArray permissionId;
	QString fieldName;
};


const imtauth::IUserInfo* GetRequestUser(const imtgql::CGqlRequest& gqlRequest)
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();

	return gqlContextPtr != nullptr ? gqlContextPtr->GetUserInfo() : nullptr;
}


/**
	The permissions decide like the command check: the user's own, scoped to the product and the organization,
	together with the scope an owner holds for the organization.
*/
bool HoldsPermissionInContext(const imtgql::IGqlContext& gqlContext, const QByteArray& permissionId)
{
	const QByteArrayList heldPermissions = imtauth::CollectHeldPermissions(
				imtservergql::GetUserPermissions(gqlContext),
				gqlContext.GetOrganizationPermissions(),
				{permissionId});

	return imtauth::HasPermission(heldPermissions, permissionId);
}


bool CheckFieldChanges(
			const imtgql::CGqlRequest& gqlRequest,
			const QList<FieldChange>& changes,
			const QString& documentName,
			QString& errorMessage)
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	const imtauth::IUserInfo* userInfoPtr = GetRequestUser(gqlRequest);
	if (gqlContextPtr == nullptr || userInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to update the %1. Error: User is unknown").arg(documentName);

		return false;
	}

	if (userInfoPtr->IsAdmin()){
		return true;
	}

	QStringList deniedFields;
	for (const FieldChange& change : changes){
		if (change.isChanged && !HoldsPermissionInContext(*gqlContextPtr, change.permissionId) && !deniedFields.contains(change.fieldName)){
			deniedFields << change.fieldName;
		}
	}

	if (!deniedFields.isEmpty()){
		errorMessage = QStringLiteral("No permission to change: %1").arg(deniedFields.join(QStringLiteral(", ")));

		return false;
	}

	return true;
}


QByteArray GetOrderId(const istd::IPolymorphic& object)
{
	const prolifedata::IOrdered* orderedPtr = dynamic_cast<const prolifedata::IOrdered*>(&object);

	return orderedPtr != nullptr ? orderedPtr->GetOrderId() : QByteArray();
}


/**
	The customers of an order with their roles, sorted. An order stored before the roles existed has only its customer,
	which is its ordering party.
*/
QByteArrayList GetCustomerRoleKeys(const prolifedata::IOrderInfo& order)
{
	QByteArrayList retVal;

	const imtbase::IObjectCollection* rolesPtr = order.GetCustomerRoles();
	if (rolesPtr != nullptr){
		const imtbase::ICollectionInfo::Ids roleIds = rolesPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& roleId : roleIds){
			imtbase::IObjectCollection::DataPtr roleDataPtr;
			if (rolesPtr->GetObjectData(roleId, roleDataPtr)){
				const prolifedata::IOrderCustomerRole* rolePtr = dynamic_cast<const prolifedata::IOrderCustomerRole*>(roleDataPtr.GetPtr());
				if (rolePtr != nullptr){
					retVal << rolePtr->GetCustomerId() + ':' + prolifedata::GetIdFromCustomerRoleType(rolePtr->GetRoleType());
				}
			}
		}
	}

	if (retVal.isEmpty() && !order.GetCustomerId().isEmpty()){
		retVal << order.GetCustomerId() + ':' + prolifedata::GetIdFromCustomerRoleType(prolifedata::IOrderCustomerRole::RT_ORDERING_PARTY);
	}

	std::sort(retVal.begin(), retVal.end());

	return retVal;
}


/**
	The license instances of a license with their expiration dates, sorted. A license without expiration has an empty date.
*/
QMap<QByteArray, QDate> GetLicenseExpirations(const imtlic::IProductInstanceInfo& license)
{
	QMap<QByteArray, QDate> retVal;

	const imtbase::ICollectionInfo::Ids licenseIds = license.GetLicenseInstances().GetElementIds();
	for (const imtbase::ICollectionInfo::Id& licenseId : licenseIds){
		const imtlic::ILicenseInstance* licenseInstancePtr = license.GetLicenseInstance(licenseId);
		retVal.insert(licenseId, licenseInstancePtr != nullptr ? licenseInstancePtr->GetExpiration().date() : QDate());
	}

	return retVal;
}


/**
	The addresses of a company, sorted; an address without any value counts as none.
*/
QStringList GetAddressKeys(const imtaccount::ICompanyInfo& company)
{
	QStringList retVal;

	const imtaccount::IAddressProvider* addressesPtr = company.GetAddresses();
	if (addressesPtr == nullptr){
		return retVal;
	}

	const imtbase::ICollectionInfo::Ids addressIds = addressesPtr->GetAddressList().GetElementIds();
	for (const imtbase::ICollectionInfo::Id& addressId : addressIds){
		const imtaccount::IAddress* addressPtr = addressesPtr->GetAddress(addressId);
		if (addressPtr == nullptr){
			continue;
		}

		const QString postalCode = addressPtr->GetPostalCode() != 0 ? QString::number(addressPtr->GetPostalCode()) : QString();
		const QStringList values{addressPtr->GetCountry(), addressPtr->GetCity(), postalCode, addressPtr->GetStreet()};
		if (!values.join(QString()).isEmpty()){
			retVal << values.join(QChar('\n'));
		}
	}

	retVal.sort();

	return retVal;
}


QByteArrayList Sorted(QByteArrayList list)
{
	list.removeAll(QByteArray());
	std::sort(list.begin(), list.end());

	return list;
}


} // namespace


bool HoldsPermission(const imtgql::CGqlRequest& gqlRequest, const QByteArray& permissionId)
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	const imtauth::IUserInfo* userInfoPtr = GetRequestUser(gqlRequest);
	if (gqlContextPtr == nullptr || userInfoPtr == nullptr){
		return false;
	}

	return userInfoPtr->IsAdmin() || HoldsPermissionInContext(*gqlContextPtr, permissionId);
}


bool CheckDeviceObjectChangePermissions(
			const prolifedata::IDeviceInfo& oldDevice,
			const prolifedata::IDeviceInfo& newDevice,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage)
{
	const QList<FieldChange> changes{
		{oldDevice.GetMacAddress() != newDevice.GetMacAddress(), "/SensorManagement/Edit/Change/MacAddress", QStringLiteral("MAC-Address")},
		{oldDevice.GetSerialNumber() != newDevice.GetSerialNumber(), "/SensorManagement/Edit/Change/SerialNumber", QStringLiteral("Serial Number")},
		{oldDevice.GetConfigurationType() != newDevice.GetConfigurationType(), "/SensorManagement/Edit/Change/HardwareConfiguration", QStringLiteral("Hardware Configuration")},
		{oldDevice.GetDeviceType() != newDevice.GetDeviceType(), "/SensorManagement/Edit/Change/DeviceType", QStringLiteral("Device Type")},
		{oldDevice.GetProject() != newDevice.GetProject(), "/SensorManagement/Edit/Change/Project", QStringLiteral("Project")},
		{GetOrderId(oldDevice) != GetOrderId(newDevice), "/SensorManagement/Edit/Change/Order", QStringLiteral("Order")},
		{oldDevice.GetDescription() != newDevice.GetDescription(), "/SensorManagement/Edit/Change/Description", QStringLiteral("Description")},
		{oldDevice.GetDeviceProductionStatus() != newDevice.GetDeviceProductionStatus(), "/SensorManagement/Edit/Change/ProductionStatus", QStringLiteral("Production Status")}};

	return CheckFieldChanges(gqlRequest, changes, QStringLiteral("hardware"), errorMessage);
}


bool CheckOrderChangePermissions(
			const prolifedata::IOrderInfo& oldOrder,
			const prolifedata::IOrderInfo& newOrder,
			bool productsChanged,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage)
{
	// Legacy states are stored as Created, so the states are compared as the client sees them
	const QList<FieldChange> changes{
		{oldOrder.GetOrderId() != newOrder.GetOrderId(), "/OrderManagement/Edit/Change/DeliveryId", QStringLiteral("Delivery-ID")},
		{oldOrder.GetPurchaseOrderId() != newOrder.GetPurchaseOrderId(), "/OrderManagement/Edit/Change/PurchaseOrderId", QStringLiteral("Purchase Order-ID")},
		{oldOrder.GetDescription() != newOrder.GetDescription(), "/OrderManagement/Edit/Change/Description", QStringLiteral("Description")},
		{GetCustomerRoleKeys(oldOrder) != GetCustomerRoleKeys(newOrder), "/OrderManagement/Edit/Change/Customer", QStringLiteral("Customer")},
		{prolifedata::GetIdFromOrderStatus(oldOrder.GetOrderStatus()) != prolifedata::GetIdFromOrderStatus(newOrder.GetOrderStatus()), "/OrderManagement/Edit/Change/Status", QStringLiteral("Status")},
		{productsChanged, "/OrderManagement/Edit/Change/Products", QStringLiteral("Products")}};

	return CheckFieldChanges(gqlRequest, changes, QStringLiteral("order"), errorMessage);
}


bool CheckSoftwareChangePermissions(
			const imtlic::IProductInstanceInfo& oldLicense,
			const imtlic::IProductInstanceInfo& newLicense,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage)
{
	const QMap<QByteArray, QDate> oldExpirations = GetLicenseExpirations(oldLicense);
	const QMap<QByteArray, QDate> newExpirations = GetLicenseExpirations(newLicense);

	bool isExpirationChanged = false;
	for (auto iter = newExpirations.cbegin(); iter != newExpirations.cend(); ++iter){
		if (oldExpirations.contains(iter.key()) && oldExpirations.value(iter.key()) != iter.value()){
			isExpirationChanged = true;
		}
	}

	const bool areLicensesChanged =
				oldExpirations.keys() != newExpirations.keys() ||
				oldLicense.IsMultiProduct() != newLicense.IsMultiProduct() ||
				oldLicense.GetProductCount() != newLicense.GetProductCount();

	// The customer of a license follows its order
	const bool isOrderChanged =
				GetOrderId(oldLicense) != GetOrderId(newLicense) ||
				oldLicense.GetCustomerId() != newLicense.GetCustomerId();

	const QList<FieldChange> changes{
		{oldLicense.GetSerialNumber() != newLicense.GetSerialNumber(), "/LicenseManagement/Edit/Change/Number", QStringLiteral("Number")},
		{isExpirationChanged, "/LicenseManagement/Edit/Change/Expiration", QStringLiteral("Expiration")},
		{areLicensesChanged, "/LicenseManagement/Edit/Change/ProductLicenses", QStringLiteral("Product Licenses")},
		{oldLicense.GetProductId() != newLicense.GetProductId(), "/LicenseManagement/Edit/Change/Product", QStringLiteral("Product")},
		{isOrderChanged, "/LicenseManagement/Edit/Change/Order", QStringLiteral("Order")},
		{oldLicense.GetProject() != newLicense.GetProject(), "/LicenseManagement/Edit/Change/Project", QStringLiteral("Project")}};

	return CheckFieldChanges(gqlRequest, changes, QStringLiteral("license"), errorMessage);
}


bool CheckAccountChangePermissions(
			const prolifedata::ICustomerInfo& oldAccount,
			const prolifedata::ICustomerInfo& newAccount,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage)
{
	// The editor offers the website and the company roles together with the name
	const bool isNameChanged =
				oldAccount.GetName() != newAccount.GetName() ||
				oldAccount.GetWebsite() != newAccount.GetWebsite() ||
				oldAccount.IsManufacturer() != newAccount.IsManufacturer() ||
				oldAccount.IsSupplier() != newAccount.IsSupplier();

	const QList<FieldChange> changes{
		{isNameChanged, "/AccountManagement/Edit/Change/Name", QStringLiteral("Name")},
		{oldAccount.GetDescription() != newAccount.GetDescription(), "/AccountManagement/Edit/Change/Description", QStringLiteral("Description")},
		{oldAccount.GetEmail() != newAccount.GetEmail(), "/AccountManagement/Edit/Change/Email", QStringLiteral("Email")},
		{GetAddressKeys(oldAccount) != GetAddressKeys(newAccount), "/AccountManagement/Edit/Change/CompanyAddress", QStringLiteral("Company Address")},
		{oldAccount.GetCustomerId() != newAccount.GetCustomerId(), "/AccountManagement/Edit/Change/CustomerId", QStringLiteral("Customer-ID")},
		{Sorted(oldAccount.GetGroups()) != Sorted(newAccount.GetGroups()), "/AccountManagement/Edit/Change/Groups", QStringLiteral("Groups")}};

	return CheckFieldChanges(gqlRequest, changes, QStringLiteral("account"), errorMessage);
}


bool CheckOrderReference(
			const imtbase::IObjectCollection& orderCollection,
			const imtauth::IDocumentShareManager* shareManagerPtr,
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& currentOrderId,
			const QByteArray& newOrderId,
			QString& errorMessage)
{
	if (newOrderId.isEmpty() || newOrderId == currentOrderId){
		return true;
	}

	if (imtservergql::HasObjectAccess(orderCollection, shareManagerPtr, gqlRequest, {newOrderId}, imtauth::IDocumentShareInfo::DAL_EDIT)){
		return true;
	}

	errorMessage = QStringLiteral("Order does not exist or the organization may not change it");

	return false;
}


bool CheckDeviceChangePermissions(
			const prolifedata::IDeviceInfo& device,
			const QByteArray& orderId,
			const sdl::V1_0::prolife::CDeviceData& deviceData,
			const imtgql::CGqlRequest& gqlRequest,
			bool isNewDevice,
			QString& errorMessage)
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr != nullptr ? gqlContextPtr->GetUserInfo() : nullptr;
	if (userInfoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to update the hardware. Error: User is unknown");
		return false;
	}

	if (userInfoPtr->IsAdmin()){
		return true;
	}

	if (isNewDevice){
		if (HoldsPermissionInContext(*gqlContextPtr, QByteArrayLiteral("/SensorManagement/Edit/Add"))){
			return true;
		}

		errorMessage = QStringLiteral("No permission to create new hardware");
		return false;
	}

	QStringList deniedFields;
	auto checkField = [gqlContextPtr, &deniedFields](bool isChanged, const QByteArray& permissionId, const QString& fieldName){
		if (isChanged && !HoldsPermissionInContext(*gqlContextPtr, permissionId)){
			deniedFields << fieldName;
		}
	};

	const QByteArray macAddress = deviceData.macAddress ? deviceData.macAddress->toUtf8() : QByteArray();
	checkField(macAddress != device.GetMacAddress(), "/SensorManagement/Edit/Change/MacAddress", QStringLiteral("MAC-Address"));

	const QByteArray serialNumber = deviceData.serialNumber ? deviceData.serialNumber->toUtf8() : QByteArray();
	checkField(serialNumber != device.GetSerialNumber(), "/SensorManagement/Edit/Change/SerialNumber", QStringLiteral("Serial Number"));

	const QByteArray configurationType = deviceData.licenseName ? deviceData.licenseName->toUtf8() : QByteArray();
	checkField(configurationType != device.GetConfigurationType(), "/SensorManagement/Edit/Change/HardwareConfiguration", QStringLiteral("Hardware Configuration"));

	const QByteArray deviceType = deviceData.deviceType ? deviceData.deviceType->toUtf8() : QByteArray();
	checkField(deviceType != device.GetDeviceType(), "/SensorManagement/Edit/Change/DeviceType", QStringLiteral("Device Type"));

	if (deviceData.project){
		checkField(deviceData.project->toUtf8() != device.GetProject(), "/SensorManagement/Edit/Change/Project", QStringLiteral("Project"));
	}

	if (deviceData.orderId){
		checkField(deviceData.orderId->toUtf8() != orderId, "/SensorManagement/Edit/Change/Order", QStringLiteral("Order"));
	}

	if (deviceData.description){
		checkField(*deviceData.description != device.GetDescription(), "/SensorManagement/Edit/Change/Description", QStringLiteral("Description"));
	}

	if (deviceData.productionStatus){
		const prolifedata::IDeviceInfo::DeviceProductionStatus status = prolifedata::GetProductionStatusFromId(deviceData.productionStatus->toUtf8());
		checkField(status != device.GetDeviceProductionStatus(), "/SensorManagement/Edit/Change/ProductionStatus", QStringLiteral("Production Status"));
	}

	if (!deniedFields.isEmpty()){
		errorMessage = QStringLiteral("No permission to change: %1").arg(deniedFields.join(QStringLiteral(", ")));
		return false;
	}

	return true;
}


} // namespace prolifegql


