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
// Focus on Split operations only for now (Revoke to be added later)
static void BuildTreeRecursive(
	const QByteArray& licenseId,
	const imtbase::IObjectCollection& licenseCollection,
	const imtauth::IUserActionManager& userActionManager,
	sdl::prolife::Licenses::LicenseTreeNode& node,
	QSet<QByteArray>& visitedLicenses)
{
	// Prevent infinite loops
	if (visitedLicenses.contains(licenseId)){
		return;
	}
	visitedLicenses.insert(licenseId);

	// Get license data from collection
	imtbase::IDataObjectSharedPtr licensePtr = licenseCollection.GetObjectById(licenseId);
	if (!licensePtr.IsValid()){
		return;
	}

	// Populate node with license data
	node.Version_1_0.Emplace();
	node.Version_1_0->id = licenseId;
	
	QByteArray serialNumber;
	if (licensePtr->GetPropertyValue("serialNumber", serialNumber)){
		node.Version_1_0->serialNumber = QString::fromUtf8(serialNumber);
	}

	QByteArray parentId;
	if (licensePtr->GetPropertyValue("parentInstanceId", parentId)){
		node.Version_1_0->parentId = parentId;
	}

	qint32 productCount = 0;
	if (licensePtr->GetPropertyValue("productCount", productCount)){
		node.Version_1_0->productCount = productCount;
	}

	QByteArray accountId;
	if (licensePtr->GetPropertyValue("customerId", accountId)){
		node.Version_1_0->accountId = accountId;
	}

	// Find child licenses via SplitOut actions
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "targetId";
	fieldFilter.filterValue = licenseId;

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.AddFieldFilter(fieldFilter);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::IObjectCollection::Ids actionIds = userActionManager.GetUserActionIds(0, -1, &filterParam);

	// Process SplitOut actions to find children
	QVector<sdl::prolife::Licenses::LicenseTreeNode> children;
	for (const QByteArray& actionId : std::as_const(actionIds)){
		imtauth::IUserActionInfoUniquePtr actionPtr = userActionManager.GetUserAction(actionId);
		if (!actionPtr.IsValid()){
			continue;
		}

		imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo = actionPtr->GetActionTypeInfo();
		QByteArray actionType = actionTypeInfo.id;

		// Only process Split operations for now
		if (actionType == "SplitOut"){
			iser::ISerializableSharedPtr actionData = actionPtr->GetActionData();
			const prolifedata::CSplitOutAction* splitOut = dynamic_cast<const prolifedata::CSplitOutAction*>(actionData.GetPtr());
			if (splitOut){
				QByteArray childLicenseId = splitOut->GetNewLicenseId();
				
				// Recursively build child node
				sdl::prolife::Licenses::LicenseTreeNode childNode;
				BuildTreeRecursive(childLicenseId, licenseCollection, userActionManager, childNode, visitedLicenses);
				
				// Store operation info in child node
				if (childNode.Version_1_0.IsPresent()){
					childNode.Version_1_0->operationType = "split";
					childNode.Version_1_0->transferredCount = splitOut->GetMovedCount();
					children.append(childNode);
				}
			}
		}
	}

	// Add children to node
	if (!children.isEmpty()){
		node.Version_1_0->children = children;
	}
}

sdl::prolife::Licenses::LicenseTreeNode BuildLicenseTreeFromActions(
	const QByteArray& licenseId,
	const imtbase::IObjectCollection& licenseCollection,
	const imtauth::IUserActionManager& userActionManager,
	QString& errorMessage)
{
	sdl::prolife::Licenses::LicenseTreeNode rootNode;
	QSet<QByteArray> visitedLicenses;

	// Build hierarchical tree recursively
	BuildTreeRecursive(licenseId, licenseCollection, userActionManager, rootNode, visitedLicenses);

	return rootNode;
}


} // namespace prolifedata


