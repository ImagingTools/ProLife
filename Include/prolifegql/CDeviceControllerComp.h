#pragma once


// ProLife includes
#include <prolifedata/IHardwareProductBinding.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Sensors.h>


#undef GetObject


namespace prolifegql
{


class CDeviceControllerComp: public sdl::prolife::Sensors::V1_0::CGraphQlHandlerCompBase
{
public:
	typedef sdl::prolife::Sensors::V1_0::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceControllerComp)
		I_ASSIGN(m_softwareProductCollectionCompPtr, "SoftwareProductCollection", "Software product collection", true, "SoftwareProductCollection");
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
		I_ASSIGN(m_deviceBindingCollectionCompPtr, "DeviceBindingCollection", "Device binding collection", true, "DeviceBindingCollection");
		I_ASSIGN(m_deviceOperationContextControllerCompPtr, "DeviceOperationContextController", "Device operation context controller", true, "DeviceOperationContextController");
		I_ASSIGN(m_softwareOperationContextControllerCompPtr, "SoftwareOperationContextController", "Software operation context controller", true, "SoftwareOperationContextController");
	I_END_COMPONENT

protected:
	virtual sdl::imtbase::ImtCollection::CVisualStatus::V1_0 OnGetObjectVisualStatus(const sdl::prolife::Sensors::V1_0::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::prolife::Sensors::CDevicesListPayload::V1_0 OnDevicesList(const sdl::prolife::Sensors::V1_0::CDevicesListGqlRequest& devicesListRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::prolife::Sensors::CDeviceDataPayload::V1_0 OnDeviceItem(const sdl::prolife::Sensors::V1_0::CDeviceItemGqlRequest& deviceItemRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::prolife::Sensors::CDeviceBindingData::V1_0 OnGetDeviceBinding(const sdl::prolife::Sensors::V1_0::CGetDeviceBindingGqlRequest& getDeviceBindingRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0 OnDeviceUpdate(const sdl::prolife::Sensors::V1_0::CDeviceUpdateGqlRequest& deviceUpdateRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CAddedNotificationPayload::V1_0 OnDeviceAdd(const sdl::prolife::Sensors::V1_0::CDeviceAddGqlRequest& deviceAddRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0 OnUpdateDeviceBinding(const sdl::prolife::Sensors::V1_0::CUpdateDeviceBindingGqlRequest& updateDeviceBindingRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual sdl::prolife::Sensors::CTransferLicensesPayload::V1_0 OnTransferLicenses(const sdl::prolife::Sensors::V1_0::CTransferLicensesGqlRequest& transferLicensesRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	prolifedata::IHardwareProductBinding* GetOrCreateDeviceBinding(const QByteArray& deviceId) const;
	void CreateDeviceOperationContext(const QByteArray& deviceId, const QByteArray& project, QByteArrayList addedLicenses, QByteArrayList removedLicenses) const;
	void CreateSoftwareOperationContext(const QByteArray& deviceId, const QByteArray& project, QByteArrayList addedLicenses, QByteArrayList removedLicenses) const;

protected:
	I_REF(imtbase::IObjectCollection, m_softwareProductCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceBindingCollectionCompPtr);

	I_REF(imtbase::IOperationContextController, m_deviceOperationContextControllerCompPtr);
	I_REF(imtbase::IOperationContextController, m_softwareOperationContextControllerCompPtr);
};


} // namespace prolifegql


