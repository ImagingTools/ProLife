#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtbase/IObjectCollection.h>
#include <imtbase/IObjectCollection.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService_fwd.h>

// ProLife includes
#include <prolifesdl/SDL/1.0/CPP/HardwareComponentCollectionDocumentService_fwd.h>


namespace prolifegql
{


/**
	Reads and writes the representation of a hardware component opened in the server document service.
*/
class CHardwareComponentCollectionDocumentServiceComp: public sdl::V1_0::prolife::CHardwareComponentCollectionDocumentServiceGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::prolife::CHardwareComponentCollectionDocumentServiceGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CHardwareComponentCollectionDocumentServiceComp)
		I_ASSIGN(m_documentServiceCompPtr, "CollectionDocumentService", "Document service of the hardware components", true, "CollectionDocumentService");
		I_ASSIGN(m_companyCollectionCompPtr, "CompanyCollection", "Companies: the manufacturer and the suppliers of a component are checked against it and shown by name", true, "CompanyCollection");
	I_END_COMPONENT

protected:
	// reimplemented (sdl::V1_0::prolife::CHardwareComponentCollectionDocumentServiceGqlHandlerCompBase)
	virtual sdl::V1_0::prolife::CHardwareComponentData OnGetHardwareComponentRepresentation(
				const sdl::V1_0::prolife::CGetHardwareComponentRepresentationGqlRequest& getRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateHardwareComponentFromRepresentation(
				const sdl::V1_0::prolife::CUpdateHardwareComponentFromRepresentationGqlRequest& updateRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	I_REF(imtdoc::IDocumentService, m_documentServiceCompPtr);
	I_REF(imtbase::IObjectCollection, m_companyCollectionCompPtr);
};


} // namespace prolifegql


