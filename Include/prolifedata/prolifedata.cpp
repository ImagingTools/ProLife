#include <prolifedata/prolifedata.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CEnableableParam.h>
#include <iprm/CParamsSet.h>

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
	iprm::CTextParam valueParam;
	valueParam.SetText(macAddress);

	iprm::CEnableableParam isEqualParam;
	isEqualParam.SetEnabled(true);

	iprm::CParamsSet valueParamsSet;
	valueParamsSet.SetEditableParameter("Value", &valueParam);
	valueParamsSet.SetEditableParameter("IsEqual", &isEqualParam);

	iprm::CParamsSet paramsSet1;
	paramsSet1.SetEditableParameter("MacAddress", &valueParamsSet);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet1);

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
	iprm::CTextParam valueParam;
	valueParam.SetText(serialNumber);

	iprm::CEnableableParam isEqualParam;
	isEqualParam.SetEnabled(true);

	iprm::CParamsSet valueParamsSet;
	valueParamsSet.SetEditableParameter("Value", &valueParam);
	valueParamsSet.SetEditableParameter("IsEqual", &isEqualParam);

	iprm::CParamsSet paramsSet1;
	paramsSet1.SetEditableParameter("SerialNumber", &valueParamsSet);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet1);

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
	iprm::CTextParam valueParam;
	valueParam.SetText(serialNumber);

	iprm::CParamsSet paramsSet;
	paramsSet.SetEditableParameter("SerialNumber", &valueParam);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

	imtbase::IObjectCollection::Ids collectionIds = collection.GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty() && !serialNumber.isEmpty()){
		QByteArray objectId = collectionIds[0];
		if (objectId != deviceUuid){
			return false;
		}
	}

	return true;
}


} // namespace prolifedata


