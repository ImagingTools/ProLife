#pragma once


// ProLife includes
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/IOrderInfo.h>


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


} // namespace prolifedata


