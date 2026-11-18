#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>

// ProLife includes
#include <prolifedata/IHardwareComponentInfo.h>
#include <prolifedata/IHardwareConfigurationInfo.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/HardwareComponents.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/HardwareConfigurations.h>


namespace prolifegql
{


// The conversions between the hardware catalog objects and their GraphQL representations,
// shared by the collection controllers and the document services.

void FillComponentFromRepresentation(
			const sdl::V1_0::prolife::CHardwareComponentData& representation,
			prolifedata::IHardwareComponentInfo& componentInfo);
/**
	The manufacturer and the suppliers are shown by name, read from \a companyCollectionPtr.
*/
void FillRepresentationFromComponent(
			const prolifedata::IHardwareComponentInfo& componentInfo,
			const imtbase::IObjectCollection* companyCollectionPtr,
			sdl::V1_0::prolife::CHardwareComponentData& representation);

/**
	Name of the company \a companyId; its ID if the company cannot be read, empty for an empty ID.
*/
QString GetCompanyName(const imtbase::IObjectCollection* companyCollectionPtr, const QByteArray& companyId);

/**
	Name of the product  productId; empty if the product cannot be read.
*/
QString GetProductName(const imtbase::IObjectCollection* productCollectionPtr, const QByteArray& productId);

void FillConfigurationFromRepresentation(
			const sdl::V1_0::prolife::CHardwareConfigurationData& representation,
			prolifedata::IHardwareConfigurationInfo& configurationInfo);
void FillRepresentationFromConfiguration(
			const prolifedata::IHardwareConfigurationInfo& configurationInfo,
			sdl::V1_0::prolife::CHardwareConfigurationData& representation);

/**
	Names and part numbers of the components of a configuration, in the order of the IDs.
	A component missing in the collection is listed with its ID as the name.
*/
imtsdl::TElementList<sdl::V1_0::prolife::CHardwareConfigurationComponent> CreateComponentsRepresentation(
			const imtbase::IObjectCollection* componentCollectionPtr,
			const QByteArrayList& componentIds);


} // namespace prolifegql


