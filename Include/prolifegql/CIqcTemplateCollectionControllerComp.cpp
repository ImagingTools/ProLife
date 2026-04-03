#include <prolifegql/CIqcTemplateCollectionControllerComp.h>


// ProLife includes
#include <prolifedata/IIqcTemplateInfo.h>


namespace prolifegql
{


// reimplemented

bool CIqcTemplateCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::prolife::IQC::CIqcTemplateListGqlRequest& /*iqcTemplateListRequest*/,
			sdl::prolife::IQC::CIqcTemplateItem::V1_0& representationObject,
			QString& /*errorMessage*/) const
{
	const prolifedata::IIqcTemplateInfo* templateInfoPtr =
		dynamic_cast<const prolifedata::IIqcTemplateInfo*>(objectCollectionIterator.GetCurrentObject());

	if (templateInfoPtr == nullptr){
		return false;
	}

	representationObject.Set_name(templateInfoPtr->GetTemplateName());
	representationObject.Set_supplierId(templateInfoPtr->GetSupplierId());
	representationObject.Set_componentId(templateInfoPtr->GetComponentId());
	representationObject.Set_version(templateInfoPtr->GetVersion());
	representationObject.Set_active(templateInfoPtr->IsActive());
	representationObject.Set_completionRule(
		QString::fromUtf8(prolifedata::IIqcTemplateInfo::GetCompletionRuleEnumId(
			templateInfoPtr->GetCompletionRule())));

	return true;
}


istd::IChangeableUniquePtr CIqcTemplateCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::prolife::IQC::CIqcTemplateData::V1_0& iqcTemplateDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	istd::IChangeableUniquePtr newObjectPtr = m_iqcTemplateInfoFactCompPtr->CreateObject();

	if (!FillObjectFromRepresentation(iqcTemplateDataRepresentation, *newObjectPtr, newObjectId, errorMessage)){
		return nullptr;
	}

	return newObjectPtr;
}


bool CIqcTemplateCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::prolife::IQC::CIqcTemplateItemGqlRequest& /*iqcTemplateItemRequest*/,
			sdl::prolife::IQC::CIqcTemplateData::V1_0& representationPayload,
			QString& /*errorMessage*/) const
{
	const prolifedata::IIqcTemplateInfo* templateInfoPtr =
		dynamic_cast<const prolifedata::IIqcTemplateInfo*>(&data);

	if (templateInfoPtr == nullptr){
		return false;
	}

	representationPayload.Set_name(templateInfoPtr->GetTemplateName());
	representationPayload.Set_supplierId(templateInfoPtr->GetSupplierId());
	representationPayload.Set_componentId(templateInfoPtr->GetComponentId());
	representationPayload.Set_version(templateInfoPtr->GetVersion());
	representationPayload.Set_active(templateInfoPtr->IsActive());
	representationPayload.Set_completionRule(
		QString::fromUtf8(prolifedata::IIqcTemplateInfo::GetCompletionRuleEnumId(
			templateInfoPtr->GetCompletionRule())));
	representationPayload.Set_passRuleDescription(templateInfoPtr->GetPassRuleDescription());

	return true;
}


bool CIqcTemplateCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const ::imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::prolife::IQC::CIqcTemplateUpdateGqlRequest& iqcTemplateUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	QByteArray objectId;
	return FillObjectFromRepresentation(iqcTemplateUpdateRequest.Get_item(), object, objectId, errorMessage);
}


bool CIqcTemplateCollectionControllerComp::OnBeforeRemoveElements(
			const QByteArrayList& /*elementIds*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return true;
}


// private methods

bool CIqcTemplateCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::prolife::IQC::CIqcTemplateData::V1_0& representation,
			istd::IChangeable& object,
			QByteArray& objectId,
			QString& /*errorMessage*/) const
{
	prolifedata::IIqcTemplateInfo* templateInfoPtr =
		dynamic_cast<prolifedata::IIqcTemplateInfo*>(&object);

	if (templateInfoPtr == nullptr){
		return false;
	}

	objectId = representation.Get_id().toUtf8();

	templateInfoPtr->SetTemplateName(representation.Get_name());
	templateInfoPtr->SetSupplierId(representation.Get_supplierId().toUtf8());
	templateInfoPtr->SetComponentId(representation.Get_componentId());
	templateInfoPtr->SetVersion(representation.Get_version());
	templateInfoPtr->SetActive(representation.Get_active());
	templateInfoPtr->SetPassRuleDescription(representation.Get_passRuleDescription());

	QString completionRuleStr = representation.Get_completionRule();
	if (!completionRuleStr.isEmpty()){
		prolifedata::IIqcTemplateInfo::CompletionRule rule;
		if (prolifedata::IIqcTemplateInfo::ParseCompletionRuleEnum(completionRuleStr.toUtf8(), rule)){
			templateInfoPtr->SetCompletionRule(rule);
		}
	}

	return true;
}


} // namespace prolifegql
