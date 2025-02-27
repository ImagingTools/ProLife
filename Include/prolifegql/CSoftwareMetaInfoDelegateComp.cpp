#include <prolifegql/CSoftwareMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>

// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>


namespace prolifegql
{


// protected methods

bool CSoftwareMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray project = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PROJECT).toByteArray();
	representation["Project"] = QString(project);
	
	QByteArray serialNumber = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_SERIAL_NUMBER).toByteArray();
	representation["SerialNumber"] = QString(serialNumber);

	bool inUse = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_IN_USE).toBool();
	representation["InUse"] = inUse;
	
	bool isPaired = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_IS_PAIRED).toBool();
	representation["IsPaired"] = isPaired;
	
	QByteArray customerId = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_CUSTOMER_ID).toByteArray();
	representation["CustomerId"] = QString(customerId);
	
	QString customerName = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_CUSTOMER_NAME).toString();
	representation["CustomerName"] = QString(customerName);
	
	QByteArray hardwareId = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_HARDWARE_ID).toByteArray();
	representation["HardwareId"] = QString(hardwareId);
	
	QByteArray deliveryId = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_DELIVERY_ID).toByteArray();
	representation["DeliveryId"] = QString(deliveryId);
	
	QByteArray purchaseId = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PURCHASE_ID).toByteArray();
	representation["PurchaseId"] = QString(purchaseId);
	
	QByteArray licenseId = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_ID).toByteArray();
	representation["LicenseId"] = QString(licenseId);
	
	QString licenseName = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_NAME).toString();
	representation["LicenseName"] = QString(licenseName);
	
	QString productName = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME).toString();
	representation["ProductName"] = QString(productName);
	
	return true;
}


bool CSoftwareMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation) const
{
	if (representation.contains("Project")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PROJECT, representation.value("Project"));
	}
	
	if (representation.contains("SerialNumber")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_SERIAL_NUMBER, representation.value("SerialNumber"));
	}
	
	if (representation.contains("InUse")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_IN_USE, representation.value("InUse"));
	}
	
	if (representation.contains("IsPaired")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_IS_PAIRED, representation.value("IsPaired"));
	}
	
	if (representation.contains("CustomerId")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_CUSTOMER_ID, representation.value("CustomerId"));
	}
	
	if (representation.contains("CustomerName")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_CUSTOMER_NAME, representation.value("CustomerName"));
	}
	
	if (representation.contains("HardwareId")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_HARDWARE_ID, representation.value("HardwareId"));
	}
	
	if (representation.contains("DeliveryId")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PURCHASE_ID, representation.value("DeliveryId"));
	}
	
	if (representation.contains("PurchaseId")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_DELIVERY_ID, representation.value("PurchaseId"));
	}
	
	if (representation.contains("LicenseId")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_ID, representation.value("LicenseId"));
	}
	
	if (representation.contains("LicenseName")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_NAME, representation.value("LicenseName"));
	}
	
	if (representation.contains("ProductName")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME, representation.value("ProductName"));
	}
	
	return true;
}


} // namespace prolifegql


