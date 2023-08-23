#include <prolifegql/CSoftwareProductCollectionControllerComp.h>


// ImtCore includes
#include <imtlic/CHardwareInstanceInfo.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/IDeviceInfo.h>


namespace prolifegql
{


bool CSoftwareProductCollectionControllerComp::SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& /*errorMessage*/) const
{
	if (objectCollectionIterator == nullptr){
		return false;
	}

	bool retVal = true;

	QByteArray collectionId = objectCollectionIterator->GetObjectId();
	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		prolifedata::CIdentifiableOrderInfo* productOrderInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr orderDataPtr;
		if (objectCollectionIterator->GetObjectData(orderDataPtr)){
			productOrderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(orderDataPtr.GetPtr());
		}

		if (productOrderInfoPtr != nullptr){
			QByteArray orderUuid = productOrderInfoPtr->GetObjectUuid();
			QByteArray serialNumber;
			QByteArray productId;

			imtbase::IObjectCollection* productCollectionPtr = productOrderInfoPtr->GetProducts();
			if (productCollectionPtr != nullptr){
				imtbase::IObjectCollection::DataPtr productDataPtr;
				if (productCollectionPtr->GetObjectData(collectionId, productDataPtr)){
					imtlic::IProductInstanceInfo* softwareProductPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
					if (softwareProductPtr != nullptr){
						serialNumber = softwareProductPtr->GetSerialNumber();
						productId = softwareProductPtr->GetProductId();
					}
				}
			}

//			QByteArray hardwareMacAddress;
//			QByteArray hardwareUuid;
//			if (m_orderCollectionCompPtr.IsValid() && m_deviceCollectionCompPtr.IsValid()){
//				imtbase::IObjectCollection::DataPtr dataPtr;
//				if (m_orderCollectionCompPtr->GetObjectData(orderUuid, dataPtr)){
//					prolifedata::IOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(dataPtr.GetPtr());
//					if (orderInfoPtr != nullptr){
//						imtbase::IObjectCollection* orderProductCollectionPtr = orderInfoPtr->GetProducts();
//						if (orderProductCollectionPtr != nullptr){
//							imtbase::IObjectCollection::Ids productIds = orderProductCollectionPtr->GetElementIds();

//							for (const imtbase::IObjectCollection::Id& productId : productIds){
//								imtbase::IObjectCollection::DataPtr hardwareProductDataPtr;
//								if (orderProductCollectionPtr->GetObjectData(productId, hardwareProductDataPtr)){
//									const imtlic::CIdentifiableHardwareInstanceInfo* hardwareProductPtr = dynamic_cast<const imtlic::CIdentifiableHardwareInstanceInfo*>(hardwareProductDataPtr.GetPtr());
//									if (hardwareProductPtr != nullptr){
//										QByteArray softwareId = hardwareProductPtr->GetSoftwareId();
//										if (softwareId == collectionId){
//											QByteArray deviceId = hardwareProductPtr->GetObjectUuid();
//											hardwareUuid = productId;

//											imtbase::IObjectCollection::DataPtr deviceDataPtr;
//											if (m_deviceCollectionCompPtr->GetObjectData(deviceId, deviceDataPtr)){
//												const prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
//												if (deviceInfoPtr != nullptr){
//													hardwareMacAddress = deviceInfoPtr->GetMacAddress();
//												}
//											}
//										}
//									}
//								}
//							}
//						}
//					}
//				}
//			}

			QByteArray hardwareMacAddress = objectCollectionIterator->GetElementInfo("DeviceId").toByteArray();
			QByteArray hardwareUuid = objectCollectionIterator->GetElementInfo("DeviceUuid").toByteArray();;

			for (const QByteArray& informationId : informationIds){
				QVariant elementInformation;
				if (informationId == "Id"){
					elementInformation = collectionId;
				}
				else if (informationId == "OrderId"){
					elementInformation = productOrderInfoPtr->GetOrderId();
				}
				else if (informationId == "Customer"){
					elementInformation = productOrderInfoPtr->GetCustomerId();
				}
				else if (informationId == "OrderUuid"){
					elementInformation = orderUuid;
				}
				else if (informationId == "HardwareUuid"){
					elementInformation = hardwareUuid;
				}
				else if (informationId == "ProductId"){
					elementInformation = productId;
				}
				else if (informationId == "SerialNumber"){
					elementInformation = serialNumber;
				}
				else if (informationId == "IsPaired"){
					elementInformation = !hardwareMacAddress.isEmpty();
				}
				else if (informationId == "DeviceId"){
					if (!hardwareMacAddress.isEmpty()){
						elementInformation = hardwareMacAddress;
					}
					else{
						elementInformation = "";
					}
				}

				if (elementInformation.isNull()){
					elementInformation = "";
				}

				retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
			}
		}
	}

	return retVal;
}


void CSoftwareProductCollectionControllerComp::SetObjectFilter(
			const imtgql::CGqlRequest& gqlRequest,
			const imtbase::CTreeItemModel& objectFilterModel,
			iprm::CParamsSet& filterParams) const
{
	BaseClass::SetObjectFilter(gqlRequest, objectFilterModel, filterParams);

	if (!m_accountCollectionCompPtr.IsValid() || !m_orderCollectionCompPtr.IsValid()){
		return;
	}

	imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return;
	}

	bool filterByGroup = true;

	QByteArrayList userGroupIds;
	imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr != nullptr){
		userGroupIds = userInfoPtr->GetGroups();

		if (userInfoPtr->IsAdmin()){
			filterByGroup = false;
		}
	}

	if (filterByGroup){
		iprm::CParamsSet accountFilter;

		iprm::CParamsSet groupsObjectFilter;
		iprm::COptionsManager groupsOptionsManager;

		for (const QByteArray& groupId : userGroupIds){
			groupsOptionsManager.InsertOption("", groupId);
		}

		groupsObjectFilter.SetEditableParameter("Groups", &groupsOptionsManager);
		accountFilter.SetEditableParameter("ObjectFilter", &groupsObjectFilter);

		iprm::COptionsManager accountsOptionsManager;
		imtbase::ICollectionInfo::Ids accountIds = m_accountCollectionCompPtr->GetElementIds(0, -1, &accountFilter);
		for (const QByteArray& accountId : accountIds){
			accountsOptionsManager.InsertOption("", accountId);
		}

		istd::TDelPtr<iprm::COptionsManager> ordersOptionsManagerPtr;
		ordersOptionsManagerPtr.SetPtr(new iprm::COptionsManager());

		iprm::CParamsSet orderFilter;

		iprm::CParamsSet accountParams;
		accountParams.SetEditableParameter("OrderCustomers", &accountsOptionsManager);
		orderFilter.SetEditableParameter("ObjectFilter", &accountParams);

		imtbase::ICollectionInfo::Ids ordersIds = m_orderCollectionCompPtr->GetElementIds(0, -1, &orderFilter);
		for (const QByteArray& orderId : ordersIds){
			ordersOptionsManagerPtr->InsertOption("", orderId);
		}

		filterParams.SetEditableParameter("Orders", ordersOptionsManagerPtr.PopPtr());
	}
}


} // namespace prolifegql


