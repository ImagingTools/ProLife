#include <prolifegql/CIqcIngestControllerComp.h>


// Qt includes
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>


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

	// Build the result items JSON from the ingest input
	QString resultItemsJson;
	{
		QJsonArray itemsArray;
		const auto& resultItems = ingestRequest.Get_resultItems();
		if (resultItems){
			for (const auto& item : *resultItems){
				QJsonObject obj;
				obj["name"] = item.Get_name();
				obj["valueType"] = item.Get_valueType();
				obj["valueText"] = item.Get_valueText();
				obj["passResult"] = item.Get_passResult();
				obj["unit"] = item.Get_unit();
				obj["evidenceRefs"] = item.Get_evidenceRefs();
				if (!item.Get_id().isEmpty()){
					obj["id"] = item.Get_id();
				}
				if (!item.Get_templateItemUuid().isEmpty()){
					obj["templateItemUuid"] = item.Get_templateItemUuid();
				}
				itemsArray.append(obj);
			}
		}
		resultItemsJson = QString::fromUtf8(
			QJsonDocument(itemsArray).toJson(QJsonDocument::Compact));
	}

	// Try to find an existing run with the same (systemId, externalRunId) key (idempotent upsert)
	prolifedata::IIqcRunInfo* existingRunPtr = FindExistingRun(systemId, externalRunId);

	// Affected run ID; populated when an existing run is found and updated.
	// For newly created runs the ID is only known after the document manager saves.
	QByteArray affectedRunId;

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

		if (!resultItemsJson.isEmpty()){
			existingRunPtr->SetResultItemsJson(resultItemsJson);
		}

		existingRunPtr->SetRunStatus(prolifedata::IIqcRunInfo::RS_COMPLETED);

		const iser::IObject* objPtr = dynamic_cast<const iser::IObject*>(existingRunPtr);
		if (objPtr != nullptr){
			affectedRunId = objPtr->GetObjectId();
		}
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
		newIqcRunPtr->SetResultItemsJson(resultItemsJson);

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

	// Populate the response payload so clients know which run was affected
	sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0& response =
		result.Version_1_0.emplace();
	if (!affectedRunId.isEmpty()){
		response.Set_id(affectedRunId);
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

	// NOTE: This iterates over the collection loaded in memory. The database migration
	// (migration_20.sql) creates a unique index on (Document->>'SystemId', Document->>'ExternalRunId')
	// which enforces uniqueness at the DB level. For high-volume scenarios, callers should
	// pre-filter the collection using the SystemId/ExternalRunId filter params supported by
	// CIqcRunDatabaseDelegateComp::CreateAdditionalFiltersQuery.
	std::unique_ptr<imtbase::IObjectCollectionIterator> iterPtr(
		m_iqcRunCollectionCompPtr->CreateIterator());

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
