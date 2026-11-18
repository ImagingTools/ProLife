#include <prolifedata/CCustomerInfo.h>


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

void CCustomerInfo::SetCustomerId(const QByteArray& customerId)
{
	if (m_customerId != customerId){
		istd::CChangeNotifier notifier(this);

		m_customerId = customerId;
	}
}


// reimplemented (ICustomerInfo)

QByteArray CCustomerInfo::GetCustomerId() const
{
	return m_customerId;
}


QString CCustomerInfo::GetWebsite() const
{
	return m_website;
}


void CCustomerInfo::SetWebsite(const QString& website)
{
	if (m_website != website){
		istd::CChangeNotifier changeNotifier(this);

		m_website = website;
	}
}


bool CCustomerInfo::IsCustomer() const
{
	return m_isCustomer;
}


void CCustomerInfo::SetCustomer(bool isCustomer)
{
	if (m_isCustomer != isCustomer){
		istd::CChangeNotifier changeNotifier(this);

		m_isCustomer = isCustomer;
	}
}


bool CCustomerInfo::IsManufacturer() const
{
	return m_isManufacturer;
}


void CCustomerInfo::SetManufacturer(bool isManufacturer)
{
	if (m_isManufacturer != isManufacturer){
		istd::CChangeNotifier changeNotifier(this);

		m_isManufacturer = isManufacturer;
	}
}


bool CCustomerInfo::IsSupplier() const
{
	return m_isSupplier;
}


void CCustomerInfo::SetSupplier(bool isSupplier)
{
	if (m_isSupplier != isSupplier){
		istd::CChangeNotifier changeNotifier(this);

		m_isSupplier = isSupplier;
	}
}


// reimplemented (iser::ISerializable)

bool CCustomerInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	const iser::IVersionInfo& versionInfo = archive.GetVersionInfo();

	quint32 imtCoreVersion;
	if (!versionInfo.GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion)){
		imtCoreVersion = 0;
	}

	bool retVal = true;

	if (imtCoreVersion > 9541){
		iser::CArchiveTag customerIdTag("CustomerId", "Customer-ID", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(customerIdTag);
		retVal = retVal && archive.Process(m_customerId);
		retVal = retVal && archive.EndTag(customerIdTag);
	}

	iser::CArchiveTag websiteTag("Website", "Website of the company", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(websiteTag);
	retVal = retVal && archive.Process(m_website);
	retVal = retVal && archive.EndTag(websiteTag);

	iser::CArchiveTag isCustomerTag("IsCustomer", "The company is a customer", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isCustomerTag);
	retVal = retVal && archive.Process(m_isCustomer);
	retVal = retVal && archive.EndTag(isCustomerTag);

	iser::CArchiveTag isManufacturerTag("IsManufacturer", "The company manufactures hardware components", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isManufacturerTag);
	retVal = retVal && archive.Process(m_isManufacturer);
	retVal = retVal && archive.EndTag(isManufacturerTag);

	iser::CArchiveTag isSupplierTag("IsSupplier", "The company supplies hardware components", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isSupplierTag);
	retVal = retVal && archive.Process(m_isSupplier);
	retVal = retVal && archive.EndTag(isSupplierTag);

	retVal = retVal && BaseClass::Serialize(archive);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CCustomerInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CCustomerInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CCustomerInfo* sourcePtr = dynamic_cast<const CCustomerInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_customerId = sourcePtr->m_customerId;
		m_website = sourcePtr->m_website;
		m_isCustomer = sourcePtr->m_isCustomer;
		m_isManufacturer = sourcePtr->m_isManufacturer;
		m_isSupplier = sourcePtr->m_isSupplier;

		bool retVal = BaseClass::CopyFrom(object);

		return retVal;
	}

	return false;
}


istd::IChangeableUniquePtr CCustomerInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CCustomerInfo());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CCustomerInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_customerId.clear();
	m_website.clear();
	m_isCustomer = false;
	m_isManufacturer = false;
	m_isSupplier = false;

	BaseClass::ResetData();

	return true;
}


} // namespace prolifedata


