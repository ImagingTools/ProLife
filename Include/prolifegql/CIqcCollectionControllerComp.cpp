#include <prolifegql/CIqcCollectionControllerComp.h>


// ProLife includes
#include <prolifedata/CIqcRunInfo.h>


namespace prolifegql
{


// reimplemented (sdl::prolife::IQC::CIqcRunCollectionControllerCompBase)

bool CIqcCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::prolife::IQC::CIqcRunListGqlRequest& /*iqcRunListRequest*/,
			sdl::prolife::IQC::CIqcRunItem::V1_0& representationObject,
			QString& /*errorMessage*/) const
{
	const prolifedata::IIqcRunInfo* iqcRunInfoPtr =
		dynamic_cast<const prolifedata::IIqcRunInfo*>(objectCollectionIterator.GetCurrentObject());

	if (iqcRunInfoPtr == nullptr){
		return false;
	}

	representationObject.Set_batchUuid(iqcRunInfoPtr->GetBatchUuid());
	representationObject.Set_templateUuid(iqcRunInfoPtr->GetTemplateUuid());
	representationObject.Set_runMode(
		QString::fromUtf8(prolifedata::IIqcRunInfo::GetRunModeEnumId(iqcRunInfoPtr->GetRunMode())));
	representationObject.Set_runStatus(
		QString::fromUtf8(prolifedata::IIqcRunInfo::GetRunStatusEnumId(iqcRunInfoPtr->GetRunStatus())));
	representationObject.Set_runResult(
		QString::fromUtf8(prolifedata::IIqcRunInfo::GetRunResultEnumId(iqcRunInfoPtr->GetRunResult())));
	representationObject.Set_disposition(
		QString::fromUtf8(prolifedata::IIqcRunInfo::GetDispositionEnumId(iqcRunInfoPtr->GetDisposition())));
	representationObject.Set_systemId(iqcRunInfoPtr->GetSystemId());
	representationObject.Set_externalRunId(iqcRunInfoPtr->GetExternalRunId());
	representationObject.Set_startedAt(iqcRunInfoPtr->GetStartedAt());
	representationObject.Set_completedAt(iqcRunInfoPtr->GetCompletedAt());

	return true;
}


istd::IChangeableUniquePtr CIqcCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::prolife::IQC::CIqcRunData::V1_0& iqcRunDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	istd::IChangeableUniquePtr newObjectPtr = m_iqcRunInfoFactCompPtr->CreateObject();

	if (!FillObjectFromRepresentation(iqcRunDataRepresentation, *newObjectPtr, newObjectId, errorMessage)){
		return nullptr;
	}

	return newObjectPtr;
}


bool CIqcCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::prolife::IQC::CIqcRunItemGqlRequest& /*iqcRunItemRequest*/,
			sdl::prolife::IQC::CIqcRunData::V1_0& representationPayload,
			QString& /*errorMessage*/) const
{
	const prolifedata::IIqcRunInfo* iqcRunInfoPtr =
		dynamic_cast<const prolifedata::IIqcRunInfo*>(&data);

	if (iqcRunInfoPtr == nullptr){
		return false;
	}

	representationPayload.Set_batchUuid(iqcRunInfoPtr->GetBatchUuid());
	representationPayload.Set_templateUuid(iqcRunInfoPtr->GetTemplateUuid());
	representationPayload.Set_runMode(
		QString::fromUtf8(prolifedata::IIqcRunInfo::GetRunModeEnumId(iqcRunInfoPtr->GetRunMode())));
	representationPayload.Set_runStatus(
		QString::fromUtf8(prolifedata::IIqcRunInfo::GetRunStatusEnumId(iqcRunInfoPtr->GetRunStatus())));
	representationPayload.Set_runResult(
		QString::fromUtf8(prolifedata::IIqcRunInfo::GetRunResultEnumId(iqcRunInfoPtr->GetRunResult())));
	representationPayload.Set_disposition(
		QString::fromUtf8(prolifedata::IIqcRunInfo::GetDispositionEnumId(iqcRunInfoPtr->GetDisposition())));
	representationPayload.Set_operatorUserId(iqcRunInfoPtr->GetOperatorUserId());
	representationPayload.Set_systemId(iqcRunInfoPtr->GetSystemId());
	representationPayload.Set_externalRunId(iqcRunInfoPtr->GetExternalRunId());
	representationPayload.Set_startedAt(iqcRunInfoPtr->GetStartedAt());
	representationPayload.Set_completedAt(iqcRunInfoPtr->GetCompletedAt());
	representationPayload.Set_annotations(iqcRunInfoPtr->GetAnnotations());
	representationPayload.Set_defectCodes(iqcRunInfoPtr->GetDefectCodes());

	return true;
}


