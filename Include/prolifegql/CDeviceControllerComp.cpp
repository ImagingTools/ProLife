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

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(dataPtr.GetPtr());
		if (deviceInfoPtr != nullptr){
			QByteArray macAddress = deviceInfoPtr->GetMacAddress();
			QByteArray serialNumber = deviceInfoPtr->GetSerialNumber();
			QString description = deviceInfoPtr->GetDescription();

			dataModelPtr->SetData("Name", serialNumber);
			dataModelPtr->SetData("MacAddress", macAddress);
			dataModelPtr->SetData("SerialNumber", serialNumber);
			dataModelPtr->SetData("Description", description);
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

			name = serialNumber;
			objectId = serialNumber;
		}

		if (itemModel.ContainsKey("Description")){
			QString description = itemModel.GetData("Description").toString();

			devicePtr->SetDescription(description);
		}

		if (itemModel.ContainsKey("Status")){
			QByteArray status = itemModel.GetData("Status").toByteArray();

			if (status == QByteArray("InProgress")){
				devicePtr->SetDeviceStatus(prolifedata::IDeviceInfo::OS_IN_PROGRESS);
			}
			else if (status == QByteArray("Created")){
				devicePtr->SetDeviceStatus(prolifedata::IDeviceInfo::OS_CREATED);
			}
			else if (status == QByteArray("OnFinished")){
				devicePtr->SetDeviceStatus(prolifedata::IDeviceInfo::OS_FINISHED);
			}
			else{
				devicePtr->SetDeviceStatus(prolifedata::IDeviceInfo::OS_NONE);
			}
		}

		return devicePtr.PopPtr();
	}

	errorMessage = QObject::tr("Can not create order: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace prolifegql


