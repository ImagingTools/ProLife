#pragma once


// ImtCore includes
#include <imtgql/IGqlRequestProvider.h>

// ProLife includes
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/IGroupFilterParamJoiner.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Sensors.h>


namespace prolifegql
{


class CDeviceCollectionControllerComp: public sdl::prolife::Sensors::CDeviceCollectionControllerCompBase
{
public:
	typedef sdl::prolife::Sensors::CDeviceCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceCollectionControllerComp);
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
		I_ASSIGN(m_bindingCollectionCompPtr, "BindingCollection", "Hardware product binding collection", true, "BindingCollection");
		I_ASSIGN(m_softwareProductCollectionCompPtr, "SoftwareProductCollection", "Software product collection", true, "SoftwareProductCollection");
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "Remote License collection", true, "LicenseCollection");
		I_ASSIGN(m_deviceInfoFactCompPtr, "DeviceFactory", "Factory used for creation of the new device instance", true, "DeviceFactory");
		I_ASSIGN(m_orderOperationContextControllerCompPtr, "OrderOperationContextController", "Order operation context controller", true, "OrderOperationContextController");
		I_ASSIGN(m_groupFilterParamJoinerCompPtr, "GroupFilterParamJoiner", "Group filter param joiner", true, "GroupFilterParamJoiner");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::ImtCollection::CVisualStatus OnGetObjectVisualStatus(
				const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::prolife::Sensors::CDeviceCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const ::imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::prolife::Sensors::CDevicesListGqlRequest& devicesListRequest,
				sdl::prolife::Sensors::CDeviceItem::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::prolife::Sensors::CDeviceData::V1_0& deviceDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::prolife::Sensors::CDeviceItemGqlRequest& deviceItemRequest,
				sdl::prolife::Sensors::CDeviceData::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::prolife::Sensors::CDeviceUpdateGqlRequest& deviceUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

	virtual imtbase::CTreeItemModel* DeleteObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest,const imtgql::CGqlObject& viewParamsGql, iprm::CParamsSet* filterParams) const override;

private:
	bool FillObjectFromRepresentation(
				const sdl::prolife::Sensors::CDeviceData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;
	bool RemoveDeviceFromOrder(const QByteArray& deviceId, const QByteArray& orderId) const;
	bool AddDeviceToOrder(const QByteArray& deviceId, const QByteArray& orderId) const;

private:
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_bindingCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareProductCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
	I_FACT(prolifedata::IDeviceInfo, m_deviceInfoFactCompPtr);
	I_REF(imtbase::IOperationContextController, m_orderOperationContextControllerCompPtr);
	I_REF(prolifedata::IGroupFilterParamJoiner, m_groupFilterParamJoinerCompPtr);
};


} // namespace prolifegql


