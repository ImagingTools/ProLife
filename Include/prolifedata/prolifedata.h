#pragma once


// Qt includes
#include <QString>

// ImtCore includes
#include <imtbase/IObjectCollection.h>

// ProLife includes
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/IOrderInfo.h>
#include <prolifedata/IOrderCustomerRole.h>

// Generated includes
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Licenses_fwd.h>


namespace prolifedata
{


QString GetNameFromDeviceProductionStatus(prolifedata::IDeviceInfo::DeviceProductionStatus status);
QByteArray GetIdFromDeviceProductionStatus(prolifedata::IDeviceInfo::DeviceProductionStatus status);
prolifedata::IDeviceInfo::DeviceProductionStatus GetProductionStatusFromId(const QByteArray& statusId);

QString GetNameFromOrderStatus(prolifedata::IOrderInfo::OrderStatus status);
QByteArray GetIdFromOrderStatus(prolifedata::IOrderInfo::OrderStatus status);
prolifedata::IOrderInfo::OrderStatus GetOrderStatusFromId(const QByteArray& statusId);

bool CheckDeviceMacAddressExists(const QByteArray& deviceUuid, const QByteArray& macAddress, const imtbase::IObjectCollection& collection);
bool CheckDeviceSerialNumberExists(const QByteArray& deviceUuid, const QByteArray& serialNumber, const imtbase::IObjectCollection& collection);

bool CheckSoftwareSerialNumberExists(const QByteArray& deviceUuid, const QByteArray& serialNumber, const imtbase::IObjectCollection& collection);

QByteArray GetIdFromCustomerRoleType(prolifedata::IOrderCustomerRole::RoleType roleType);
prolifedata::IOrderCustomerRole::RoleType GetCustomerRoleTypeFromId(const QByteArray& roleTypeId);

// Build hierarchical license tree from UserActions
// Returns root node of the tree (or empty if error)
// fullHierarchy: if true, builds complete tree from root; if false, shows only one level (parent+children of given license)
sdl::V1_0::prolife::CLicenseTreeNode BuildLicenseTreeFromActions(
			const QByteArray& licenseId,
			const imtbase::IObjectCollection& licenseCollection,
			const imtauth::IUserActionManager& userActionManager,
			QString& errorMessage,
			bool fullHierarchy = true);


} // namespace prolifedata


