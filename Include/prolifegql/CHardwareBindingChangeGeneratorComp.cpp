#include <prolifegql/CHardwareBindingChangeGeneratorComp.h>


// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/ILicenseDefinition.h>

// ProLife includes
#include <prolifedata/CHardwareProductBinding.h>
#include <prolifedata/IDeviceInfo.h>


namespace prolifegql
{


// protected methods

bool CHardwareBindingChangeGeneratorComp::CompareDocuments(
			const istd::IChangeable& oldDocument,
			const istd::IChangeable& newDocument,
			imtbase::CObjectCollection& documentChangeCollection,
			QString& errorMessage)
{
	const prolifedata::CHardwareProductBinding* oldHardwareBindingInfoPtr = dynamic_cast<const prolifedata::CHardwareProductBinding*>(&oldDocument);
	if (oldHardwareBindingInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. Old document is invalid");

		return false;
	}

	const prolifedata::CHardwareProductBinding* newHardwareBindingInfoPtr = dynamic_cast<const prolifedata::CHardwareProductBinding*>(&newDocument);
	if (newHardwareBindingInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. New document is invalid");

		return false;
	}

	InsertChange(
				documentChangeCollection,
				"HardwareId",
				QT_TRANSLATE_NOOP("Attribute", "Hardware-ID"),
				oldHardwareBindingInfoPtr->GetHardwareId(),
				newHardwareBindingInfoPtr->GetHardwareId());

	InsertListChanges(
				documentChangeCollection,
				"AddLicense",
				"RemoveLicense",
				"License",
				QT_TRANSLATE_NOOP("Attribute", "License"),
				oldHardwareBindingInfoPtr->GetSoftwareIds(),
				newHardwareBindingInfoPtr->GetSoftwareIds());

	return true;
}


QString CHardwareBindingChangeGeneratorComp::CreateCustomOperationDescription(
			const imtbase::COperationDescription& operationDescription,
			const QByteArray& languageId) const
{
	static const QByteArray translationContext = QByteArrayLiteral("prolifegql::CHardwareBindingChangeGeneratorComp");

	const QByteArray typeId = operationDescription.GetOperationTypeId();

	if (typeId == "AddLicense"){
		return Translate(QT_TR_NOOP("Bound the license '%1'"), languageId, translationContext)
					.arg(GetLicenseName(operationDescription.GetNewValue()).toHtmlEscaped());
	}

	if (typeId == "RemoveLicense"){
		return Translate(QT_TR_NOOP("Unbound the license '%1'"), languageId, translationContext)
					.arg(GetLicenseName(operationDescription.GetOldValue()).toHtmlEscaped());
	}

	return QString();
}


QString CHardwareBindingChangeGeneratorComp::GetKeyNameForOperation(const QByteArray& key, const QByteArray& value) const
{
	if (key == "HardwareId"){
		return GetHardwareName(value);
	}

	return BaseClass::GetKeyNameForOperation(key, value);
}


QString CHardwareBindingChangeGeneratorComp::GetLicenseName(const QByteArray& productUuid) const
{
	if (!IsUuid(productUuid) || !m_softwareInstanceCollectionCompPtr.IsValid() || !m_licenseCollectionCompPtr.IsValid()){
		return productUuid;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_softwareInstanceCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
		return productUuid;
	}

	const imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
	if (productInstanceInfoPtr == nullptr){
		return productUuid;
	}

	// A product instance can carry several license instances, all of them describe the binding.
	QStringList licenseNames;
	const imtbase::ICollectionInfo::Ids licenseCollectionIds = productInstanceInfoPtr->GetLicenseInstances().GetElementIds();
	for (const QByteArray& licenseDefinitionUuid : licenseCollectionIds){
		imtbase::IObjectCollection::DataPtr licenseDataPtr;
		if (!m_licenseCollectionCompPtr->GetObjectData(licenseDefinitionUuid, licenseDataPtr)){
			continue;
		}

		const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
		if (licenseInfoPtr == nullptr){
			continue;
		}

		const QByteArray licenseId = licenseInfoPtr->GetLicenseId();
		const QString licenseName = licenseInfoPtr->GetLicenseName();

		if (licenseName.isEmpty()){
			licenseNames << QString::fromUtf8(licenseId);
		}
		else{
			licenseNames << licenseName + " (" + QString::fromUtf8(licenseId) + ")";
		}
	}

	if (licenseNames.isEmpty()){
		const QByteArray serialNumber = productInstanceInfoPtr->GetSerialNumber();

		return serialNumber.isEmpty() ? QString::fromUtf8(productUuid) : QString::fromUtf8(serialNumber);
	}

	return licenseNames.join(", ");
}


QString CHardwareBindingChangeGeneratorComp::GetHardwareName(const QByteArray& hardwareId) const
{
	if (!IsUuid(hardwareId) || !m_deviceCollectionCompPtr.IsValid()){
		return hardwareId;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_deviceCollectionCompPtr->GetObjectData(hardwareId, dataPtr)){
		const prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::IDeviceInfo*>(dataPtr.GetPtr());
		if (deviceInfoPtr != nullptr){
			return deviceInfoPtr->GetMacAddress();
		}
	}

	return hardwareId;
}


} // namespace prolifegql
