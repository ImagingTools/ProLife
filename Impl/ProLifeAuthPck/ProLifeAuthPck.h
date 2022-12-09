#pragma once


// ACF includes
#include <icomp/TModelCompWrap.h>
#include <icomp/TMakeComponentWrap.h>

// ProLife includes
#include <prolifeauth/CProductInfo.h>


namespace ProLifeAuthPck
{
typedef icomp::TModelCompWrap<
			icomp::TMakeComponentWrap <
						prolifeauth::CProductInfo,
						prolifeauth::IProductInfo,
						iser::IObject,
						iser::ISerializable,
						istd::IChangeable>> ProductInfo;


} // namespace ProLifeAuthPck


