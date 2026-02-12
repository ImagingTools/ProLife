#include <prolifedata/CSoftwareMetaInfoCreatorComp.h>


// Qt includes
#include <QJsonArray>

// ACF includes
#include <imod/TModelWrap.h>
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtlic/CProductInstanceInfo.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/ILicenseDefinition.h>

// ProLife includes
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>
#include <prolifedata/IOrderInfo.h>
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/ICustomerInfo.h>


namespace prolifedata
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CSoftwareMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr);
	if (softwareInfoPtr == nullptr){
		return false;
	}
	
	QByteArray objectId = softwareInfoPtr->GetObjectUuid();
	
	QByteArray orderId = softwareInfoPtr->GetOrderId();
	QByteArray customerId = softwareInfoPtr->GetCustomerId();
	
	metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_ORDER_ID, orderId);
	
	if (m_orderCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr orderDataPtr;
		if (m_orderCollectionCompPtr->GetObjectData(orderId, orderDataPtr)){
			const IOrderInfo* orderInfoPtr = dynamic_cast<const IOrderInfo*>(orderDataPtr.GetPtr());
			if (orderInfoPtr != nullptr){
				QByteArray deliveryId = orderInfoPtr->GetOrderId();
				QByteArray purchaseId = orderInfoPtr->GetPurchaseOrderId();
				
				metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_DELIVERY_ID, deliveryId);
				metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PURCHASE_ID, purchaseId);
				
				customerId = orderInfoPtr->GetCustomerId();
			}
		}
	}
	
	metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_CUSTOMER_ID, customerId);
	
	if (m_accountCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr customerDataPtr;
		if (m_accountCollectionCompPtr->GetObjectData(customerId, customerDataPtr)){
			const ICustomerInfo* customerInfoPtr = dynamic_cast<const ICustomerInfo*>(customerDataPtr.GetPtr());
			if (customerInfoPtr != nullptr){
				QString customerName = customerInfoPtr->GetName();
				
				metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_CUSTOMER_NAME, customerName);
			}
		}
	}
	
	QByteArray productId = softwareInfoPtr->GetProductId();
	metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_UUID, productId);
	
	if (m_productCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr productDataPtr;
		if (m_productCollectionCompPtr->GetObjectData(productId, productDataPtr)){
			const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(productDataPtr.GetPtr());
			if (productInfoPtr != nullptr){
				
				QByteArray id = productInfoPtr->GetProductId();
				metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_ID, id);
				
				QString productName = productInfoPtr->GetName();
				metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME, productName);
			}
		}
	}
	
	imtbase::ICollectionInfo::Ids licenseIds = softwareInfoPtr->GetLicenseInstances().GetElementIds();
	if (!licenseIds.isEmpty()){
		QByteArray licenseId = licenseIds[0];
		metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_UUID, licenseId);
		
		if (m_licenseCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr licenseDataPtr;
			if (m_licenseCollectionCompPtr->GetObjectData(licenseId, licenseDataPtr)){
				const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
				if (licenseInfoPtr != nullptr){
					
					QByteArray id = licenseInfoPtr->GetLicenseId();
					metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_ID, id);
					
					QString licenseName = licenseInfoPtr->GetLicenseName();
					metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_NAME, licenseName);
				}
			}
		}
	}

	if (m_hardwareBindingCollectionCompPtr.IsValid()){
		imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
		fieldFilter.fieldId = "SoftwareIds";
		fieldFilter.filterValue = objectId;
		fieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FO_CONTAINS;
		
		imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
		groupFilter.fieldFilters << fieldFilter;
		
		imtbase::CComplexCollectionFilter complexFilter;
		complexFilter.SetFilterExpression(groupFilter);
		
		iprm::CParamsSet filterParam;
		filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

		QByteArrayList ids = m_hardwareBindingCollectionCompPtr->GetElementIds(0, -1, &filterParam);

		metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_IS_PAIRED, !ids.isEmpty());

		QJsonArray jsonArray;
		for (const QByteArray& item : std::as_const(ids)){
			jsonArray.append(QString::fromUtf8(item));
		}

		metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_HARDWARE_ID, jsonArray);

		if (ids.size() == 1){
			QByteArray hardwareId = ids[0];
			imtbase::IObjectCollection::DataPtr hardwareDataPtr;
			if (m_deviceCollectionCompPtr->GetObjectData(hardwareId, hardwareDataPtr)){
				const IDeviceInfo* deviceInfoPtr = dynamic_cast<const IDeviceInfo*>(hardwareDataPtr.GetPtr());
				if (deviceInfoPtr != nullptr){
					QByteArray macAddress = deviceInfoPtr->GetMacAddress();
					metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_HARDWARE_MAC_ADDRESS, macAddress);
				}
			}
		}
	}

	metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PROJECT, softwareInfoPtr->GetProject());
	metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_IN_USE, softwareInfoPtr->IsInUse());
	metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_INTERNAL_USE, softwareInfoPtr->IsInternalUse());
	metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_SERIAL_NUMBER, softwareInfoPtr->GetSerialNumber());
	metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_IS_MULTI_PRODUCT, softwareInfoPtr->IsMultiProduct());
	metaInfoPtr->SetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_COUNT, softwareInfoPtr->GetProductCount());

	return true;
}


// public methods of embedded class MetaInfo

QString CSoftwareMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int /*metaInfoType*/) const
{
	return QString();
}


} // namespace prolifedata


