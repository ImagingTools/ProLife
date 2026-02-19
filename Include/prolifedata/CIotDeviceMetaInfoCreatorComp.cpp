// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

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
	if (m_objectTypeIdsAttrPtr.FindValue(typeId) == -1){
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

QString CIotDeviceMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
		case IIotDeviceInfo::MIT_FACTORY_NUMBER:
			return QStringLiteral("Factory Number");
		case IIotDeviceInfo::MIT_MODEM_NUMBER:
			return QStringLiteral("Modem Number");
		case IIotDeviceInfo::MIT_MANUFACTURER:
			return QStringLiteral("Manufacturer");
		case IIotDeviceInfo::MIT_BRAND_MODEL:
			return QStringLiteral("Brand/Model");
		case IIotDeviceInfo::MIT_INSTALLATION_LOCATION:
			return QStringLiteral("Installation Location");
		case IIotDeviceInfo::MIT_CONNECTION_TYPE:
			return QStringLiteral("Connection Type");
		case IIotDeviceInfo::MIT_RESOURCE_TYPE:
			return QStringLiteral("Resource Type");
		case IIotDeviceInfo::MIT_HOLE_DIAMETER:
			return QStringLiteral("Hole Diameter");
		case IIotDeviceInfo::MIT_DEVICE_CATEGORY:
			return QStringLiteral("Device Category");
		case IIotDeviceInfo::MIT_CALIBRATION_DATE:
			return QStringLiteral("Calibration Date");
		case IIotDeviceInfo::MIT_COMMISSION_DATE:
			return QStringLiteral("Commission Date");
		default:
			return BaseClass::GetMetaInfoName(metaInfoType);
	}
}


} // namespace prolifedata

