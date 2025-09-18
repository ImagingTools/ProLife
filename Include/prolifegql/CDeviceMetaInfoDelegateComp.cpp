#include <prolifegql/CDeviceMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>

// ProLife includes
#include <prolifedata/IDeviceInfo.h>


namespace prolifegql
{


// protected methods

bool CDeviceMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo) const
{
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
	
	int softwareCount = metaInfo.GetMetaInfo(prolifedata::IDeviceInfo::MIT_COUNT_BINDED_LICENSES).toInt();
	representation["SoftwareCount"] = softwareCount;
	
	return true;
}


bool CDeviceMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation) const
{
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
	
	if (representation.contains("SoftwareCount")){
		metaInfo.SetMetaInfo(prolifedata::IDeviceInfo::MIT_COUNT_BINDED_LICENSES, representation.value("SoftwareCount"));
	}
	
	return true;
}


} // namespace prolifegql


