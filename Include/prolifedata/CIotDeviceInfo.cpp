#include <prolifedata/CIotDeviceInfo.h>


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

CIotDeviceInfo::CIotDeviceInfo()
{
}


// reimplemented (IIotDeviceInfo)

QByteArray CIotDeviceInfo::GetFactoryNumber() const
{
	return m_factoryNumber;
}


void CIotDeviceInfo::SetFactoryNumber(const QByteArray& factoryNumber)
{
	if (m_factoryNumber != factoryNumber){
		istd::CChangeNotifier changeNotifier(this);

		m_factoryNumber = factoryNumber;
	}
}


QByteArray CIotDeviceInfo::GetModemNumber() const
{
	return m_modemNumber;
}


void CIotDeviceInfo::SetModemNumber(const QByteArray& modemNumber)
{
	if (m_modemNumber != modemNumber){
		istd::CChangeNotifier changeNotifier(this);

		m_modemNumber = modemNumber;
	}
}


QString CIotDeviceInfo::GetManufacturer() const
{
	return m_manufacturer;
}


void CIotDeviceInfo::SetManufacturer(const QString& manufacturer)
{
	if (m_manufacturer != manufacturer){
		istd::CChangeNotifier changeNotifier(this);

		m_manufacturer = manufacturer;
	}
}


QString CIotDeviceInfo::GetBrandModel() const
{
	return m_brandModel;
}


void CIotDeviceInfo::SetBrandModel(const QString& brandModel)
{
	if (m_brandModel != brandModel){
		istd::CChangeNotifier changeNotifier(this);

		m_brandModel = brandModel;
	}
}


QString CIotDeviceInfo::GetInstallationLocation() const
{
	return m_installationLocation;
}


void CIotDeviceInfo::SetInstallationLocation(const QString& installationLocation)
{
	if (m_installationLocation != installationLocation){
		istd::CChangeNotifier changeNotifier(this);

		m_installationLocation = installationLocation;
	}
}


QString CIotDeviceInfo::GetConnectionType() const
{
	return m_connectionType;
}


void CIotDeviceInfo::SetConnectionType(const QString& connectionType)
{
	if (m_connectionType != connectionType){
		istd::CChangeNotifier changeNotifier(this);

		m_connectionType = connectionType;
	}
}


QString CIotDeviceInfo::GetResourceType() const
{
	return m_resourceType;
}


void CIotDeviceInfo::SetResourceType(const QString& resourceType)
{
	if (m_resourceType != resourceType){
		istd::CChangeNotifier changeNotifier(this);

		m_resourceType = resourceType;
	}
}


QString CIotDeviceInfo::GetHoleDiameter() const
{
	return m_holeDiameter;
}


void CIotDeviceInfo::SetHoleDiameter(const QString& holeDiameter)
{
	if (m_holeDiameter != holeDiameter){
		istd::CChangeNotifier changeNotifier(this);

		m_holeDiameter = holeDiameter;
	}
}


QString CIotDeviceInfo::GetDeviceCategory() const
{
	return m_deviceCategory;
}


void CIotDeviceInfo::SetDeviceCategory(const QString& deviceCategory)
{
	if (m_deviceCategory != deviceCategory){
		istd::CChangeNotifier changeNotifier(this);

		m_deviceCategory = deviceCategory;
	}
}


QString CIotDeviceInfo::GetCalibrationDate() const
{
	return m_calibrationDate;
}


void CIotDeviceInfo::SetCalibrationDate(const QString& calibrationDate)
{
	if (m_calibrationDate != calibrationDate){
		istd::CChangeNotifier changeNotifier(this);

		m_calibrationDate = calibrationDate;
	}
}


QString CIotDeviceInfo::GetCommissionDate() const
{
	return m_commissionDate;
}


void CIotDeviceInfo::SetCommissionDate(const QString& commissionDate)
{
	if (m_commissionDate != commissionDate){
		istd::CChangeNotifier changeNotifier(this);

		m_commissionDate = commissionDate;
	}
}


QString CIotDeviceInfo::GetDescription() const
{
	return m_description;
}


void CIotDeviceInfo::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


// reimplemented (iser::IObject)

