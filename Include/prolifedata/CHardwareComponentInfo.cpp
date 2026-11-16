#include <prolifedata/CHardwareComponentInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


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


QString CHardwareComponentInfo::GetManufacturer() const
{
	return m_manufacturer;
}


void CHardwareComponentInfo::SetManufacturer(const QString& manufacturer)
{
	if (m_manufacturer != manufacturer){
		istd::CChangeNotifier changeNotifier(this);

		m_manufacturer = manufacturer;
	}
}


QString CHardwareComponentInfo::GetSupplier() const
{
	return m_supplier;
}


void CHardwareComponentInfo::SetSupplier(const QString& supplier)
{
	if (m_supplier != supplier){
		istd::CChangeNotifier changeNotifier(this);

		m_supplier = supplier;
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


QByteArrayList CHardwareComponentInfo::GetRequirements() const
{
	return m_requirements;
}


void CHardwareComponentInfo::SetRequirements(const QByteArrayList& componentIds)
{
	if (m_requirements != componentIds){
		istd::CChangeNotifier changeNotifier(this);

		m_requirements = componentIds;
	}
}


QByteArrayList CHardwareComponentInfo::GetIncompatibilities() const
{
	return m_incompatibilities;
}


void CHardwareComponentInfo::SetIncompatibilities(const QByteArrayList& componentIds)
{
	if (m_incompatibilities != componentIds){
		istd::CChangeNotifier changeNotifier(this);

		m_incompatibilities = componentIds;
	}
}


IHardwareComponentInfo::Parameters CHardwareComponentInfo::GetParameters() const
{
	return m_parameters;
}


void CHardwareComponentInfo::SetParameters(const Parameters& parameters)
{
	if (m_parameters != parameters){
		istd::CChangeNotifier changeNotifier(this);

		m_parameters = parameters;
	}
}


IHardwareComponentInfo::Bom CHardwareComponentInfo::GetBom() const
{
	return m_bom;
}


void CHardwareComponentInfo::SetBom(const Bom& bom)
{
	if (m_bom != bom){
		istd::CChangeNotifier changeNotifier(this);

		m_bom = bom;
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

	static iser::CArchiveTag manufacturerTag("Manufacturer", "Manufacturer", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(manufacturerTag);
	retVal = retVal && archive.Process(m_manufacturer);
	retVal = retVal && archive.EndTag(manufacturerTag);

	static iser::CArchiveTag supplierTag("Supplier", "Supplier", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(supplierTag);
	retVal = retVal && archive.Process(m_supplier);
	retVal = retVal && archive.EndTag(supplierTag);

	static iser::CArchiveTag mpnTag("Mpn", "Manufacturer part number", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(mpnTag);
	retVal = retVal && archive.Process(m_mpn);
	retVal = retVal && archive.EndTag(mpnTag);

	static iser::CArchiveTag lifecycleStatusTag("LifecycleStatus", "Lifecycle status", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(lifecycleStatusTag);
	retVal = retVal && archive.Process(m_lifecycleStatus);
	retVal = retVal && archive.EndTag(lifecycleStatusTag);

	static iser::CArchiveTag serializedTag("Serialized", "Units carry serial numbers", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(serializedTag);
	retVal = retVal && archive.Process(m_serialized);
	retVal = retVal && archive.EndTag(serializedTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_requirements, "Requirements", "ComponentId");
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_incompatibilities, "Incompatibilities", "ComponentId");
	retVal = retVal && SerializeParameters(archive);
	retVal = retVal && SerializeBom(archive);

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
		m_manufacturer = sourcePtr->m_manufacturer;
		m_supplier = sourcePtr->m_supplier;
		m_mpn = sourcePtr->m_mpn;
		m_lifecycleStatus = sourcePtr->m_lifecycleStatus;
		m_serialized = sourcePtr->m_serialized;
		m_requirements = sourcePtr->m_requirements;
		m_incompatibilities = sourcePtr->m_incompatibilities;
		m_parameters = sourcePtr->m_parameters;
		m_bom = sourcePtr->m_bom;

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
	m_manufacturer.clear();
	m_supplier.clear();
	m_mpn.clear();
	m_lifecycleStatus = QByteArrayLiteral("Active");
	m_serialized = false;
	m_requirements.clear();
	m_incompatibilities.clear();
	m_parameters.clear();
	m_bom.clear();

	return true;
}


// private methods

bool CHardwareComponentInfo::SerializeParameters(iser::IArchive& archive)
{
	static iser::CArchiveTag parametersTag("Parameters", "Technical parameters", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag parameterTag("Parameter", "Technical parameter", iser::CArchiveTag::TT_GROUP, &parametersTag);
	static iser::CArchiveTag keyTag("Key", "Parameter key", iser::CArchiveTag::TT_LEAF, &parameterTag);
	static iser::CArchiveTag nameTag("Name", "Parameter name", iser::CArchiveTag::TT_LEAF, &parameterTag);
	static iser::CArchiveTag valueTag("Value", "Parameter value", iser::CArchiveTag::TT_LEAF, &parameterTag);
	static iser::CArchiveTag unitTag("Unit", "Unit of the value", iser::CArchiveTag::TT_LEAF, &parameterTag);

	int count = m_parameters.count();
	bool retVal = archive.BeginMultiTag(parametersTag, parameterTag, count);
	if (retVal && !archive.IsStoring()){
		m_parameters.clear();
		for (int i = 0; i < count; ++i){
			m_parameters.append(Parameter());
		}
	}

	for (int i = 0; retVal && i < count; ++i){
		Parameter& parameter = m_parameters[i];

		retVal = retVal && archive.BeginTag(parameterTag);

		retVal = retVal && archive.BeginTag(keyTag);
		retVal = retVal && archive.Process(parameter.key);
		retVal = retVal && archive.EndTag(keyTag);

		retVal = retVal && archive.BeginTag(nameTag);
		retVal = retVal && archive.Process(parameter.name);
		retVal = retVal && archive.EndTag(nameTag);

		retVal = retVal && archive.BeginTag(valueTag);
		retVal = retVal && archive.Process(parameter.value);
		retVal = retVal && archive.EndTag(valueTag);

		retVal = retVal && archive.BeginTag(unitTag);
		retVal = retVal && archive.Process(parameter.unit);
		retVal = retVal && archive.EndTag(unitTag);

		retVal = retVal && archive.EndTag(parameterTag);
	}

	retVal = retVal && archive.EndTag(parametersTag);

	return retVal;
}


bool CHardwareComponentInfo::SerializeBom(iser::IArchive& archive)
{
	static iser::CArchiveTag bomTag("Bom", "Bill of materials", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag entryTag("BomEntry", "Line of the bill of materials", iser::CArchiveTag::TT_GROUP, &bomTag);
	static iser::CArchiveTag componentIdTag("ComponentId", "ID of the sub-component", iser::CArchiveTag::TT_LEAF, &entryTag);
	static iser::CArchiveTag quantityTag("Quantity", "Number of units", iser::CArchiveTag::TT_LEAF, &entryTag);
	static iser::CArchiveTag optionalTag("Optional", "The line can be left out", iser::CArchiveTag::TT_LEAF, &entryTag);
	static iser::CArchiveTag alternativeGroupTag("AlternativeGroup", "Lines of one group exclude each other", iser::CArchiveTag::TT_LEAF, &entryTag);

	int count = m_bom.count();
	bool retVal = archive.BeginMultiTag(bomTag, entryTag, count);
	if (retVal && !archive.IsStoring()){
		m_bom.clear();
		for (int i = 0; i < count; ++i){
			m_bom.append(BomEntry());
		}
	}

	for (int i = 0; retVal && i < count; ++i){
		BomEntry& entry = m_bom[i];

		retVal = retVal && archive.BeginTag(entryTag);

		retVal = retVal && archive.BeginTag(componentIdTag);
		retVal = retVal && archive.Process(entry.componentId);
		retVal = retVal && archive.EndTag(componentIdTag);

		retVal = retVal && archive.BeginTag(quantityTag);
		retVal = retVal && archive.Process(entry.quantity);
		retVal = retVal && archive.EndTag(quantityTag);

		retVal = retVal && archive.BeginTag(optionalTag);
		retVal = retVal && archive.Process(entry.optional);
		retVal = retVal && archive.EndTag(optionalTag);

		retVal = retVal && archive.BeginTag(alternativeGroupTag);
		retVal = retVal && archive.Process(entry.alternativeGroup);
		retVal = retVal && archive.EndTag(alternativeGroupTag);

		retVal = retVal && archive.EndTag(entryTag);
	}

	retVal = retVal && archive.EndTag(bomTag);

	return retVal;
}


} // namespace prolifedata


