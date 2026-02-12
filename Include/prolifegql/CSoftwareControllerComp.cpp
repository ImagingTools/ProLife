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
	retVal.Version_1_0.Emplace();
	retVal.Version_1_0->ok = false;

	if (!m_softwareProductCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to split license. Error: Software product collection is not set");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	if (!m_accountCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to split license. Error: Account collection is not set");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	if (!m_softwareInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to split license. Error: Software factory is not set");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	sdl::prolife::Licenses::SplitLicenseRequestArguments inputArguments = splitLicenseRequest.GetRequestedArguments();
	if (!inputArguments.input.Version_1_0){
		errorMessage = QString("Unable to split license. Error: Invalid input arguments");
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

	QByteArray productId = originalSoftwarePtr->GetProductId();

	// Copy properties from original license
	if (!newSoftwarePtr->CopyFrom(*originalSoftwarePtr)){
		errorMessage = QString("Unable to split license. Error: Failed to copy software");
		retVal.Version_1_0->ok = false;
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	// Generate new UUID for the split license
	QByteArray newLicenseId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	newSoftwarePtr->SetObjectUuid(newLicenseId);

	newSoftwarePtr->SetupProductInstance(productId, "", accountId);
	newSoftwarePtr->SetProductCount(licenseCount);
	newSoftwarePtr->SetSerialNumber("");
	newSoftwarePtr->SetOrderId("");
	newSoftwarePtr->SetParentInstanceId(licenseId);

	// Add the new software instance to the collection
	QByteArray result = m_softwareProductCollectionCompPtr->InsertNewObject("SoftwareProduct", "", "", newSoftwareInstancePtr.GetPtr(), newLicenseId);
	if (result.isEmpty()){
		errorMessage = QString("Unable to split license. Error: Failed to add new license to collection");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	// Update the original license count
	int remainingCount = originalCount - licenseCount;
	originalSoftwarePtr->SetProductCount(remainingCount);

	// Update original software
	if (!m_softwareProductCollectionCompPtr->SetObjectData(licenseId, *originalSoftwarePtr)){
		errorMessage = QString("Unable to split license. Error: Failed to update software");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	retVal.Version_1_0->ok = true;
	retVal.Version_1_0->message = QString("License split successfully. New license ID: %1").arg(QString::fromUtf8(newLicenseId));

	return retVal;
}


sdl::prolife::Licenses::CChildLicensesListPayload CSoftwareControllerComp::OnChildLicensesList(
			const sdl::prolife::Licenses::CChildLicensesListGqlRequest& childLicensesRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Licenses::CChildLicensesListPayload retVal;
	retVal.Version_1_0.Emplace();
	retVal.Version_1_0->ok = false;

	if (!m_softwareProductCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get child licenses. Error: Software product collection is not set");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	sdl::prolife::Licenses::ChildLicensesListRequestArguments inputArguments = childLicensesRequest.GetRequestedArguments();
	if (!inputArguments.input.Version_1_0){
		errorMessage = QString("Unable to get child licenses. Error: Invalid input arguments");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	sdl::prolife::Licenses::CChildLicensesListInput::V1_0& input = *inputArguments.input.Version_1_0;

	if (!input.parentLicenseId){
		errorMessage = QString("Unable to get child licenses. Error: Parent license ID is missing");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	QByteArray parentLicenseId = *input.parentLicenseId;

	// Get all objects from collection and filter by ParentInstanceId
	QList<QByteArray> allIds = m_softwareProductCollectionCompPtr->GetObjectsIds();
	
	QList<sdl::prolife::Licenses::CChildLicenseItem> childItems;
	
	for (const QByteArray& id : allIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_softwareProductCollectionCompPtr->GetObjectData(id, dataPtr)){
			continue;
		}

		prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwarePtr = 
			dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
		
		if (softwarePtr == nullptr){
			continue;
		}

		// Check if this license has the specified parent
		QByteArray currentParentId = softwarePtr->GetParentInstanceId();
		if (currentParentId == parentLicenseId){
			sdl::prolife::Licenses::CChildLicenseItem item;
			item.Version_1_0.Emplace();
			
			item.Version_1_0->id = id;
			item.Version_1_0->productCount = softwarePtr->GetProductCount();
			
			// Get account ID
			QByteArray customerId = softwarePtr->GetCustomerId();
			if (!customerId.isEmpty()){
				item.Version_1_0->accountId = customerId;
				
				// Try to get account name
				if (m_accountCollectionCompPtr.IsValid()){
					imtbase::IObjectCollection::DataPtr accountDataPtr;
					if (m_accountCollectionCompPtr->GetObjectData(customerId, accountDataPtr)){
						imtbase::IIdentifiableInfo* accountPtr = 
							dynamic_cast<imtbase::IIdentifiableInfo*>(accountDataPtr.GetPtr());
						if (accountPtr != nullptr){
							item.Version_1_0->accountName = accountPtr->GetName();
						}
					}
				}
			}
			
			// Check if bound to hardware
			QByteArray serialNumber = softwarePtr->GetSerialNumber();
			item.Version_1_0->isBound = !serialNumber.isEmpty();
			item.Version_1_0->hardwareId = serialNumber;
			
			childItems.append(item);
		}
	}

	retVal.Version_1_0->items = childItems;
	retVal.Version_1_0->ok = true;
	retVal.Version_1_0->message = QString("Found %1 child license(s)").arg(childItems.size());

	return retVal;
}


sdl::prolife::Licenses::CRevokeLicensePayload CSoftwareControllerComp::OnRevokeLicense(
			const sdl::prolife::Licenses::CRevokeLicenseGqlRequest& revokeLicenseRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Licenses::CRevokeLicensePayload retVal;
	retVal.Version_1_0.Emplace();
	retVal.Version_1_0->ok = false;

	if (!m_softwareProductCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to revoke license. Error: Software product collection is not set");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	sdl::prolife::Licenses::RevokeLicenseRequestArguments inputArguments = revokeLicenseRequest.GetRequestedArguments();
	if (!inputArguments.input.Version_1_0){
		errorMessage = QString("Unable to revoke license. Error: Invalid input arguments");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	sdl::prolife::Licenses::CRevokeLicenseInput::V1_0& input = *inputArguments.input.Version_1_0;

	if (!input.childLicenseId){
		errorMessage = QString("Unable to revoke license. Error: Child license ID is missing");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	if (!input.revokeCount){
		errorMessage = QString("Unable to revoke license. Error: Revoke count is missing");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	QByteArray childLicenseId = *input.childLicenseId;
	int revokeCount = *input.revokeCount;

	if (revokeCount <= 0){
		errorMessage = QString("Unable to revoke license. Error: Revoke count must be greater than 0");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	// Get the child license
	imtbase::IObjectCollection::DataPtr childDataPtr;
	if (!m_softwareProductCollectionCompPtr->GetObjectData(childLicenseId, childDataPtr)){
		errorMessage = QString("Unable to revoke license. Error: Child license not found with ID '%1'").arg(QString::fromUtf8(childLicenseId));
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	prolifedata::COrderedIdentifiableSoftwareInstanceInfo* childSoftwarePtr = 
		dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(childDataPtr.GetPtr());
	
	if (childSoftwarePtr == nullptr){
		errorMessage = QString("Unable to revoke license. Error: Invalid child software instance");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	// Check if license is bound to hardware
	QByteArray serialNumber = childSoftwarePtr->GetSerialNumber();
	if (!serialNumber.isEmpty()){
		errorMessage = QString("Unable to revoke license. Error: License is bound to hardware and cannot be revoked");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	// Get parent license ID
	QByteArray parentLicenseId = childSoftwarePtr->GetParentInstanceId();
	if (parentLicenseId.isEmpty()){
		errorMessage = QString("Unable to revoke license. Error: Child license has no parent");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	// Get parent license
	imtbase::IObjectCollection::DataPtr parentDataPtr;
	if (!m_softwareProductCollectionCompPtr->GetObjectData(parentLicenseId, parentDataPtr)){
		errorMessage = QString("Unable to revoke license. Error: Parent license not found with ID '%1'").arg(QString::fromUtf8(parentLicenseId));
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	prolifedata::COrderedIdentifiableSoftwareInstanceInfo* parentSoftwarePtr = 
		dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(parentDataPtr.GetPtr());
	
	if (parentSoftwarePtr == nullptr){
		errorMessage = QString("Unable to revoke license. Error: Invalid parent software instance");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	// Check if we have enough licenses to revoke
	int currentChildCount = childSoftwarePtr->GetProductCount();
	if (revokeCount > currentChildCount){
		errorMessage = QString("Unable to revoke license. Error: Revoke count (%1) exceeds available count (%2)").arg(revokeCount).arg(currentChildCount);
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	// Update parent license count
	int currentParentCount = parentSoftwarePtr->GetProductCount();
	parentSoftwarePtr->SetProductCount(currentParentCount + revokeCount);
	
	if (!m_softwareProductCollectionCompPtr->SetObjectData(parentLicenseId, *parentSoftwarePtr)){
		errorMessage = QString("Unable to revoke license. Error: Failed to update parent license");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	// Update or delete child license
	if (revokeCount == currentChildCount){
		// Remove child license entirely
		if (!m_softwareProductCollectionCompPtr->RemoveObject(childLicenseId)){
			errorMessage = QString("Unable to revoke license. Error: Failed to remove child license");
			retVal.Version_1_0->message = errorMessage;
			// Try to rollback parent update
			parentSoftwarePtr->SetProductCount(currentParentCount);
			m_softwareProductCollectionCompPtr->SetObjectData(parentLicenseId, *parentSoftwarePtr);
			return retVal;
		}
		retVal.Version_1_0->message = QString("License revoked successfully. Child license removed.");
	}
	else {
		// Update child license count
		childSoftwarePtr->SetProductCount(currentChildCount - revokeCount);
		
		if (!m_softwareProductCollectionCompPtr->SetObjectData(childLicenseId, *childSoftwarePtr)){
			errorMessage = QString("Unable to revoke license. Error: Failed to update child license");
			retVal.Version_1_0->message = errorMessage;
			// Try to rollback parent update
			parentSoftwarePtr->SetProductCount(currentParentCount);
			m_softwareProductCollectionCompPtr->SetObjectData(parentLicenseId, *parentSoftwarePtr);
			return retVal;
		}
		retVal.Version_1_0->message = QString("License revoked successfully. %1 license(s) returned to parent.").arg(revokeCount);
	}

	retVal.Version_1_0->ok = true;
	
	return retVal;
}


} // namespace prolifegql


