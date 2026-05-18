#pragma once


// ImtCore includes
#include <imtdoc/IDocumentService.h>
#include <imtdoc/IDocumentServiceEventHandler.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentService.h>

// ControlsGallery includes
#include <prolifesdl/SDL/1.0/CPP/DeviceCollectionDocumentService.h>


namespace prolifegql
{


class CDeviceCollectionDocumentServiceComp: 
			public sdl::prolife::DeviceCollectionDocumentService::CGraphQlHandlerCompBase
{
public:
	typedef sdl::prolife::DeviceCollectionDocumentService::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceCollectionDocumentServiceComp)
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentService", "Collection document manager", false, "CollectionDocumentService");
		I_ASSIGN(m_bindingCollectionCompPtr, "BindingCollection", "Hardware product binding collection", true, "BindingCollection");
		I_ASSIGN(m_softwareProductCollectionCompPtr, "SoftwareProductCollection", "Software product collection", true, "SoftwareProductCollection");
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "Remote License collection", true, "LicenseCollection");
	I_END_COMPONENT

protected:
	// reimplemented (sdl::prolife::DeviceCollectionDocumentService::CGraphQlHandlerCompBase)
	virtual sdl::prolife::Sensors::CDeviceData OnGetDeviceRepresentation(
				const sdl::prolife::DeviceCollectionDocumentService::CGetDeviceRepresentationGqlRequest& getDeviceRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus OnUpdateDeviceFromRepresentation(
				const sdl::prolife::DeviceCollectionDocumentService::CUpdateDeviceFromRepresentationGqlRequest& updateDeviceFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Sensors::CIotDeviceData OnGetIotDeviceRepresentation(
				const sdl::prolife::DeviceCollectionDocumentService::CGetIotDeviceRepresentationGqlRequest& getIotDeviceRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentService::CDocumentOperationStatus OnUpdateIotDeviceFromRepresentation(
				const sdl::prolife::DeviceCollectionDocumentService::CUpdateIotDeviceFromRepresentationGqlRequest& updateIotDeviceFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	QByteArrayList GetBindedSoftware(const QByteArray& deviceId) const;
	bool GetSoftwareInfo(const QByteArray& softwareId, sdl::prolife::Sensors::CSoftwareBindingInfo::V1_0& softwareInfo) const;

private:
	I_REF(imtdoc::IDocumentService, m_documentManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_bindingCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareProductCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
};


} // namespace prolifegql
