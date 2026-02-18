#include <prolifedata/CSplitInAction.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace prolifedata
{


// public methods

CSplitInAction::CSplitInAction()
	:m_receivedCount(0)
{
}


CSplitInAction::CSplitInAction(const QByteArray& sourceLicenseId, int receivedCount)
	:m_receivedCount(receivedCount),
	m_sourceLicenseId(sourceLicenseId)
{
}


QByteArray CSplitInAction::GetSourceLicenseId() const
{
	return m_sourceLicenseId;
}


void CSplitInAction::SetSourceLicenseId(const QByteArray& sourceLicenseId)
{
	if (m_sourceLicenseId != sourceLicenseId){
		istd::CChangeNotifier notifier(this);

		m_sourceLicenseId = sourceLicenseId;
	}
}


int CSplitInAction::GetReceivedCount() const
{
	return m_receivedCount;
}


void CSplitInAction::SetReceivedCount(int receivedCount)
{
	if (m_receivedCount != receivedCount){
		istd::CChangeNotifier notifier(this);

		m_receivedCount = receivedCount;
	}
}


// reimplemented (iser::ISerializable)

bool CSplitInAction::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag sourceLicenseIdTag("SourceLicenseId", "Source License-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sourceLicenseIdTag);
	retVal = retVal && archive.Process(m_sourceLicenseId);
	retVal = retVal && archive.EndTag(sourceLicenseIdTag);

	iser::CArchiveTag receivedCountTag("ReceivedCount", "Received Count", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(receivedCountTag);
	retVal = retVal && archive.Process(m_receivedCount);
	retVal = retVal && archive.EndTag(receivedCountTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

bool CSplitInAction::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CSplitInAction* sourcePtr = dynamic_cast<const CSplitInAction*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_sourceLicenseId = sourcePtr->m_sourceLicenseId;
		m_receivedCount = sourcePtr->m_receivedCount;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CSplitInAction::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CSplitInAction());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CSplitInAction::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_sourceLicenseId.clear();
	m_receivedCount = 0;

	return true;
}


} // namespace prolifedata


