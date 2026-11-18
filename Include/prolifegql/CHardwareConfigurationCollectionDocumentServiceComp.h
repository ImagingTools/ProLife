#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtbase/IObjectCollection.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService_fwd.h>

// ProLife includes
#include <prolifesdl/SDL/1.0/CPP/HardwareConfigurationCollectionDocumentService_fwd.h>


namespace prolifegql
{


/**
	Reads and writes the representation of a hardware configuration opened in the server document service.
*/
class CHardwareConfigurationCollectionDocumentServiceComp: public sdl::V1_0::prolife::CHardwareConfigurationCollectionDocumentServiceGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::prolife::CHardwareConfigurationCollectionDocumentServiceGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CHardwareConfigurationCollectionDocumentServiceComp)
		I_ASSIGN(m_documentServiceCompPtr, "CollectionDocumentService", "Document service of the hardware configurations", true, "CollectionDocumentService");
		I_ASSIGN(m_componentCollectionCompPtr, "HardwareComponentCollection", "Components: the components of a configuration are shown by name", true, "HardwareComponentCollection");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Products: the hardware model of a configuration is shown by name", true, "ProductCollection");
	I_END_COMPONENT

protected:
	// reimplemented (sdl::V1_0::prolife::CHardwareConfigurationCollectionDocumentServiceGqlHandlerCompBase)
	virtual sdl::V1_0::prolife::CHardwareConfigurationData OnGetHardwareConfigurationRepresentation(
				const sdl::V1_0::prolife::CGetHardwareConfigurationRepresentationGqlRequest& getRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateHardwareConfigurationFromRepresentation(
				const sdl::V1_0::prolife::CUpdateHardwareConfigurationFromRepresentationGqlRequest& updateRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentService, m_documentServiceCompPtr);
	I_REF(imtbase::IObjectCollection, m_componentCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
};


} // namespace prolifegql


