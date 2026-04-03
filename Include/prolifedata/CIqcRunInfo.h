#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>

// ProLife includes
#include <prolifedata/IIqcRunInfo.h>
#include <prolifedata/IIqcResultItemInfo.h>


namespace prolifedata
{


class CIqcRunInfo: virtual public IIqcRunInfo
{
public:
	static QByteArray GetTypeId();

	CIqcRunInfo();

	// reimplemented (IIqcRunInfo)
	virtual QByteArray GetBatchUuid() const override;
	virtual void SetBatchUuid(const QByteArray& batchUuid) override;
	virtual QByteArray GetTemplateUuid() const override;
	virtual void SetTemplateUuid(const QByteArray& templateUuid) override;
	virtual RunMode GetRunMode() const override;
	virtual void SetRunMode(RunMode mode) override;
	virtual RunStatus GetRunStatus() const override;
	virtual void SetRunStatus(RunStatus status) override;
	virtual RunResult GetRunResult() const override;
	virtual void SetRunResult(RunResult result) override;
	virtual Disposition GetDisposition() const override;
	virtual void SetDisposition(Disposition disposition) override;
	virtual QByteArray GetOperatorUserId() const override;
	virtual void SetOperatorUserId(const QByteArray& userId) override;
	virtual QString GetSystemId() const override;
	virtual void SetSystemId(const QString& systemId) override;
	virtual QString GetExternalRunId() const override;
	virtual void SetExternalRunId(const QString& externalRunId) override;
	virtual QString GetStartedAt() const override;
	virtual void SetStartedAt(const QString& startedAt) override;
	virtual QString GetCompletedAt() const override;
	virtual void SetCompletedAt(const QString& completedAt) override;
	virtual QString GetAnnotations() const override;
	virtual void SetAnnotations(const QString& annotations) override;
	virtual QString GetDefectCodes() const override;
	virtual void SetDefectCodes(const QString& defectCodes) override;
	virtual imtbase::IObjectCollection* GetResultItems() override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_batchUuid;
	QByteArray m_templateUuid;
	RunMode m_runMode;
	RunStatus m_runStatus;
	RunResult m_runResult;
	Disposition m_disposition;
	QByteArray m_operatorUserId;
	QString m_systemId;
	QString m_externalRunId;
	QString m_startedAt;
	QString m_completedAt;
	QString m_annotations;
	QString m_defectCodes;

	imtbase::CObjectCollection m_resultItemCollection;
};


typedef imtbase::TIdentifiableWrap<CIqcRunInfo> CIdentifiableIqcRunInfo;


} // namespace prolifedata
