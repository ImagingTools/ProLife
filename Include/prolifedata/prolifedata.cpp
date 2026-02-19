#include <prolifedata/prolifedata.h>


// Qt includes
#include <QSet>

// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtlic/CProductInstanceInfo.h>
#include <imtlic/IProductInfo.h>
#include <imtauth/IUserRecentAction.h>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>
#include <prolifedata/CSplitOutAction.h>
#include <prolifedata/CSplitInAction.h>
#include <prolifedata/CRevokeOutAction.h>
#include <prolifedata/CRevokeInAction.h>
#include <prolifedata/CGroupFilter.h>


namespace prolifedata
{

// Constants for operation types
const QString OPERATION_TYPE_SPLIT = "split";


QString GetNameFromDeviceProductionStatus(prolifedata::IDeviceInfo::DeviceProductionStatus status)
{
	switch (status){
	case prolifedata::IDeviceInfo::DPS_NONE:
		return QString("None");
	case prolifedata::IDeviceInfo::DPS_ACCEPTED:
		return QString("Accepted");
	case prolifedata::IDeviceInfo::DPS_IN_PROGRESS:
		return QString("In Progress");
	case prolifedata::IDeviceInfo::DPS_CANCELED:
		return QString("Canceled");
	case prolifedata::IDeviceInfo::DPS_ON_HOLD:
		return QString("On Hold");
	case prolifedata::IDeviceInfo::DPS_FINISHED:
		return QString("Finished");
	case prolifedata::IDeviceInfo::DPS_DEFECTED:
		return QString("Defect");
	case prolifedata::IDeviceInfo::DPS_IN_REPAIR:
		return QString("In Repair");
	case prolifedata::IDeviceInfo::DPS_DECOMMISSIONED:
		return QString("Decommissioned");
	default:
		return QByteArray("None");
	}
}


QByteArray GetIdFromDeviceProductionStatus(prolifedata::IDeviceInfo::DeviceProductionStatus status)
{
	switch (status){
	case prolifedata::IDeviceInfo::DPS_NONE:
		return QByteArray("None");
	case prolifedata::IDeviceInfo::DPS_ACCEPTED:
		return QByteArray("Accepted");
	case prolifedata::IDeviceInfo::DPS_IN_PROGRESS:
		return QByteArray("InProgress");
	case prolifedata::IDeviceInfo::DPS_CANCELED:
		return QByteArray("Canceled");
	case prolifedata::IDeviceInfo::DPS_ON_HOLD:
		return QByteArray("OnHold");
	case prolifedata::IDeviceInfo::DPS_FINISHED:
		return QByteArray("Finished");
	case prolifedata::IDeviceInfo::DPS_DEFECTED:
		return QByteArray("Defected");
	case prolifedata::IDeviceInfo::DPS_IN_REPAIR:
		return QByteArray("InRepair");
	case prolifedata::IDeviceInfo::DPS_DECOMMISSIONED:
		return QByteArray("Decommissioned");
	default:
		return QByteArray("None");
	}
}


prolifedata::IDeviceInfo::DeviceProductionStatus GetProductionStatusFromId(const QByteArray& statusId)
{
	if (statusId == "None"){
		return prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_NONE;
	}
	else if (statusId == "Accepted"){
		return prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_ACCEPTED;
	}
	else if (statusId == "InProgress"){
		return prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_IN_PROGRESS;
	}
	else if (statusId == "Canceled"){
		return prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_CANCELED;
	}
	else if (statusId == "OnHold"){
		return prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_ON_HOLD;
	}
	else if (statusId == "Finished"){
		return prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_FINISHED;
	}
	else if (statusId == "Defected"){
		return prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_DEFECTED;
	}
	else if (statusId == "InRepair"){
		return prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_IN_REPAIR;
	}
	else if (statusId == "Decommissioned"){
		return prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_DECOMMISSIONED;
	}

	return prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_NONE;
}


QString GetNameFromOrderStatus(prolifedata::IOrderInfo::OrderStatus status)
{
	switch (status){
	case prolifedata::IOrderInfo::OrderStatus::OS_NONE:
	case prolifedata::IOrderInfo::OrderStatus::OS_ON_HOLD:
	case prolifedata::IOrderInfo::OrderStatus::OS_FINISHED:
	case prolifedata::IOrderInfo::OrderStatus::OS_CREATED:
		return QString("Created");
	case prolifedata::IOrderInfo::OrderStatus::OS_IN_PROGRESS:
		return QString("In Progress");
	case prolifedata::IOrderInfo::OrderStatus::OS_CANCELED:
		return QString("Canceled");
	case prolifedata::IOrderInfo::OrderStatus::OS_CLOSED:
		return QString("Closed");
	default:
		return QString("None");
	}
}


QByteArray GetIdFromOrderStatus(prolifedata::IOrderInfo::OrderStatus status)
{
	switch (status){
	case prolifedata::IOrderInfo::OrderStatus::OS_NONE:
	case prolifedata::IOrderInfo::OrderStatus::OS_ON_HOLD:
	case prolifedata::IOrderInfo::OrderStatus::OS_FINISHED:
	case prolifedata::IOrderInfo::OrderStatus::OS_CREATED:
		return QByteArray("Created");
	case prolifedata::IOrderInfo::OrderStatus::OS_IN_PROGRESS:
		return QByteArray("InProgress");
	case prolifedata::IOrderInfo::OrderStatus::OS_CANCELED:
		return QByteArray("Canceled");
	case prolifedata::IOrderInfo::OrderStatus::OS_CLOSED:
		return QByteArray("Closed");
	default:
		return QByteArray("None");
	}
}


prolifedata::IOrderInfo::OrderStatus GetOrderStatusFromId(const QByteArray& statusId)
{
	if (statusId == "None" ||
		statusId == "Created" ||
		statusId == "OnHold" ||
		statusId == "Finished"){
		return prolifedata::IOrderInfo::OrderStatus::OS_CREATED;
	}
	else if (statusId == "InProgress"){
		return prolifedata::IOrderInfo::OrderStatus::OS_IN_PROGRESS;
	}
	else if (statusId == "Canceled"){
		return prolifedata::IOrderInfo::OrderStatus::OS_CANCELED;
	}
	else if (statusId == "Closed"){
		return prolifedata::IOrderInfo::OrderStatus::OS_CLOSED;
	}

	return prolifedata::IOrderInfo::OrderStatus::OS_NONE;
}


bool CheckDeviceMacAddressExists(const QByteArray& deviceUuid, const QByteArray& macAddress, const imtbase::IObjectCollection& collection)
{
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "MacAddress";
	fieldFilter.filterValue = macAddress;
	
	imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
	groupFilter.fieldFilters << fieldFilter;
	
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(groupFilter);
	
	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::ICollectionInfo::Ids collectionIds = collection.GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty()){
		QByteArray id = collectionIds[0];
		if (deviceUuid != id){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (collection.GetObjectData(id, dataPtr)){
				prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(dataPtr.GetPtr());
				if (deviceInfoPtr != nullptr){
					QByteArray currentMacAddress = deviceInfoPtr->GetMacAddress().toLower();
					if (currentMacAddress == macAddress.toLower()){
						return false;
					}
				}
			}
		}
	}

