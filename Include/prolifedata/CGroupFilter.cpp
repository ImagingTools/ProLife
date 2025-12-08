#include <prolifedata/CGroupFilter.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace prolifedata
{


// public methods

// reimplemented (prolifedata::IGroupFilter)

QByteArray CGroupFilter::GetUserId() const
{
	return m_userId;
}


void CGroupFilter::SetUserId(const QByteArray& userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier notifier(this);
		
		m_userId = userId;
	}
}


QByteArrayList CGroupFilter::GetGroupIds() const
{
	return m_groupIds;
}


void CGroupFilter::SetGroupIds(const QByteArrayList& groupIds)
{
	if (m_groupIds != groupIds){
		istd::CChangeNotifier notifier(this);
		
		m_groupIds = groupIds;
	}
}


// reimplemented (iser::ISerializable)

bool CGroupFilter::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag userIdTag("UserId", "User-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);
	
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_groupIds, "GroupIds", "GroupIds");

	return retVal;
}


// reimplemented (istd::IChangeable)

int CGroupFilter::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CGroupFilter::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CGroupFilter* sourcePtr = dynamic_cast<const CGroupFilter*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = sourcePtr->m_userId;
		m_groupIds = sourcePtr->m_groupIds;
		
		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CGroupFilter::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CGroupFilter());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CGroupFilter::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_userId.clear();
	m_groupIds.clear();

	return true;
}


} // namespace prolifedata


