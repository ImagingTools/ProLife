#pragma once


// ProLife includes
#include <prolifedata/IIqcRunInfo.h>
#include <prolifedata/IGroupFilterParamJoiner.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/IQC.h>


namespace prolifegql
{


class CIqcCollectionControllerComp: public sdl::prolife::IQC::CIqcRunCollectionControllerCompBase
{
public:
	typedef sdl::prolife::IQC::CIqcRunCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CIqcCollectionControllerComp);
		I_ASSIGN(m_iqcRunInfoFactCompPtr, "IqcRunFactory", "Factory for IQC run instance", true, "IqcRunFactory");
		I_ASSIGN(m_groupFilterParamJoinerCompPtr, "GroupFilterParamJoiner", "Group filter param joiner", true, "GroupFilterParamJoiner");
	I_END_COMPONENT;

protected:
	virtual bool OnBeforeRemoveElements(
				const QByteArrayList& elementIds,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::prolife::IQC::CIqcRunCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::prolife::IQC::CIqcRunListGqlRequest& iqcRunListRequest,
				sdl::prolife::IQC::CIqcRunItem::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::prolife::IQC::CIqcRunData::V1_0& iqcRunDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::prolife::IQC::CIqcRunItemGqlRequest& iqcRunItemRequest,
				sdl::prolife::IQC::CIqcRunData::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::prolife::IQC::CIqcRunUpdateGqlRequest& iqcRunUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;
	virtual void SetAdditionalFilters(const imtgql::CGqlRequest& gqlRequest, const imtgql::CGqlParamObject& viewParamsGql, iprm::CParamsSet* filterParams) const override;

private:
	bool FillObjectFromRepresentation(
				const sdl::prolife::IQC::CIqcRunData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;

protected:
	I_FACT(prolifedata::IIqcRunInfo, m_iqcRunInfoFactCompPtr);
	I_REF(prolifedata::IGroupFilterParamJoiner, m_groupFilterParamJoinerCompPtr);
};


} // namespace prolifegql
