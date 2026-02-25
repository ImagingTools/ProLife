#include <prolifedata/CRevokeInAction.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace prolifedata
{


// public methods

CRevokeInAction::CRevokeInAction()
	:m_remainingCount(0)
{
}


CRevokeInAction::CRevokeInAction(const QByteArray& childLicenseId, int remainingCount)
	:m_childLicenseId(childLicenseId),
	m_remainingCount(remainingCount)
{
}


QByteArray CRevokeInAction::GetChildLicenseId() const
{
	return m_childLicenseId;
}


void CRevokeInAction::SetChildLicenseId(const QByteArray& childLicenseId)
{
	if (m_childLicenseId != childLicenseId){
		istd::CChangeNotifier notifier(this);

		m_childLicenseId = childLicenseId;
	}
}


int CRevokeInAction::GetRemainingCount() const
{
	return m_remainingCount;
}


void CRevokeInAction::SetRemainingCount(int remainingCount)
{
	if (m_remainingCount != remainingCount){
		istd::CChangeNotifier notifier(this);

		m_remainingCount = remainingCount;
	}
}


// reimplemented (iser::ISerializable)

bool CRevokeInAction::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag childLicenseIdTag("ChildLicenseId", "Child License-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(childLicenseIdTag);
	retVal = retVal && archive.Process(m_childLicenseId);
	retVal = retVal && archive.EndTag(childLicenseIdTag);

	iser::CArchiveTag remainingCountTag("RemainingCount", "Remaining Count", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(remainingCountTag);
	retVal = retVal && archive.Process(m_remainingCount);
	retVal = retVal && archive.EndTag(remainingCountTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CRevokeInAction::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CRevokeInAction* sourcePtr = dynamic_cast<const CRevokeInAction*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_childLicenseId = sourcePtr->m_childLicenseId;
		m_remainingCount = sourcePtr->m_remainingCount;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CRevokeInAction::CloneMe(CompatibilityMode mode) const 
{
	istd::IChangeableUniquePtr clonePtr(new CRevokeInAction());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CRevokeInAction::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_childLicenseId.clear();
	m_remainingCount = 0;

	return true;
}


} // namespace prolifedata


