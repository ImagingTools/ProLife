#include "ProLifeGqlPck.h"


// ACF includes
#include <icomp/export.h>


namespace ProLifeGqlPck
{


I_EXPORT_PACKAGE(
		"ProLifeLicenseGqlPck",
		"ImagingTools license management Gql-component package",
		IM_PROJECT("\"ImagingTools Core Framework\"") IM_COMPANY("ImagingTools"));

I_EXPORT_COMPONENT(
		OrderCollectionController,
		"Collection controller for order documents",
		"Collection Controller Order Document");

I_EXPORT_COMPONENT(
		DeviceCollectionController,
		"Collection controller for device documents",
		"Collection Controller Device Document");

I_EXPORT_COMPONENT(
		SoftwareProductCollectionController,
		"Software product collection controller",
		"Software Product Collection Controller");

I_EXPORT_COMPONENT(
		HardwareProductBindingController,
		"Hardware product binding controller",
		"Hardware Product Binding Controller");

I_EXPORT_COMPONENT(
		DeviceChangeGenerator,
		"Change generator for device document",
		"Device DeviceInfo Change Generator History Document");

I_EXPORT_COMPONENT(
		LicenseChangeGenerator,
		"Change generator for license document",
		"License LicenseInfo Change Generator History Document");

I_EXPORT_COMPONENT(
		OrderChangeGenerator,
		"Change generator for order document",
		"Order OrderInfo Change Generator History Document");

I_EXPORT_COMPONENT(
		HardwareBindingChangeGenerator,
		"Change generator for hardware binding",
		"Hardware Binding Bind Change Generator History Document");

I_EXPORT_COMPONENT(
		CustomerCollectionController,
		"Customer collection controller",
		"Customer Collection Controller");

I_EXPORT_COMPONENT(
		CustomerInfo,
		"Customer info",
		"Customer Info");


} // namespace ProLifeGqlPck


