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
						prolifedata::CIdentifiableOrderInfo,
						prolifedata::IOrderInfo,
						imtbase::IIdentifiable,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> OrderInfo;
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						prolifedata::CIdentifiableDeviceInfo,
						prolifedata::IDeviceInfo,
						imtbase::IIdentifiable,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> DeviceInfo;
typedef prolifedata::CKeyDataProviderComp KeyDataProvider;


} // namespace ProLifeDataPck


