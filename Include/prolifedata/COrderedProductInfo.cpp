#include <prolifedata/COrderedProductInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>

// ImtCore includes
#include <imtlic/ILicenseInstance.h>
#include <imtlic/IProductLicensingInfo.h>


namespace prolifedata
{


// public methods

COrderedProductInfo::COrderedProductInfo()
	:m_customerCollectionPtr(nullptr),
	m_productCollectionPtr(nullptr)
{
}


// reimplemented (imtlic::IProductInstanceInfo)

const imtbase::IObjectCollection* COrderedProductInfo::GetProductDatabase() const
{
	return m_productCollectionPtr;
}


const imtbase::IObjectCollection* COrderedProductInfo::GetCustomerDatabase() const
{
	return m_customerCollectionPtr;
}


void COrderedProductInfo::SetupProductInstance(
			const QByteArray& productId,
			const QByteArray& instanceId,
			const QByteArray& customerId,
			const QByteArray& orderId)
{
	istd::CChangeNotifier changeNotifier(this);

	m_productId = productId;
	m_instanceId = instanceId;
	m_customerId = customerId;
	m_orderId = orderId;
}


void COrderedProductInfo::AddLicense(const QByteArray& licenseId, const QDateTime& expirationDate)
{
	if (m_productId.isEmpty()){
		return;
	}
	LicenseInstancePtr licenseInstancePtr(new imtlic::CLicenseInstance);
	licenseInstancePtr->SetLicenseId(licenseId);
	licenseInstancePtr->SetExpiration(expirationDate);

	m_licenses[licenseId] = licenseInstancePtr;

	m_licenseContainerInfo.InsertItem(licenseId, "", "");
}


void COrderedProductInfo::RemoveLicense(const QByteArray& licenseId)
{
	istd::CChangeNotifier changeNotifier(this);

	m_licenses.remove(licenseId);

	m_licenseContainerInfo.RemoveItem(licenseId);
}


void COrderedProductInfo::ClearLicenses()
{
	istd::CChangeNotifier changeNotifier(this);

	m_licenses.clear();
}


QByteArray COrderedProductInfo::GetProductId() const
{
	return m_productId;
}


QByteArray COrderedProductInfo::GetProductInstanceId() const
{
	return m_instanceId;
}


QByteArray COrderedProductInfo::GetCustomerId() const
{
	return m_customerId;
}

QByteArray COrderedProductInfo::GetOrderId() const
{
	return m_orderId;
}


// reimplemented (imtlic::ILicenseInfoProvider)

const imtbase::ICollectionInfo& COrderedProductInfo::GetLicenseInstances() const
{
	return m_licenseContainerInfo;
}


const imtlic::ILicenseInstance* COrderedProductInfo::GetLicenseInstance(const QByteArray& licenseId) const
{
	if (m_licenses.contains(licenseId)){
		return m_licenses[licenseId].GetPtr();
	}

	return nullptr;
}


// reimplemented (iser::ISerializable)

bool COrderedProductInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag productIdTag("ProductId", "ID of the product", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productIdTag);

	static iser::CArchiveTag instanceIdTag("InstanceId", "ID of the product instance", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(instanceIdTag);
	retVal = retVal && archive.Process(m_instanceId);
	retVal = retVal && archive.EndTag(instanceIdTag);

	static iser::CArchiveTag customerIdTag("CustomerId", "ID of the customer", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(customerIdTag);
	retVal = retVal && archive.Process(m_customerId);
	retVal = retVal && archive.EndTag(customerIdTag);

	static iser::CArchiveTag orderIdTag("OrderId", "ID of the order", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(orderIdTag);
	retVal = retVal && archive.Process(m_orderId);
	retVal = retVal && archive.EndTag(orderIdTag);

	static iser::CArchiveTag licensesTag("Licenses", "List of licenses", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag licenseInstanceTag("LicenseInstance", "License instance", iser::CArchiveTag::TT_GROUP);
	static iser::CArchiveTag licenseTag("LicenseData", "License data", iser::CArchiveTag::TT_GROUP);

	int licensesCount = m_licenses.count();

	retVal = retVal && archive.BeginMultiTag(licensesTag, licenseInstanceTag, licensesCount);

	if (!archive.IsStoring()){
		m_licenses.clear();
		m_licenseContainerInfo.ResetData();
	}

	if (archive.IsStoring()){
		for (LicenseInstances::Iterator iter = m_licenses.begin(); iter != m_licenses.end(); ++iter){
			retVal = retVal && archive.BeginTag(licenseInstanceTag);

			Q_ASSERT(iter.value().IsValid());

			retVal = archive.BeginTag(licenseTag);
			retVal = retVal && iter.value()->Serialize(archive);
			retVal = retVal && archive.EndTag(licenseTag);

			retVal = retVal && archive.EndTag(licenseInstanceTag);
		}
	}
	else{
		for (int i = 0; i < licensesCount; ++i){
			retVal = retVal && archive.BeginTag(licenseInstanceTag);

			LicenseInstancePtr licenseDataPtr(new imtlic::CLicenseInstance);
			retVal = archive.BeginTag(licenseTag);
			retVal = retVal && licenseDataPtr->Serialize(archive);
			retVal = retVal && archive.EndTag(licenseTag);

			retVal = retVal && archive.EndTag(licenseInstanceTag);

			if (retVal){
				m_licenses[licenseDataPtr->GetLicenseId()] = licenseDataPtr;
				m_licenseContainerInfo.InsertItem(licenseDataPtr->GetLicenseId(), "", "");
			}
		}
	}

	retVal = retVal && archive.EndTag(licensesTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int COrderedProductInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool COrderedProductInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const COrderedProductInfo* sourcePtr = dynamic_cast<const COrderedProductInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_productId = sourcePtr->m_productId;
		m_customerId = sourcePtr->m_customerId;
		m_instanceId = sourcePtr->m_instanceId;
		m_orderId = sourcePtr->m_orderId;
		m_licenses = sourcePtr->m_licenses;
		m_licenseContainerInfo = sourcePtr->m_licenseContainerInfo;

		return true;
	}

	return false;
}


istd::IChangeable* COrderedProductInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<COrderedProductInfo> clonePtr(new COrderedProductInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool COrderedProductInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_productId.clear();
	m_customerId.clear();
	m_instanceId.clear();
	m_licenses.clear();
	m_licenseContainerInfo.ResetData();
	m_orderId.clear();

	return true;
}


QByteArray COrderedProductInfo::FindProductByName(const QString& productName) const
{
	if (m_productCollectionPtr != nullptr){
		imtbase::ICollectionInfo::Ids productCollectionIds = m_productCollectionPtr->GetElementIds();
		for (const QByteArray productCollectionId : productCollectionIds){
			if (m_productCollectionPtr->GetElementInfo(productCollectionId, imtbase::ICollectionInfo::EIT_NAME).toString() == productName){
				return productCollectionId;
			}
		}
	}

	return QByteArray();
}


} // namespace imtlic


