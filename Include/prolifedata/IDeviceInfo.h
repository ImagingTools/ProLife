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
			Device Id given as QString.
		*/
		MIT_DEVICE_ID = idoc::IDocumentMetaInfo::MIT_USER,

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
			Device status given as DeviceStatus.
		*/
		MIT_DEVICE_STATUS
	};

	enum DeviceStatus
	{
		OS_NONE,
		OS_CREATED,
		OS_IN_PROGRESS,
		OS_CANCELED,
		OS_ON_HOLD,
		OS_FINISHED,
		OS_CLOSED
	};

	I_DECLARE_ENUM(DeviceStatus, OS_NONE, OS_CREATED, OS_IN_PROGRESS, OS_CANCELED, OS_ON_HOLD, OS_FINISHED, OS_CLOSED);

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
	virtual DeviceStatus GetDeviceStatus() const = 0;

	/**
		Set the status of this order.
	*/
	virtual void SetDeviceStatus(DeviceStatus status) = 0;
};


} // namespace prolifedata


