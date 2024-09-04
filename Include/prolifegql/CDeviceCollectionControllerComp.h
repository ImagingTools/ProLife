#pragma once


// ProLife includes
#include <prolifedata/IDeviceInfo.h>
#include <GeneratedFiles/prolifesdl/SDL/CPP/Sensors/SensorsAPIv1_0.h>


namespace prolifegql
{


class CDeviceCollectionControllerComp: public prolife::sdl::Sensors::CDeviceCollectionControllerCompBase
{
public:
	typedef prolife::sdl::Sensors::CDeviceCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDeviceCollectionControllerComp);
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
		I_ASSIGN(m_accountCollectionCompPtr, "AccountCollection", "Account collection", true, "AccountCollection");
		I_ASSIGN(m_bindingCollectionCompPtr, "BindingCollection", "Hardware product binding collection", true, "BindingCollection");
		I_ASSIGN(m_softwareProductCollectionCompPtr, "SoftwareProductCollection", "Software product collection", true, "SoftwareProductCollection");
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "Remote License collection", true, "LicenseCollection");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Remote product collection", true, "ProductCollection");
		I_ASSIGN(m_permissionIdAttrPtr, "PermissionId", "Permission ID for show all devices", true, "");
		I_ASSIGN(m_deviceInfoFactCompPtr, "DeviceFactory", "Factory used for creation of the new device instance", true, "DeviceFactory");
		I_ASSIGN(m_orderOperationContextControllerCompPtr, "OrderOperationContextController", "Order operation context controller", true, "OrderOperationContextController");
	I_END_COMPONENT;

protected:
	// reimplemented (prolife::sdl::Sensors::CDeviceCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const prolife::sdl::Sensors::CDevicesListGqlRequest& devicesListRequest,
				prolife::sdl::Sensors::CDeviceItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const prolife::sdl::Sensors::CDeviceData& deviceDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const prolife::sdl::Sensors::CDeviceItemGqlRequest& deviceItemRequest,
				prolife::sdl::Sensors::CDeviceDataPayload& representationPayload,
				QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* DeleteObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual void SetObjectFilter(const imtgql::CGqlRequest& gqlRequest, const imtbase::CTreeItemModel& objectFilterModel, iprm::CParamsSet& filterParams) const override;

private:
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_accountCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_bindingCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareProductCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_ATTR(QByteArray, m_permissionIdAttrPtr);
	I_FACT(prolifedata::IDeviceInfo, m_deviceInfoFactCompPtr);
	I_REF(imtgql::IOperationContextController, m_orderOperationContextControllerCompPtr);
};


} // namespace prolifegql


