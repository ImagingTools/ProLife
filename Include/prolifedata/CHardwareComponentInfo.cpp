#include <prolifedata/CHardwareComponentInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace prolifedata
{


// public methods

// reimplemented (IHardwareComponentInfo)

QByteArray CHardwareComponentInfo::GetPartNumber() const
{
	return m_partNumber;
}


void CHardwareComponentInfo::SetPartNumber(const QByteArray& partNumber)
{
	if (m_partNumber != partNumber){
		istd::CChangeNotifier changeNotifier(this);

		m_partNumber = partNumber;
	}
}


QString CHardwareComponentInfo::GetName() const
{
	return m_name;
}


void CHardwareComponentInfo::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier changeNotifier(this);

		m_name = name;
	}
}


QString CHardwareComponentInfo::GetDescription() const
{
	return m_description;
}


void CHardwareComponentInfo::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


QByteArray CHardwareComponentInfo::GetComponentType() const
{
	return m_componentType;
}


void CHardwareComponentInfo::SetComponentType(const QByteArray& componentType)
{
	if (m_componentType != componentType){
		istd::CChangeNotifier changeNotifier(this);

		m_componentType = componentType;
	}
}


QString CHardwareComponentInfo::GetRevision() const
{
	return m_revision;
}


void CHardwareComponentInfo::SetRevision(const QString& revision)
{
	if (m_revision != revision){
		istd::CChangeNotifier changeNotifier(this);

		m_revision = revision;
	}
}


QByteArray CHardwareComponentInfo::GetManufacturerId() const
{
	return m_manufacturerId;
}


void CHardwareComponentInfo::SetManufacturerId(const QByteArray& value)
{
	if (m_manufacturerId != value){
		istd::CChangeNotifier changeNotifier(this);

		m_manufacturerId = value;
	}
}


QString CHardwareComponentInfo::GetMpn() const
{
	return m_mpn;
}


void CHardwareComponentInfo::SetMpn(const QString& mpn)
{
	if (m_mpn != mpn){
		istd::CChangeNotifier changeNotifier(this);

		m_mpn = mpn;
	}
}


IHardwareComponentInfo::SupplierOffers CHardwareComponentInfo::GetSupplierOffers() const
{
	return m_supplierOffers;
}


void CHardwareComponentInfo::SetSupplierOffers(const SupplierOffers& supplierOffers)
{
	if (m_supplierOffers != supplierOffers){
		istd::CChangeNotifier changeNotifier(this);

		m_supplierOffers = supplierOffers;
	}
}


QByteArray CHardwareComponentInfo::GetLifecycleStatus() const
{
	return m_lifecycleStatus;
}


void CHardwareComponentInfo::SetLifecycleStatus(const QByteArray& lifecycleStatus)
{
	if (m_lifecycleStatus != lifecycleStatus){
		istd::CChangeNotifier changeNotifier(this);

		m_lifecycleStatus = lifecycleStatus;
	}
}


bool CHardwareComponentInfo::IsSerialized() const
{
	return m_serialized;
}


void CHardwareComponentInfo::SetSerialized(bool serialized)
{
	if (m_serialized != serialized){
		istd::CChangeNotifier changeNotifier(this);

		m_serialized = serialized;
	}
}


// reimplemented (iser::IObject)

QByteArray CHardwareComponentInfo::GetFactoryId() const
{
	return QByteArrayLiteral("HardwareComponent");
}


// reimplemented (iser::ISerializable)

