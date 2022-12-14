#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ProLife includes
#include <prolifedata/CProductInfo.h>


namespace ProLifeDataPck
{
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
                        prolifedata::CProductInfo,
                        prolifedata::IProductInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> ProductInfo;


} // namespace ProLifeAuthPck


