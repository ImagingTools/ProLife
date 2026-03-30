// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/CHardwareProductBinding.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/CDeviceInfoComp.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>
#include <prolifedata/CDeviceMetaInfoCreatorComp.h>
#include <prolifedata/CSoftwareMetaInfoCreatorComp.h>
#include <prolifedata/COrderMetaInfoCreatorComp.h>
#include <prolifedata/CCustomerMetaInfoCreatorComp.h>
#include <prolifedata/CHardwareBindingMetaInfoCreatorComp.h>
#include <prolifedata/CSoftwareTransferInfo.h>
#include <prolifedata/CIotDeviceInfo.h>
#include <prolifedata/CIotDeviceMetaInfoCreatorComp.h>
#include <prolifedata/CSplitInAction.h>
#include <prolifedata/CSplitOutAction.h>
#include <prolifedata/CRevokeInAction.h>
#include <prolifedata/CRevokeOutAction.h>
#include <prolifedata/CDeviceDocumentValidatorComp.h>
#include <prolifedata/CDeviceDocumentNameProviderComp.h>


namespace ProLifeDataPck
{


typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						prolifedata::CIdentifiableOrderInfo,
						prolifedata::IOrderInfo,
						imtbase::IIdentifiable,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> OrderInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						prolifedata::CHardwareProductBinding,
						prolifedata::IHardwareProductBinding,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> HardwareProductBindingInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						prolifedata::COrderedIdentifiableSoftwareInstanceInfo,
						imtlic::IProductInstanceInfo,
						imtbase::IIdentifiable,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> OrderedIdentifiableSoftwareInstanceInfo;
typedef icomp::TModelCompWrap<prolifedata::CDeviceInfoComp> DeviceInfo;
typedef prolifedata::CDeviceMetaInfoCreatorComp DeviceMetaInfoCreator;
typedef prolifedata::CSoftwareMetaInfoCreatorComp SoftwareMetaInfoCreator;
typedef prolifedata::COrderMetaInfoCreatorComp OrderMetaInfoCreator;
typedef prolifedata::CCustomerMetaInfoCreatorComp CustomerMetaInfoCreator;
typedef prolifedata::CHardwareBindingMetaInfoCreatorComp HardwareBindingMetaInfoCreator;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						prolifedata::CSoftwareTransferInfo,
						prolifedata::ISoftwareTransfer,
						iser::ISerializable,
						istd::IChangeable>> SoftwareTransferInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						prolifedata::COrderedIdentifiableIotDeviceInfo,
						prolifedata::IIotDeviceInfo,
						iser::ISerializable,
						istd::IChangeable>> IotDeviceInfo;
typedef prolifedata::CIotDeviceMetaInfoCreatorComp IotDeviceMetaInfoCreator;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						prolifedata::CSplitInAction,
						iser::ISerializable,
						istd::IChangeable>> SplitInAction;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						prolifedata::CSplitOutAction,
						iser::ISerializable,
						istd::IChangeable>> SplitOutAction;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						prolifedata::CRevokeInAction,
						iser::ISerializable,
						istd::IChangeable>> RevokeInAction;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						prolifedata::CRevokeOutAction,
						iser::ISerializable,
						istd::IChangeable>> RevokeOutAction;
typedef prolifedata::CDeviceDocumentValidatorComp DeviceDocumentValidator;
typedef prolifedata::CDeviceDocumentNameProviderComp DeviceDocumentNameProvider;


} // namespace ProLifeDataPck


