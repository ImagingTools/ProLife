#pragma once


// ProLife includes
#include <prolifedata/IOrderInfo.h>
#include <GeneratedFiles/prolifesdl/SDL/CPP/Orders/OrdersAPIv1_0.h>


namespace prolifegql
{


class COrderCollectionControllerComp: public prolife::sdl::Orders::COrderCollectionControllerCompBase
{
public:
	typedef prolife::sdl::Orders::COrderCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(COrderCollectionControllerComp);
		I_ASSIGN(m_permissionIdAttrPtr, "PermissionId", "Permission ID for show all orders", true, "");
		I_ASSIGN(m_orderInfoFactCompPtr, "OrderFactory", "Factory for order instance", true, "OrderFactory");
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
		I_ASSIGN(m_bindingCollectionCompPtr, "BindingCollection", "Hardware product binding collection", true, "BindingCollection");
		I_ASSIGN(m_softwareInstanceCollectionCompPtr, "SoftwareInstanceCollection", "Software instance collection", true, "SoftwareInstanceCollection");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
		I_ASSIGN(m_licenseDefinitionCollectionCompPtr, "LicenseDefinitionCollection", "License definition collection", true, "LicenseDefinitionCollection");
		I_ASSIGN(m_deviceOperationContextControllerCompPtr, "DeviceOperationContextController", "Device operation context controller", true, "DeviceOperationContextController");
		I_ASSIGN(m_softwareOperationContextControllerCompPtr, "SoftwareOperationContextController", "Software operation context controller", true, "SoftwareOperationContextController");
	I_END_COMPONENT;

protected:
	bool CheckProducts(const QByteArray& orderUuid, const QList<prolife::sdl::Orders::CProductItem>& productsModel, QString& errorMessage) const;
	QString GetProductName(const QByteArray& productUuid) const;
	void GenerateDifferences(
				prolifedata::IOrderInfo& currentOrder,
				prolifedata::IOrderInfo& newOrder,
				QByteArrayList& addedProducts,
				QByteArrayList& removedProducts,
				QByteArrayList& updatedProducts) const;

	// reimplemented (prolife::sdl::Orders::COrderCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const prolife::sdl::Orders::COrdersListGqlRequest& ordersListRequest,
				prolife::sdl::Orders::COrderItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeable* CreateObjectFromRepresentation(
				const prolife::sdl::Orders::COrderData& orderDataRepresentation,
				QByteArray& newObjectId,
				QString& name,
				QString& description,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const prolife::sdl::Orders::COrderItemGqlRequest& orderItemRequest,
				prolife::sdl::Orders::COrderDataPayload& representationPayload,
				QString& errorMessage) const override;
	virtual void SetObjectFilter(const imtgql::CGqlRequest& gqlRequest, const imtbase::CTreeItemModel& objectFilterModel, iprm::CParamsSet& filterParams) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_bindingCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareInstanceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseDefinitionCollectionCompPtr);
	I_REF(imtgql::IOperationContextController, m_deviceOperationContextControllerCompPtr);
	I_REF(imtgql::IOperationContextController, m_softwareOperationContextControllerCompPtr);
	I_FACT(prolifedata::IOrderInfo, m_orderInfoFactCompPtr);
	I_ATTR(QByteArray, m_permissionIdAttrPtr);
};


} // namespace prolifegql


