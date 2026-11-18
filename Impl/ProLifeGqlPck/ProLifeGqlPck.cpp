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
		HardwareComponentCollectionController,
		"Hardware component collection controller",
		"Hardware Component Catalog Collection Controller");

I_EXPORT_COMPONENT(
		HardwareConfigurationCollectionController,
		"Hardware configuration collection controller",
		"Hardware Configuration Variant Collection Controller");

I_EXPORT_COMPONENT(
		CustomerInfo,
		"Customer info",
		"Customer Info");

I_EXPORT_COMPONENT(
		DeviceController,
		"Device controller",
		"Device Controller");

I_EXPORT_COMPONENT(
		CustomerChangeGenerator,
		"Customer change generator",
		"Customer Change Generator");

I_EXPORT_COMPONENT(
		DeviceMetaInfoDelegate,
		"Device meta info delegate",
		"Device Meta Info Delegate");

I_EXPORT_COMPONENT(
		SoftwareMetaInfoDelegate,
		"Software meta info delegate",
		"Software Meta Info Delegate");

I_EXPORT_COMPONENT(
		OrderMetaInfoDelegate,
		"Order meta info delegate",
		"Order Meta Info Delegate");

I_EXPORT_COMPONENT(
		CustomerMetaInfoDelegate,
		"Customer meta info delegate",
		"Customer Meta Info Delegate");

I_EXPORT_COMPONENT(
		HardwareBindingMetaInfoDelegate,
		"Hardware meta info delegate",
		"Hardware Meta Info Delegate");

I_EXPORT_COMPONENT(
		GroupFilterParamJoiner,
		"Group filter param joiner",
		"Group Filter Param Joiner");

I_EXPORT_COMPONENT(
		WorkspaceController,
		"Workspace controller",
		"Workspace Controller");

I_EXPORT_COMPONENT(
		SoftwareController,
		"Software controller",
		"Software Controller");

I_EXPORT_COMPONENT(
		HardwareComponentCollectionDocumentService,
		"Hardware component collection document service",
		"Hardware Component Collection Document Service");

I_EXPORT_COMPONENT(
		HardwareConfigurationCollectionDocumentService,
		"Hardware configuration collection document service",
		"Hardware Configuration Collection Document Service");

I_EXPORT_COMPONENT(
		HardwareComponentDocumentValidator,
		"Validator of hardware component documents",
		"Hardware Component Document Validator");

I_EXPORT_COMPONENT(
		HardwareConfigurationDocumentValidator,
		"Validator of hardware configuration documents",
		"Hardware Configuration Document Validator");

I_EXPORT_COMPONENT(
		HardwareCatalogDocumentNameProvider,
		"Name of hardware component and configuration documents",
		"Hardware Catalog Document Name Provider");

I_EXPORT_COMPONENT(
		HardwareComponentReferenceCleaner,
		"Removes a removed hardware component from the configurations",
		"Hardware Component Reference Cleaner Configuration Cascade");

I_EXPORT_COMPONENT(
		CompanyReferenceCleaner,
		"Removes a removed company from the components",
		"Company Reference Cleaner Component Cascade");

I_EXPORT_COMPONENT(
		LicenseImportConverter,
		"Sorts imported licenses: those of hardware models become hardware configurations",
		"License Import Converter Hardware Configuration Migration");

I_EXPORT_COMPONENT(
		DeviceCollectionDocumentService,
		"Device collection document service",
		"Device Collection Document Service");

I_EXPORT_COMPONENT(
		SelectableItemInfoProvider,
		"Object meta info as selectable item parameters",
		"Selectable Item Info Provider Filterable Select");


} // namespace ProLifeGqlPck