QByteArray CIotDeviceInfo::GetFactoryId() const
{
	return QByteArray("IotDeviceInfo");
}


// reimplemented (iser::ISerializable)

bool CIotDeviceInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag factoryNumberTag("FactoryNumber", "Factory number", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(factoryNumberTag);
	retVal = retVal && archive.Process(m_factoryNumber);
	retVal = retVal && archive.EndTag(factoryNumberTag);

	iser::CArchiveTag modemNumberTag("ModemNumber", "Modem number", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(modemNumberTag);
	retVal = retVal && archive.Process(m_modemNumber);
	retVal = retVal && archive.EndTag(modemNumberTag);

	iser::CArchiveTag manufacturerTag("Manufacturer", "Manufacturer", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(manufacturerTag);
	retVal = retVal && archive.Process(m_manufacturer);
	retVal = retVal && archive.EndTag(manufacturerTag);

	iser::CArchiveTag brandModelTag("BrandModel", "Brand/Model", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(brandModelTag);
	retVal = retVal && archive.Process(m_brandModel);
	retVal = retVal && archive.EndTag(brandModelTag);

	iser::CArchiveTag installationLocationTag("InstallationLocation", "Installation location", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(installationLocationTag);
	retVal = retVal && archive.Process(m_installationLocation);
	retVal = retVal && archive.EndTag(installationLocationTag);

	iser::CArchiveTag connectionTypeTag("ConnectionType", "Connection type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(connectionTypeTag);
	retVal = retVal && archive.Process(m_connectionType);
	retVal = retVal && archive.EndTag(connectionTypeTag);

	iser::CArchiveTag resourceTypeTag("ResourceType", "Resource type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(resourceTypeTag);
	retVal = retVal && archive.Process(m_resourceType);
	retVal = retVal && archive.EndTag(resourceTypeTag);

	iser::CArchiveTag holeDiameterTag("HoleDiameter", "Hole diameter", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(holeDiameterTag);
	retVal = retVal && archive.Process(m_holeDiameter);
	retVal = retVal && archive.EndTag(holeDiameterTag);

	iser::CArchiveTag deviceCategoryTag("DeviceCategory", "Device category", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(deviceCategoryTag);
	retVal = retVal && archive.Process(m_deviceCategory);
	retVal = retVal && archive.EndTag(deviceCategoryTag);

	iser::CArchiveTag calibrationDateTag("CalibrationDate", "Calibration date", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(calibrationDateTag);
	retVal = retVal && archive.Process(m_calibrationDate);
	retVal = retVal && archive.EndTag(calibrationDateTag);

	iser::CArchiveTag commissionDateTag("CommissionDate", "Commission date", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(commissionDateTag);
	retVal = retVal && archive.Process(m_commissionDate);
	retVal = retVal && archive.EndTag(commissionDateTag);

	iser::CArchiveTag descriptionTag("Description", "Description of the IoT device", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CIotDeviceInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CIotDeviceInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CIotDeviceInfo* sourcePtr = dynamic_cast<const CIotDeviceInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_factoryNumber = sourcePtr->m_factoryNumber;
		m_modemNumber = sourcePtr->m_modemNumber;
		m_manufacturer = sourcePtr->m_manufacturer;
		m_brandModel = sourcePtr->m_brandModel;
		m_installationLocation = sourcePtr->m_installationLocation;
		m_connectionType = sourcePtr->m_connectionType;
		m_resourceType = sourcePtr->m_resourceType;
		m_holeDiameter = sourcePtr->m_holeDiameter;
		m_deviceCategory = sourcePtr->m_deviceCategory;
		m_calibrationDate = sourcePtr->m_calibrationDate;
		m_commissionDate = sourcePtr->m_commissionDate;
		m_description = sourcePtr->m_description;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CIotDeviceInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CIotDeviceInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CIotDeviceInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_factoryNumber.clear();
	m_modemNumber.clear();
	m_manufacturer.clear();
	m_brandModel.clear();
	m_installationLocation.clear();
	m_connectionType.clear();
	m_resourceType.clear();
	m_holeDiameter.clear();
	m_deviceCategory.clear();
	m_calibrationDate.clear();
	m_commissionDate.clear();
	m_description.clear();

	return true;
}


} // namespace prolifedata
