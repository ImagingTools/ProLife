#include <prolifedata/CIqcTemplateInfo.h>


// ACF includes
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>
#include <iser/IArchive.h>
#include <istd/CChangeGroup.h>
#include <istd/CChangeNotifier.h>


namespace prolifedata
{


// public methods

QByteArray CIqcTemplateInfo::GetTypeId()
{
	return "IqcTemplateInfo";
}


CIqcTemplateInfo::CIqcTemplateInfo():
	m_version(1),
	m_active(true),
	m_completionRule(CR_ALL_REQUIRED_DONE)
{
}


// reimplemented (IIqcTemplateInfo)

QByteArray CIqcTemplateInfo::GetSupplierId() const
{
	return m_supplierId;
}


void CIqcTemplateInfo::SetSupplierId(const QByteArray& supplierId)
{
	if (m_supplierId != supplierId){
		istd::CChangeNotifier changeNotifier(this);

		m_supplierId = supplierId;
	}
}


QString CIqcTemplateInfo::GetComponentId() const
{
	return m_componentId;
}


void CIqcTemplateInfo::SetComponentId(const QString& componentId)
{
	if (m_componentId != componentId){
		istd::CChangeNotifier changeNotifier(this);

		m_componentId = componentId;
	}
}


QString CIqcTemplateInfo::GetTemplateName() const
{
	return m_name;
}


void CIqcTemplateInfo::SetTemplateName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier changeNotifier(this);

		m_name = name;
	}
}


int CIqcTemplateInfo::GetVersion() const
{
	return m_version;
}


void CIqcTemplateInfo::SetVersion(int version)
{
	if (m_version != version){
		istd::CChangeNotifier changeNotifier(this);

		m_version = version;
	}
}


bool CIqcTemplateInfo::IsActive() const
{
	return m_active;
}


void CIqcTemplateInfo::SetActive(bool active)
{
	if (m_active != active){
		istd::CChangeNotifier changeNotifier(this);

		m_active = active;
	}
}


IIqcTemplateInfo::CompletionRule CIqcTemplateInfo::GetCompletionRule() const
{
	return m_completionRule;
}


void CIqcTemplateInfo::SetCompletionRule(CompletionRule rule)
{
	if (m_completionRule != rule){
		istd::CChangeNotifier changeNotifier(this);

		m_completionRule = rule;
	}
}


QString CIqcTemplateInfo::GetPassRuleDescription() const
{
	return m_passRuleDescription;
}


void CIqcTemplateInfo::SetPassRuleDescription(const QString& passRuleDescription)
{
	if (m_passRuleDescription != passRuleDescription){
		istd::CChangeNotifier changeNotifier(this);

		m_passRuleDescription = passRuleDescription;
	}
}


QString CIqcTemplateInfo::GetChecklistItemsJson() const
{
	return m_checklistItemsJson;
}


void CIqcTemplateInfo::SetChecklistItemsJson(const QString& checklistItemsJson)
{
	if (m_checklistItemsJson != checklistItemsJson){
		istd::CChangeNotifier changeNotifier(this);

		m_checklistItemsJson = checklistItemsJson;
	}
}


// reimplemented (iser::IObject)

QByteArray CIqcTemplateInfo::GetFactoryId() const
{
	return QByteArray();
}


// reimplemented (iser::ISerializable)

bool CIqcTemplateInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag nameTag("Name", "Template name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	iser::CArchiveTag supplierIdTag("SupplierId", "Supplier ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(supplierIdTag);
	retVal = retVal && archive.Process(m_supplierId);
	retVal = retVal && archive.EndTag(supplierIdTag);

	iser::CArchiveTag componentIdTag("ComponentId", "Component ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(componentIdTag);
	retVal = retVal && archive.Process(m_componentId);
	retVal = retVal && archive.EndTag(componentIdTag);

	iser::CArchiveTag versionTag("Version", "Version", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(versionTag);
	retVal = retVal && archive.Process(m_version);
	retVal = retVal && archive.EndTag(versionTag);

	iser::CArchiveTag activeTag("Active", "Active", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(activeTag);
	retVal = retVal && archive.Process(m_active);
	retVal = retVal && archive.EndTag(activeTag);

	iser::CArchiveTag completionRuleTag("CompletionRule", "Completion rule", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(completionRuleTag);
	retVal = retVal && I_SERIALIZE_ENUM(CompletionRule, archive, m_completionRule);
	retVal = retVal && archive.EndTag(completionRuleTag);

	iser::CArchiveTag passRuleTag("PassRuleDescription", "Pass rule description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(passRuleTag);
	retVal = retVal && archive.Process(m_passRuleDescription);
	retVal = retVal && archive.EndTag(passRuleTag);

	iser::CArchiveTag checklistItemsTag("ChecklistItems", "Checklist items JSON", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(checklistItemsTag);
	retVal = retVal && archive.Process(m_checklistItemsJson);
	retVal = retVal && archive.EndTag(checklistItemsTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CIqcTemplateInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CIqcTemplateInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CIqcTemplateInfo* sourcePtr = dynamic_cast<const CIqcTemplateInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_supplierId = sourcePtr->m_supplierId;
		m_componentId = sourcePtr->m_componentId;
		m_name = sourcePtr->m_name;
		m_version = sourcePtr->m_version;
		m_active = sourcePtr->m_active;
		m_completionRule = sourcePtr->m_completionRule;
		m_passRuleDescription = sourcePtr->m_passRuleDescription;
		m_checklistItemsJson = sourcePtr->m_checklistItemsJson;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CIqcTemplateInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CIqcTemplateInfo());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CIqcTemplateInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_supplierId.clear();
	m_componentId.clear();
	m_name.clear();
	m_version = 1;
	m_active = true;
	m_completionRule = CR_ALL_REQUIRED_DONE;
	m_passRuleDescription.clear();
	m_checklistItemsJson.clear();

	return true;
}


} // namespace prolifedata
