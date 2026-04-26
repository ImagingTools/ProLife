#pragma once


// ProLife includes
#include <prolifedata/IIqcTemplateInfo.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/IQC.h>


namespace prolifegql
{


class CIqcTemplateCollectionControllerComp: public sdl::prolife::IQC::CIqcTemplateCollectionControllerCompBase
{
public:
	typedef sdl::prolife::IQC::CIqcTemplateCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CIqcTemplateCollectionControllerComp);
		I_ASSIGN(m_iqcTemplateInfoFactCompPtr, "IqcTemplateFactory", "Factory for IQC template instance", true, "IqcTemplateFactory");
	I_END_COMPONENT;

protected:
	virtual bool OnBeforeRemoveElements(
				const QByteArrayList& elementIds,
				const imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;

	// reimplemented (sdl::prolife::IQC::CIqcTemplateCollectionControllerCompBase)
	virtual bool CreateRepresentationFromObject(
				const imtbase::IObjectCollectionIterator& objectCollectionIterator,
				const sdl::prolife::IQC::CIqcTemplateListGqlRequest& iqcTemplateListRequest,
				sdl::prolife::IQC::CIqcTemplateItem::V1_0& representationObject,
				QString& errorMessage) const override;
	virtual istd::IChangeableUniquePtr CreateObjectFromRepresentation(
				const sdl::prolife::IQC::CIqcTemplateData::V1_0& iqcTemplateDataRepresentation,
				QByteArray& newObjectId,
				QString& errorMessage) const override;
	virtual bool CreateRepresentationFromObject(
				const istd::IChangeable& data,
				const sdl::prolife::IQC::CIqcTemplateItemGqlRequest& iqcTemplateItemRequest,
				sdl::prolife::IQC::CIqcTemplateData::V1_0& representationPayload,
				QString& errorMessage) const override;
	virtual bool UpdateObjectFromRepresentationRequest(
				const ::imtgql::CGqlRequest& rawGqlRequest,
				const sdl::prolife::IQC::CIqcTemplateUpdateGqlRequest& iqcTemplateUpdateRequest,
				istd::IChangeable& object,
				QString& errorMessage) const override;

private:
	bool FillObjectFromRepresentation(
				const sdl::prolife::IQC::CIqcTemplateData::V1_0& representation,
				istd::IChangeable& object,
				QByteArray& objectId,
				QString& errorMessage) const;

protected:
	I_FACT(prolifedata::IIqcTemplateInfo, m_iqcTemplateInfoFactCompPtr);
};


} // namespace prolifegql