	return true;
}


bool CheckDeviceSerialNumberExists(const QByteArray& deviceUuid, const QByteArray& serialNumber, const imtbase::IObjectCollection& collection)
{
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "SerialNumber";
	fieldFilter.filterValue = serialNumber;
	
	imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
	groupFilter.fieldFilters << fieldFilter;
	
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(groupFilter);
	
	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::ICollectionInfo::Ids collectionIds = collection.GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty()){
		QByteArray id = collectionIds[0];
		if (deviceUuid != id){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (collection.GetObjectData(id, dataPtr)){
				prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(dataPtr.GetPtr());
				if (deviceInfoPtr != nullptr){
					QByteArray currentSerialNumber = deviceInfoPtr->GetSerialNumber().toLower();
					if (currentSerialNumber == serialNumber.toLower()){
						return false;
					}
				}
			}
		}
	}

	return true;
}


bool CheckSoftwareSerialNumberExists(const QByteArray& deviceUuid, const QByteArray& serialNumber, const imtbase::IObjectCollection& collection)
{
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "SerialNumber";
	fieldFilter.filterValue = serialNumber;
	
	imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
	groupFilter.fieldFilters << fieldFilter;
	
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(groupFilter);
	
	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::IObjectCollection::Ids collectionIds = collection.GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty() && !serialNumber.isEmpty()){
		QByteArray objectId = collectionIds[0];
		if (objectId != deviceUuid){
			return false;
		}
	}

	return true;
}


