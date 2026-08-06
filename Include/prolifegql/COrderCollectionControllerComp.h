#pragma once


// ProLife includes
#include <prolifedata/IGroupFilterParamJoiner.h>
#include <prolifedata/IOrderInfo.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Orders_fwd.h>


namespace prolifegql
{


class COrderCollectionControllerComp: public sdl::V1_0::prolife::COrderCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::prolife::COrderCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(COrderCollectionControllerComp);
		I_ASSIGN(m_orderInfoFactCompPtr, "OrderFactory", "Factory for order instance", true, "OrderFactory");
		I_ASSIGN(m_deviceCollectionCompPtr, "DeviceCollection", "Device collection", true, "DeviceCollection");
		I_ASSIGN(m_bindingCollectionCompPtr, "BindingCollection", "Hardware product binding collection", true, "BindingCollection");
		I_ASSIGN(m_softwareInstanceCollectionCompPtr, "SoftwareInstanceCollection", "Software instance collection", true, "SoftwareInstanceCollection");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
		I_ASSIGN(m_licenseDefinitionCollectionCompPtr, "LicenseDefinitionCollection", "License definition collection", true, "LicenseDefinitionCollection");
		I_ASSIGN(m_deviceOperationContextControllerCompPtr, "DeviceOperationContextController", "Device operation context controller", true, "DeviceOperationContextController");
		I_ASSIGN(m_softwareOperationContextControllerCompPtr, "SoftwareOperationContextController", "Software operation context controller", true, "SoftwareOperationContextController");
		I_ASSIGN(m_groupFilterParamJoinerCompPtr, "GroupFilterParamJoiner", "Group filter param joiner", true, "GroupFilterParamJoiner");
		I_ASSIGN(m_accountCollectionCompPtr, "AccountCollection", "Account collection for customer name resolution", false, "AccountCollection");
	I_END_COMPONENT;

protected:
	bool CheckProducts(const QByteArray& orderUuid, const imtsdl::TElementList<sdl::V1_0::prolife::COrderedProduct>& productsModel, QString& errorMessage) const;
	QString GetProductName(const QByteArray& productUuid) const;

	virtual bool OnBeforeRemoveElements(
				const QByteArrayList& elementIds,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual void OnAfterSetObjectDescription(
				const QByteArray& objectId,
				const QString& description,
				const imtgql::CGqlRequest& gqlRequest) const override;
	virtual bool IsDescriptionStoredInDocument() const override;

	// reimplemented (sdl::V1_0::prolife::COrderCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::prolife::COrdersListGqlRequest& ordersListRequest,
				sdl::V1_0::prolife::COrderItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::prolife::COrderData& orderDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::V1_0::prolife::COrderItemGqlRequest& orderItemRequest,
				sdl::V1_0::prolife::COrderData& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::prolife::COrderUpdateGqlRequest& orderUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest,const imtgql::CGqlParamObject& viewParamsGql, iprm::CParamsSet* filterParams) const override;

private:
	bool FillObjectFromRepresentation(
				const sdl::V1_0::prolife::COrderData& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;

	/**
		Get IDs of all products of the given collection referencing the order \a orderUuid.
		The order reference stored on the product is the single source of truth for the Order-Product relation.
	*/
	imtbase::ICollectionInfo::Ids GetProductIdsForOrder(const imtbase::IObjectCollection* productCollectionPtr, const QByteArray& orderUuid) const;

	bool UpdateOrderForHardware(const QByteArray& deviceId, const QByteArray& orderId) const;
	bool UpdateOrderForSoftware(const QByteArray& softwareId, const QByteArray& orderId) const;
	bool CheckNewProducts(QList<sdl::V1_0::prolife::COrderedProduct> orderProducts, const QByteArray& orderId) const;
	bool CreateNewHardware(const sdl::V1_0::prolife::COrderedProduct& product, const QByteArray& orderId) const;
	bool CreateNewSoftware(const sdl::V1_0::prolife::COrderedProduct& product, const QByteArray& orderId) const;

protected:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_bindingCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_softwareInstanceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_deviceCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_licenseDefinitionCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_accountCollectionCompPtr);
	I_REF(imtbase::IOperationContextController, m_deviceOperationContextControllerCompPtr);
	I_REF(imtbase::IOperationContextController, m_softwareOperationContextControllerCompPtr);
	I_FACT(prolifedata::IOrderInfo, m_orderInfoFactCompPtr);
	I_REF(prolifedata::IGroupFilterParamJoiner, m_groupFilterParamJoinerCompPtr);
	
};


} // namespace prolifegql


