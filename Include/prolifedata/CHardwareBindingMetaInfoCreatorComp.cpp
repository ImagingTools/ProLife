// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#include <prolifedata/CHardwareBindingMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ProLife includes
#include <prolifedata/IHardwareProductBinding.h>


namespace prolifedata
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool CHardwareBindingMetaInfoCreatorComp::CreateMetaInfo(
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

	const IHardwareProductBinding* hardwareInfoPtr = dynamic_cast<const IHardwareProductBinding*>(dataPtr);
	if (hardwareInfoPtr == nullptr){
		return false;
	}

	metaInfoPtr->SetMetaInfo(IHardwareProductBinding::MIT_HARDWARE_ID, hardwareInfoPtr->GetHardwareId());
	metaInfoPtr->SetMetaInfo(IHardwareProductBinding::MIT_SOFTWARE_IDS, hardwareInfoPtr->GetSoftwareIds().join(';'));

	return true;
}


// public methods of embedded class MetaInfo

QString CHardwareBindingMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int /*metaInfoType*/) const
{
	return QString();
}


} // namespace imtauth


