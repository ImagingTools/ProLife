#include <prolifegql/CDeviceControllerComp.h>


// ProLife includes
#include <prolifedata/TOrderedWrap.h>


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
		prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(dataPtr.GetPtr());
		if (deviceInfoPtr != nullptr){
			QByteArray macAddress = deviceInfoPtr->GetMacAddress();
			QByteArray serialNumber = deviceInfoPtr->GetSerialNumber();
			QByteArray deviceType = deviceInfoPtr->GetDeviceType();
			QString description = deviceInfoPtr->GetDescription();
			prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();

			dataModelPtr->SetData("Name", serialNumber);
			dataModelPtr->SetData("MacAddress", macAddress);
			dataModelPtr->SetData("SerialNumber", serialNumber);
			dataModelPtr->SetData("Description", description);
			dataModelPtr->SetData("ProductionStatus", status);
			dataModelPtr->SetData("DeviceType", deviceType);
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
		istd::TDelPtr<prolifedata::IDeviceInfo> devicePtr = m_deviceCompPtr.CreateInstance();
		if (!devicePtr.IsValid()) {
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		if (!itemModel.CreateFromJson(itemData)){
			return nullptr;
		}

//		if (itemModel.ContainsKey("OrderId")){
//			QByteArray orderId = itemModel.GetData("OrderId").toByteArray();

//			devicePtr->SetOrderId(orderId);
//		}

		if (itemModel.ContainsKey("MacAddress")){
			QByteArray macAddress = itemModel.GetData("MacAddress").toByteArray();

			devicePtr->SetMacAddress(macAddress);
		}

		if (itemModel.ContainsKey("SerialNumber")){
			QByteArray serialNumber = itemModel.GetData("SerialNumber").toByteArray();

			devicePtr->SetSerialNumber(serialNumber);

			if (serialNumber.isEmpty()){
				errorMessage = QT_TR_NOOP("Serial Number cannot be empty!");
				return nullptr;
			}

			name = serialNumber;
			objectId = serialNumber;
		}

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

		if (itemModel.ContainsKey("DeviceType")){
			QByteArray deviceType = itemModel.GetData("DeviceType").toByteArray();

			devicePtr->SetDeviceType(deviceType);
		}

		return devicePtr.PopPtr();
	}

	errorMessage = QObject::tr("Can not create order: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace prolifegql


