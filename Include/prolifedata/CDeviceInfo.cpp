#include <prolifedata/CDeviceInfo.h>


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

CDeviceInfo::CDeviceInfo() :
	m_status(DPS_NONE)
{

}


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

QByteArray CDeviceInfo::GetDeviceType() const
{
	return m_deviceType;
}


void CDeviceInfo::SetDeviceType(const QByteArray& deviceType)
{
	if (m_deviceType != deviceType){
		istd::CChangeNotifier changeNotifier(this);

		m_deviceType = deviceType;
	}
}


QByteArray CDeviceInfo::GetConfigurationType() const
{
	return m_configurationType;
}


void CDeviceInfo::SetConfigurationType(const QByteArray& configurationType)
{
	if (m_configurationType != configurationType){
		istd::CChangeNotifier changeNotifier(this);

		m_configurationType = configurationType;
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


IDeviceInfo::DeviceProductionStatus CDeviceInfo::GetDeviceProductionStatus() const
{
	return m_status;
}


void CDeviceInfo::SetDeviceProductionStatus(DeviceProductionStatus status)
{
	if (m_status != status){
		istd::CChangeNotifier changeNotifier(this);

		m_status = status;
	}
}


QByteArray CDeviceInfo::GetProject() const
{
	return m_project;
}


void CDeviceInfo::SetProject(const QByteArray& project)
{
	if (m_project != project){
		istd::CChangeNotifier changeNotifier(this);

		m_project = project;
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

	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();
	quint32 imtCoreVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion)){
		imtCoreVersion = 0;
	}

	bool retVal = true;

	iser::CArchiveTag serialNumberTag("SerialNumber", "Serial number", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(serialNumberTag);
	retVal = retVal && archive.Process(m_serialNumber);
	retVal = retVal && archive.EndTag(serialNumberTag);

	iser::CArchiveTag macAddressTag("MacAddress", "Mac address", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(macAddressTag);
	retVal = retVal && archive.Process(m_macAddress);
	retVal = retVal && archive.EndTag(macAddressTag);

	iser::CArchiveTag deviceTypeTag("DeviceType", "Device type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(deviceTypeTag);
	retVal = retVal && archive.Process(m_deviceType);
	retVal = retVal && archive.EndTag(deviceTypeTag);

	if (imtCoreVersion >= 7386){
		iser::CArchiveTag configurationTypeTag("ConfigurationType", "Configuration type", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(configurationTypeTag);
		retVal = retVal && archive.Process(m_configurationType);
		retVal = retVal && archive.EndTag(configurationTypeTag);
	}

	iser::CArchiveTag descriptionTag("Description", "Description of the device", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	if (imtCoreVersion >= 9644){
		iser::CArchiveTag projectTag("Project", "Project", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(projectTag);
		retVal = retVal && archive.Process(m_project);
		retVal = retVal && archive.EndTag(projectTag);
	}

	iser::CArchiveTag statusTag("Status", "Device status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	retVal = retVal && I_SERIALIZE_ENUM(DeviceProductionStatus, archive, m_status);
	retVal = retVal && archive.EndTag(statusTag);

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
		m_deviceType = sourcePtr->m_deviceType;
		m_configurationType = sourcePtr->m_configurationType;
		m_description = sourcePtr->m_description;
		m_project = sourcePtr->m_project;
		m_status = sourcePtr->m_status;

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
	m_deviceType.clear();
	m_configurationType.clear();
	m_description.clear();
	m_project.clear();
	m_status = DPS_NONE;

	return true;
}


} // namespace prolifedata


