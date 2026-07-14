#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>


namespace prolifedata
{


class IDeviceInfo: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_DEVICE_TYPE = idoc::IDocumentMetaInfo::MIT_USER + 1,
		MIT_DEVICE_MAC_ADDRESS,
		MIT_DEVICE_SERIAL_NUMBER,
		MIT_DEVICE_STATUS,
		MIT_DEVICE_PROJECT,
		MIT_CONFIGURATION_TYPE,
		MIT_ORDER_ID,
		MIT_DELIVERY_ID,
		MIT_PURCHASE_ID,
		MIT_CUSTOMER_ID,
		MIT_CUSTOMER_NAME,
		MIT_PRODUCT_NAME,
		MIT_PRODUCT_ID,
		MIT_LICENSE_NAME,
		MIT_LICENSE_ID,
		MIT_IN_USE,
		MIT_INTERNAL_USE,
		MIT_END_CUSTOMER_ID,
		MIT_END_CUSTOMER_NAME,
		MIT_STATION,
		MIT_AREA
	};

	enum DeviceProductionStatus
	{
		DPS_NONE,
		DPS_ACCEPTED,
		DPS_IN_PROGRESS,
		DPS_CANCELED,
		DPS_ON_HOLD,
		DPS_FINISHED,
		DPS_DEFECTED,
		DPS_IN_REPAIR,
		DPS_DECOMMISSIONED
	};

	I_DECLARE_ENUM(DeviceProductionStatus, DPS_NONE, DPS_ACCEPTED, DPS_IN_PROGRESS, DPS_CANCELED, DPS_ON_HOLD, DPS_FINISHED, DPS_DEFECTED, DPS_IN_REPAIR, DPS_DECOMMISSIONED);

	/**
		Get the serial number of this device instance.
	*/
	virtual QByteArray GetSerialNumber() const = 0;

	/**
		Set the serial number of this device instance.
	*/
	virtual void SetSerialNumber(const QByteArray& serialNumber) = 0;

	/**
		Get the mac address of this device instance.
	*/
	virtual QByteArray GetMacAddress() const = 0;

	/**
		Set the mac address of this device instance.
	*/
	virtual void SetMacAddress(const QByteArray& macAddress) = 0;

	/**
		Get the type of this device instance.
	*/
	virtual QByteArray GetDeviceType() const = 0;

	/**
		Set the type of this device instance.
	*/
	virtual void SetDeviceType(const QByteArray& deviceType) = 0;

	/**
		Get the configuration type of this device instance.
	*/
	virtual QByteArray GetConfigurationType() const = 0;

	/**
		Set the configuration type of this device instance.
	*/
	virtual void SetConfigurationType(const QByteArray& configurationType) = 0;

	/**
		Get the description of this device instance.
	*/
	virtual QString GetDescription() const = 0;

	/**
		Set the description of this device instance.
	*/
	virtual void SetDescription(const QString& description) = 0;

	/**
		Get the status of this order.
	*/
	virtual DeviceProductionStatus GetDeviceProductionStatus() const = 0;

	/**
		Set the status of this order.
	*/
	virtual void SetDeviceProductionStatus(DeviceProductionStatus status) = 0;

	/**
		Get the project of this device instance.
	*/
	virtual QByteArray GetProject() const = 0;

	/**
		Set the project of this device instance.
	*/
	virtual void SetProject(const QByteArray& project) = 0;

	virtual QByteArray GetEndCustomerId() const = 0;
	virtual void SetEndCustomerId(const QByteArray& endCustomerId) = 0;

	virtual QString GetStation() const = 0;
	virtual void SetStation(const QString& station) = 0;

	virtual QString GetArea() const = 0;
	virtual void SetArea(const QString& area) = 0;

	/**
		Checks whether this product instance is marked for internal use only.
	*/
	virtual bool IsInternalUse() const = 0;

	/**
		Sets whether this product instance is for internal use only.
	*/
	virtual void SetInternalUse(bool internalUse) = 0;
};


} // namespace prolifedata

