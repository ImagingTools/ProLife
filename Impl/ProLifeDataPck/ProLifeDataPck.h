#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/CKeyDataProviderComp.h>
#include <prolifedata/CDeviceInfoComp.h>


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
typedef prolifedata::CKeyDataProviderComp KeyDataProvider;
typedef icomp::TModelCompWrap<prolifedata::CDeviceInfoComp> DeviceInfo;


} // namespace ProLifeDataPck


