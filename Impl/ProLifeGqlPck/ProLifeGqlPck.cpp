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
		OrderController,
		"Order controller for order document",
		"Order Controller");

I_EXPORT_COMPONENT(
		OrderCollectionController,
		"Collection controller for order documents",
		"Collection Controller Order Document");

I_EXPORT_COMPONENT(
		DeviceController,
		"Controller for device document",
		"Device Controller");

I_EXPORT_COMPONENT(
		DeviceCollectionController,
		"Collection controller for device documents",
		"Collection Controller Device Document");

I_EXPORT_COMPONENT(
		SoftwareProductCollectionController,
		"Software product collection controller",
		"Software Product Collection Controller");

I_EXPORT_COMPONENT(
		HardwareProductCollectionController,
		"Hardware product collection controller",
		"Hardware Product Collection Controller");

I_EXPORT_COMPONENT(
		SoftwareProductController,
		"Software product controller",
		"Software Product Controller");

I_EXPORT_COMPONENT(
		OrderHistoryController,
		"Order history controller",
		"Order History Controller");


} // namespace ProLifeGqlPck


