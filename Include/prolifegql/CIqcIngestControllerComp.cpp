#include <prolifegql/CIqcIngestControllerComp.h>


// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/CObjectCollection.h>


namespace prolifegql
{


// reimplemented (sdl::prolife::IQC::CIqcIngestControllerCompBase)

sdl::imtbase::ImtCollection::CUpdatedNotificationPayload CIqcIngestControllerComp::OnIqcRunIngest(
			const sdl::prolife::IQC::CIqcRunIngestGqlRequest& ingestRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CUpdatedNotificationPayload result;

	const QString systemId = ingestRequest.Get_systemId();
	const QString externalRunId = ingestRequest.Get_externalRunId();

	if (systemId.isEmpty() || externalRunId.isEmpty()){
		errorMessage = "systemId and externalRunId are required for IQC run ingestion";
		return result;
	}

	// Try to find an existing run with the same (systemId, externalRunId) key (idempotent upsert)
	prolifedata::IIqcRunInfo* existingRunPtr = FindExistingRun(systemId, externalRunId);

	if (existingRunPtr != nullptr){
		// Update the existing run with new result data
		QString runResultStr = ingestRequest.Get_runResult();
		if (!runResultStr.isEmpty()){
			prolifedata::IIqcRunInfo::RunResult runResult;
			if (prolifedata::IIqcRunInfo::ParseRunResultEnum(runResultStr.toUtf8(), runResult)){
				existingRunPtr->SetRunResult(runResult);
			}
		}

		QString dispositionStr = ingestRequest.Get_disposition();
		if (!dispositionStr.isEmpty()){
			prolifedata::IIqcRunInfo::Disposition disp;
			if (prolifedata::IIqcRunInfo::ParseDispositionEnum(dispositionStr.toUtf8(), disp)){
				existingRunPtr->SetDisposition(disp);
			}
		}

		if (!ingestRequest.Get_annotations().isEmpty()){
			existingRunPtr->SetAnnotations(ingestRequest.Get_annotations());
		}

		if (!ingestRequest.Get_defectCodes().isEmpty()){
			existingRunPtr->SetDefectCodes(ingestRequest.Get_defectCodes());
		}

		existingRunPtr->SetRunStatus(prolifedata::IIqcRunInfo::RS_COMPLETED);
	}
	else{
		// Create a new IQC run for this automated inspection result
		istd::IChangeableUniquePtr newRunPtr = m_iqcRunInfoFactCompPtr->CreateObject();
		prolifedata::IIqcRunInfo* newIqcRunPtr = dynamic_cast<prolifedata::IIqcRunInfo*>(newRunPtr.get());

		if (newIqcRunPtr == nullptr){
			errorMessage = "Failed to create new IQC run instance";
			return result;
		}

		newIqcRunPtr->SetBatchUuid(ingestRequest.Get_batchUuid().toUtf8());
		newIqcRunPtr->SetSystemId(systemId);
		newIqcRunPtr->SetExternalRunId(externalRunId);
		newIqcRunPtr->SetRunMode(prolifedata::IIqcRunInfo::RM_AUTOMATED);
		newIqcRunPtr->SetRunStatus(prolifedata::IIqcRunInfo::RS_COMPLETED);
		newIqcRunPtr->SetAnnotations(ingestRequest.Get_annotations());
		newIqcRunPtr->SetDefectCodes(ingestRequest.Get_defectCodes());

		QString runModeStr = ingestRequest.Get_runMode();
		if (!runModeStr.isEmpty()){
			prolifedata::IIqcRunInfo::RunMode runMode;
			if (prolifedata::IIqcRunInfo::ParseRunModeEnum(runModeStr.toUtf8(), runMode)){
				newIqcRunPtr->SetRunMode(runMode);
			}
		}

		QString runResultStr = ingestRequest.Get_runResult();
		if (!runResultStr.isEmpty()){
			prolifedata::IIqcRunInfo::RunResult runResult;
			if (prolifedata::IIqcRunInfo::ParseRunResultEnum(runResultStr.toUtf8(), runResult)){
				newIqcRunPtr->SetRunResult(runResult);
			}
		}

		QString dispositionStr = ingestRequest.Get_disposition();
		if (!dispositionStr.isEmpty()){
			prolifedata::IIqcRunInfo::Disposition disp;
			if (prolifedata::IIqcRunInfo::ParseDispositionEnum(dispositionStr.toUtf8(), disp)){
				newIqcRunPtr->SetDisposition(disp);
			}
		}

		if (m_iqcRunCollectionCompPtr != nullptr){
			m_iqcRunCollectionCompPtr->AddObject(newRunPtr.release());
		}
	}

	return result;
}


prolifedata::IIqcRunInfo* CIqcIngestControllerComp::FindExistingRun(
			const QString& systemId,
			const QString& externalRunId) const
{
	if (m_iqcRunCollectionCompPtr == nullptr){
		return nullptr;
	}

	imtbase::IObjectCollectionIterator* iterPtr = m_iqcRunCollectionCompPtr->CreateIterator();
	if (iterPtr == nullptr){
		return nullptr;
	}

	while (iterPtr->IsValid()){
		prolifedata::IIqcRunInfo* runInfoPtr =
			dynamic_cast<prolifedata::IIqcRunInfo*>(iterPtr->GetCurrentObject());

		if (runInfoPtr != nullptr &&
			runInfoPtr->GetSystemId() == systemId &&
			runInfoPtr->GetExternalRunId() == externalRunId)
		{
			return runInfoPtr;
		}

		iterPtr->Next();
	}

	return nullptr;
}


} // namespace prolifegql
