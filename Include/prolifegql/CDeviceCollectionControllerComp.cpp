#include <prolifegql/CDeviceCollectionControllerComp.h>


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iprm/CTextParam.h>

// ProLife includes
#include <prolifedata/IOrderInfo.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/TOrderedWrap.h>


namespace prolifegql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

imtbase::CTreeItemModel* CDeviceCollectionControllerComp::ListObjects(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	imtbase::CTreeItemModel* resultModelPtr = BaseClass::ListObjects(gqlRequest, errorMessage);
	if (resultModelPtr == nullptr){
		return nullptr;
	}

	iprm::CTextParam textParam;
	textParam.SetText("none");

	iprm::CParamsSet objectFilter;
	objectFilter.SetEditableParameter("Status", &textParam);

	iprm::CParamsSet filterParams;
	filterParams.SetEditableParameter("ObjectFilter", &objectFilter);

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParams);

	if (resultModelPtr->ContainsKey("data")){
		imtbase::CTreeItemModel* dataModelPtr = resultModelPtr->GetTreeItemModel("data");
		if (dataModelPtr != nullptr){
			if (dataModelPtr->ContainsKey("notification")){
				imtbase::CTreeItemModel* notificationModelPtr = dataModelPtr->GetTreeItemModel("notification");
				if (notificationModelPtr != nullptr){
					notificationModelPtr->SetData("NewCount", collectionIds.count());
				}
			}
		}
	}

	return resultModelPtr;
}


bool CDeviceCollectionControllerComp::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		imtbase::CTreeItemModel& model,
		int itemIndex,
		const imtbase::IObjectCollectionIterator* objectCollectionIterator,
		QString& errorMessage) const
{
	bool retVal = true;

	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		prolifedata::TOrderedWrap<prolifedata::CDeviceInfo>* deviceInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (objectCollectionIterator->GetObjectData(dataPtr)){
			deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CDeviceInfo>*>(dataPtr.GetPtr());
		}

		if (deviceInfoPtr != nullptr){
			QByteArray collectionId = objectCollectionIterator->GetObjectId();

			for (QByteArray informationId : informationIds){
				QVariant elementInformation;

				if(informationId == "TypeId"){
					elementInformation = m_objectCollectionCompPtr->GetObjectTypeId(collectionId);
				}
				else if(informationId == "Id"){
					elementInformation = collectionId;
				}
				else if(informationId == "Name"){
					QByteArray deviceType = deviceInfoPtr->GetDeviceType();
					QByteArray macAddress = deviceInfoPtr->GetMacAddress();

					elementInformation = deviceType;
					if (!macAddress.isEmpty()){
						elementInformation = deviceType + " (" + macAddress + ")";
					}
				}
				else if(informationId == "MacAddress"){
					elementInformation = deviceInfoPtr->GetMacAddress();
				}
				else if(informationId == "SerialNumber"){
					elementInformation = deviceInfoPtr->GetSerialNumber();
				}
				else if(informationId == "Description"){
					elementInformation = deviceInfoPtr->GetDescription();
				}
				else if(informationId == "DeviceType"){
					elementInformation = deviceInfoPtr->GetDeviceType();
				}
				else if(informationId == "OrderId"){
					if (m_orderCollectionCompPtr.IsValid()){
						QByteArray orderUuid = deviceInfoPtr->GetOrderId();
						imtbase::IObjectCollection::DataPtr dataPtr;
						if (m_orderCollectionCompPtr->GetObjectData(orderUuid, dataPtr)){
							prolifedata::IOrderInfo* orderPtr = dynamic_cast<prolifedata::IOrderInfo*>(dataPtr.GetPtr());
							if (orderPtr != nullptr){
								QByteArray orderId = orderPtr->GetOrderId();

								elementInformation = orderId;
							}
						}
					}
				}
				else if(informationId == "OrderUuid"){
					if (m_orderCollectionCompPtr.IsValid()){
						QByteArray orderId = deviceInfoPtr->GetOrderId();
						imtbase::IObjectCollection::Ids orderIds = m_orderCollectionCompPtr->GetElementIds();
						if (orderIds.contains(orderId)){
							elementInformation = orderId;
						}
					}
				}
				else if(informationId == "Status"){
					int status = deviceInfoPtr->GetDeviceProductionStatus();
					switch (status){
					case prolifedata::IDeviceInfo::DPS_NONE:
						elementInformation = "None";
						break;
					case prolifedata::IDeviceInfo::DPS_ACCEPTED:
						elementInformation = "Accepted";
						break;
					case prolifedata::IDeviceInfo::DPS_IN_PROGRESS:
						elementInformation = "In Progress";
						break;
					case prolifedata::IDeviceInfo::DPS_CANCELED:
						elementInformation = "Canceled";
						break;
					case prolifedata::IDeviceInfo::DPS_ON_HOLD:
						elementInformation = "Hold";
						break;
					case prolifedata::IDeviceInfo::DPS_FINISHED:
						elementInformation = "Finished";
						break;
					}
				}
				else if(informationId == "Added"){
					QDateTime addedTime =  objectCollectionIterator->GetElementInfo("added").toDateTime();
					elementInformation = addedTime.toString("dd.MM.yyyy hh:mm:ss");
				}
				else if(informationId == "LastModified"){
					QDateTime lastTime =  objectCollectionIterator->GetElementInfo("lastmodified").toDateTime();
					elementInformation = lastTime.toString("dd.MM.yyyy hh:mm:ss");
				}

				if (elementInformation.isNull()){
					elementInformation = "";
				}

				retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
			}

			return true;
		}
	}

	return false;
}


imtbase::CTreeItemModel* CDeviceCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	return nullptr;
}


} // namespace prolifegql


