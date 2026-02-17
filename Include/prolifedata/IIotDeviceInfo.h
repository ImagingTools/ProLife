#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>


namespace prolifedata
{


class IIotDeviceInfo: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_FACTORY_NUMBER = idoc::IDocumentMetaInfo::MIT_USER + 100,
		MIT_MODEM_NUMBER,
		MIT_MANUFACTURER,
		MIT_BRAND_MODEL,
		MIT_INSTALLATION_LOCATION,
		MIT_CONNECTION_TYPE,
		MIT_RESOURCE_TYPE,
		MIT_HOLE_DIAMETER,
		MIT_DEVICE_CATEGORY,
		MIT_CALIBRATION_DATE,
		MIT_COMMISSION_DATE
	};

	/**
		Get the factory number of this IoT device instance.
	*/
	virtual QByteArray GetFactoryNumber() const = 0;

	/**
		Set the factory number of this IoT device instance.
	*/
	virtual void SetFactoryNumber(const QByteArray& factoryNumber) = 0;

	/**
		Get the modem number of this IoT device instance.
	*/
	virtual QByteArray GetModemNumber() const = 0;

	/**
		Set the modem number of this IoT device instance.
	*/
	virtual void SetModemNumber(const QByteArray& modemNumber) = 0;

	/**
		Get the manufacturer of this IoT device instance.
	*/
	virtual QString GetManufacturer() const = 0;

	/**
		Set the manufacturer of this IoT device instance.
	*/
	virtual void SetManufacturer(const QString& manufacturer) = 0;

	/**
		Get the brand/model of this IoT device instance.
	*/
	virtual QString GetBrandModel() const = 0;

	/**
		Set the brand/model of this IoT device instance.
	*/
	virtual void SetBrandModel(const QString& brandModel) = 0;

	/**
		Get the installation location of this IoT device instance.
	*/
	virtual QString GetInstallationLocation() const = 0;

	/**
		Set the installation location of this IoT device instance.
	*/
	virtual void SetInstallationLocation(const QString& installationLocation) = 0;

	/**
		Get the connection type of this IoT device instance.
	*/
	virtual QString GetConnectionType() const = 0;

	/**
		Set the connection type of this IoT device instance.
	*/
	virtual void SetConnectionType(const QString& connectionType) = 0;

	/**
		Get the resource type of this IoT device instance.
	*/
	virtual QString GetResourceType() const = 0;

	/**
		Set the resource type of this IoT device instance.
	*/
	virtual void SetResourceType(const QString& resourceType) = 0;

	/**
		Get the hole diameter of this IoT device instance.
	*/
	virtual QString GetHoleDiameter() const = 0;

	/**
		Set the hole diameter of this IoT device instance.
	*/
	virtual void SetHoleDiameter(const QString& holeDiameter) = 0;

	/**
		Get the device category (ОДПУ/ИПУ) of this IoT device instance.
	*/
	virtual QString GetDeviceCategory() const = 0;

	/**
		Set the device category (ОДПУ/ИПУ) of this IoT device instance.
	*/
	virtual void SetDeviceCategory(const QString& deviceCategory) = 0;

	/**
		Get the calibration date of this IoT device instance.
	*/
	virtual QString GetCalibrationDate() const = 0;

	/**
		Set the calibration date of this IoT device instance.
	*/
	virtual void SetCalibrationDate(const QString& calibrationDate) = 0;

	/**
		Get the commission date of this IoT device instance.
	*/
	virtual QString GetCommissionDate() const = 0;

	/**
		Set the commission date of this IoT device instance.
	*/
	virtual void SetCommissionDate(const QString& commissionDate) = 0;

	/**
		Get the description of this IoT device instance.
	*/
	virtual QString GetDescription() const = 0;

	/**
		Set the description of this IoT device instance.
	*/
	virtual void SetDescription(const QString& description) = 0;
};


} // namespace prolifedata


