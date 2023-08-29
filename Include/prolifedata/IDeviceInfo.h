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
		/**
			Device customer given as QString.
		*/
		MIT_DEVICE_CUSTOMER,

		/**
			Device MAC-Address given as QByteArray.
		*/
		MIT_DEVICE_MAC_ADDRESS,

		/**
			Device MAC-Address given as QByteArray.
		*/
		MIT_DEVICE_SERIAL_NUMBER,

		/**
			Device status given as DeviceProductionStatus.
		*/
		MIT_DEVICE_STATUS
	};

	enum DeviceProductionStatus
	{
		DPS_NONE,
		DPS_ACCEPTED,
		DPS_IN_PROGRESS,
		DPS_CANCELED,
		DPS_ON_HOLD,
		DPS_FINISHED
	};

	I_DECLARE_ENUM(DeviceProductionStatus, DPS_NONE, DPS_ACCEPTED, DPS_IN_PROGRESS, DPS_CANCELED, DPS_ON_HOLD, DPS_FINISHED);

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
};


} // namespace prolifedata


