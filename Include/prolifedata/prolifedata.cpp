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


// Helper function to recursively collect license actions
static void CollectLicenseActionsRecursive(
	const QByteArray& licenseId,
	const imtauth::IUserActionManager& userActionManager,
	QVector<sdl::prolife::Licenses::LicenseTreeNode>& result,
	QSet<QByteArray>& visitedLicenses)
{
	// Prevent infinite loops
	if (visitedLicenses.contains(licenseId)){
		return;
	}
	visitedLicenses.insert(licenseId);

	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "targetId";
	fieldFilter.filterValue = licenseId;

	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.AddFieldFilter(fieldFilter);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

	imtbase::IObjectCollection::Ids actionIds = userActionManager.GetUserActionIds(0, -1, &filterParam);

	// Process each action and create corresponding node
	for (const QByteArray& actionId : std::as_const(actionIds)){
		imtauth::IUserActionInfoUniquePtr actionPtr = userActionManager.GetUserAction(actionId);
		if (!actionPtr.IsValid()){
			continue;
		}

		imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo = actionPtr->GetActionTypeInfo();
		QByteArray actionType = actionTypeInfo.id;

		iser::ISerializableSharedPtr actionData = actionPtr->GetActionData();
		if (actionType == "SplitOut"){
			const prolifedata::CSplitOutAction* splitOut = dynamic_cast<const prolifedata::CSplitOutAction*>(actionData.GetPtr());
			if (splitOut){
				sdl::prolife::Licenses::CSplitOutNode node;
				node.Version_1_0.Emplace();

				node.Version_1_0->id = licenseId;
				node.Version_1_0->nodeType = sdl::prolife::Licenses::NodeType::SplitOut;
				node.Version_1_0->newLicenseId = splitOut->GetNewLicenseId();
				node.Version_1_0->initialCount = splitOut->GetInitialCount();
				node.Version_1_0->movedCount = splitOut->GetMovedCount();

				sdl::prolife::Licenses::LicenseTreeNode treeNode(node);
				result.append(treeNode);

				// Recursively collect actions for the new license
				CollectLicenseActionsRecursive(splitOut->GetNewLicenseId(), userActionManager, result, visitedLicenses);
			}
		}
		else if (actionType == "SplitIn"){
			const prolifedata::CSplitInAction* splitIn = dynamic_cast<const prolifedata::CSplitInAction*>(actionData.GetPtr());
			if (splitIn){
				sdl::prolife::Licenses::CSplitInNode node;
				node.Version_1_0.Emplace();
				node.Version_1_0->id = licenseId;
				node.Version_1_0->nodeType = sdl::prolife::Licenses::NodeType::SplitIn;
				node.Version_1_0->sourceLicenseId = splitIn->GetSourceLicenseId();
				node.Version_1_0->receivedCount = splitIn->GetReceivedCount();

				sdl::prolife::Licenses::LicenseTreeNode treeNode(node);
				result.append(treeNode);

				// Recursively collect actions for the source license
				CollectLicenseActionsRecursive(splitIn->GetSourceLicenseId(), userActionManager, result, visitedLicenses);
			}
		}
		else if (actionType == "RevokeOut"){
			const prolifedata::CRevokeOutAction* revokeOut = dynamic_cast<const prolifedata::CRevokeOutAction*>(actionData.GetPtr());
			if (revokeOut){
				sdl::prolife::Licenses::CRevokeOutNode node;
				node.Version_1_0.Emplace();
				node.Version_1_0->id = licenseId;
				node.Version_1_0->nodeType = sdl::prolife::Licenses::NodeType::RevokeOut;
				node.Version_1_0->parentLicenseId = revokeOut->GetParentLicenseId();
				node.Version_1_0->initialCount = revokeOut->GetInitialCount();
				node.Version_1_0->revokedCount = revokeOut->GetRevokedCount();

				sdl::prolife::Licenses::LicenseTreeNode treeNode(node);
				result.append(treeNode);

				// Recursively collect actions for the parent license
				CollectLicenseActionsRecursive(revokeOut->GetParentLicenseId(), userActionManager, result, visitedLicenses);
			}
		}
		else if (actionType == "RevokeIn"){
			const prolifedata::CRevokeInAction* revokeIn = dynamic_cast<const prolifedata::CRevokeInAction*>(actionData.GetPtr());
			if (revokeIn){
				sdl::prolife::Licenses::CRevokeInNode node;
				node.Version_1_0.Emplace();
				node.Version_1_0->id = licenseId;
				node.Version_1_0->nodeType = sdl::prolife::Licenses::NodeType::RevokeIn;
				node.Version_1_0->childId = revokeIn->GetChildLicenseId();
				node.Version_1_0->remainingCount = revokeIn->GetRemainingCount();

				sdl::prolife::Licenses::LicenseTreeNode treeNode(node);
				result.append(treeNode);

				// Recursively collect actions for the child license
				CollectLicenseActionsRecursive(revokeIn->GetChildLicenseId(), userActionManager, result, visitedLicenses);
			}
		}
	}
}

QVector<sdl::prolife::Licenses::LicenseTreeNode> BuildLicenseTreeFromActions(
	const QByteArray& licenseId,
	const imtauth::IUserActionManager& userActionManager,
	QString& errorMessage)
{
	QVector<sdl::prolife::Licenses::LicenseTreeNode> result;
	QSet<QByteArray> visitedLicenses;

	// Recursively collect all related license actions
	CollectLicenseActionsRecursive(licenseId, userActionManager, result, visitedLicenses);

	return result;
}


} // namespace prolifedata


