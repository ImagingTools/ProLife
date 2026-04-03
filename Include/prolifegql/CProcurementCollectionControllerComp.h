#pragma once


// ProLife includes
#include <prolifedata/IProcurementOrderInfo.h>
#include <prolifedata/IGroupFilterParamJoiner.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Procurement.h>


namespace prolifegql
{


class CProcurementCollectionControllerComp: public sdl::prolife::Procurement::CProcurementCollectionControllerCompBase
{
public:
	typedef sdl::prolife::Procurement::CProcurementCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CProcurementCollectionControllerComp);
		I_ASSIGN(m_procurementInfoFactCompPtr, "ProcurementFactory", "Factory for procurement order instance", true, "ProcurementFactory");
		I_ASSIGN(m_groupFilterParamJoinerCompPtr, "GroupFilterParamJoiner", "Group filter param joiner", true, "GroupFilterParamJoiner");
	I_END_COMPONENT;

protected:
	virtual bool OnBeforeRemoveElements(
				const QByteArrayList& elementIds,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::prolife::Procurement::CProcurementCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::prolife::Procurement::CProcurementListGqlRequest& procurementListRequest,
				sdl::prolife::Procurement::CProcurementItem::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::prolife::Procurement::CProcurementData::V1_0& procurementDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::prolife::Procurement::CProcurementItemGqlRequest& procurementItemRequest,
				sdl::prolife::Procurement::CProcurementData::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::prolife::Procurement::CProcurementUpdateGqlRequest& procurementUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest, const imtgql::CGqlParamObject& viewParamsGql, iprm::CParamsSet* filterParams) const override;

private:
	bool FillObjectFromRepresentation(
				const sdl::prolife::Procurement::CProcurementData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;

protected:
	I_FACT(prolifedata::IProcurementOrderInfo, m_procurementInfoFactCompPtr);
	I_REF(prolifedata::IGroupFilterParamJoiner, m_groupFilterParamJoinerCompPtr);
};


} // namespace prolifegql
