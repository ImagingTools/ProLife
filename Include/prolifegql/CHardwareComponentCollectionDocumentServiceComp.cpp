#include <prolifegql/CHardwareComponentCollectionDocumentServiceComp.h>


// ImtCore includes
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>
#include <imtdoc/imtdoc.h>

// ProLife includes
#include <prolifesdl/SDL/1.0/CPP/HardwareComponentCollectionDocumentService.h>
#include <prolifegql/HardwareRepresentation.h>


namespace prolifegql
{


// protected methods

// reimplemented (sdl::V1_0::prolife::CHardwareComponentCollectionDocumentServiceGqlHandlerCompBase)

sdl::V1_0::prolife::CHardwareComponentData CHardwareComponentCollectionDocumentServiceComp::OnGetHardwareComponentRepresentation(
			const sdl::V1_0::prolife::CGetHardwareComponentRepresentationGqlRequest& getRepresentationRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	auto arguments = getRepresentationRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->id){
		errorMessage = QStringLiteral("Unable to get the hardware component. Error: Document-ID is missing");

		return sdl::V1_0::prolife::CHardwareComponentData();
	}

	const QByteArray userId = GetUserId(gqlRequest);
	const QByteArray documentId = *arguments.input->id;

	istd::IChangeableSharedPtr documentPtr;
	m_documentServiceCompPtr->GetDocumentData(userId, documentId, documentPtr);

	const prolifedata::IHardwareComponentInfo* infoPtr = dynamic_cast<const prolifedata::IHardwareComponentInfo*>(documentPtr.GetPtr());
	if (infoPtr == nullptr){
		errorMessage = QStringLiteral("Unable to get the hardware component. Error: Document is not opened");

		return sdl::V1_0::prolife::CHardwareComponentData();
	}

	sdl::V1_0::prolife::CHardwareComponentData response;
	FillRepresentationFromComponent(*infoPtr, m_companyCollectionCompPtr.GetPtr(), response);

	// The ID of the stored object, empty until the document is saved.
	response.id = imtdoc::GetDocumentObjectId(*m_documentServiceCompPtr, userId, documentId);

	return response;
}


sdl::V1_0::imtbase::CDocumentOperationStatus CHardwareComponentCollectionDocumentServiceComp::OnUpdateHardwareComponentFromRepresentation(
			const sdl::V1_0::prolife::CUpdateHardwareComponentFromRepresentationGqlRequest& updateRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& /*errorMessage*/) const
{
	sdl::V1_0::imtbase::CDocumentOperationStatus response;
	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Failed;

	auto arguments = updateRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->documentId || !arguments.input->componentData){
		return response;
	}

	const QByteArray userId = GetUserId(gqlRequest);
	const QByteArray documentId = *arguments.input->documentId;

	istd::IChangeableSharedPtr documentPtr;
	m_documentServiceCompPtr->GetDocumentData(userId, documentId, documentPtr);

	prolifedata::IHardwareComponentInfo* infoPtr = dynamic_cast<prolifedata::IHardwareComponentInfo*>(documentPtr.GetPtr());
	if (infoPtr == nullptr){
		response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::InvalidDocumentId;

		return response;
	}

	FillComponentFromRepresentation(*arguments.input->componentData, *infoPtr);

	m_documentServiceCompPtr->SetDocumentData(userId, documentId, *infoPtr);

	response.status = sdl::V1_0::imtbase::EDocumentOperationStatus::Success;

	return response;
}


} // namespace prolifegql


