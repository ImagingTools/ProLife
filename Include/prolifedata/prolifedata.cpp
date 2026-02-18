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


QVector<sdl::prolife::Licenses::CLicenseTreeNode> BuildLicenseTreeFromActions(
			const QByteArray& licenseId,
			const imtbase::IObjectCollection& userActionCollection,
			QString& errorMessage)
{
	QVector<sdl::prolife::Licenses::CLicenseTreeNode> result;
	
	// Query all user actions for this license (as target)
	prolifedata::CGroupFilter filter;
	filter.AddFilter("TargetId", "eq", licenseId);
	
	imtbase::CComplexFilter complexFilter;
	complexFilter.SetFilterExpression(filter);
	
	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);
	
	imtbase::IObjectCollection::Ids actionIds = userActionCollection.GetElementIds(0, -1, &filterParam);
	
	// Process each action and create corresponding node
	for (const QByteArray& actionId : std::as_const(actionIds)){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!userActionCollection.GetObjectData(actionId, dataPtr)){
			continue; // Skip if can't get data
		}
		
		const imtauth::IUserRecentAction* actionPtr = dynamic_cast<const imtauth::IUserRecentAction*>(dataPtr.GetPtr());
		if (actionPtr == nullptr){
			continue; // Skip if wrong type
		}
		
		QString actionType = actionPtr->GetActionType();
		iser::ISerializableSharedPtr actionData = actionPtr->GetActionData();
		
		if (actionType == "SplitOut"){
			const prolifedata::CSplitOutAction* splitOut = dynamic_cast<const prolifedata::CSplitOutAction*>(actionData.GetPtr());
			if (splitOut){
				sdl::prolife::Licenses::CSplitOutNode::V1_0 node;
				node.id = actionId;
				node.nodeType = sdl::prolife::Licenses::NodeType::Split;
				node.newLicenseId = splitOut->GetNewLicenseId();
				node.initialCount = splitOut->GetInitialCount();
				node.movedCount = splitOut->GetMovedCount();
				
				sdl::prolife::Licenses::CLicenseTreeNode treeNode;
				treeNode.SplitOutNode.Emplace(std::move(node));
				result.append(std::move(treeNode));
			}
		}
		else if (actionType == "SplitIn"){
			const prolifedata::CSplitInAction* splitIn = dynamic_cast<const prolifedata::CSplitInAction*>(actionData.GetPtr());
			if (splitIn){
				sdl::prolife::Licenses::CSplitInNode::V1_0 node;
				node.id = actionId;
				node.nodeType = sdl::prolife::Licenses::NodeType::Split;
				node.sourceLicenseId = splitIn->GetSourceLicenseId();
				node.receivedCount = splitIn->GetReceivedCount();
				
				sdl::prolife::Licenses::CLicenseTreeNode treeNode;
				treeNode.SplitInNode.Emplace(std::move(node));
				result.append(std::move(treeNode));
			}
		}
		else if (actionType == "RevokeOut"){
			const prolifedata::CRevokeOutAction* revokeOut = dynamic_cast<const prolifedata::CRevokeOutAction*>(actionData.GetPtr());
			if (revokeOut){
				sdl::prolife::Licenses::CRevokeOutNode::V1_0 node;
				node.id = actionId;
				node.nodeType = sdl::prolife::Licenses::NodeType::Revoke;
				node.parentLicenseId = revokeOut->GetParentLicenseId();
				node.initialCount = revokeOut->GetInitialCount();
				node.revokedCount = revokeOut->GetRevokedCount();
				
				sdl::prolife::Licenses::CLicenseTreeNode treeNode;
				treeNode.RevokeOutNode.Emplace(std::move(node));
				result.append(std::move(treeNode));
			}
		}
		else if (actionType == "RevokeIn"){
			const prolifedata::CRevokeInAction* revokeIn = dynamic_cast<const prolifedata::CRevokeInAction*>(actionData.GetPtr());
			if (revokeIn){
				sdl::prolife::Licenses::CRevokeInNode::V1_0 node;
				node.id = actionId;
				node.nodeType = sdl::prolife::Licenses::NodeType::Revoke;
				node.childId = revokeIn->GetChildLicenseId();
				node.remainingCount = revokeIn->GetRemainingCount();
				
				sdl::prolife::Licenses::CLicenseTreeNode treeNode;
				treeNode.RevokeInNode.Emplace(std::move(node));
				result.append(std::move(treeNode));
			}
		}
	}
	
	return result;
}


} // namespace prolifedata


