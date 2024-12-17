#pragma once


// ProLife includes
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/SensorBinding.h>


#undef GetObject


namespace prolifegql
{


class CHardwareProductBindingControllerComp: public sdl::prolife::SensorBinding::V1_0::CSensorBindingCollectionControllerCompBase
{
public:
	typedef sdl::prolife::SensorBinding::V1_0::CSensorBindingCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CHardwareProductBindingControllerComp)
		I_ASSIGN(m_softwareProductCollectionCompPtr, "SoftwareProductCollection", "Software product collection", true, "SoftwareProductCollection");
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "License collection", true, "LicenseCollection");
		I_ASSIGN(m_deviceOperationContextControllerCompPtr, "DeviceOperationContextController", "Device operation context controller", true, "DeviceOperationContextController");
		I_ASSIGN(m_softwareOperationContextControllerCompPtr, "SoftwareOperationContextController", "Software operation context controller", true, "SoftwareOperationContextController");
	I_END_COMPONENT

protected:
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::prolife::SensorBinding::V1_0::CGetSensorBindingListGqlRequest& getSensorBindingListRequest,
				sdl::prolife::SensorBinding::CSensorBindingItem::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::prolife::SensorBinding::CSensorBindingData::V1_0& sensorBindingDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::prolife::SensorBinding::V1_0::CGetSensorBindingGqlRequest& getSensorBindingRequest,
				sdl::prolife::SensorBinding::CSensorBindingDataPayload::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::prolife::SensorBinding::V1_0::CUpdateSensorBindingGqlRequest& updateSensorBindingRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

	virtual imtbase::CTreeItemModel* UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_softwareProductCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);

	I_REF(imtbase::IOperationContextController, m_deviceOperationContextControllerCompPtr);
	I_REF(imtbase::IOperationContextController, m_softwareOperationContextControllerCompPtr);
};


} // namespace prolifegql


