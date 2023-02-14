#include "ProLifeDataPck.h"


// ACF includes
#include <icomp/export.h>


namespace ProLifeDataPck
{


I_EXPORT_PACKAGE(
			"ProLifeDataPck",
			"ImagingTools license management component package",
			IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
			OrderInfo,
			"Order information",
			"Oredr Information Authority");

I_EXPORT_COMPONENT(
			DeviceInfo,
			"Device information",
			"Device Information Authority");

I_EXPORT_COMPONENT(
			KeyDataProvider,
			"Key data provider",
			"License Key Provider Order Product");


} // namespace ProLifeDataPck


