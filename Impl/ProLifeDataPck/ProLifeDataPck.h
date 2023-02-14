#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/CKeyDataProviderComp.h>


namespace ProLifeDataPck
{


typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						prolifedata::COrderInfo,
						prolifedata::IOrderInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> OrderInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						prolifedata::CDeviceInfo,
						prolifedata::IDeviceInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> DeviceInfo;
typedef prolifedata::CKeyDataProviderComp KeyDataProvider;


} // namespace ProLifeDataPck