// Helper function to recursively build hierarchical license tree from UserActions
// Processes both Split and Revoke operations from UserActions
// maxDepth: -1 for unlimited, 0 to not recurse to children, 1 for one level of children, etc.
static void BuildTreeRecursive(
	const QByteArray& licenseId,
	const imtbase::IObjectCollection& licenseCollection,
	const imtauth::IUserActionManager& userActionManager,
	sdl::prolife::Licenses::CLicenseTreeNode::V1_0& node,
	QSet<QByteArray>& visitedLicenses,
	bool searchParents,
	int maxDepth = -1)
{
	// Prevent infinite loops
	if (visitedLicenses.contains(licenseId)){
		return;
	}
	visitedLicenses.insert(licenseId);

	// Get license data from collection
	imtlic::IProductInstanceInfo* licensePtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (licenseCollection.GetObjectData(licenseId, dataPtr)){
		licensePtr = dynamic_cast<imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
	}

	if (licensePtr == nullptr){
		return;
	}

	// Populate node with license data
	node.id = licenseId;
	node.serialNumber = licensePtr->GetSerialNumber();
	node.parentId = licensePtr->GetParentInstanceId();
	node.productCount = licensePtr->GetProductCount();
	node.accountId = licensePtr->GetCustomerId();

	// Find child licenses via SplitOut and RevokeOut actions
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "targetId";
	fieldFilter.filterValue = licenseId;

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.AddFieldFilter(fieldFilter);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::IObjectCollection::Ids actionIds = userActionManager.GetUserActionIds(0, -1, &filterParam);

	// Process actions to find children, revoke edges, and calculate counts
	QList<sdl::prolife::Licenses::CLicenseTreeNode::V1_0> children;
	QList<sdl::prolife::Licenses::CRevokeEdge::V1_0> revokeEdges;
	
	// Track counts from UserActions data
	int initialCountFromAction = 0;
	int remainingCountFromAction = 0;
	bool hasInitialCount = false;
	bool hasRemainingCount = false;
	
	for (const QByteArray& actionId : std::as_const(actionIds)){
		imtauth::IUserActionInfoUniquePtr actionPtr = userActionManager.GetUserAction(actionId);
		if (!actionPtr.IsValid()){
			continue;
		}

		imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo = actionPtr->GetActionTypeInfo();
		QByteArray actionType = actionTypeInfo.id;

		// Process Split operations
		if (actionType == "SplitOut"){
			iser::ISerializableSharedPtr actionData = actionPtr->GetActionData();
			const prolifedata::CSplitOutAction* splitOut = dynamic_cast<const prolifedata::CSplitOutAction*>(actionData.GetPtr());
			if (splitOut){
				QByteArray childLicenseId = splitOut->GetNewLicenseId();

				// Get initial and remaining counts from SplitOut action
				if (!hasInitialCount){
					initialCountFromAction = splitOut->GetInitialCount();
					hasInitialCount = true;
				}
				if (!hasRemainingCount){
					remainingCountFromAction = splitOut->GetInitialCount() - splitOut->GetMovedCount();
					hasRemainingCount = true;
				}

				// Recursively build child node (only if maxDepth allows)
				if (maxDepth != 0){
					sdl::prolife::Licenses::CLicenseTreeNode::V1_0 childNode;
					int nextDepth = (maxDepth == -1) ? -1 : (maxDepth - 1);
					BuildTreeRecursive(childLicenseId, licenseCollection, userActionManager, childNode, visitedLicenses, false, nextDepth);

					if (childNode.id.HasValue()){
						childNode.operationType = "split";
						childNode.transferredCount = splitOut->GetMovedCount();
						childNode.initialCount = splitOut->GetInitialCount();
						childNode.remainingCount = splitOut->GetInitialCount() - splitOut->GetMovedCount();
						children.append(childNode);
					}
				}
			}
		}
		// Process Revoke operations
		else if (actionType == "RevokeOut"){
			iser::ISerializableSharedPtr actionData = actionPtr->GetActionData();
			const prolifedata::CRevokeOutAction* revokeOut = dynamic_cast<const prolifedata::CRevokeOutAction*>(actionData.GetPtr());
			if (revokeOut){
				// Create revoke edge from this node to parent
				sdl::prolife::Licenses::CRevokeEdge::V1_0 edge;
				edge.fromNodeId = licenseId;
				edge.toNodeId = revokeOut->GetParentLicenseId();
				edge.revokedCount = revokeOut->GetRevokedCount();
				revokeEdges.append(edge);
			}
		}
	}
	
	// Set initial and remaining counts on this node if found from actions
	if (hasInitialCount){
		node.initialCount = initialCountFromAction;
	}
	if (hasRemainingCount){
		node.remainingCount = remainingCountFromAction;
	}

	// Add children to node
	if (!children.isEmpty()){
		node.children.Emplace().FromList(children);
	}
	
	// Add revoke edges to node
	if (!revokeEdges.isEmpty()){
		node.revokeEdges.Emplace().FromList(revokeEdges);
	}
}

