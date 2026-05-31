#pragma once


// ImtCore includes
#include <imtgql/IGqlRequestProvider.h>

// ProLife includes
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/IGroupFilterParamJoiner.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Sensors_fwd.h>


namespace prolifegql
{


class CDeviceCollectionControllerComp: public sdl::V1_0::prolife::CDeviceCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::prolife::CDeviceCollectionControllerCompBase BaseClass;

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
	// reimplemented (sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CVisualStatus OnGetObjectVisualStatus(
				const sdl::V1_0::imtbase::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CGetElementMetaInfoPayload OnGetElementMetaInfo(
				const sdl::V1_0::imtbase::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual bool OnBeforeRemoveElements(
				const QByteArrayList& elementIds,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::V1_0::prolife::CDeviceCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const ::imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::prolife::CDevicesListGqlRequest& devicesListRequest,
				sdl::V1_0::prolife::CDeviceItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::prolife::CDeviceData& deviceDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::V1_0::prolife::CDeviceItemGqlRequest& deviceItemRequest,
				sdl::V1_0::prolife::CDeviceData& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::prolife::CDeviceUpdateGqlRequest& deviceUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest,const imtgql::CGqlParamObject& viewParamsGql, iprm::CParamsSet* filterParams) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	bool FillObjectFromRepresentation(
				const sdl::V1_0::prolife::CDeviceData& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;
	bool RemoveDeviceFromOrder(const QByteArray& deviceId, const QByteArray& orderId) const;
	bool AddDeviceToOrder(const QByteArray& deviceId, const QByteArray& orderId) const;
	QByteArrayList GetBindedSoftware(const QByteArray& deviceId) const;
	bool GetSoftwareInfo(const QByteArray& softwareId, sdl::V1_0::prolife::CSoftwareBindingInfo& softwareInfo) const;

private:
	imtbase::CTimeFilterParam m_licenseCreationTimeParam;
	istd::TDelPtr<imtserverapp::CTimeFilterParamRepresentationController> m_timeFilterParamRepresentationControllerPtr;

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


