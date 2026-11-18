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
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Products: the hardware model of a configuration is checked against it and shown by name", true, "ProductCollection");
		I_ASSIGN(m_componentCollectionCompPtr, "HardwareComponentCollection", "Components: the components of a configuration are checked against it and shown by name", true, "HardwareComponentCollection");
	I_END_COMPONENT;

protected:
	/**
		A configuration refers to an existing hardware model and to existing components only.
	*/
	bool CheckConfiguration(const prolifedata::IHardwareConfigurationInfo& configurationInfo, QString& errorMessage) const;

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