bool CHardwareComponentInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag partNumberTag("PartNumber", "Part number", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(partNumberTag);
	retVal = retVal && archive.Process(m_partNumber);
	retVal = retVal && archive.EndTag(partNumberTag);

	static iser::CArchiveTag nameTag("Name", "Component name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	static iser::CArchiveTag descriptionTag("Description", "Component description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	static iser::CArchiveTag componentTypeTag("ComponentType", "Component type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(componentTypeTag);
	retVal = retVal && archive.Process(m_componentType);
	retVal = retVal && archive.EndTag(componentTypeTag);

	static iser::CArchiveTag revisionTag("Revision", "Revision", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(revisionTag);
	retVal = retVal && archive.Process(m_revision);
	retVal = retVal && archive.EndTag(revisionTag);

	static iser::CArchiveTag manufacturerIdTag("ManufacturerId", "ID of the manufacturer", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(manufacturerIdTag);
	retVal = retVal && archive.Process(m_manufacturerId);
	retVal = retVal && archive.EndTag(manufacturerIdTag);

	static iser::CArchiveTag mpnTag("Mpn", "Manufacturer part number", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(mpnTag);
	retVal = retVal && archive.Process(m_mpn);
	retVal = retVal && archive.EndTag(mpnTag);

	static iser::CArchiveTag supplierOffersTag("SupplierOffers", "Suppliers the component is bought from", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag supplierOfferTag("SupplierOffer", "Supplier and the article number there", iser::CArchiveTag::TT_GROUP, &supplierOffersTag);
	static iser::CArchiveTag supplierIdTag("SupplierId", "ID of the supplier", iser::CArchiveTag::TT_LEAF, &supplierOfferTag);
	static iser::CArchiveTag skuTag("Sku", "Article number of the supplier", iser::CArchiveTag::TT_LEAF, &supplierOfferTag);

	int offersCount = m_supplierOffers.count();
	retVal = retVal && archive.BeginMultiTag(supplierOffersTag, supplierOfferTag, offersCount);
	if (retVal && !archive.IsStoring()){
		m_supplierOffers.resize(offersCount);
	}

	for (int i = 0; retVal && i < offersCount; ++i){
		SupplierOffer& offer = m_supplierOffers[i];

		retVal = retVal && archive.BeginTag(supplierOfferTag);

		retVal = retVal && archive.BeginTag(supplierIdTag);
		retVal = retVal && archive.Process(offer.supplierId);
		retVal = retVal && archive.EndTag(supplierIdTag);

		retVal = retVal && archive.BeginTag(skuTag);
		retVal = retVal && archive.Process(offer.sku);
		retVal = retVal && archive.EndTag(skuTag);

		retVal = retVal && archive.EndTag(supplierOfferTag);
	}

	retVal = retVal && archive.EndTag(supplierOffersTag);

	static iser::CArchiveTag lifecycleStatusTag("LifecycleStatus", "Lifecycle status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(lifecycleStatusTag);
	retVal = retVal && archive.Process(m_lifecycleStatus);
	retVal = retVal && archive.EndTag(lifecycleStatusTag);

	static iser::CArchiveTag serializedTag("Serialized", "Units carry serial numbers", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(serializedTag);
	retVal = retVal && archive.Process(m_serialized);
	retVal = retVal && archive.EndTag(serializedTag);


	return retVal;
}


// reimplemented (istd::IChangeable)

int CHardwareComponentInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CHardwareComponentInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CHardwareComponentInfo* sourcePtr = dynamic_cast<const CHardwareComponentInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);
		m_partNumber = sourcePtr->m_partNumber;
		m_name = sourcePtr->m_name;
		m_description = sourcePtr->m_description;
		m_componentType = sourcePtr->m_componentType;
		m_revision = sourcePtr->m_revision;
		m_manufacturerId = sourcePtr->m_manufacturerId;
		m_mpn = sourcePtr->m_mpn;
		m_supplierOffers = sourcePtr->m_supplierOffers;
		m_lifecycleStatus = sourcePtr->m_lifecycleStatus;
		m_serialized = sourcePtr->m_serialized;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CHardwareComponentInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CHardwareComponentInfo());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CHardwareComponentInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_partNumber.clear();
	m_name.clear();
	m_description.clear();
	m_componentType = QByteArrayLiteral("Part");
	m_revision.clear();
	m_manufacturerId.clear();
	m_mpn.clear();
	m_supplierOffers.clear();
	m_lifecycleStatus = QByteArrayLiteral("Active");
	m_serialized = false;

	return true;
}


} // namespace prolifedata