bool CIqcCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const ::imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::prolife::IQC::CIqcRunUpdateGqlRequest& iqcRunUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	QByteArray objectId;
	return FillObjectFromRepresentation(iqcRunUpdateRequest.Get_item(), object, objectId, errorMessage);
}


void CIqcCollectionControllerComp::SetAdditionalFilters(
			const imtgql::CGqlRequest& gqlRequest,
			const imtgql::CGqlParamObject& viewParamsGql,
			iprm::CParamsSet* filterParams) const
{
	if (m_groupFilterParamJoinerCompPtr != nullptr && filterParams != nullptr){
		m_groupFilterParamJoinerCompPtr->JoinGroupFilter(gqlRequest, *filterParams);
	}
}


bool CIqcCollectionControllerComp::OnBeforeRemoveElements(
			const QByteArrayList& /*elementIds*/,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	return true;
}


// private methods

bool CIqcCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::prolife::IQC::CIqcRunData::V1_0& representation,
			istd::IChangeable& object,
			QByteArray& objectId,
			QString& /*errorMessage*/) const
{
	prolifedata::IIqcRunInfo* iqcRunInfoPtr =
		dynamic_cast<prolifedata::IIqcRunInfo*>(&object);

	if (iqcRunInfoPtr == nullptr){
		return false;
	}

	objectId = representation.Get_id().toUtf8();

	iqcRunInfoPtr->SetBatchUuid(representation.Get_batchUuid().toUtf8());
	iqcRunInfoPtr->SetTemplateUuid(representation.Get_templateUuid().toUtf8());
	iqcRunInfoPtr->SetOperatorUserId(representation.Get_operatorUserId().toUtf8());
	iqcRunInfoPtr->SetSystemId(representation.Get_systemId());
	iqcRunInfoPtr->SetExternalRunId(representation.Get_externalRunId());
	iqcRunInfoPtr->SetStartedAt(representation.Get_startedAt());
	iqcRunInfoPtr->SetCompletedAt(representation.Get_completedAt());
	iqcRunInfoPtr->SetAnnotations(representation.Get_annotations());
	iqcRunInfoPtr->SetDefectCodes(representation.Get_defectCodes());

	QString runModeStr = representation.Get_runMode();
	if (!runModeStr.isEmpty()){
		prolifedata::IIqcRunInfo::RunMode mode;
		if (prolifedata::IIqcRunInfo::ParseRunModeEnum(runModeStr.toUtf8(), mode)){
			iqcRunInfoPtr->SetRunMode(mode);
		}
	}

	QString runStatusStr = representation.Get_runStatus();
	if (!runStatusStr.isEmpty()){
		prolifedata::IIqcRunInfo::RunStatus status;
		if (prolifedata::IIqcRunInfo::ParseRunStatusEnum(runStatusStr.toUtf8(), status)){
			iqcRunInfoPtr->SetRunStatus(status);
		}
	}

	QString runResultStr = representation.Get_runResult();
	if (!runResultStr.isEmpty()){
		prolifedata::IIqcRunInfo::RunResult result;
		if (prolifedata::IIqcRunInfo::ParseRunResultEnum(runResultStr.toUtf8(), result)){
			iqcRunInfoPtr->SetRunResult(result);
		}
	}

	QString dispositionStr = representation.Get_disposition();
	if (!dispositionStr.isEmpty()){
		prolifedata::IIqcRunInfo::Disposition disp;
		if (prolifedata::IIqcRunInfo::ParseDispositionEnum(dispositionStr.toUtf8(), disp)){
			iqcRunInfoPtr->SetDisposition(disp);
		}
	}

	return true;
}


} // namespace prolifegql
