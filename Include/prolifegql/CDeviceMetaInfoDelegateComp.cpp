// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#include <prolifegql/CDeviceMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>

// ProLife includes
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/IIotDeviceInfo.h>


namespace prolifegql
{


// protected methods

bool CDeviceMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo, const QByteArray& typeId) const
{
	if (typeId == QByteArrayLiteral("Device")){
		QByteArray macAddress = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_MAC_ADDRESS).toByteArray();
		representation["MacAddress"] = QString(macAddress);
	
		QByteArray serialNumber = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_SERIAL_NUMBER).toByteArray();
		representation["SerialNumber"] = QString(serialNumber);
	
		QByteArray deviceType = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_TYPE).toByteArray();
		representation["DeviceType"] = QString(deviceType);
	
		QByteArray configurationType = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_CONFIGURATION_TYPE).toByteArray();
		representation["ConfigurationType"] = QString(configurationType);
	
		QByteArray project = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_PROJECT).toByteArray();
		representation["Project"] = QString(project);
	
		int status = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_STATUS).toInt();
		representation["Status"] = status;
	
		QByteArray orderId = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_ORDER_ID).toByteArray();
		representation["OrderId"] = QString(orderId);
	
		QByteArray deliveryId = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_DELIVERY_ID).toByteArray();
		representation["DeliveryId"] = QString(deliveryId);
	
		QByteArray purchaseId = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_PURCHASE_ID).toByteArray();
		representation["PurchaseId"] = QString(purchaseId);
	
		QByteArray customerId = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_CUSTOMER_ID).toByteArray();
		representation["CustomerId"] = QString(customerId);
	
		QString customerName = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_CUSTOMER_NAME).toString();
		representation["CustomerName"] = customerName;
	
		QString productName = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_PRODUCT_NAME).toByteArray();
		representation["ProductName"] = productName;
	
		QByteArray productId = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_PRODUCT_ID).toByteArray();
		representation["ProductId"] = QString(productId);
	
		QByteArray licenseId = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_LICENSE_ID).toByteArray();
		representation["LicenseId"] = QString(licenseId);
	
		QString licenseName = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_LICENSE_NAME).toByteArray();
		representation["LicenseName"] = licenseName;
	
		bool inUse = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_IN_USE).toBool();
		representation["InUse"] = inUse;
	
		bool isInternalUse = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_INTERNAL_USE).toBool();
		representation["InternalUse"] = isInternalUse;

		return true;
	}
	else if (typeId == QByteArrayLiteral("IotDevice")){
		QByteArray factoryNumber = metaInfo.GetMetaInfo(prolifedata::IIotDeviceInfo::MIT_FACTORY_NUMBER).toByteArray();
		representation["FactoryNumber"] = QString(factoryNumber);

		QByteArray modelNumber = metaInfo.GetMetaInfo(prolifedata::IIotDeviceInfo::MIT_MODEM_NUMBER).toByteArray();
		representation["ModelNumber"] = QString(modelNumber);

		QByteArray manufacturer = metaInfo.GetMetaInfo(prolifedata::IIotDeviceInfo::MIT_MANUFACTURER).toByteArray();
		representation["Manufacturer"] = QString(manufacturer);

		QByteArray brandModel = metaInfo.GetMetaInfo(prolifedata::IIotDeviceInfo::MIT_BRAND_MODEL).toByteArray();
		representation["BrandModel"] = QString(brandModel);

		QByteArray installationLocation = metaInfo.GetMetaInfo(prolifedata::IIotDeviceInfo::MIT_INSTALLATION_LOCATION).toByteArray();
		representation["InstallationLocation"] = QString(installationLocation);

		QByteArray connectionType = metaInfo.GetMetaInfo(prolifedata::IIotDeviceInfo::MIT_CONNECTION_TYPE).toByteArray();
		representation["ConnectionType"] = QString(connectionType);

		QByteArray resourceType = metaInfo.GetMetaInfo(prolifedata::IIotDeviceInfo::MIT_RESOURCE_TYPE).toByteArray();
		representation["ResourceType"] = QString(resourceType);

		QByteArray calibrationDate = metaInfo.GetMetaInfo(prolifedata::IIotDeviceInfo::MIT_CALIBRATION_DATE).toByteArray();
		representation["CalibrationDate"] = QString(calibrationDate);

		QByteArray comissionDate = metaInfo.GetMetaInfo(prolifedata::IIotDeviceInfo::MIT_COMMISSION_DATE).toByteArray();
		representation["ComissionDate"] = QString(comissionDate);

		return true;
	}

	return false;
}


