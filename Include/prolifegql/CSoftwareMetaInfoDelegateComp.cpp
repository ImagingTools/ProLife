#include <prolifegql/CSoftwareMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>
#include <QJsonArray>

// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>


namespace prolifegql
{


// protected methods

bool CSoftwareMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo, const QByteArray& /*typeId*/) const
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
	
	QJsonArray hardwareId = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_HARDWARE_ID).toJsonArray();
	representation["HardwareId"] = hardwareId;
	
	QByteArray macAddress = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_HARDWARE_MAC_ADDRESS).toByteArray();
	representation["MacAddress"] = QString(macAddress);
	
	QByteArray orderId = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_ORDER_ID).toByteArray();
	representation["OrderId"] = QString(orderId);
	
	QByteArray deliveryId = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_DELIVERY_ID).toByteArray();
	representation["DeliveryId"] = QString(deliveryId);
	
	QByteArray purchaseId = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PURCHASE_ID).toByteArray();
	representation["PurchaseId"] = QString(purchaseId);
	
	QByteArray licenseUuid = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_UUID).toByteArray();
	representation["LicenseUuid"] = QString(licenseUuid);
	
	QByteArray licenseId = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_ID).toByteArray();
	representation["LicenseId"] = QString(licenseId);
	
	QString licenseName = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_NAME).toString();
	representation["LicenseName"] = QString(licenseName);
	
	QByteArray productUuid = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_UUID).toByteArray();
	representation["ProductUuid"] = QString(productUuid);
	
	QByteArray productId = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_ID).toByteArray();
	representation["ProductId"] = QString(productId);
	
	QString productName = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME).toString();
	representation["ProductName"] = QString(productName);

	bool isInternalUse = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_INTERNAL_USE).toBool();
	representation["InternalUse"] = isInternalUse;

	bool isMultiProduct = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_IS_MULTI_PRODUCT).toBool();
	representation["IsMultiProduct"] = isMultiProduct;

	int productCount = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_COUNT).toInt();
	representation["ProductCount"] = productCount;

	QString parentInstanceId = metaInfo.GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PARENT_INSTANCE_ID).toString();
	representation["ParentInstanceId"] = QString(parentInstanceId);

	return true;
}


bool CSoftwareMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation, const QByteArray& /*typeId*/) const
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
	
	if (representation.contains("MacAddress")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_HARDWARE_MAC_ADDRESS, representation.value("MacAddress"));
	}
	
	if (representation.contains("OrderId")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_ORDER_ID, representation.value("OrderId"));
	}
	
	if (representation.contains("DeliveryId")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_DELIVERY_ID, representation.value("DeliveryId"));
	}
	
	if (representation.contains("PurchaseId")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PURCHASE_ID, representation.value("PurchaseId"));
	}
	
	if (representation.contains("LicenseUuid")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_UUID, representation.value("LicenseUuid"));
	}
	
	if (representation.contains("LicenseId")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_ID, representation.value("LicenseId"));
	}
	
	if (representation.contains("LicenseName")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_NAME, representation.value("LicenseName"));
	}
	
	if (representation.contains("ProductUuid")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_UUID, representation.value("ProductUuid"));
	}
	
	if (representation.contains("ProductId")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_ID, representation.value("ProductId"));
	}
	
	if (representation.contains("ProductName")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME, representation.value("ProductName"));
	}

	if (representation.contains("IsMultiProduct")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_IS_MULTI_PRODUCT, representation.value("IsMultiProduct"));
	}

	if (representation.contains("ProductCount")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_COUNT, representation.value("ProductCount"));
	}

	if (representation.contains("ParentInstanceId")){
		metaInfo.SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PARENT_INSTANCE_ID, representation.value("ParentInstanceId"));
	}

	return true;
}


} // namespace prolifegql


