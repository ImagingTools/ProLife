#pragma once


// ImtCore includes
#include <imtdoc/IDocumentManager.h>
#include <imtdoc/IDocumentManagerEventHandler.h>
#include <imtbasesdl/SDL/1.0/CPP/CollectionDocumentManager.h>

// ControlsGallery includes
#include <prolifesdl/SDL/1.0/CPP/DeviceCollectionDocumentManager.h>


namespace prolifegql
{


class CDeviceCollectionDocumentManagerComp: 
			public sdl::prolife::DeviceCollectionDocumentManager::CGraphQlHandlerCompBase
{
public:
	typedef sdl::prolife::DeviceCollectionDocumentManager::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceCollectionDocumentManagerComp)
		I_REGISTER_INTERFACE(imtdoc::IDocumentManagerEventHandler);
		I_ASSIGN(m_documentManagerCompPtr, "CollectionDocumentManager", "Collection document manager", false, "CollectionDocumentManager");
		I_ASSIGN(m_bindingCollectionCompPtr, "BindingCollection", "Hardware product binding collection", true, "BindingCollection");
		I_ASSIGN(m_softwareProductCollectionCompPtr, "SoftwareProductCollection", "Software product collection", true, "SoftwareProductCollection");
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "Remote License collection", true, "LicenseCollection");
	I_END_COMPONENT

protected:
	// reimplemented (sdl::prolife::DeviceCollectionDocumentManager::CGraphQlHandlerCompBase)
	virtual sdl::prolife::Sensors::CDeviceData OnGetDeviceRepresentation(
				const sdl::prolife::DeviceCollectionDocumentManager::CGetDeviceRepresentationGqlRequest& getDeviceRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus OnUpdateDeviceFromRepresentation(
				const sdl::prolife::DeviceCollectionDocumentManager::CUpdateDeviceFromRepresentationGqlRequest& updateDeviceFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::prolife::Sensors::CIotDeviceData OnGetIotDeviceRepresentation(
				const sdl::prolife::DeviceCollectionDocumentManager::CGetIotDeviceRepresentationGqlRequest& getIotDeviceRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::CollectionDocumentManager::CDocumentOperationStatus OnUpdateIotDeviceFromRepresentation(
				const sdl::prolife::DeviceCollectionDocumentManager::CUpdateIotDeviceFromRepresentationGqlRequest& updateIotDeviceFromRepresentationRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

private:
	QByteArrayList GetBindedSoftware(const QByteArray& deviceId) const;
	bool GetSoftwareInfo(const QByteArray& softwareId, sdl::prolife::Sensors::CSoftwareBindingInfo::V1_0& softwareInfo) const;
	QByteArray GetUserId(const ::imtgql::CGqlRequest& gqlRequest) const;

private:
	I_REF(imtdoc::IDocumentManager, m_documentManagerCompPtr);
	I_REF(imtbase::IObjectCollection, m_bindingCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareProductCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
};


} // namespace prolifegql
