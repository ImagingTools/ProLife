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

	QByteArray objectId;
	const QList<imtgql::CGqlObject>* inputParams = gqlRequest.GetParams();
	if (inputParams != nullptr){
		objectId = GetObjectIdFromInputParams(*inputParams);
	}

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
			prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();

			dataModelPtr->SetData("Uuid", objectUuid);
			dataModelPtr->SetData("Id", deviceId);
			dataModelPtr->SetData("Name", deviceId);
			dataModelPtr->SetData("MacAddress", macAddress);
			dataModelPtr->SetData("SerialNumber", serialNumber);
			dataModelPtr->SetData("Description", description);
			dataModelPtr->SetData("ProductionStatus", status);
			dataModelPtr->SetData("DeviceType", deviceType);
			dataModelPtr->SetData("OrderId", orderId);
		}
	}

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CDeviceControllerComp::CreateObject(
		const QList<imtgql::CGqlObject>& inputParams,
		QByteArray& objectId,
		QString& name,
		QString& description,
		QString &errorMessage) const
{
	if (!m_deviceCompPtr.IsValid()){
		return nullptr;
	}

	if (inputParams.isEmpty()){
		return nullptr;
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
			macAddress = itemModel.GetData("MacAddress").toByteArray();

			devicePtr->SetMacAddress(macAddress);
		}

		QByteArray serialNumber;
		if (itemModel.ContainsKey("SerialNumber")){
			serialNumber = itemModel.GetData("SerialNumber").toByteArray();

			devicePtr->SetSerialNumber(serialNumber);
		}

		if (itemModel.ContainsKey("Id")){
			QByteArray id = itemModel.GetData("Id").toByteArray();
			if (!id.isEmpty()){
				objectId = id;
			}
		}

		if (objectId.isEmpty()){
			objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
		}

		devicePtr->SetObjectUuid(objectId);

		if (itemModel.ContainsKey("Description")){
			QString description = itemModel.GetData("Description").toString();

			devicePtr->SetDescription(description);
		}

		if (itemModel.ContainsKey("ProductionStatus")){
			int status = itemModel.GetData("ProductionStatus").toInt();
			if (status >= 0){
				devicePtr->SetDeviceProductionStatus((prolifedata::IDeviceInfo::DeviceProductionStatus) status);
			}
		}

		QByteArray deviceType;
		if (itemModel.ContainsKey("DeviceType")){
			deviceType = itemModel.GetData("DeviceType").toByteArray();

			devicePtr->SetDeviceType(deviceType);
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


