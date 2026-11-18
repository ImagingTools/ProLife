#include <prolifegql/CHardwareConfigurationCollectionDocumentServiceComp.h>


// ImtCore includes
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>
#include <imtdoc/imtdoc.h>

// ProLife includes
#include <prolifesdl/SDL/1.0/CPP/HardwareConfigurationCollectionDocumentService.h>
#include <prolifegql/HardwareRepresentation.h>


namespace prolifegql
{


// protected methods

// reimplemented (sdl::V1_0::prolife::CHardwareConfigurationCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::prolife::CHardwareConfigurationData CHardwareConfigurationCollectionDocumentServiceComp::OnGetHardwareConfigurationRepresentation(
			const sdl::V1_0::prolife::CGetHardwareConfigurationRepresentationGqlRequest& getRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	auto arguments = getRepresentationRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->id){
		errorMessage = QStringLiteral("Unable to get the hardware configuration. Error: Document-ID is missing");

		return sdl::V1_0::prolife::CHardwareConfigurationData();
	}

	const QByteArray userId = GetUserId(gqlRequest);
	const QByteArray documentId = *arguments.input->id;

	istd::IChangeableSharedPtr documentPtr;
	m_documentServiceCompPtr->GetDocumentData(userId, documentId, documentPtr);

	const prolifedata::IHardwareConfigurationInfo* infoPtr = dynamic_cast<const prolifedata::IHardwareConfigurationInfo*>(documentPtr.GetPtr());
	if (infoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to get the hardware configuration. Error: Document is not opened");

		return sdl::V1_0::prolife::CHardwareConfigurationData();
	}

	sdl::V1_0::prolife::CHardwareConfigurationData response;
	FillRepresentationFromConfiguration(*infoPtr, response);

	// The ID of the stored object, empty until the document is saved.
	response.id = imtdoc::GetDocumentObjectId(*m_documentServiceCompPtr, userId, documentId);
	response.productName = GetProductName(m_productCollectionCompPtr.GetPtr(), infoPtr->GetProductId());
	response.components = CreateComponentsRepresentation(m_componentCollectionCompPtr.GetPtr(), infoPtr->GetComponentIds());

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CHardwareConfigurationCollectionDocumentServiceComp::OnUpdateHardwareConfigurationFromRepresentation(
			const sdl::V1_0::prolife::CUpdateHardwareConfigurationFromRepresentationGqlRequest& updateRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	auto arguments = updateRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->documentId || !arguments.input->configurationData){
		return response;
	}

	const QByteArray userId = GetUserId(gqlRequest);
	const QByteArray documentId = *arguments.input->documentId;

	istd::IChangeableSharedPtr documentPtr;
	m_documentServiceCompPtr->GetDocumentData(userId, documentId, documentPtr);

	prolifedata::IHardwareConfigurationInfo* infoPtr = dynamic_cast<prolifedata::IHardwareConfigurationInfo*>(documentPtr.GetPtr());
	if (infoPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;

		return response;
	}

	FillConfigurationFromRepresentation(*arguments.input->configurationData, *infoPtr);

	m_documentServiceCompPtr->SetDocumentData(userId, documentId, *infoPtr);

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace prolifegql


