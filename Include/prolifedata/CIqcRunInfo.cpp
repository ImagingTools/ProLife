#include <prolifedata/CIqcRunInfo.h>


// ACF includes
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iser/IArchive.h>
#include <istd/CChangeGroup.h>
#include <istd/CChangeNotifier.h>


namespace prolifedata
{


// public methods

QByteArray CIqcRunInfo::GetTypeId()
{
	return "IqcRunInfo";
}


CIqcRunInfo::CIqcRunInfo():
	m_runMode(RM_MANUAL),
	m_runStatus(RS_PENDING),
	m_runResult(RR_NONE),
	m_disposition(DISP_NONE)
{
}


// reimplemented (IIqcRunInfo)

QByteArray CIqcRunInfo::GetBatchUuid() const
{
	return m_batchUuid;
}


void CIqcRunInfo::SetBatchUuid(const QByteArray& batchUuid)
{
	if (m_batchUuid != batchUuid){
		istd::CChangeNotifier changeNotifier(this);

		m_batchUuid = batchUuid;
	}
}


QByteArray CIqcRunInfo::GetTemplateUuid() const
{
	return m_templateUuid;
}


void CIqcRunInfo::SetTemplateUuid(const QByteArray& templateUuid)
{
	if (m_templateUuid != templateUuid){
		istd::CChangeNotifier changeNotifier(this);

		m_templateUuid = templateUuid;
	}
}


IIqcRunInfo::RunMode CIqcRunInfo::GetRunMode() const
{
	return m_runMode;
}


void CIqcRunInfo::SetRunMode(RunMode mode)
{
	if (m_runMode != mode){
		istd::CChangeNotifier changeNotifier(this);

		m_runMode = mode;
	}
}


IIqcRunInfo::RunStatus CIqcRunInfo::GetRunStatus() const
{
	return m_runStatus;
}


void CIqcRunInfo::SetRunStatus(RunStatus status)
{
	if (m_runStatus != status){
		istd::CChangeNotifier changeNotifier(this);

		m_runStatus = status;
	}
}


IIqcRunInfo::RunResult CIqcRunInfo::GetRunResult() const
{
	return m_runResult;
}


void CIqcRunInfo::SetRunResult(RunResult result)
{
	if (m_runResult != result){
		istd::CChangeNotifier changeNotifier(this);

		m_runResult = result;
	}
}


IIqcRunInfo::Disposition CIqcRunInfo::GetDisposition() const
{
	return m_disposition;
}


void CIqcRunInfo::SetDisposition(Disposition disposition)
{
	if (m_disposition != disposition){
		istd::CChangeNotifier changeNotifier(this);

		m_disposition = disposition;
	}
}


QByteArray CIqcRunInfo::GetOperatorUserId() const
{
	return m_operatorUserId;
}


void CIqcRunInfo::SetOperatorUserId(const QByteArray& userId)
{
	if (m_operatorUserId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_operatorUserId = userId;
	}
}


QString CIqcRunInfo::GetSystemId() const
{
	return m_systemId;
}


void CIqcRunInfo::SetSystemId(const QString& systemId)
{
	if (m_systemId != systemId){
		istd::CChangeNotifier changeNotifier(this);

		m_systemId = systemId;
	}
}


QString CIqcRunInfo::GetExternalRunId() const
{
	return m_externalRunId;
}


void CIqcRunInfo::SetExternalRunId(const QString& externalRunId)
{
	if (m_externalRunId != externalRunId){
		istd::CChangeNotifier changeNotifier(this);

		m_externalRunId = externalRunId;
	}
}


QString CIqcRunInfo::GetStartedAt() const
{
	return m_startedAt;
}


void CIqcRunInfo::SetStartedAt(const QString& startedAt)
{
	if (m_startedAt != startedAt){
		istd::CChangeNotifier changeNotifier(this);

		m_startedAt = startedAt;
	}
}


QString CIqcRunInfo::GetCompletedAt() const
{
	return m_completedAt;
}


void CIqcRunInfo::SetCompletedAt(const QString& completedAt)
{
	if (m_completedAt != completedAt){
		istd::CChangeNotifier changeNotifier(this);

		m_completedAt = completedAt;
	}
}


QString CIqcRunInfo::GetAnnotations() const
{
	return m_annotations;
}


void CIqcRunInfo::SetAnnotations(const QString& annotations)
{
	if (m_annotations != annotations){
		istd::CChangeNotifier changeNotifier(this);

		m_annotations = annotations;
	}
}


QString CIqcRunInfo::GetDefectCodes() const
{
	return m_defectCodes;
}


void CIqcRunInfo::SetDefectCodes(const QString& defectCodes)
{
	if (m_defectCodes != defectCodes){
		istd::CChangeNotifier changeNotifier(this);

		m_defectCodes = defectCodes;
	}
}


// reimplemented (iser::IObject)

QByteArray CIqcRunInfo::GetFactoryId() const
{
	return QByteArray();
}


// reimplemented (iser::ISerializable)

bool CIqcRunInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag batchUuidTag("BatchUuid", "Batch UUID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(batchUuidTag);
	retVal = retVal && archive.Process(m_batchUuid);
	retVal = retVal && archive.EndTag(batchUuidTag);

	iser::CArchiveTag templateUuidTag("TemplateUuid", "Template UUID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(templateUuidTag);
	retVal = retVal && archive.Process(m_templateUuid);
	retVal = retVal && archive.EndTag(templateUuidTag);

	iser::CArchiveTag runModeTag("RunMode", "Run mode", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(runModeTag);
	retVal = retVal && I_SERIALIZE_ENUM(RunMode, archive, m_runMode);
	retVal = retVal && archive.EndTag(runModeTag);

	iser::CArchiveTag runStatusTag("RunStatus", "Run status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(runStatusTag);
	retVal = retVal && I_SERIALIZE_ENUM(RunStatus, archive, m_runStatus);
	retVal = retVal && archive.EndTag(runStatusTag);

	iser::CArchiveTag runResultTag("RunResult", "Run result", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(runResultTag);
	retVal = retVal && I_SERIALIZE_ENUM(RunResult, archive, m_runResult);
	retVal = retVal && archive.EndTag(runResultTag);

	iser::CArchiveTag dispositionTag("Disposition", "Disposition", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(dispositionTag);
	retVal = retVal && I_SERIALIZE_ENUM(Disposition, archive, m_disposition);
	retVal = retVal && archive.EndTag(dispositionTag);

	iser::CArchiveTag operatorUserIdTag("OperatorUserId", "Operator user ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(operatorUserIdTag);
	retVal = retVal && archive.Process(m_operatorUserId);
	retVal = retVal && archive.EndTag(operatorUserIdTag);

	iser::CArchiveTag systemIdTag("SystemId", "System ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(systemIdTag);
	retVal = retVal && archive.Process(m_systemId);
	retVal = retVal && archive.EndTag(systemIdTag);

	iser::CArchiveTag externalRunIdTag("ExternalRunId", "External run ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(externalRunIdTag);
	retVal = retVal && archive.Process(m_externalRunId);
	retVal = retVal && archive.EndTag(externalRunIdTag);

	iser::CArchiveTag startedAtTag("StartedAt", "Started at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(startedAtTag);
	retVal = retVal && archive.Process(m_startedAt);
	retVal = retVal && archive.EndTag(startedAtTag);

	iser::CArchiveTag completedAtTag("CompletedAt", "Completed at", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(completedAtTag);
	retVal = retVal && archive.Process(m_completedAt);
	retVal = retVal && archive.EndTag(completedAtTag);

	iser::CArchiveTag annotationsTag("Annotations", "Annotations", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(annotationsTag);
	retVal = retVal && archive.Process(m_annotations);
	retVal = retVal && archive.EndTag(annotationsTag);

	iser::CArchiveTag defectCodesTag("DefectCodes", "Defect codes", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(defectCodesTag);
	retVal = retVal && archive.Process(m_defectCodes);
	retVal = retVal && archive.EndTag(defectCodesTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CIqcRunInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CIqcRunInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CIqcRunInfo* sourcePtr = dynamic_cast<const CIqcRunInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_batchUuid = sourcePtr->m_batchUuid;
		m_templateUuid = sourcePtr->m_templateUuid;
		m_runMode = sourcePtr->m_runMode;
		m_runStatus = sourcePtr->m_runStatus;
		m_runResult = sourcePtr->m_runResult;
		m_disposition = sourcePtr->m_disposition;
		m_operatorUserId = sourcePtr->m_operatorUserId;
		m_systemId = sourcePtr->m_systemId;
		m_externalRunId = sourcePtr->m_externalRunId;
		m_startedAt = sourcePtr->m_startedAt;
		m_completedAt = sourcePtr->m_completedAt;
		m_annotations = sourcePtr->m_annotations;
		m_defectCodes = sourcePtr->m_defectCodes;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CIqcRunInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CIqcRunInfo());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CIqcRunInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_batchUuid.clear();
	m_templateUuid.clear();
	m_runMode = RM_MANUAL;
	m_runStatus = RS_PENDING;
	m_runResult = RR_NONE;
	m_disposition = DISP_NONE;
	m_operatorUserId.clear();
	m_systemId.clear();
	m_externalRunId.clear();
	m_startedAt.clear();
	m_completedAt.clear();
	m_annotations.clear();
	m_defectCodes.clear();

	return true;
}


} // namespace prolifedata
