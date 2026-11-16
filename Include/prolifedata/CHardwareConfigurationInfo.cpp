#include <prolifedata/CHardwareConfigurationInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace prolifedata
{


// public methods

// reimplemented (IHardwareConfigurationInfo)

QByteArray CHardwareConfigurationInfo::GetConfigurationId() const
{
	return m_configurationId;
}


void CHardwareConfigurationInfo::SetConfigurationId(const QByteArray& configurationId)
{
	if (m_configurationId != configurationId){
		istd::CChangeNotifier changeNotifier(this);

		m_configurationId = configurationId;
	}
}


QString CHardwareConfigurationInfo::GetName() const
{
	return m_name;
}


void CHardwareConfigurationInfo::SetName(const QString& name)
{
	if (m_name != name){
		istd::CChangeNotifier changeNotifier(this);

		m_name = name;
	}
}


QString CHardwareConfigurationInfo::GetDescription() const
{
	return m_description;
}


void CHardwareConfigurationInfo::SetDescription(const QString& description)
{
	if (m_description != description){
		istd::CChangeNotifier changeNotifier(this);

		m_description = description;
	}
}


QByteArray CHardwareConfigurationInfo::GetProductId() const
{
	return m_productId;
}


void CHardwareConfigurationInfo::SetProductId(const QByteArray& productId)
{
	if (m_productId != productId){
		istd::CChangeNotifier changeNotifier(this);

		m_productId = productId;
	}
}


QByteArrayList CHardwareConfigurationInfo::GetComponentIds() const
{
	return m_componentIds;
}


void CHardwareConfigurationInfo::SetComponentIds(const QByteArrayList& componentIds)
{
	if (m_componentIds != componentIds){
		istd::CChangeNotifier changeNotifier(this);

		m_componentIds = componentIds;
	}
}


QByteArrayList CHardwareConfigurationInfo::GetBundledLicenseIds() const
{
	return m_bundledLicenseIds;
}


void CHardwareConfigurationInfo::SetBundledLicenseIds(const QByteArrayList& bundledLicenseIds)
{
	if (m_bundledLicenseIds != bundledLicenseIds){
		istd::CChangeNotifier changeNotifier(this);

		m_bundledLicenseIds = bundledLicenseIds;
	}
}


// reimplemented (iser::IObject)

QByteArray CHardwareConfigurationInfo::GetFactoryId() const
{
	return QByteArrayLiteral("HardwareConfiguration");
}


// reimplemented (iser::ISerializable)

bool CHardwareConfigurationInfo::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	static iser::CArchiveTag configurationIdTag("ConfigurationId", "Article (order code) of the configuration", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(configurationIdTag);
	retVal = retVal && archive.Process(m_configurationId);
	retVal = retVal && archive.EndTag(configurationIdTag);

	static iser::CArchiveTag nameTag("Name", "Configuration name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(nameTag);
	retVal = retVal && archive.Process(m_name);
	retVal = retVal && archive.EndTag(nameTag);

	static iser::CArchiveTag descriptionTag("Description", "Configuration description", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(descriptionTag);
	retVal = retVal && archive.Process(m_description);
	retVal = retVal && archive.EndTag(descriptionTag);

	static iser::CArchiveTag productIdTag("ProductId", "ID of the hardware model (product) the configuration belongs to", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productIdTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productIdTag);

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_componentIds, "ComponentIds", "Id");

	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_bundledLicenseIds, "BundledLicenseIds", "Id");

	return retVal;
}


// reimplemented (istd::IChangeable)

int CHardwareConfigurationInfo::GetSupportedOperations() const
{
	return SO_CLONE | SO_COPY | SO_RESET;
}


bool CHardwareConfigurationInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeGroup changeGroup(this);

	const CHardwareConfigurationInfo* sourcePtr = dynamic_cast<const CHardwareConfigurationInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_configurationId = sourcePtr->m_configurationId;
		m_name = sourcePtr->m_name;
		m_description = sourcePtr->m_description;
		m_productId = sourcePtr->m_productId;
		m_componentIds = sourcePtr->m_componentIds;
		m_bundledLicenseIds = sourcePtr->m_bundledLicenseIds;

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CHardwareConfigurationInfo::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CHardwareConfigurationInfo());
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CHardwareConfigurationInfo::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_configurationId.clear();
	m_name.clear();
	m_description.clear();
	m_productId.clear();
	m_componentIds.clear();
	m_bundledLicenseIds.clear();

	return true;
}


// private methods

} // namespace prolifedata


