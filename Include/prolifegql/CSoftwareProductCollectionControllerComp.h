#pragma once


// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>

// ProLife includes
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Licenses.h>


namespace prolifegql
{


class CSoftwareProductCollectionControllerComp: public  sdl::prolife::Licenses::V1_0::CSoftwareProductCollectionControllerCompBase
{
public:
	typedef sdl::prolife::Licenses::V1_0::CSoftwareProductCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSoftwareProductCollectionControllerComp);
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
		I_ASSIGN(m_accountCollectionCompPtr, "AccountCollection", "Account collection", true, "AccountCollection");
		I_ASSIGN(m_licenseCollectionCompPtr, "LicenseCollection", "Remote License collection", true, "LicenseCollection");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Remote product collection", true, "ProductCollection");
		I_ASSIGN(m_permissionIdAttrPtr, "PermissionId", "Permission ID for show all licenses", true, "ViewAllLicenses");
		I_ASSIGN(m_softwareInfoFactCompPtr, "SoftwareFactory", "Factory for software instance", true, "SoftwareFactory");
		I_ASSIGN(m_orderOperationContextControllerCompPtr, "OrderOperationContextController", "Operation context controller for order info", true, "OrderOperationContextController");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::prolife::Licenses::V1_0::CSoftwareProductCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::prolife::Licenses::V1_0::CSoftwareProductsListGqlRequest& softwareProductsListRequest,
				sdl::prolife::Licenses::V1_0::CSoftwareProductItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const sdl::prolife::Licenses::V1_0::CSoftwareProductData& softwareProductDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::prolife::Licenses::V1_0::CSoftwareProductItemGqlRequest& softwareProductItemRequest,
				sdl::prolife::Licenses::V1_0::CSoftwareProductDataPayload& representationPayload,
				QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* DeleteObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual void SetObjectFilter(const imtgql::CGqlRequest& gqlRequest, const imtbase::CTreeItemModel& objectFilterModel, iprm::CParamsSet& filterParams) const override;

private:
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_accountCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtgql::IOperationContextController, m_orderOperationContextControllerCompPtr);
	I_ATTR(QByteArray, m_permissionIdAttrPtr);
	I_FACT(imtlic::IProductInstanceInfo, m_softwareInfoFactCompPtr);
};


} // namespace prolifegql


