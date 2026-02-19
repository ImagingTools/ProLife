#pragma once


// Qt includes
#include <QString>

// ImtCore includes
#include <imtbase/IObjectCollection.h>

// ProLife includes
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/IOrderInfo.h>

// Generated includes
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Licenses.h>


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

// Build hierarchical license tree from UserActions
// Returns root node of the tree (or empty if error)
// fullHierarchy: if true, builds complete tree from root; if false, shows only one level (parent+children of given license)
sdl::prolife::Licenses::CLicenseTreeNode::V1_0 BuildLicenseTreeFromActions(
			const QByteArray& licenseId,
			const imtbase::IObjectCollection& licenseCollection,
			const imtbase::IObjectCollection& customerCollection,
			const imtauth::IUserActionManager& userActionManager,
			QString& errorMessage,
			bool fullHierarchy = true);


} // namespace prolifedata


