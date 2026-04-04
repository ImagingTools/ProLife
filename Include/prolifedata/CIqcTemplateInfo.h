#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>

// ProLife includes
#include <prolifedata/IIqcTemplateInfo.h>


namespace prolifedata
{


class CIqcTemplateInfo: virtual public IIqcTemplateInfo
{
public:
	static QByteArray GetTypeId();

	CIqcTemplateInfo();

	// reimplemented (IIqcTemplateInfo)
	virtual QByteArray GetSupplierId() const override;
	virtual void SetSupplierId(const QByteArray& supplierId) override;
	virtual QString GetComponentId() const override;
	virtual void SetComponentId(const QString& componentId) override;
	virtual QString GetTemplateName() const override;
	virtual void SetTemplateName(const QString& name) override;
	virtual int GetVersion() const override;
	virtual void SetVersion(int version) override;
	virtual bool IsActive() const override;
	virtual void SetActive(bool active) override;
	virtual CompletionRule GetCompletionRule() const override;
	virtual void SetCompletionRule(CompletionRule rule) override;
	virtual QString GetPassRuleDescription() const override;
	virtual void SetPassRuleDescription(const QString& passRuleDescription) override;
	virtual QString GetChecklistItemsJson() const override;
	virtual void SetChecklistItemsJson(const QString& checklistItemsJson) override;

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
	QByteArray m_supplierId;
	QString m_componentId;
	QString m_name;
	int m_version;
	bool m_active;
	CompletionRule m_completionRule;
	QString m_passRuleDescription;
	QString m_checklistItemsJson;
};


typedef imtbase::TIdentifiableWrap<CIqcTemplateInfo> CIdentifiableIqcTemplateInfo;


} // namespace prolifedata
