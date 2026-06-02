#pragma once


// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <imtauth/IUserActionManager.h>

// ProLife includes
#include <prolifedata/IGroupFilterParamJoiner.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Licenses_fwd.h>


namespace prolifegql
{


class CSoftwareProductCollectionControllerComp: public  sdl::V1_0::prolife::CSoftwareProductCollectionControllerCompBase
{
public:
	typedef sdl::V1_0::prolife::CSoftwareProductCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSoftwareProductCollectionControllerComp);
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Remote product collection", true, "ProductCollection");
		I_ASSIGN(m_bindingCollectionCompPtr, "BindingCollection", "Binding collection", true, "BindingCollection");
		I_ASSIGN(m_softwareInfoFactCompPtr, "SoftwareFactory", "Factory for software instance", true, "SoftwareFactory");
		I_ASSIGN(m_orderOperationContextControllerCompPtr, "OrderOperationContextController", "Operation context controller for order info", true, "OrderOperationContextController");
		I_ASSIGN(m_groupFilterParamJoinerCompPtr, "GroupFilterParamJoiner", "Group filter param joiner", true, "GroupFilterParamJoiner");
		I_ASSIGN(m_userActionManagerCompPtr, "UserActionManager", "User action manager for tracking operations", true, "UserActionManager");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CVisualStatus OnGetObjectVisualStatus(
				const sdl::V1_0::imtbase::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual bool OnBeforeRemoveElements(
				const QByteArrayList& elementIds,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::V1_0::prolife::CSoftwareProductCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::V1_0::prolife::CSoftwareProductsListGqlRequest& softwareProductsListRequest,
				sdl::V1_0::prolife::CSoftwareProductItem& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::V1_0::prolife::CSoftwareProductData& softwareProductDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::V1_0::prolife::CSoftwareProductItemGqlRequest& softwareProductItemRequest,
				sdl::V1_0::prolife::CSoftwareProductData& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::V1_0::prolife::CSoftwareProductUpdateGqlRequest& softwareProductUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest,const imtgql::CGqlParamObject& viewParamsGql, iprm::CParamsSet* filterParams) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	bool FillObjectFromRepresentation(
				const sdl::V1_0::prolife::CSoftwareProductData& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;
	bool RemoveSoftwareFromOrder(const QByteArray& softwareId, const QByteArray& orderId) const;
	bool AddSoftwareToOrder(const QByteArray& softwareId, const QByteArray& orderId) const;
	void PopulateBoundCountInTree(sdl::V1_0::prolife::CLicenseTreeNode& node) const;

private:
	imtbase::CTimeFilterParam m_licenseCreationTimeParam;
	istd::TDelPtr<imtserverapp::CTimeFilterParamRepresentationController> m_timeFilterParamRepresentationControllerPtr;

private:
	I_REF(imtbase::IObjectCollection, m_bindingCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_orderCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IOperationContextController, m_orderOperationContextControllerCompPtr);
	I_FACT(imtlic::IProductInstanceInfo, m_softwareInfoFactCompPtr);
	I_REF(prolifedata::IGroupFilterParamJoiner, m_groupFilterParamJoinerCompPtr);
	I_REF(imtauth::IUserActionManager, m_userActionManagerCompPtr);
};


} // namespace prolifegql


