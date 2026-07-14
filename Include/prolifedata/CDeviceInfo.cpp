#include <prolifedata/CDeviceInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtcore/Version.h>


namespace prolifedata
{


// public methods

CDeviceInfo::CDeviceInfo() :
	m_status(DPS_NONE),
	m_internalUse(false)
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


QByteArray CDeviceInfo::GetEndCustomerId() const
{
	return m_endCustomerId;
}


void CDeviceInfo::SetEndCustomerId(const QByteArray& endCustomerId)
{
	if (m_endCustomerId != endCustomerId){
		istd::CChangeNotifier changeNotifier(this);

		m_endCustomerId = endCustomerId;
	}
}


QString CDeviceInfo::GetStation() const
{
	return m_station;
}


void CDeviceInfo::SetStation(const QString& station)
{
	if (m_station != station){
		istd::CChangeNotifier changeNotifier(this);

		m_station = station;
	}
}


QString CDeviceInfo::GetArea() const
{
	return m_area;
}


void CDeviceInfo::SetArea(const QString& area)
{
	if (m_area != area){
		istd::CChangeNotifier changeNotifier(this);

		m_area = area;
	}
}


bool CDeviceInfo::IsInternalUse() const
{
	return m_internalUse;
}


void CDeviceInfo::SetInternalUse(bool internalUse)
{
	if (m_internalUse != internalUse){
		istd::CChangeNotifier changeNotifier(this);

		m_internalUse = internalUse;
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

	iser::CArchiveTag projectTag("Project", "Project", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(projectTag);
	retVal = retVal && archive.Process(m_project);
	retVal = retVal && archive.EndTag(projectTag);

	iser::CArchiveTag statusTag("Status", "Device status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(statusTag);
	retVal = retVal && I_SERIALIZE_ENUM(DeviceProductionStatus, archive, m_status);
	retVal = retVal && archive.EndTag(statusTag);

	if (imtCoreVersion >= 15177){
		iser::CArchiveTag internalUseTag("InternalUse", "Internal Use", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(internalUseTag);
		retVal = retVal && archive.Process(m_internalUse);
		retVal = retVal && archive.EndTag(internalUseTag);
	}

	iser::CArchiveTag endCustomerDataTag("EndCustomerData", "End customer data", iser::CArchiveTag::TT_GROUP);
	if (archive.IsStoring()){
		retVal = retVal && archive.BeginTag(endCustomerDataTag);

		iser::CArchiveTag endCustomerIdTag("EndCustomerId", "End customer ID", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(endCustomerIdTag);
		retVal = retVal && archive.Process(m_endCustomerId);
		retVal = retVal && archive.EndTag(endCustomerIdTag);

		iser::CArchiveTag stationTag("Station", "Station", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(stationTag);
		retVal = retVal && archive.Process(m_station);
		retVal = retVal && archive.EndTag(stationTag);

		iser::CArchiveTag areaTag("Area", "Area", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(areaTag);
		retVal = retVal && archive.Process(m_area);
		retVal = retVal && archive.EndTag(areaTag);

		retVal = retVal && archive.EndTag(endCustomerDataTag);
	}
	else if (archive.BeginTag(endCustomerDataTag)){
		iser::CArchiveTag endCustomerIdTag("EndCustomerId", "End customer ID", iser::CArchiveTag::TT_LEAF);
		bool endCustomerDataReadOk = archive.BeginTag(endCustomerIdTag)
			&& archive.Process(m_endCustomerId)
			&& archive.EndTag(endCustomerIdTag);

		iser::CArchiveTag stationTag("Station", "Station", iser::CArchiveTag::TT_LEAF);
		endCustomerDataReadOk = endCustomerDataReadOk
			&& archive.BeginTag(stationTag)
			&& archive.Process(m_station)
			&& archive.EndTag(stationTag);

		iser::CArchiveTag areaTag("Area", "Area", iser::CArchiveTag::TT_LEAF);
		endCustomerDataReadOk = endCustomerDataReadOk
			&& archive.BeginTag(areaTag)
			&& archive.Process(m_area)
			&& archive.EndTag(areaTag);

		archive.EndTag(endCustomerDataTag);
		if (!endCustomerDataReadOk){
			m_endCustomerId.clear();
			m_station.clear();
			m_area.clear();
		}
	}

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
		m_endCustomerId = sourcePtr->m_endCustomerId;
		m_station = sourcePtr->m_station;
		m_area = sourcePtr->m_area;
		m_status = sourcePtr->m_status;
		m_internalUse = sourcePtr->m_internalUse;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CDeviceInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CDeviceInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
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
	m_endCustomerId.clear();
	m_station.clear();
	m_area.clear();
	m_internalUse = false;
	m_status = DPS_NONE;

	return true;
}


} // namespace prolifedata
