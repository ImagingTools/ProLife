#include <prolifedata/CDeviceMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ImtCore includes
#include <imtlic/IProductInfo.h>
#include <imtlic/ILicenseDefinition.h>

// ProLife includes
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/CIotDeviceInfo.h>
#include <prolifedata/IOrderInfo.h>
#include <prolifedata/ICustomerInfo.h>
#include <prolifedata/IHardwareProductBinding.h>


namespace prolifedata
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CDeviceMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<const COrderedIdentifiableDeviceInfo*>(dataPtr);
	if (deviceInfoPtr != nullptr){
		QByteArray objectId = deviceInfoPtr->GetObjectUuid();
	
		QByteArray orderId = deviceInfoPtr->GetOrderId();
		QByteArray productId = deviceInfoPtr->GetDeviceType();
		QByteArray licenseId = deviceInfoPtr->GetConfigurationType();
	
		metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_DEVICE_MAC_ADDRESS, deviceInfoPtr->GetMacAddress());
		metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_DEVICE_SERIAL_NUMBER, deviceInfoPtr->GetSerialNumber());
		metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_DEVICE_STATUS, deviceInfoPtr->GetDeviceProductionStatus());
		metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_DEVICE_PROJECT, deviceInfoPtr->GetProject());
		metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_ORDER_ID, orderId);
		metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_INTERNAL_USE, deviceInfoPtr->IsInternalUse());
	
		QByteArray customerId;
		if (m_orderCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr orderDataPtr;
			if (m_orderCollectionCompPtr->GetObjectData(orderId, orderDataPtr)){
				const IOrderInfo* orderInfoPtr = dynamic_cast<const IOrderInfo*>(orderDataPtr.GetPtr());
				if (orderInfoPtr != nullptr){
					QByteArray deliveryId = orderInfoPtr->GetOrderId();
					metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_DELIVERY_ID, deliveryId);
	
					QByteArray purchaseId = orderInfoPtr->GetPurchaseOrderId();
					metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_PURCHASE_ID, purchaseId);
	
					customerId = orderInfoPtr->GetCustomerId();
				}
			}
		}
	
		metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_CUSTOMER_ID, customerId);
	
		if (m_accountCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr customerDataPtr;
			if (m_accountCollectionCompPtr->GetObjectData(customerId, customerDataPtr)){
				const ICustomerInfo* customerInfoPtr = dynamic_cast<const ICustomerInfo*>(customerDataPtr.GetPtr());
				if (customerInfoPtr != nullptr){
					
					QString customerName = customerInfoPtr->GetName();
					metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_CUSTOMER_NAME, customerName);
				}
			}
		}
	
		metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_DEVICE_TYPE, productId);
	
		if (m_productCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr productDataPtr;
			if (m_productCollectionCompPtr->GetObjectData(productId, productDataPtr)){
				const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(productDataPtr.GetPtr());
				if (productInfoPtr != nullptr){
					QByteArray id = productInfoPtr->GetProductId();
					metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_PRODUCT_ID, id);
	
					QString productName = productInfoPtr->GetName();
					metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_PRODUCT_NAME, productName);
				}
			}
		}
	
		metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_CONFIGURATION_TYPE, licenseId);
	
		if (m_licenseCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr licenseDataPtr;
			if (m_licenseCollectionCompPtr->GetObjectData(licenseId, licenseDataPtr)){
				const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
				if (licenseInfoPtr != nullptr){
					QByteArray id = licenseInfoPtr->GetLicenseId();
					metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_LICENSE_ID, id);
	
					QString licenseName = licenseInfoPtr->GetLicenseName();
					metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_LICENSE_NAME, licenseName);
				}
			}
		}
	
		bool inUse = false;
		if (m_deviceBindingCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr bindingDataPtr;
			if (m_deviceBindingCollectionCompPtr->GetObjectData(objectId, bindingDataPtr)){
				const IHardwareProductBinding* bindingInfoPtr = dynamic_cast<const IHardwareProductBinding*>(bindingDataPtr.GetPtr());
				if (bindingInfoPtr != nullptr){
					QByteArrayList softwareIds = bindingInfoPtr->GetSoftwareIds();
					softwareIds.removeAll("");
	
					inUse = softwareIds.size() > 0;
				}
			}
		}
	
		metaInfoPtr->SetMetaInfo(IDeviceInfo::MIT_IN_USE, inUse);

		return true;
	}

	const COrderedIdentifiableIotDeviceInfo* iotDeviceInfoPtr = dynamic_cast<const COrderedIdentifiableIotDeviceInfo*>(dataPtr);
	if (iotDeviceInfoPtr != nullptr){
		metaInfoPtr->SetMetaInfo(IIotDeviceInfo::MIT_FACTORY_NUMBER, iotDeviceInfoPtr->GetFactoryNumber());
		metaInfoPtr->SetMetaInfo(IIotDeviceInfo::MIT_MODEM_NUMBER, iotDeviceInfoPtr->GetModemNumber());
		metaInfoPtr->SetMetaInfo(IIotDeviceInfo::MIT_MANUFACTURER, iotDeviceInfoPtr->GetManufacturer());
		metaInfoPtr->SetMetaInfo(IIotDeviceInfo::MIT_BRAND_MODEL, iotDeviceInfoPtr->GetBrandModel());
		metaInfoPtr->SetMetaInfo(IIotDeviceInfo::MIT_INSTALLATION_LOCATION, iotDeviceInfoPtr->GetInstallationLocation());
		metaInfoPtr->SetMetaInfo(IIotDeviceInfo::MIT_CONNECTION_TYPE, iotDeviceInfoPtr->GetConnectionType());
		metaInfoPtr->SetMetaInfo(IIotDeviceInfo::MIT_RESOURCE_TYPE, iotDeviceInfoPtr->GetResourceType());
		metaInfoPtr->SetMetaInfo(IIotDeviceInfo::MIT_HOLE_DIAMETER, iotDeviceInfoPtr->GetHoleDiameter());
		metaInfoPtr->SetMetaInfo(IIotDeviceInfo::MIT_DEVICE_CATEGORY, iotDeviceInfoPtr->GetDeviceCategory());
		metaInfoPtr->SetMetaInfo(IIotDeviceInfo::MIT_CALIBRATION_DATE, iotDeviceInfoPtr->GetCalibrationDate());
		metaInfoPtr->SetMetaInfo(IIotDeviceInfo::MIT_COMMISSION_DATE, iotDeviceInfoPtr->GetCommissionDate());

		return true;
	}

	return false;
}


// public methods of embedded class MetaInfo

QString CDeviceMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int /*metaInfoType*/) const
{
	return QString();
}


} // namespace imtauth



