#include <prolifegql/CDeviceControllerComp.h>


// ProLife includes
#include <prolifedata/TOrderedWrap.h>
#include <prolifedata/CDeviceInfo.h>


namespace prolifegql
{


imtbase::CTreeItemModel* CDeviceControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);

	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	dataModelPtr->SetData("Id", "");
	dataModelPtr->SetData("Name", "");

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(dataPtr.GetPtr());
		if (deviceInfoPtr != nullptr){
			QByteArray objectUuid = deviceInfoPtr->GetObjectUuid();
			QByteArray macAddress = deviceInfoPtr->GetMacAddress();
			QByteArray serialNumber = deviceInfoPtr->GetSerialNumber();
			QByteArray deviceType = deviceInfoPtr->GetDeviceType();
			QByteArray orderId = deviceInfoPtr->GetOrderId();
			QString description = deviceInfoPtr->GetDescription();
			QByteArray deviceId = deviceInfoPtr->GetObjectUuid();
			QByteArray configurationType = deviceInfoPtr->GetConfigurationType();

			prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();

			dataModelPtr->SetData("Uuid", objectUuid);
			dataModelPtr->SetData("Id", deviceId);
			dataModelPtr->SetData("MacAddress", macAddress);
			dataModelPtr->SetData("SerialNumber", serialNumber);
			dataModelPtr->SetData("Description", description);
			dataModelPtr->SetData("DeviceType", deviceType);
			dataModelPtr->SetData("OrderId", orderId);
			dataModelPtr->SetData("ConfigurationType", configurationType);

			switch (status){
			case prolifedata::IDeviceInfo::DPS_NONE:
				dataModelPtr->SetData("ProductionStatus", "None");
				break;
			case prolifedata::IDeviceInfo::DPS_ACCEPTED:
				dataModelPtr->SetData("ProductionStatus", "Accepted");
				break;
			case prolifedata::IDeviceInfo::DPS_IN_PROGRESS:
				dataModelPtr->SetData("ProductionStatus", "InProgress");
				break;
			case prolifedata::IDeviceInfo::DPS_CANCELED:
				dataModelPtr->SetData("ProductionStatus", "Canceled");
				break;
			case prolifedata::IDeviceInfo::DPS_ON_HOLD:
				dataModelPtr->SetData("ProductionStatus", "OnHold");
				break;
			case prolifedata::IDeviceInfo::DPS_FINISHED:
				dataModelPtr->SetData("ProductionStatus", "Finished");
				break;
			}

			QString name = deviceType;
			if (!macAddress.isEmpty()){
				name = deviceType + " (" + macAddress + ")";
			}
			dataModelPtr->SetData("Name", name);
		}
	}

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CDeviceControllerComp::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray& objectId,
			QString& name,
			QString& /*description*/,
			QString &errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_deviceCompPtr.IsValid()){
		return nullptr;
	}

	if (inputParams.isEmpty()){
		return nullptr;
	}

	objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>> devicePtr = new prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>();
		if (!devicePtr.IsValid()){
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		if (!itemModel.CreateFromJson(itemData)){
			return nullptr;
		}

		if (itemModel.ContainsKey("OrderId")){
			QByteArray orderId = itemModel.GetData("OrderId").toByteArray();

			devicePtr->SetOrderId(orderId);
		}

		QByteArray macAddress;
		if (itemModel.ContainsKey("MacAddress")){
			macAddress = itemModel.GetData("MacAddress").toByteArray().toLower();

			devicePtr->SetMacAddress(macAddress);
		}

		QByteArray serialNumber;
		if (itemModel.ContainsKey("SerialNumber")){
			serialNumber = itemModel.GetData("SerialNumber").toByteArray();

			devicePtr->SetSerialNumber(serialNumber);
		}

		devicePtr->SetObjectUuid(objectId);

		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();

		for (imtbase::ICollectionInfo::Id collectionId : collectionIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
				prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(dataPtr.GetPtr());
				if (deviceInfoPtr != nullptr){
					QByteArray currentObjectUuid = deviceInfoPtr->GetObjectUuid();
					if (currentObjectUuid != objectId){
						QByteArray currentMacAddress = deviceInfoPtr->GetMacAddress().toLower();
						if (!macAddress.isEmpty() && currentMacAddress == macAddress){
							errorMessage = QT_TR_NOOP("MAC-Address already exists");
							return nullptr;
						}

						QByteArray currentSerialNumber = deviceInfoPtr->GetSerialNumber().toLower();
						if (!serialNumber.isEmpty() && currentSerialNumber == serialNumber.toLower()){
							errorMessage = QT_TR_NOOP("Serial Number already exists");
							return nullptr;
						}
					}
				}
			}
		}

		if (itemModel.ContainsKey("Description")){
			QString description = itemModel.GetData("Description").toString();

			devicePtr->SetDescription(description);
		}

		if (itemModel.ContainsKey("ProductionStatus")){
			QString status = itemModel.GetData("ProductionStatus").toString();
			if (status == "None"){
				devicePtr->SetDeviceProductionStatus(prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_NONE);
			}
			else if (status == "Accepted"){
				devicePtr->SetDeviceProductionStatus(prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_ACCEPTED);
			}
			else if (status == "InProgress"){
				devicePtr->SetDeviceProductionStatus(prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_IN_PROGRESS);
			}
			else if (status == "Canceled"){
				devicePtr->SetDeviceProductionStatus(prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_CANCELED);
			}
			else if (status == "OnHold"){
				devicePtr->SetDeviceProductionStatus(prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_ON_HOLD);
			}
			else if (status == "Finished"){
				devicePtr->SetDeviceProductionStatus(prolifedata::IDeviceInfo::DeviceProductionStatus::DPS_FINISHED);
			}
		}

		QByteArray deviceType;
		if (itemModel.ContainsKey("DeviceType")){
			deviceType = itemModel.GetData("DeviceType").toByteArray();

			devicePtr->SetDeviceType(deviceType);
		}

		QByteArray configurationType;
		if (itemModel.ContainsKey("ConfigurationType")){
			configurationType = itemModel.GetData("ConfigurationType").toByteArray();

			devicePtr->SetConfigurationType(configurationType);
		}

		if (deviceType.isEmpty()){
			errorMessage = QObject::tr("Device type cannot be empty");
			return nullptr;
		}

		name = deviceType;
		if (!macAddress.isEmpty()){
			name += " (" + macAddress + ")";
		}

		return devicePtr.PopPtr();
	}

	errorMessage = QObject::tr("Can not create order: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace prolifegql


