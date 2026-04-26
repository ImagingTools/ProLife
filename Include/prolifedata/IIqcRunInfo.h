#pragma once


// ACF includes
#include <iser/IObject.h>


namespace prolifedata
{


class IIqcRunInfo: virtual public iser::IObject
{
public:
	enum RunMode
	{
		RM_MANUAL,
		RM_AUTOMATED,
		RM_HYBRID
	};

	enum RunStatus
	{
		RS_PENDING,
		RS_IN_PROGRESS,
		RS_COMPLETED,
		RS_ABORTED
	};

	enum RunResult
	{
		RR_NONE,
		RR_PASS,
		RR_FAIL,
		RR_HOLD,
		RR_PARTIAL_PASS
	};

	enum Disposition
	{
		DISP_NONE,
		DISP_RELEASED,
		DISP_QUARANTINED,
		DISP_RETURN_TO_SUPPLIER,
		DISP_SCRAPPED,
		DISP_USE_AS_IS
	};

	I_DECLARE_ENUM(RunMode, RM_MANUAL, RM_AUTOMATED, RM_HYBRID);
	I_DECLARE_ENUM(RunStatus, RS_PENDING, RS_IN_PROGRESS, RS_COMPLETED, RS_ABORTED);
	I_DECLARE_ENUM(RunResult, RR_NONE, RR_PASS, RR_FAIL, RR_HOLD, RR_PARTIAL_PASS);
	I_DECLARE_ENUM(Disposition, DISP_NONE, DISP_RELEASED, DISP_QUARANTINED, DISP_RETURN_TO_SUPPLIER, DISP_SCRAPPED, DISP_USE_AS_IS);

	/**
		Get the batch UUID this run is for.
	*/
	virtual QByteArray GetBatchUuid() const = 0;

	/**
		Set the batch UUID this run is for.
	*/
	virtual void SetBatchUuid(const QByteArray& batchUuid) = 0;

	/**
		Get the IQC template UUID used for this run (nullable).
	*/
	virtual QByteArray GetTemplateUuid() const = 0;

	/**
		Set the IQC template UUID used for this run.
	*/
	virtual void SetTemplateUuid(const QByteArray& templateUuid) = 0;

	/**
		Get the run mode (manual / automated / hybrid).
	*/
	virtual RunMode GetRunMode() const = 0;

	/**
		Set the run mode.
	*/
	virtual void SetRunMode(RunMode mode) = 0;

	/**
		Get the current status of the run.
	*/
	virtual RunStatus GetRunStatus() const = 0;

	/**
		Set the current status of the run.
	*/
	virtual void SetRunStatus(RunStatus status) = 0;

	/**
		Get the overall result.
	*/
	virtual RunResult GetRunResult() const = 0;

	/**
		Set the overall result.
	*/
	virtual void SetRunResult(RunResult result) = 0;

	/**
		Get the disposition decision.
	*/
	virtual Disposition GetDisposition() const = 0;

	/**
		Set the disposition decision.
	*/
	virtual void SetDisposition(Disposition disposition) = 0;

	/**
		Get the operator user ID (nullable; empty for fully automated runs).
	*/
	virtual QByteArray GetOperatorUserId() const = 0;

	/**
		Set the operator user ID.
	*/
	virtual void SetOperatorUserId(const QByteArray& userId) = 0;

	/**
		Get the external system ID for automated inspection systems.
	*/
	virtual QString GetSystemId() const = 0;

	/**
		Set the external system ID.
	*/
	virtual void SetSystemId(const QString& systemId) = 0;

	/**
		Get the external run ID assigned by the automated inspection system.
		Used for idempotent upsert operations.
	*/
	virtual QString GetExternalRunId() const = 0;

	/**
		Set the external run ID.
	*/
	virtual void SetExternalRunId(const QString& externalRunId) = 0;

	/**
		Get start timestamp as ISO8601 string.
	*/
	virtual QString GetStartedAt() const = 0;

	/**
		Set start timestamp.
	*/
	virtual void SetStartedAt(const QString& startedAt) = 0;

	/**
		Get completion timestamp as ISO8601 string.
	*/
	virtual QString GetCompletedAt() const = 0;

	/**
		Set completion timestamp.
	*/
	virtual void SetCompletedAt(const QString& completedAt) = 0;

	/**
		Get freetext annotations.
	*/
	virtual QString GetAnnotations() const = 0;

	/**
		Set freetext annotations.
	*/
	virtual void SetAnnotations(const QString& annotations) = 0;

	/**
		Get comma-separated defect codes / issue tags.
	*/
	virtual QString GetDefectCodes() const = 0;

	/**
		Set comma-separated defect codes / issue tags.
	*/
	virtual void SetDefectCodes(const QString& defectCodes) = 0;

	/**
		Get result items as a JSON array string.
		Each element is a JSON object with the IqcResultItem fields.
	*/
	virtual QString GetResultItemsJson() const = 0;

	/**
		Set result items from a JSON array string.
	*/
	virtual void SetResultItemsJson(const QString& resultItemsJson) = 0;
};


} // namespace prolifedata
