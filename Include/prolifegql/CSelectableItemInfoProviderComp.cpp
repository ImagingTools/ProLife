#include <prolifegql/CSelectableItemInfoProviderComp.h>


// Qt includes
#include <QtCore/QHash>

// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>

// ProLife includes
#include <prolifedata/IDeviceInfo.h>


namespace prolifegql
{


// reimplemented (imtservergql::IObjectParamsFiller)

bool CSelectableItemInfoProviderComp::FillParams(
	const QByteArray& objectId,
	iprm::IParamsSet& paramsSet,
	const QByteArray& /*contextTenantId*/) const
{
	iprm::CParamsSet* paramsSetPtr = dynamic_cast<iprm::CParamsSet*>(&paramsSet);
	if (paramsSetPtr == nullptr){
		return false;
	}

	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	int count = qMin(m_paramIdsAttrPtr.GetCount(), m_metaInfoIdsAttrPtr.GetCount());
	if (count <= 0){
		return false;
	}

	// The identifiers live on the object meta info; GetElementMetaInfo() answers with
	// the collection meta info, which does not carry them.
	idoc::MetaInfoPtr metaInfoPtr = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
	if (!metaInfoPtr.IsValid()){
		return false;
	}

	for (int i = 0; i < count; ++i){
		const QByteArray paramId = m_paramIdsAttrPtr[i];
		if (paramId.isEmpty()){
			continue;
		}

		int metaInfoType = GetMetaInfoType(m_metaInfoIdsAttrPtr[i]);
		if (metaInfoType < 0){
			SendWarningMessage(0,
						QString("Unknown meta info '%1'").arg(QString::fromUtf8(m_metaInfoIdsAttrPtr[i])),
						"CSelectableItemInfoProviderComp");
			continue;
		}

		const QString value = metaInfoPtr->GetMetaInfo(metaInfoType).toString();
		if (value.isEmpty()){
			continue;
		}

		iprm::CTextParam* paramPtr = new iprm::CTextParam;
		paramPtr->SetText(value);
		paramsSetPtr->SetEditableParameter(paramId, paramPtr, true);
	}

	return true;
}


// private methods

int CSelectableItemInfoProviderComp::GetMetaInfoType(const QByteArray& metaInfoId)
{
	static const QHash<QByteArray, int> s_metaInfoTypes = {
		{ "ProductName", imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME },
		{ "ProductId", imtlic::IProductInstanceInfo::MIT_PRODUCT_ID },
		{ "ProductUuid", imtlic::IProductInstanceInfo::MIT_PRODUCT_UUID },
		{ "SerialNumber", imtlic::IProductInstanceInfo::MIT_SERIAL_NUMBER },
		{ "Project", imtlic::IProductInstanceInfo::MIT_PROJECT },
		{ "CustomerName", imtlic::IProductInstanceInfo::MIT_CUSTOMER_NAME },
		{ "LicenseId", imtlic::IProductInstanceInfo::MIT_LICENSE_ID },
		{ "LicenseName", imtlic::IProductInstanceInfo::MIT_LICENSE_NAME },
		{ "OrderId", imtlic::IProductInstanceInfo::MIT_ORDER_ID },
		{ "DeliveryId", imtlic::IProductInstanceInfo::MIT_DELIVERY_ID },
		{ "PurchaseId", imtlic::IProductInstanceInfo::MIT_PURCHASE_ID },

		{ "DeviceProductName", prolifedata::IDeviceInfo::MIT_PRODUCT_NAME },
		{ "DeviceProductId", prolifedata::IDeviceInfo::MIT_PRODUCT_ID },
		{ "DeviceType", prolifedata::IDeviceInfo::MIT_DEVICE_TYPE },
		{ "DeviceConfigurationType", prolifedata::IDeviceInfo::MIT_CONFIGURATION_TYPE },
		{ "DeviceSerialNumber", prolifedata::IDeviceInfo::MIT_DEVICE_SERIAL_NUMBER },
		{ "DeviceMacAddress", prolifedata::IDeviceInfo::MIT_DEVICE_MAC_ADDRESS },
		{ "DeviceProject", prolifedata::IDeviceInfo::MIT_DEVICE_PROJECT },
		{ "DeviceCustomerName", prolifedata::IDeviceInfo::MIT_CUSTOMER_NAME },
		{ "DeviceLicenseId", prolifedata::IDeviceInfo::MIT_LICENSE_ID },
		{ "DeviceLicenseName", prolifedata::IDeviceInfo::MIT_LICENSE_NAME },
		{ "DeviceOrderId", prolifedata::IDeviceInfo::MIT_ORDER_ID }
	};

	return s_metaInfoTypes.value(metaInfoId, -1);
}


} // namespace prolifegql
