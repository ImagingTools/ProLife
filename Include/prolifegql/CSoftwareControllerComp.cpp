#include <prolifegql/CSoftwareControllerComp.h>

// Standard includes
#include <optional>

// ACF includes
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtbase/CObjectLink.h>
#include <imtlic/IProductInfo.h>
#include <imtgql/imtgql.h>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/ICustomerInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>


namespace prolifegql
{


sdl::prolife::Licenses::CSplitLicensePayload CSoftwareControllerComp::OnSplitLicense(
			const sdl::prolife::Licenses::CSplitLicenseGqlRequest& splitLicenseRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
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
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
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

	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "ParentInstanceId";
	fieldFilter.filterValue = parentLicenseId;

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.AddFieldFilter(fieldFilter);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	// Get all objects from collection and filter by ParentInstanceId
	QByteArrayList allIds = m_softwareProductCollectionCompPtr->GetElementIds(0, -1, &filterParam);

	QList<sdl::prolife::Licenses::CChildLicenseItem::V1_0> childItems;

	for (const QByteArray& id : std::as_const(allIds)){
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
		if (currentParentId != parentLicenseId){
			continue;
		}

		sdl::prolife::Licenses::CChildLicenseItem::V1_0 item;

		item.id = id;
		item.productCount = softwarePtr->GetProductCount();

		// Get account ID
		QByteArray customerId = softwarePtr->GetCustomerId();
		if (!customerId.isEmpty()){
			item.accountId = customerId;

			// Try to get account name
			if (m_accountCollectionCompPtr.IsValid()){
				imtbase::IObjectCollection::DataPtr accountDataPtr;
				if (m_accountCollectionCompPtr->GetObjectData(customerId, accountDataPtr)){
					const prolifedata::ICustomerInfo* accountPtr = dynamic_cast<const prolifedata::ICustomerInfo*>(accountDataPtr.GetPtr());
					if (accountPtr != nullptr){
						item.accountName = accountPtr->GetName();
					}
				}
			}
		}

		// Calculate bound and available counts
		int boundCount = 0;
		if (m_hardwareBindingCollectionCompPtr.IsValid()){
			imtbase::IComplexCollectionFilter::FieldFilter arrayFieldFilter;
			arrayFieldFilter.fieldId = "SoftwareIds";
			arrayFieldFilter.filterValue = QVariantList({id});
			arrayFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_ARRAY_HAS_ANY;

			imtbase::CComplexCollectionFilter arrayComplexFilter;
			arrayComplexFilter.AddFieldFilter(arrayFieldFilter);

			iprm::CParamsSet arrayFilterParam;
			arrayFilterParam.SetEditableParameter("ComplexFilter", &arrayComplexFilter);

			QByteArrayList hardwareBindingIds = m_hardwareBindingCollectionCompPtr->GetElementIds(0, -1, &arrayFilterParam);
			boundCount = hardwareBindingIds.size();
		}
		
		int totalCount = softwarePtr->GetProductCount();
		item.boundCount = boundCount;
		item.availableCount = totalCount - boundCount;
		
		// Check if bound to hardware
		QByteArray serialNumber = softwarePtr->GetSerialNumber();
		item.softwareId = serialNumber;

		childItems.append(item);
	}

	retVal.Version_1_0->items.Emplace().FromList(childItems);
	retVal.Version_1_0->ok = true;
	retVal.Version_1_0->message = QString("Found %1 child license(s)").arg(childItems.size());

	return retVal;
}


sdl::prolife::Licenses::CRevokeLicensePayload CSoftwareControllerComp::OnRevokeLicense(
			const sdl::prolife::Licenses::CRevokeLicenseGqlRequest& revokeLicenseRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
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

	// Calculate how many licenses are bound to hardware
	int boundCount = 0;
	if (m_hardwareBindingCollectionCompPtr.IsValid()){
		imtbase::IComplexCollectionFilter::FieldFilter arrayFieldFilter;
		arrayFieldFilter.fieldId = "SoftwareIds";
		arrayFieldFilter.filterValue = QVariantList({childLicenseId});
		arrayFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_ARRAY_HAS_ANY;

		imtbase::CComplexCollectionFilter arrayComplexFilter;
		arrayComplexFilter.AddFieldFilter(arrayFieldFilter);

		iprm::CParamsSet arrayFilterParam;
		arrayFilterParam.SetEditableParameter("ComplexFilter", &arrayComplexFilter);

		QByteArrayList hardwareBindingIds = m_hardwareBindingCollectionCompPtr->GetElementIds(0, -1, &arrayFilterParam);
		boundCount = hardwareBindingIds.size();
	}

	int currentChildCount = childSoftwarePtr->GetProductCount();
	int availableCount = currentChildCount - boundCount;

	// Check if we can revoke the requested count
	if (revokeCount > availableCount){
		errorMessage = QString("Unable to revoke license. Error: Revoke count (%1) exceeds available count (%2). %3 licenses are bound to hardware.").arg(revokeCount).arg(availableCount).arg(boundCount);
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

	// Update parent license count
	int currentParentCount = parentSoftwarePtr->GetProductCount();
	parentSoftwarePtr->SetProductCount(currentParentCount + revokeCount);
	
	if (!m_softwareProductCollectionCompPtr->SetObjectData(parentLicenseId, *parentSoftwarePtr)){
		errorMessage = QString("Unable to revoke license. Error: Failed to update parent license");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	// Update or delete child license
	// Only delete if revoking all available licenses AND no licenses are bound
	if (revokeCount == availableCount && boundCount == 0){
		// Remove child license entirely (no licenses left at all)
		if (!m_softwareProductCollectionCompPtr->RemoveElements({childLicenseId})){
			errorMessage = QString("Unable to revoke license. Error: Failed to remove child license");
			retVal.Version_1_0->message = errorMessage;
			// Try to rollback parent update
			parentSoftwarePtr->SetProductCount(currentParentCount);
			if (!m_softwareProductCollectionCompPtr->SetObjectData(parentLicenseId, *parentSoftwarePtr)){
				errorMessage += QString(". CRITICAL: Rollback failed - parent license count may be inconsistent");
				retVal.Version_1_0->message = errorMessage;
			}
			return retVal;
		}
		retVal.Version_1_0->message = QString("License revoked successfully. Child license removed.");
	}
	else {
		// Update child license count (either some available left, or some are bound)
		childSoftwarePtr->SetProductCount(currentChildCount - revokeCount);
		
		if (!m_softwareProductCollectionCompPtr->SetObjectData(childLicenseId, *childSoftwarePtr)){
			errorMessage = QString("Unable to revoke license. Error: Failed to update child license");
			retVal.Version_1_0->message = errorMessage;
			// Try to rollback parent update
			parentSoftwarePtr->SetProductCount(currentParentCount);
			if (!m_softwareProductCollectionCompPtr->SetObjectData(parentLicenseId, *parentSoftwarePtr)){
				errorMessage += QString(". CRITICAL: Rollback failed - parent license count may be inconsistent");
				retVal.Version_1_0->message = errorMessage;
			}
			return retVal;
		}
		retVal.Version_1_0->message = QString("License revoked successfully. %1 license(s) returned to parent.").arg(revokeCount);
	}

	retVal.Version_1_0->ok = true;
	
	return retVal;
}


sdl::prolife::Licenses::CParentChainListPayload CSoftwareControllerComp::OnParentChainList(
			const sdl::prolife::Licenses::CParentChainListGqlRequest& parentChainRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::Licenses::CParentChainListPayload retVal;
	retVal.Version_1_0.Emplace();
	retVal.Version_1_0->ok = false;

	if (!m_softwareProductCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get parent chain. Error: Software product collection is not set");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	sdl::prolife::Licenses::ParentChainListRequestArguments inputArguments = parentChainRequest.GetRequestedArguments();
	if (!inputArguments.input.Version_1_0){
		errorMessage = QString("Unable to get parent chain. Error: Invalid input arguments");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	sdl::prolife::Licenses::CParentChainListInput::V1_0& input = *inputArguments.input.Version_1_0;

	if (!input.licenseId){
		errorMessage = QString("Unable to get parent chain. Error: License ID is missing");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	QByteArray currentLicenseId = *input.licenseId;
	QList<sdl::prolife::Licenses::CParentChainItem::V1_0> chainItems;

	// Traverse up the parent chain
	int level = 0;
	QSet<QByteArray> visitedIds; // Prevent infinite loops in case of circular references
	const int MAX_PARENT_CHAIN_DEPTH = 100; // Maximum depth to prevent infinite loops

	while (!currentLicenseId.isEmpty() && level < MAX_PARENT_CHAIN_DEPTH) {
		// Check for circular reference
		if (visitedIds.contains(currentLicenseId)){
			errorMessage = QString("Unable to get parent chain. Error: Circular reference detected");
			retVal.Version_1_0->message = errorMessage;
			return retVal;
		}
		visitedIds.insert(currentLicenseId);

		// Get software data
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_softwareProductCollectionCompPtr->GetObjectData(currentLicenseId, dataPtr)){
			break; // License not found, end of chain
		}

		imtlic::IProductInstanceInfo* softwarePtr = idata::GetData<imtlic::IProductInstanceInfo>(dataPtr);
		if (!softwarePtr){
			break;
		}

		// Add item to chain
		sdl::prolife::Licenses::CParentChainItem::V1_0 chainItem;
		chainItem.id = currentLicenseId;
		chainItem.serialNumber = softwarePtr->GetSerialNumber();
		chainItem.project = prolifedata::GetSoftwareProject(softwarePtr);
		chainItem.level = level;

		// Get product name
		QByteArray productId = prolifedata::GetSoftwareProductId(softwarePtr);
		if (!productId.isEmpty()){
			imtlic::IProductInfo* productPtr = prolifedata::GetCachedProductInfoPtr(productId);
			if (productPtr){
				chainItem.productName = productPtr->GetName();
			}
		}

		chainItems.append(chainItem);

		// Move to parent
		QByteArray parentId = softwarePtr->GetParentInstanceId();
		if (parentId.isEmpty()){
			break; // No more parents, end of chain
		}

		currentLicenseId = parentId;
		level++;
	}

	// Check if we hit the maximum depth limit
	if (level >= MAX_PARENT_CHAIN_DEPTH && !currentLicenseId.isEmpty()){
		errorMessage = QString("Unable to get complete parent chain. Error: Maximum depth limit (%1) reached. Chain may be incomplete.").arg(MAX_PARENT_CHAIN_DEPTH);
		retVal.Version_1_0->message = errorMessage;
		retVal.Version_1_0->items = chainItems;
		retVal.Version_1_0->ok = false;
		return retVal;
	}

	retVal.Version_1_0->items = chainItems;
	retVal.Version_1_0->ok = true;
	retVal.Version_1_0->message = QString("");

	return retVal;
}


sdl::prolife::Licenses::CLicenseTreePayload CSoftwareControllerComp::OnLicenseTree(
			const sdl::prolife::Licenses::CLicenseTreeGqlRequest& licenseTreeRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::Licenses::CLicenseTreePayload retVal;
	retVal.Version_1_0.Emplace();
	retVal.Version_1_0->ok = false;

	if (!m_softwareProductCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get license tree. Error: Software product collection is not set");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	sdl::prolife::Licenses::LicenseTreeRequestArguments inputArguments = licenseTreeRequest.GetRequestedArguments();
	if (!inputArguments.input.Version_1_0){
		errorMessage = QString("Unable to get license tree. Error: Invalid input arguments");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	sdl::prolife::Licenses::CLicenseTreeInput::V1_0& input = *inputArguments.input.Version_1_0;

	if (!input.licenseId){
		errorMessage = QString("Unable to get license tree. Error: License ID is missing");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	QByteArray startLicenseId = *input.licenseId;

	// First, find the root of the tree by traversing up the parent chain
	QByteArray rootLicenseId = startLicenseId;
	QSet<QByteArray> visitedForRoot;
	const int MAX_DEPTH = 100;
	int depth = 0;

	while (depth < MAX_DEPTH){
		if (visitedForRoot.contains(rootLicenseId)){
			errorMessage = QString("Unable to get license tree. Error: Circular reference detected in parent chain");
			retVal.Version_1_0->message = errorMessage;
			return retVal;
		}
		visitedForRoot.insert(rootLicenseId);

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_softwareProductCollectionCompPtr->GetObjectData(rootLicenseId, dataPtr)){
			errorMessage = QString("Unable to get license tree. Error: License not found");
			retVal.Version_1_0->message = errorMessage;
			return retVal;
		}

		imtlic::IProductInstanceInfo* softwarePtr = idata::GetData<imtlic::IProductInstanceInfo>(dataPtr);
		if (!softwarePtr){
			errorMessage = QString("Unable to get license tree. Error: Invalid license data");
			retVal.Version_1_0->message = errorMessage;
			return retVal;
		}

		QByteArray parentId = softwarePtr->GetParentInstanceId();
		if (parentId.isEmpty()){
			break; // Found root
		}

		rootLicenseId = parentId;
		depth++;
	}

	if (depth >= MAX_DEPTH){
		errorMessage = QString("Unable to get license tree. Error: Maximum depth exceeded while finding root");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	// Now build the tree recursively from the root
	auto buildNode = [this](const QByteArray& licenseId, auto& buildNodeRef, int currentDepth) -> std::optional<sdl::prolife::Licenses::CLicenseTreeNode::V1_0> {
		const int MAX_TREE_DEPTH = 100;
		if (currentDepth >= MAX_TREE_DEPTH){
			return std::nullopt;
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!m_softwareProductCollectionCompPtr->GetObjectData(licenseId, dataPtr)){
			return std::nullopt;
		}

		imtlic::IProductInstanceInfo* softwarePtr = idata::GetData<imtlic::IProductInstanceInfo>(dataPtr);
		if (!softwarePtr){
			return std::nullopt;
		}

		sdl::prolife::Licenses::CLicenseTreeNode::V1_0 node;
		node.id = licenseId;
		node.serialNumber = softwarePtr->GetSerialNumber();
		node.project = prolifedata::GetSoftwareProject(softwarePtr);
		node.productCount = softwarePtr->GetProductCount();
		node.parentId = softwarePtr->GetParentInstanceId();

		// Get product name
		QByteArray productId = prolifedata::GetSoftwareProductId(softwarePtr);
		if (!productId.isEmpty()){
			imtlic::IProductInfo* productPtr = prolifedata::GetCachedProductInfoPtr(productId);
			if (productPtr){
				node.productName = productPtr->GetName();
			}
		}

		// Calculate bound count
		int boundCount = 0;
		if (m_hardwareBindingCollectionCompPtr.IsValid()){
			imtbase::IComplexCollectionFilter::FieldFilter arrayFieldFilter;
			arrayFieldFilter.fieldId = "SoftwareIds";
			arrayFieldFilter.filterValue = QVariantList({licenseId});
			arrayFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_ARRAY_HAS_ANY;

			imtbase::CComplexCollectionFilter arrayComplexFilter;
			arrayComplexFilter.AddFieldFilter(arrayFieldFilter);

			iprm::CParamsSet arrayFilterParam;
			arrayFilterParam.SetEditableParameter("ComplexFilter", &arrayComplexFilter);

			QByteArrayList hardwareBindingIds = m_hardwareBindingCollectionCompPtr->GetElementIds(0, -1, &arrayFilterParam);
			boundCount = hardwareBindingIds.size();
		}

		node.boundCount = boundCount;
		node.availableCount = node.productCount - boundCount;

		// Find all children
		imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
		fieldFilter.fieldId = "ParentInstanceId";
		fieldFilter.filterValue = licenseId;

		imtbase::CComplexCollectionFilter complexFilter;
		complexFilter.AddFieldFilter(fieldFilter);

		iprm::CParamsSet filterParam;
		filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

		QByteArrayList childIds = m_softwareProductCollectionCompPtr->GetElementIds(0, -1, &filterParam);

		QList<sdl::prolife::Licenses::CLicenseTreeNode::V1_0> children;
		for (const QByteArray& childId : std::as_const(childIds)){
			auto childNode = buildNodeRef(childId, buildNodeRef, currentDepth + 1);
			if (childNode.has_value()){
				children.append(childNode.value());
			}
		}

		node.children = children;

		return node;
	};

	auto rootNode = buildNode(rootLicenseId, buildNode, 0);
	if (!rootNode.has_value()){
		errorMessage = QString("Unable to get license tree. Error: Failed to build tree");
		retVal.Version_1_0->message = errorMessage;
		return retVal;
	}

	retVal.Version_1_0->rootNode = rootNode.value();
	retVal.Version_1_0->ok = true;
	retVal.Version_1_0->message = QString("");

	return retVal;
}


} // namespace prolifegql


