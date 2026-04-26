#include <prolifegql/CIqcTemplateCollectionControllerComp.h>


// Qt includes
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

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

	// Deserialize checklist items from the stored JSON array
	const QString checklistJson = templateInfoPtr->GetChecklistItemsJson();
	if (!checklistJson.isEmpty()){
		QJsonArray jsonArray = QJsonDocument::fromJson(checklistJson.toUtf8()).array();
		QList<sdl::prolife::IQC::CIqcChecklistItem::V1_0> items;
		items.reserve(jsonArray.size());
		for (const QJsonValue& jsonVal : jsonArray){
			QJsonObject obj = jsonVal.toObject();
			sdl::prolife::IQC::CIqcChecklistItem::V1_0 item;
			item.Set_id(obj.value("id").toString());
			item.Set_templateUuid(obj.value("templateUuid").toString());
			item.Set_title(obj.value("title").toString());
			item.Set_description(obj.value("description").toString());
			item.Set_severity(obj.value("severity").toString());
			item.Set_required(obj.value("required").toBool(false));
			item.Set_expectedValueType(obj.value("expectedValueType").toString());
			item.Set_minValue(obj.value("minValue").toString());
			item.Set_maxValue(obj.value("maxValue").toString());
			item.Set_unit(obj.value("unit").toString());
			items.append(item);
		}
		representationPayload.Set_checklistItems(items);
	}

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

	// Serialize checklist items from the representation into the JSON string field
	const auto& checklistItemsOpt = representation.Get_checklistItems();
	if (checklistItemsOpt){
		QJsonArray jsonArray;
		for (const auto& item : *checklistItemsOpt){
			QJsonObject obj;
			obj["id"] = item.Get_id();
			obj["templateUuid"] = item.Get_templateUuid();
			obj["title"] = item.Get_title();
			obj["description"] = item.Get_description();
			obj["severity"] = item.Get_severity();
			obj["required"] = item.Get_required();
			obj["expectedValueType"] = item.Get_expectedValueType();
			obj["minValue"] = item.Get_minValue();
			obj["maxValue"] = item.Get_maxValue();
			obj["unit"] = item.Get_unit();
			jsonArray.append(obj);
		}
		templateInfoPtr->SetChecklistItemsJson(
			QString::fromUtf8(QJsonDocument(jsonArray).toJson(QJsonDocument::Compact)));
	}

	return true;
}


} // namespace prolifegql
