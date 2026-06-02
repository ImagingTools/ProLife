#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtdoc/IDocumentServiceEventHandler.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService_fwd.h>

// ControlsGallery includes
#include <prolifesdl/SDL/1.0/CPP/DeviceCollectionDocumentService_fwd.h>


namespace prolifegql
{


class CDeviceCollectionDocumentServiceComp: 
			public sdl::V1_0::prolife::CDeviceCollectionDocumentServiceGqlHandlerCompBase
{
public:
	typedef sdl::V1_0::prolife::CDeviceCollectionDocumentServiceGqlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceCollectionDocumentServiceComp)
		I_ASSIGN(m_documentServiceCompPtr, "CollectionDocumentService", "Collection document manager", false, "CollectionDocumentService");
		I_ASSIGN(m_bindingCollectionCompPtr, "BindingCollection", "Hardware product binding collection", true, "BindingCollection");
		I_ASSIGN(m_softwareProductCollectionCompPtr, "SoftwareProductCollection", "Software product collection", true, "SoftwareProductCollection");
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "Remote License collection", true, "LicenseCollection");
	I_END_COMPONENT

protected:
	// reimplemented (sdl::V1_0::prolife::CDeviceCollectionDocumentServiceGqlHandlerCompBase)
	virtual sdl::V1_0::prolife::CDeviceData OnGetDeviceRepresentation(
				const sdl::V1_0::prolife::CGetDeviceRepresentationGqlRequest& getDeviceRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateDeviceFromRepresentation(
				const sdl::V1_0::prolife::CUpdateDeviceFromRepresentationGqlRequest& updateDeviceFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::prolife::CIotDeviceData OnGetIotDeviceRepresentation(
				const sdl::V1_0::prolife::CGetIotDeviceRepresentationGqlRequest& getIotDeviceRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CDocumentOperationStatus OnUpdateIotDeviceFromRepresentation(
				const sdl::V1_0::prolife::CUpdateIotDeviceFromRepresentationGqlRequest& updateIotDeviceFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	QByteArrayList GetBindedSoftware(const QByteArray& deviceId) const;
	bool GetSoftwareInfo(const QByteArray& softwareId, sdl::V1_0::prolife::CSoftwareBindingInfo& softwareInfo) const;

private:
	I_REF(imtdoc::IDocumentService, m_documentServiceCompPtr);
	I_REF(imtbase::IObjectCollection, m_bindingCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareProductCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
};


} // namespace prolifegql
