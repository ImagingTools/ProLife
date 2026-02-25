#include <prolifedata/CSplitOutAction.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace prolifedata
{


// public methods

CSplitOutAction::CSplitOutAction()
	:m_initialCount(0),
	m_movedCount(0)
{
}


CSplitOutAction::CSplitOutAction(const QByteArray& newLicenseId, int initialCount, int movedCount)
	:m_newLicenseId(newLicenseId),
	m_initialCount(initialCount),
	m_movedCount(movedCount)
{
}


QByteArray CSplitOutAction::GetNewLicenseId() const
{
	return m_newLicenseId;
}


void CSplitOutAction::SetNewLicenseId(const QByteArray& newLicenseId)
{
	if (m_newLicenseId != newLicenseId){
		istd::CChangeNotifier notifier(this);

		m_newLicenseId = newLicenseId;
	}
}


int CSplitOutAction::GetInitialCount() const
{
	return m_initialCount;
}


void CSplitOutAction::SetInitialCount(int initialCount)
{
	if (m_initialCount != initialCount){
		istd::CChangeNotifier notifier(this);

		m_initialCount = initialCount;
	}
}


int CSplitOutAction::GetMovedCount() const
{
	return m_movedCount;
}


void CSplitOutAction::SetMovedCount(int movedCount)
{
	if (m_movedCount != movedCount){
		istd::CChangeNotifier notifier(this);

		m_movedCount = movedCount;
	}
}


// reimplemented (iser::ISerializable)

bool CSplitOutAction::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag newLicenseIdTag("NewLicenseId", "New License-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(newLicenseIdTag);
	retVal = retVal && archive.Process(m_newLicenseId);
	retVal = retVal && archive.EndTag(newLicenseIdTag);

	iser::CArchiveTag initialCountTag("InitialCount", "Initial Count", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(initialCountTag);
	retVal = retVal && archive.Process(m_initialCount);
	retVal = retVal && archive.EndTag(initialCountTag);

	iser::CArchiveTag movedCountTag("MovedCount", "Moved Count", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(movedCountTag);
	retVal = retVal && archive.Process(m_movedCount);
	retVal = retVal && archive.EndTag(movedCountTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CSplitOutAction::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CSplitOutAction* sourcePtr = dynamic_cast<const CSplitOutAction*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_newLicenseId = sourcePtr->m_newLicenseId;
		m_initialCount = sourcePtr->m_initialCount;
		m_movedCount = sourcePtr->m_movedCount;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CSplitOutAction::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CSplitOutAction());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CSplitOutAction::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_newLicenseId.clear();
	m_initialCount = 0;
	m_movedCount = 0;

	return true;
}


} // namespace prolifedata


