#include <prolifegql/CHardwareProductCollectionControllerComp.h>


// ImtCore includes
#include <imtlic/CHardwareInstanceInfo.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/IDeviceInfo.h>


namespace prolifegql
{


// protected methods

// reimplemented (imtgql::CObjectCollectionControllerCompBase)

imtbase::CTreeItemModel* CHardwareProductCollectionControllerComp::ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid() || !m_deviceCollectionCompPtr.IsValid()){
		return nullptr;
	}

	QByteArray orderId;
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr != nullptr){
		orderId = inputParamPtr->GetFieldArgumentValue("OrderId").toByteArray();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* itemsModelPtr = dataModelPtr->AddTreeModel("items");

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(orderId, dataPtr)){
		prolifedata::IOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(dataPtr.GetPtr());
		if (orderInfoPtr != nullptr){
			imtbase::IObjectCollection* productCollectionPtr = orderInfoPtr->GetProducts();
			if (productCollectionPtr != nullptr){
				imtbase::IObjectCollection::Ids productIds = productCollectionPtr->GetElementIds();

				for (const imtbase::IObjectCollection::Id& productId : productIds){
					imtbase::IObjectCollection::DataPtr hardwareProductDataPtr;
					if (productCollectionPtr->GetObjectData(productId, hardwareProductDataPtr)){
						const imtlic::CIdentifiableHardwareInstanceInfo* hardwareProductPtr = dynamic_cast<const imtlic::CIdentifiableHardwareInstanceInfo*>(hardwareProductDataPtr.GetPtr());
						if (hardwareProductPtr != nullptr){
							QByteArray softwareId = hardwareProductPtr->GetSoftwareId();
							if (softwareId.isEmpty()){
								int index = itemsModelPtr->InsertNewItem();

								QByteArray deviceId = hardwareProductPtr->GetObjectUuid();
								QByteArray hardwareProductId = hardwareProductPtr->GetProductId();

								itemsModelPtr->SetData("Id", productId, index);
								itemsModelPtr->SetData("ProductId", hardwareProductId, index);

								imtbase::IObjectCollection::DataPtr deviceDataPtr;
								if (m_deviceCollectionCompPtr->GetObjectData(deviceId, deviceDataPtr)){
									const prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
									if (deviceInfoPtr != nullptr){
										QByteArray macAddress = deviceInfoPtr->GetMacAddress();
										QByteArray serialNumber = deviceInfoPtr->GetSerialNumber();

										itemsModelPtr->SetData("MacAddress", macAddress, index);
										itemsModelPtr->SetData("SerialNumber", serialNumber, index);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace prolifegql


