#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/CHardwareProductBinding.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/CKeyDataProviderComp.h>
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
typedef prolifedata::CKeyDataProviderComp KeyDataProvider;
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


} // namespace ProLifeDataPck


