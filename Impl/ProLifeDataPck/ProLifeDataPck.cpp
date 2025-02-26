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
			HardwareProductBindingInfo,
			"Hardware product binding info",
			"Hardware Product Binding Info");

I_EXPORT_COMPONENT(
			OrderedIdentifiableSoftwareInstanceInfo,
			"Ordered identifiable software instance info",
			"Ordered Identifiable Software Instance Info");

I_EXPORT_COMPONENT(
			DeviceInfo,
			"Device information",
			"Device Information Authority");

I_EXPORT_COMPONENT(
			KeyDataProvider,
			"Key data provider",
			"License Key Provider Order Product");

I_EXPORT_COMPONENT(
			DeviceMetaInfoCreator,
			"Device meta info creator",
			"Device Meta Info Creator");

I_EXPORT_COMPONENT(
			SoftwareMetaInfoCreator,
			"Software meta info creator",
			"Software Meta Info Creator");

I_EXPORT_COMPONENT(
			OrderMetaInfoCreator,
			"Order meta info creator",
			"Order Meta Info Creator");

I_EXPORT_COMPONENT(
			CustomerMetaInfoCreator,
			"Customer meta info creator",
			"Customer Meta Info Creator");

I_EXPORT_COMPONENT(
			HardwareBindingMetaInfoCreator,
			"Hardware meta info creator",
			"Hardware Meta Info Creator");


} // namespace ProLifeDataPck