bool CDeviceMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation, const QByteArray& typeId) const
{
	if (typeId == QByteArrayLiteral("Device")){
		if (representation.contains("MacAddress")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_MAC_ADDRESS, representation.value("MacAddress"));
		}
	
		if (representation.contains("SerialNumber")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_SERIAL_NUMBER, representation.value("SerialNumber"));
		}
	
		if (representation.contains("DeviceType")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_TYPE, representation.value("DeviceType"));
		}
	
		if (representation.contains("ConfigurationType")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_CONFIGURATION_TYPE, representation.value("ConfigurationType"));
		}
	
		if (representation.contains("Project")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_PROJECT, representation.value("Project"));
		}
	
		if (representation.contains("Status")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_STATUS, representation.value("Status"));
		}
	
		if (representation.contains("OrderId")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_ORDER_ID, representation.value("OrderId"));
		}
	
		if (representation.contains("DeliveryId")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_DELIVERY_ID, representation.value("DeliveryId"));
		}
	
		if (representation.contains("PurchaseId")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_PURCHASE_ID, representation.value("PurchaseId"));
		}
	
		if (representation.contains("CustomerId")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_CUSTOMER_ID, representation.value("CustomerId"));
		}
	
		if (representation.contains("CustomerName")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_CUSTOMER_NAME, representation.value("CustomerName"));
		}
	
		if (representation.contains("ProductName")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_PRODUCT_NAME, representation.value("ProductName"));
		}
	
		if (representation.contains("ProductId")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_PRODUCT_ID, representation.value("ProductId"));
		}
	
		if (representation.contains("LicenseName")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_LICENSE_NAME, representation.value("LicenseName"));
		}
	
		if (representation.contains("LicenseId")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_LICENSE_ID, representation.value("LicenseId"));
		}
	
		if (representation.contains("InUse")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_IN_USE, representation.value("InUse"));
		}
	
		if (representation.contains("InternalUse")){
			metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_INTERNAL_USE, representation.value("InternalUse"));
		}

		return true;
	}
	else if (typeId == QByteArrayLiteral("IotDevice")){
		if (representation.contains("FactoryNumber")){
			metaInfo.SetMetaInfo(prolifedata::IIotDeviceInfo::MIT_FACTORY_NUMBER, representation.value("FactoryNumber"));
		}

		if (representation.contains("ModelNumber")){
			metaInfo.SetMetaInfo(prolifedata::IIotDeviceInfo::MIT_MODEM_NUMBER, representation.value("ModelNumber"));
		}

		if (representation.contains("Manufacturer")){
			metaInfo.SetMetaInfo(prolifedata::IIotDeviceInfo::MIT_MANUFACTURER, representation.value("Manufacturer"));
		}

		if (representation.contains("BrandModel")){
			metaInfo.SetMetaInfo(prolifedata::IIotDeviceInfo::MIT_BRAND_MODEL, representation.value("BrandModel"));
		}

		if (representation.contains("InstallationLocation")){
			metaInfo.SetMetaInfo(prolifedata::IIotDeviceInfo::MIT_INSTALLATION_LOCATION, representation.value("InstallationLocation"));
		}

		if (representation.contains("ConnectionType")){
			metaInfo.SetMetaInfo(prolifedata::IIotDeviceInfo::MIT_CONNECTION_TYPE, representation.value("ConnectionType"));
		}

		if (representation.contains("ResourceType")){
			metaInfo.SetMetaInfo(prolifedata::IIotDeviceInfo::MIT_RESOURCE_TYPE, representation.value("ResourceType"));
		}

		if (representation.contains("CalibrationDate")){
			metaInfo.SetMetaInfo(prolifedata::IIotDeviceInfo::MIT_CALIBRATION_DATE, representation.value("CalibrationDate"));
		}

		if (representation.contains("ComissionDate")){
			metaInfo.SetMetaInfo(prolifedata::IIotDeviceInfo::MIT_COMMISSION_DATE, representation.value("ComissionDate"));
		}

		return true;
	}

	return false;
}


} // namespace prolifegql


