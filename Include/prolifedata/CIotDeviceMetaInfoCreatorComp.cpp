// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

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

	metaInfoPtr->SetMetaInfo(IIotDeviceInfo::MIT_SERIAL_NUMBER, iotDeviceInfoPtr->GetSerialNumber());
	metaInfoPtr->SetMetaInfo(IIotDeviceInfo::MIT_MANUFACTURER, iotDeviceInfoPtr->GetManufacturer());
	metaInfoPtr->SetMetaInfo(IIotDeviceInfo::MIT_MODEL_ID, iotDeviceInfoPtr->GetModelId());

	return true;
}


// public methods of embedded class MetaInfo

QString CIotDeviceMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int metaInfoType) const
{
	switch (metaInfoType){
		case IIotDeviceInfo::MIT_SERIAL_NUMBER:
			return QStringLiteral("Serial Number");
		case IIotDeviceInfo::MIT_MANUFACTURER:
			return QStringLiteral("Manufacturer");
		case IIotDeviceInfo::MIT_MODEL_ID:
			return QStringLiteral("Model-ID");
		case IIotDeviceInfo::MIT_MODEL_NAME:
			return QStringLiteral("Model Name");
		default:
			return BaseClass::GetMetaInfoName(metaInfoType);
	}
}


} // namespace prolifedata

