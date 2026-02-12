#include <prolifegql/CSoftwareControllerComp.h>


// ACF includes
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtbase/CObjectLink.h>
#include <imtlic/IProductInfo.h>
#include <imtgql/imtgql.h>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>


namespace prolifegql
{


sdl::prolife::Licenses::CSplitLicensePayload CSoftwareControllerComp::OnSplitLicense(
	const sdl::prolife::Licenses::CSplitLicenseGqlRequest& splitLicenseRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	sdl::prolife::Licenses::CSplitLicensePayload retVal;
	retVal.Version_1_0 = std::make_unique<sdl::prolife::Licenses::CSplitLicensePayload::V1_0>();
	
	if (!m_softwareProductCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to split license. Error: Software product collection is not set");
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	if (!m_accountCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to split license. Error: Account collection is not set");
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	if (!m_softwareInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to split license. Error: Software factory is not set");
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	sdl::prolife::Licenses::SplitLicenseRequestArguments inputArguments = splitLicenseRequest.GetRequestedArguments();
	if (!inputArguments.input.Version_1_0){
		errorMessage = QString("Unable to split license. Error: Invalid input arguments");
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	sdl::prolife::Licenses::CSplitLicenseInput::V1_0& input = *inputArguments.input.Version_1_0;
	
	// Validate input
	if (!input.licenseId){
		errorMessage = QString("Unable to split license. Error: License ID is missing");
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	if (!input.licenseCount){
		errorMessage = QString("Unable to split license. Error: License count is missing");
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	if (!input.accountId){
		errorMessage = QString("Unable to split license. Error: Account ID is missing");
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	QByteArray licenseId = *input.licenseId;
	int licenseCount = *input.licenseCount;
	QByteArray accountId = *input.accountId;
	
	// Validate license count
	if (licenseCount <= 0){
		errorMessage = QString("Unable to split license. Error: License count must be greater than 0");
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	// Get the original license
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!m_softwareProductCollectionCompPtr->GetObjectData(licenseId, dataPtr)){
		errorMessage = QString("Unable to split license. Error: License not found with ID '%1'").arg(QString::fromUtf8(licenseId));
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	prolifedata::COrderedIdentifiableSoftwareInstanceInfo* originalSoftwarePtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
	if (originalSoftwarePtr == nullptr){
		errorMessage = QString("Unable to split license. Error: Invalid software instance");
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	// Check if license supports multiple products (can be split)
	if (!originalSoftwarePtr->IsMultiProduct()){
		errorMessage = QString("Unable to split license. Error: License does not support multiple products");
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	// Check if there are enough licenses to split
	int originalCount = originalSoftwarePtr->GetProductCount();
	if (licenseCount >= originalCount){
		errorMessage = QString("Unable to split license. Error: License count to split (%1) must be less than available count (%2)").arg(licenseCount).arg(originalCount);
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	// Verify account exists
	imtbase::IObjectCollection::DataPtr accountDataPtr;
	if (!m_accountCollectionCompPtr->GetObjectData(accountId, accountDataPtr)){
		errorMessage = QString("Unable to split license. Error: Account not found with ID '%1'").arg(QString::fromUtf8(accountId));
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	// Create a new software instance for the split license
	istd::TUniqueInterfacePtr<imtlic::IProductInstanceInfo> newSoftwareInstancePtr = m_softwareInfoFactCompPtr.CreateInstance();
	if (!newSoftwareInstancePtr.IsValid()){
		errorMessage = QString("Unable to split license. Error: Failed to create new software instance");
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	prolifedata::COrderedIdentifiableSoftwareInstanceInfo* newSoftwarePtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(newSoftwareInstancePtr.GetPtr());
	if (newSoftwarePtr == nullptr){
		errorMessage = QString("Unable to split license. Error: Failed to cast new software instance");
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	// Generate new UUID for the split license
	QByteArray newLicenseId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	newSoftwarePtr->SetObjectUuid(newLicenseId);
	
	// Copy properties from original license
	newSoftwarePtr->SetProductId(originalSoftwarePtr->GetProductId());
	newSoftwarePtr->SetFactoryId(originalSoftwarePtr->GetFactoryId());
	newSoftwarePtr->SetSerialNumber(originalSoftwarePtr->GetSerialNumber());
	newSoftwarePtr->SetProject(originalSoftwarePtr->GetProject());
	newSoftwarePtr->SetInternalUse(originalSoftwarePtr->IsInternalUse());
	
	// Set the split count and account
	newSoftwarePtr->SetProductCount(licenseCount);
	newSoftwarePtr->SetMultiProduct(true);
	newSoftwarePtr->SetAccountId(accountId);
	
	// Copy license instances
	imtbase::ICollectionInfo::Ids licenseIds = originalSoftwarePtr->GetLicenseInstances().GetElementIds();
	for (const QByteArray& licId : licenseIds){
		const imtlic::ILicenseInstance* licenseInstancePtr = originalSoftwarePtr->GetLicenseInstance(licId);
		if (licenseInstancePtr != nullptr){
			newSoftwarePtr->GetLicenseInstances().AddElement(licenseInstancePtr->CloneAsChangeable());
		}
	}
	
	// Add the new software instance to the collection
	if (!m_softwareProductCollectionCompPtr->AddObjectData(newLicenseId, newSoftwareInstancePtr.CastToChangeable())){
		errorMessage = QString("Unable to split license. Error: Failed to add new license to collection");
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}
	
	// Update the original license count
	int remainingCount = originalCount - licenseCount;
	originalSoftwarePtr->SetProductCount(remainingCount);
	
	if (remainingCount == 1){
		// If only one license remains, it's no longer a multi-product
		originalSoftwarePtr->SetMultiProduct(false);
	}
	
	// Notify that objects have changed
	if (m_softwareOperationContextControllerCompPtr.IsValid()){
		m_softwareOperationContextControllerCompPtr->ObjectUpdated(licenseId);
		m_softwareOperationContextControllerCompPtr->ObjectAdded(newLicenseId);
	}
	
	retVal.Version_1_0->ok = true;
	retVal.Version_1_0->message = QString("License split successfully. New license ID: %1").arg(QString::fromUtf8(newLicenseId));
	
	return retVal;
}


} // namespace prolifegql


