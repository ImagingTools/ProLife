#include <prolifedata/CDeviceInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtlic/CLicensedHardwareInstanceInfo.h>


namespace prolifedata
{


// public methods

// reimplemented (IDeviceInfo)

QByteArray CDeviceInfo::GetSerialNumber() const
{
	return m_serialNumber;
}


void CDeviceInfo::SetSerialNumber(const QByteArray& serialNumber)
{
	if (m_serialNumber != serialNumber){
		istd::CChangeNotifier changeNotifier(this);

		m_serialNumber = serialNumber;
	}
}


QByteArray CDeviceInfo::GetMacAddress() const
{
	return m_macAddress;
}


void CDeviceInfo::SetMacAddress(const QByteArray& macAddress)
{
	if (m_macAddress != macAddress){
		istd::CChangeNotifier changeNotifier(this);

		m_macAddress = macAddress;
	}
}


QString CDeviceInfo::GetDescription() const
{
	return m_description;
}


void CDeviceInfo::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


IDeviceInfo::DeviceStatus CDeviceInfo::GetDeviceStatus() const
{
	return m_status;
}


void CDeviceInfo::SetDeviceStatus(DeviceStatus status)
{
	if (m_status != status){
		istd::CChangeNotifier changeNotifier(this);

		m_status = status;
	}
}


// reimplemented (iser::IObject)

QByteArray CDeviceInfo::GetFactoryId() const
{
	return QByteArray("DeviceInfo");
}


// reimplemented (iser::ISerializable)

bool CDeviceInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag deviceTag("Device", "Device item", iser::CArchiveTag::TT_GROUP);
	retVal = archive.BeginTag(deviceTag);

	static iser::CArchiveTag serialNumberTag("SerialNumber", "Serial number", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(serialNumberTag);
	retVal = retVal && archive.Process(m_serialNumber);
	retVal = retVal && archive.EndTag(serialNumberTag);

	static iser::CArchiveTag macAddressTag("MacAddress", "Mac address", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(macAddressTag);
	retVal = retVal && archive.Process(m_macAddress);
	retVal = retVal && archive.EndTag(macAddressTag);

	retVal = retVal && archive.EndTag(deviceTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CDeviceInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CDeviceInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CDeviceInfo* sourcePtr = dynamic_cast<const CDeviceInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_serialNumber = sourcePtr->m_serialNumber;
		m_macAddress = sourcePtr->m_macAddress;

		return true;
	}

	return false;
}


istd::IChangeable* CDeviceInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CDeviceInfo> clonePtr(new CDeviceInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CDeviceInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_serialNumber.clear();
	m_macAddress.clear();

	return true;
}


} // namespace prolifedata


