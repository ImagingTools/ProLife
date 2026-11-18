#include <prolifegql/CLicenseImportConverterComp.h>


// ACF includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtlic/ILicenseDefinition.h>
#include <imtlic/IProductInfo.h>

// ProLife includes
#include <prolifedata/CHardwareConfigurationInfo.h>


namespace prolifegql
{


// protected methods

// reimplemented (imtdb::IObjectImportConverter)

imtdb::IObjectImportConverter::ConversionResult CLicenseImportConverterComp::ConvertObject(
			const QByteArray& objectId,
			const istd::IChangeable& sourceObject,
			TargetObject& targetObject,
			QString& errorMessage) const
{
	const imtlic::ILicenseDefinition* licensePtr = dynamic_cast<const imtlic::ILicenseDefinition*>(&sourceObject);
	if (licensePtr == nullptr || !m_sourceProductCollectionCompPtr.IsValid()){
		errorMessage = QStringLiteral("The object '%1' is not a license definition").arg(QString::fromUtf8(objectId));

		return CR_FAILED;
	}

	// A license whose product cannot be read cannot be sorted: it must not silently land on the wrong side
	const QByteArray productId = licensePtr->GetProductId();
	imtbase::IObjectCollection::DataPtr productDataPtr;
	const imtlic::IProductInfo* productInfoPtr = nullptr;
	if (m_sourceProductCollectionCompPtr->GetObjectData(productId, productDataPtr)){
		productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(productDataPtr.GetPtr());
	}

	if (productInfoPtr == nullptr){
		errorMessage = QStringLiteral("The product '%1' of the license '%2' cannot be read").arg(QString::fromUtf8(productId), licensePtr->GetLicenseName());

		return CR_FAILED;
	}

	const bool isHardwareLicense = productInfoPtr->GetCategoryId() == QByteArrayLiteral("Hardware");
	if (isHardwareLicense != *m_importHardwareConfigurationsAttrPtr){
		return CR_SKIPPED;
	}

	if (!isHardwareLicense){
		return CR_CONVERTED;
	}

	istd::TDelPtr<prolifedata::CHardwareConfigurationInfo> configurationPtr(new prolifedata::CHardwareConfigurationInfo);
	configurationPtr->SetConfigurationId(licensePtr->GetLicenseId());
	configurationPtr->SetName(licensePtr->GetLicenseName());
	configurationPtr->SetDescription(licensePtr->GetLicenseDescription());
	configurationPtr->SetProductId(productId);

	targetObject.typeId = QByteArrayLiteral("HardwareConfiguration");
	targetObject.name = licensePtr->GetLicenseName();
	targetObject.description = licensePtr->GetLicenseDescription();
	targetObject.objectPtr = istd::IChangeableUniquePtr(configurationPtr.PopPtr());

	return CR_CONVERTED;
}


} // namespace prolifegql


