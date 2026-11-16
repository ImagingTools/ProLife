#pragma once


// ProLife includes
#include <prolifedata/IHardwareConfigurationInfo.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/HardwareConfigurations.h>


namespace prolifegql
{


/**
	GraphQL access to the configurations of the hardware models (list, get, add, update).
*/
class CHardwareConfigurationCollectionControllerComp: public sdl::V1_0::prolife::CHardwareConfigurationCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::prolife::CHardwareConfigurationCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CHardwareConfigurationCollectionControllerComp);
		I_ASSIGN(m_configurationInfoFactCompPtr, "HardwareConfigurationFactory", "Factory of new hardware configurations", true, "HardwareConfigurationFactory");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Products, to show the name of the hardware model", false, "ProductCollection");
		I_ASSIGN(m_componentCollectionCompPtr, "HardwareComponentCollection", "Components, to show the names of the components of a configuration", false, "HardwareComponentCollection");
	I_END_COMPONENT;

protected:
	static void FillConfigurationFromRepresentation(
				const sdl::V1_0::prolife::CHardwareConfigurationData& representation,
				prolifedata::IHardwareConfigurationInfo& configurationInfo);
	static void FillRepresentationFromConfiguration(
				const prolifedata::IHardwareConfigurationInfo& configurationInfo,
				sdl::V1_0::prolife::CHardwareConfigurationData& representation);

	/**
		Names and part numbers of the components of a configuration, in the order of the IDs.
		A component missing in the collection is listed with its ID as the name.
	*/
	imtsdl::TElementList<sdl::V1_0::prolife::CHardwareConfigurationComponent> CreateComponentsRepresentation(const QByteArrayList& componentIds) const;

	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	virtual QByteArray GetObjectTypeIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual QString GetObjectNameFromRequest(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (sdl::V1_0::prolife::CHardwareConfigurationCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::prolife::CHardwareConfigurationsListGqlRequest& listRequest,
				sdl::V1_0::prolife::CHardwareConfigurationItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::prolife::CHardwareConfigurationData& representation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::V1_0::prolife::CGetHardwareConfigurationItemGqlRequest& itemRequest,
				sdl::V1_0::prolife::CHardwareConfigurationData& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::prolife::CUpdateHardwareConfigurationGqlRequest& updateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

private:
	I_FACT(prolifedata::IHardwareConfigurationInfo, m_configurationInfoFactCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_componentCollectionCompPtr);
};


} // namespace prolifegql


