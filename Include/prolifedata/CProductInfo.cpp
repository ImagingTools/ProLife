#include <prolifedata/CProductInfo.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace prolifedata
{


// public static methods

QByteArray CProductInfo::GetTypeId()
{
	return "ProductInfo";
}


// public methods

CProductInfo::CProductInfo()
{
}


CProductInfo::~CProductInfo()
{
}


// reimplemented (IProductInfo)

QString CProductInfo::GetProductName() const
{
	return m_productName;
}


void CProductInfo::SetProductName(QString ProductName)
{
	if (m_productName != ProductName){
		istd::CChangeNotifier notifier(this);

		m_productName = ProductName;
	}
}


QString CProductInfo::GetProductDescription() const
{
	return m_productDescription;
}


void CProductInfo::SetProductDescription(QString ProductDescription )
{
	if (m_productDescription != ProductDescription){
		istd::CChangeNotifier notifier(this);

		m_productDescription = ProductDescription;
	}
}
QString CProductInfo::GetProductManufacturer() const
{
	return m_productManufacturer;
}


void CProductInfo::SetProductManufacturer(QString productManufacturer)
{
	if (m_productManufacturer != productManufacturer){
		istd::CChangeNotifier notifier(this);

		m_productManufacturer = productManufacturer;
	}
}


// reimplemented (iser::IObject)

QByteArray CProductInfo::GetFactoryId() const
{
	return GetTypeId();
}


// reimplemented (iser::ISerializable)

bool CProductInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag productNameTag("ProductName", "Product name", iser::CArchiveTag::TT_LEAF);
	retVal = archive.BeginTag(productNameTag);
	retVal = retVal && archive.Process(m_productName);
	retVal = retVal && archive.EndTag(productNameTag);

	static iser::CArchiveTag productDescriptionTag("ProductDescription", "Product description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productDescriptionTag);
	retVal = retVal && archive.Process(m_productDescription);
	retVal = retVal && archive.EndTag(productDescriptionTag);

	static iser::CArchiveTag productManufacturerTag("ProductManufacturer", "Product manufacturer", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productManufacturerTag);
	retVal = retVal && archive.Process(m_productManufacturer);
	retVal = retVal && archive.EndTag(productManufacturerTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CProductInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CProductInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CProductInfo* sourcePtr = dynamic_cast<const CProductInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_productName = sourcePtr->m_productName;
		m_productDescription = sourcePtr->m_productDescription;
		m_productManufacturer = sourcePtr->m_productManufacturer;

		bool retVal = true;


		return retVal;
	}

	return false;
}


istd::IChangeable* CProductInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CProductInfo> clonePtr(new CProductInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CProductInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_productName.clear();
	m_productDescription.clear();
	m_productManufacturer.clear();

	return true;
}


} // namespace prolifedata


