// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#pragma once


// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>

// ProLife includes
#include <prolifedata/IGroupFilterParamJoiner.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Licenses.h>


namespace prolifegql
{


class CSoftwareProductCollectionControllerComp: public  sdl::prolife::Licenses::CSoftwareProductCollectionControllerCompBase
{
public:
	typedef sdl::prolife::Licenses::CSoftwareProductCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSoftwareProductCollectionControllerComp);
		I_ASSIGN(m_orderCollectionCompPtr, "OrderCollection", "Order collection", true, "OrderCollection");
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Remote product collection", true, "ProductCollection");
		I_ASSIGN(m_bindingCollectionCompPtr, "BindingCollection", "Binding collection", true, "BindingCollection");
		I_ASSIGN(m_softwareInfoFactCompPtr, "SoftwareFactory", "Factory for software instance", true, "SoftwareFactory");
		I_ASSIGN(m_orderOperationContextControllerCompPtr, "OrderOperationContextController", "Operation context controller for order info", true, "OrderOperationContextController");
		I_ASSIGN(m_groupFilterParamJoinerCompPtr, "GroupFilterParamJoiner", "Group filter param joiner", true, "GroupFilterParamJoiner");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::ImtCollection::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::ImtCollection::CVisualStatus OnGetObjectVisualStatus(
				const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	virtual bool OnBeforeRemoveElements(
				const QByteArrayList& elementIds,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::prolife::Licenses::CSoftwareProductCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::prolife::Licenses::CSoftwareProductsListGqlRequest& softwareProductsListRequest,
				sdl::prolife::Licenses::CSoftwareProductItem::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::prolife::Licenses::CSoftwareProductData::V1_0& softwareProductDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::prolife::Licenses::CSoftwareProductItemGqlRequest& softwareProductItemRequest,
				sdl::prolife::Licenses::CSoftwareProductData::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::prolife::Licenses::CSoftwareProductUpdateGqlRequest& softwareProductUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest,const imtgql::CGqlParamObject& viewParamsGql, iprm::CParamsSet* filterParams) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	bool FillObjectFromRepresentation(
				const sdl::prolife::Licenses::CSoftwareProductData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;
	bool RemoveSoftwareFromOrder(const QByteArray& softwareId, const QByteArray& orderId) const;
	bool AddSoftwareToOrder(const QByteArray& softwareId, const QByteArray& orderId) const;

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
};


} // namespace prolifegql