// Helper function to find parent license ID via SplitIn action
static QByteArray FindParentLicenseId(
	const QByteArray& licenseId,
	const imtauth::IUserActionManager& userActionManager)
{
	// Search for SplitIn action for this license
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "targetId";
	fieldFilter.filterValue = licenseId;

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.AddFieldFilter(fieldFilter);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::IObjectCollection::Ids actionIds = userActionManager.GetUserActionIds(0, -1, &filterParam);

	// Find the first SplitIn action
	for (const QByteArray& actionId : std::as_const(actionIds)){
		imtauth::IUserActionInfoUniquePtr actionPtr = userActionManager.GetUserAction(actionId);
		if (!actionPtr.IsValid()){
			continue;
		}

		imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo = actionPtr->GetActionTypeInfo();
		QByteArray actionType = actionTypeInfo.id;

		if (actionType == "SplitIn"){
			iser::ISerializableSharedPtr actionData = actionPtr->GetActionData();
			const prolifedata::CSplitInAction* splitIn = dynamic_cast<const prolifedata::CSplitInAction*>(actionData.GetPtr());
			if (splitIn){
				return splitIn->GetSourceLicenseId();
			}
		}
	}

	return QByteArray();
}

sdl::prolife::Licenses::CLicenseTreeNode::V1_0 BuildLicenseTreeFromActions(
	const QByteArray& licenseId,
	const imtbase::IObjectCollection& licenseCollection,
	const imtauth::IUserActionManager& userActionManager,
	QString& errorMessage,
	bool fullHierarchy)
{
	sdl::prolife::Licenses::CLicenseTreeNode::V1_0 rootNode;
	QSet<QByteArray> visitedLicenses;

	if (fullHierarchy){
		// Full hierarchy mode: find the root and build complete tree
		QByteArray rootLicenseId = licenseId;
		QSet<QByteArray> visitedParents;  // Prevent infinite loops when finding root
		
		while (true){
			if (visitedParents.contains(rootLicenseId)){
				// Circular reference detected, stop here
				break;
			}
			visitedParents.insert(rootLicenseId);
			
			QByteArray parentId = FindParentLicenseId(rootLicenseId, userActionManager);
			if (parentId.isEmpty()){
				// No parent found, this is the root
				break;
			}
			rootLicenseId = parentId;
		}

		// Build hierarchical tree recursively from the root (unlimited depth)
		BuildTreeRecursive(rootLicenseId, licenseCollection, userActionManager, rootNode, visitedLicenses, false, -1);
	}
	else{
		// Limited view mode: show only one level (parent + given license + children)
		// Build the parent node if it exists
		QByteArray parentId = FindParentLicenseId(licenseId, userActionManager);
		
		if (!parentId.isEmpty()){
			// Has a parent - build parent node with this license as child (depth=1 to get license and its children only)
			BuildTreeRecursive(parentId, licenseCollection, userActionManager, rootNode, visitedLicenses, false, 1);
		}
		else{
			// No parent - this is root, just build this license and its children (depth=1)
			BuildTreeRecursive(licenseId, licenseCollection, userActionManager, rootNode, visitedLicenses, false, 1);
		}
	}

	return rootNode;
}


} // namespace prolifedata


