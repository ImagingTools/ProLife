#pragma once


// ProLife includes
#include <prolifedata/IHardwareComponentInfo.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/HardwareComponents.h>


namespace prolifegql
{


/**
	GraphQL access to the catalog of hardware components (list, get, add, update).
*/
class CHardwareComponentCollectionControllerComp: public sdl::V1_0::prolife::CHardwareComponentCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::prolife::CHardwareComponentCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CHardwareComponentCollectionControllerComp);
		I_ASSIGN(m_componentInfoFactCompPtr, "HardwareComponentFactory", "Factory of new hardware components", true, "HardwareComponentFactory");
	I_END_COMPONENT;

protected:
	static void FillComponentFromRepresentation(
				const sdl::V1_0::prolife::CHardwareComponentData& representation,
				prolifedata::IHardwareComponentInfo& componentInfo);
	static void FillRepresentationFromComponent(
				const prolifedata::IHardwareComponentInfo& componentInfo,
				sdl::V1_0::prolife::CHardwareComponentData& representation);

	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	virtual QByteArray GetObjectTypeIdFromRequest(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual QString GetObjectNameFromRequest(const imtgql::CGqlRequest& gqlRequest) const override;

	// reimplemented (sdl::V1_0::prolife::CHardwareComponentCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::prolife::CHardwareComponentsListGqlRequest& listRequest,
				sdl::V1_0::prolife::CHardwareComponentItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::prolife::CHardwareComponentData& representation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::V1_0::prolife::CGetHardwareComponentItemGqlRequest& itemRequest,
				sdl::V1_0::prolife::CHardwareComponentData& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::prolife::CUpdateHardwareComponentGqlRequest& updateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

private:
	I_FACT(prolifedata::IHardwareComponentInfo, m_componentInfoFactCompPtr);
};


} // namespace prolifegql


