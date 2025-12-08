#include <prolifedata/CHardwareProductBinding.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ProLife include
#include <prolife/Version.h>


namespace prolifedata
{


// public methods

// reimplemented (IHardwareProductBinding)

QByteArray CHardwareProductBinding::GetHardwareId() const
{
	return m_hardwareId;
}


void CHardwareProductBinding::SetHardwareId(const QByteArray& hardwareId)
{
	if (m_hardwareId != hardwareId){
		istd::CChangeNotifier notifier(this);

		m_hardwareId = hardwareId;
	}
}


QByteArrayList CHardwareProductBinding::GetSoftwareIds() const
{
	return m_softwareIds;
}


void CHardwareProductBinding::SetSoftwareIds(QByteArrayList softwareIds)
{
	if (m_softwareIds != softwareIds){
		istd::CChangeNotifier notifier(this);

		m_softwareIds = softwareIds;
	}
}


bool CHardwareProductBinding::Bind(const QByteArray& softwareId)
{
	if (!m_softwareIds.contains(softwareId)){
		istd::CChangeNotifier notifier(this);

		m_softwareIds << softwareId;

		return true;
	}

	return false;
}


bool CHardwareProductBinding::Unbind(const QByteArray& softwareId)
{
	if (m_softwareIds.contains(softwareId)){
		istd::CChangeNotifier notifier(this);

		m_softwareIds.removeAll(softwareId);

		return true;
	}

	return false;
}


// reimplemented (iser::ISerializable)

bool CHardwareProductBinding::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag hardwareIdTag("HardwareId", "Hardware product-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(hardwareIdTag);
	retVal = retVal && archive.Process(m_hardwareId);
	retVal = retVal && archive.EndTag(hardwareIdTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_softwareIds, "SoftwareIds", "SoftwareId");

	return retVal;
}


// reimplemented (istd::IChangeable)

int CHardwareProductBinding::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CHardwareProductBinding::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CHardwareProductBinding* sourcePtr = dynamic_cast<const CHardwareProductBinding*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_hardwareId = sourcePtr->m_hardwareId;
		m_softwareIds = sourcePtr->m_softwareIds;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CHardwareProductBinding::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CHardwareProductBinding());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CHardwareProductBinding::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_hardwareId.clear();
	m_softwareIds.clear();

	return true;
}


} // namespace prolifedata


