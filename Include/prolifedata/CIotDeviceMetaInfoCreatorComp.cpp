#include <prolifedata/CIotDeviceMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ProLife includes
#include <prolifedata/CIotDeviceInfo.h>


namespace prolifedata
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CIotDeviceMetaInfoCreatorComp::CreateMetaInfo(
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

	const COrderedIdentifiableIotDeviceInfo* iotDeviceInfoPtr = dynamic_cast<const COrderedIdentifiableIotDeviceInfo*>(dataPtr);
	if (iotDeviceInfoPtr == nullptr){
		return false;
	}

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


// public methods of embedded class MetaInfo

QString CIotDeviceMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int /*metaInfoType*/) const
{
	return QString();
}


} // namespace prolifedata

