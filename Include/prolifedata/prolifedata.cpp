#include <prolifedata/prolifedata.h>


// Qt includes
#include <QSet>

// ACF includes
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/IProductInfo.h>

// ProLife includes
#include <prolifedata/CDeviceInfo.h>


namespace prolifedata
{


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


std::optional<sdl::prolife::Licenses::CLicenseTreeNode::V1_0> BuildLicenseTree(
			const QByteArray& licenseId,
			const imtbase::IObjectCollection& softwareProductCollection,
			QString& errorMessage)
{
	// First, find the root of the tree by traversing up the parent chain
	QByteArray rootLicenseId = licenseId;
	QSet<QByteArray> visitedForRoot;
	const int MAX_DEPTH = 100;
	int depth = 0;

	while (depth < MAX_DEPTH){
		if (visitedForRoot.contains(rootLicenseId)){
			errorMessage = QString("Unable to build license tree. Error: Circular reference detected in parent chain");
			return std::nullopt;
		}
		visitedForRoot.insert(rootLicenseId);

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!softwareProductCollection.GetObjectData(rootLicenseId, dataPtr)){
			errorMessage = QString("Unable to build license tree. Error: License not found");
			return std::nullopt;
		}

		const imtlic::IProductInstanceInfo* softwarePtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
		if (softwarePtr == nullptr){
			errorMessage = QString("Unable to build license tree. Error: Invalid license data");
			return std::nullopt;
		}

		QByteArray parentId = softwarePtr->GetParentInstanceId();
		if (parentId.isEmpty()){
			break; // Found root
		}

		rootLicenseId = parentId;
		depth++;
	}

	if (depth >= MAX_DEPTH){
		errorMessage = QString("Unable to build license tree. Error: Maximum depth exceeded while finding root");
		return std::nullopt;
	}

	// Now build the tree recursively from the root
	std::function<std::optional<sdl::prolife::Licenses::CLicenseTreeNode::V1_0>(const QByteArray&, int)> buildNode;
	buildNode = [&](const QByteArray& nodeId, int currentDepth) -> std::optional<sdl::prolife::Licenses::CLicenseTreeNode::V1_0> {
		const int MAX_TREE_DEPTH = 100;
		if (currentDepth >= MAX_TREE_DEPTH){
			return std::nullopt;
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!softwareProductCollection.GetObjectData(nodeId, dataPtr)){
			return std::nullopt;
		}

		const imtlic::IProductInstanceInfo* softwarePtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
		if (softwarePtr == nullptr){
			return std::nullopt;
		}

		sdl::prolife::Licenses::CLicenseTreeNode::V1_0 node;
		node.id = nodeId;
		node.serialNumber = softwarePtr->GetSerialNumber();
		node.productCount = softwarePtr->GetProductCount();
		node.parentId = softwarePtr->GetParentInstanceId();
		node.accountId = softwarePtr->GetAccountId();
		node.accountName = softwarePtr->GetAccountName();

		// Find all children
		imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
		fieldFilter.fieldId = "ParentInstanceId";
		fieldFilter.filterValue = nodeId;

		imtbase::CComplexCollectionFilter complexFilter;
		complexFilter.AddFieldFilter(fieldFilter);

		iprm::CParamsSet filterParam;
		filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

		QByteArrayList childIds = softwareProductCollection.GetElementIds(0, -1, &filterParam);

		QList<sdl::prolife::Licenses::CLicenseTreeNode::V1_0> children;
		for (const QByteArray& childId : std::as_const(childIds)){
			auto childNode = buildNode(childId, currentDepth + 1);
			if (childNode.has_value()){
				children.append(childNode.value());
			}
		}

		node.children.Emplace().FromList(children);

		return node;
	};

	return buildNode(rootLicenseId, 0);
}


} // namespace prolifedata


