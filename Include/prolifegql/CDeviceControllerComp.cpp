#include <prolifegql/CDeviceControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>
#include <iprm/CEnableableParam.h>

// ProLife includes
#include <prolifedata/TOrderedWrap.h>
#include <prolifedata/CDeviceInfo.h>


namespace prolifegql
{


imtbase::CTreeItemModel* CDeviceControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

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
			dataModelPtr->SetData("LicenseName", configurationType);

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

			if (m_productCollectionCompPtr.IsValid()){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_productCollectionCompPtr->GetObjectData(deviceType, dataPtr)){
					imtlic::IProductInfo* remoteProductInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
					if (remoteProductInfoPtr != nullptr){
						name = remoteProductInfoPtr->GetName();
					}
				}
			}

			if (!macAddress.isEmpty()){
				name = name + " (" + macAddress + ")";
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
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

		return nullptr;
	}

	if (!m_deviceCompPtr.IsValid()){
		errorMessage = QString("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

		return nullptr;
	}

	if (inputParams.isEmpty()){
		errorMessage = QString("GQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

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
			errorMessage = QString("Unable to create representation model from JSON: %1.").arg(qPrintable(itemData)).toUtf8();
			SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

			return nullptr;
		}

		if (itemModel.ContainsKey("OrderId")){
			QByteArray orderId = itemModel.GetData("OrderId").toByteArray();

			devicePtr->SetOrderId(orderId);
		}

		QByteArray macAddress;
		if (itemModel.ContainsKey("MacAddress")){
			macAddress = itemModel.GetData("MacAddress").toByteArray().toLower();
		}

		if (!macAddress.isEmpty()){
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

			imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
			if (!collectionIds.isEmpty()){
				QByteArray id = collectionIds[0];
				if (objectId != id){
					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_objectCollectionCompPtr->GetObjectData(id, dataPtr)){
						prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(dataPtr.GetPtr());
						if (deviceInfoPtr != nullptr){
							QByteArray currentMacAddress = deviceInfoPtr->GetMacAddress().toLower();
							if (currentMacAddress == macAddress.toLower()){
								errorMessage = QT_TR_NOOP("MAC-Address already exists");
								SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

								return nullptr;
							}
						}
					}
				}
			}
		}

		devicePtr->SetMacAddress(macAddress);

		QByteArray serialNumber;
		if (itemModel.ContainsKey("SerialNumber")){
			serialNumber = itemModel.GetData("SerialNumber").toByteArray();
		}

		if (!serialNumber.isEmpty()){
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

			imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
			if (!collectionIds.isEmpty()){
				QByteArray id = collectionIds[0];
				if (objectId != id){
					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_objectCollectionCompPtr->GetObjectData(id, dataPtr)){
						prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(dataPtr.GetPtr());
						if (deviceInfoPtr != nullptr){
							QByteArray currentSerialNumber = deviceInfoPtr->GetSerialNumber().toLower();
							if (currentSerialNumber == serialNumber.toLower()){
								errorMessage = QT_TR_NOOP("Serial Number already exists");
								SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

								return nullptr;
							}
						}
					}
				}
			}
		}

		devicePtr->SetSerialNumber(serialNumber);

		devicePtr->SetObjectUuid(objectId);

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
		if (itemModel.ContainsKey("LicenseName")){
			configurationType = itemModel.GetData("LicenseName").toByteArray();

			devicePtr->SetConfigurationType(configurationType);
		}

		if (deviceType.isEmpty()){
			errorMessage = QObject::tr("Device type cannot be empty");
			SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

			return nullptr;
		}

		name = deviceType;

		if (m_productCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_productCollectionCompPtr->GetObjectData(deviceType, dataPtr)){
				imtlic::IProductInfo* remoteProductInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
				if (remoteProductInfoPtr != nullptr){
					name = remoteProductInfoPtr->GetName();
				}
			}
		}

		if (!macAddress.isEmpty()){
			name += " (" + macAddress + ")";
		}

		return devicePtr.PopPtr();
	}

	errorMessage = QObject::tr("Can not create device: %1").arg(QString(objectId));
	SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

	return nullptr;
}


} // namespace prolifegql


