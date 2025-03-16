#include <prolifedata/CSoftwareTransferInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace prolifedata
{


// public methods

CSoftwareTransferInfo::CSoftwareTransferInfo()
	:m_transferCount(0)
{
}


// reimplemented (prolifedata::ISoftwareTransfer)

QByteArray CSoftwareTransferInfo::GetSoftwareId() const
{
	return m_softwareId;
}


void CSoftwareTransferInfo::SetSoftwareId(const QByteArray& softwareId)
{
	if (m_softwareId != softwareId){
		istd::CChangeNotifier notifier(this);
		
		m_softwareId = softwareId;
	}
}


int CSoftwareTransferInfo::GetTransferCount() const
{
	return m_transferCount;
}


void CSoftwareTransferInfo::SetTransferCount(int transferCount)
{
	if (m_transferCount != transferCount){
		istd::CChangeNotifier notifier(this);
		
		m_transferCount = transferCount;
	}
}


// reimplemented (iser::ISerializable)

bool CSoftwareTransferInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);
	
	bool retVal = true;

	iser::CArchiveTag softwareIdTag("SoftwareId", "Software-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(softwareIdTag);
	retVal = retVal && archive.Process(m_softwareId);
	retVal = retVal && archive.EndTag(softwareIdTag);
	
	iser::CArchiveTag transferCountTag("TransferCount", "Transfer count", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(transferCountTag);
	retVal = retVal && archive.Process(m_transferCount);
	retVal = retVal && archive.EndTag(transferCountTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CSoftwareTransferInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CSoftwareTransferInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CSoftwareTransferInfo* sourcePtr = dynamic_cast<const CSoftwareTransferInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_softwareId = sourcePtr->m_softwareId;
		m_transferCount = sourcePtr->m_transferCount;

		return true;
	}

	return false;
}


istd::IChangeable* CSoftwareTransferInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CSoftwareTransferInfo> clonePtr(new CSoftwareTransferInfo());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CSoftwareTransferInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_softwareId.clear();
	m_transferCount = 0;

	return true;
}


} // namespace prolifedata


