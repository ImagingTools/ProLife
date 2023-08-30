#include <prolifegql/COrderControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>
#include <iprm/CEnableableParam.h>

// ImtCore includes
#include <imtlic/CHardwareInstanceInfo.h>

// ProLife includes
#include <prolifedata/TOrderedWrap.h>
#include <prolifedata/COrderInfo.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>


namespace prolifegql
{


imtbase::CTreeItemModel* COrderControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid() || !m_softwareInstanceCollectionCompPtr.IsValid() || !m_deviceCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		prolifedata::IOrderInfo* orderPtr = dynamic_cast<prolifedata::IOrderInfo*>(dataPtr.GetPtr());
		if (orderPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an product instance");
			return nullptr;
		}

		QByteArray objectUuid = objectId;
		QByteArray orderId = orderPtr->GetOrderId();
		QByteArray purchaseOrderId = orderPtr->GetPurchaseOrderId();
		QByteArray customerId = orderPtr->GetCustomerId();
		QString description = orderPtr->GetDescription();
		prolifedata::IOrderInfo::OrderStatus status = orderPtr->GetOrderStatus();

		dataModelPtr->SetData("Id", objectUuid);
		dataModelPtr->SetData("Name", orderId);
		dataModelPtr->SetData("OrderId", orderId);
		dataModelPtr->SetData("PurchaseId", purchaseOrderId);
		dataModelPtr->SetData("CustomerId", customerId);
		dataModelPtr->SetData("Description", description);

		switch (status){
		case prolifedata::IOrderInfo::OrderStatus::OS_NONE:
			dataModelPtr->SetData("OrderStatus", "None");
			break;
		case prolifedata::IOrderInfo::OrderStatus::OS_CREATED:
			dataModelPtr->SetData("OrderStatus", "Created");
			break;
		case prolifedata::IOrderInfo::OrderStatus::OS_IN_PROGRESS:
			dataModelPtr->SetData("OrderStatus", "InProgress");
			break;
		case prolifedata::IOrderInfo::OrderStatus::OS_CANCELED:
			dataModelPtr->SetData("OrderStatus", "Canceled");
			break;
		case prolifedata::IOrderInfo::OrderStatus::OS_ON_HOLD:
			dataModelPtr->SetData("OrderStatus", "OnHold");
			break;
		case prolifedata::IOrderInfo::OrderStatus::OS_FINISHED:
			dataModelPtr->SetData("OrderStatus", "Finished");
			break;
		case prolifedata::IOrderInfo::OrderStatus::OS_CLOSED:
			dataModelPtr->SetData("OrderStatus", "Closed");
			break;
		}

		imtbase::CTreeItemModel* productsModel = dataModelPtr->AddTreeModel("OrderProducts");

		imtbase::IObjectCollection* productCollectionPtr = orderPtr->GetProducts();
		if (productCollectionPtr == nullptr){
			return nullptr;
		}

		imtbase::ICollectionInfo::Ids orderedProductIds = productCollectionPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& productId : orderedProductIds){
			imtbase::ICollectionInfo::Id typeId = productCollectionPtr->GetObjectTypeId(productId);

			imtbase::IObjectCollection::DataPtr productDataPtr;
			if (typeId == QByteArray("SoftwareInfo")){
				if (m_softwareInstanceCollectionCompPtr->GetObjectData(productId, productDataPtr)){
					const imtbase::IIdentifiable* productIdentifiablePtr = dynamic_cast<const imtbase::IIdentifiable*>(productDataPtr.GetPtr());
					if (productIdentifiablePtr != nullptr){
						InsertSoftwareProductToModel(*productIdentifiablePtr, *productsModel);
					}
				}
			}
			else if (typeId == QByteArray("HardwareInfo")){
				if (m_deviceCollectionCompPtr->GetObjectData(productId, productDataPtr)){
					const imtbase::IIdentifiable* productIdentifiablePtr = dynamic_cast<const imtbase::IIdentifiable*>(productDataPtr.GetPtr());
					if (productIdentifiablePtr != nullptr){
						InsertHardwareProductToModel(*productIdentifiablePtr, *productsModel);
					}
				}
			}
		}

//		iprm::CParamsSet filterParam;
//		iprm::CParamsSet paramsSet;

//		iprm::CTextParam orderIdParam;
//		orderIdParam.SetText(objectId);

//		paramsSet.SetEditableParameter("OrderId", &orderIdParam);
//		filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

//		imtbase::ICollectionInfo::Ids orderHardwareIds = m_deviceCollectionCompPtr->GetElementIds(0, -1, &filterParam);
//		imtbase::ICollectionInfo::Ids orderSoftwareIds = m_softwareInstanceCollectionCompPtr->GetElementIds(0, -1, &filterParam);

//		for (const imtbase::ICollectionInfo::Id& orderHardwareId : orderHardwareIds){
//			imtbase::IObjectCollection::DataPtr productDataPtr;
//			if (m_deviceCollectionCompPtr->GetObjectData(orderHardwareId, productDataPtr)){
//				const imtbase::IIdentifiable* productIdentifiablePtr = dynamic_cast<const imtbase::IIdentifiable*>(productDataPtr.GetPtr());
//				if (productIdentifiablePtr != nullptr){
//					InsertHardwareProductToModel(*productIdentifiablePtr, *productsModel);
//				}
//			}
//		}

//		for (const imtbase::ICollectionInfo::Id& orderSoftwareId : orderSoftwareIds){
//			imtbase::IObjectCollection::DataPtr productDataPtr;
//			if (m_softwareInstanceCollectionCompPtr->GetObjectData(orderSoftwareId, productDataPtr)){
//				const imtbase::IIdentifiable* productIdentifiablePtr = dynamic_cast<const imtbase::IIdentifiable*>(productDataPtr.GetPtr());
//				if (productIdentifiablePtr != nullptr){
//					InsertSoftwareProductToModel(*productIdentifiablePtr, *productsModel);
//				}
//			}
//		}
	}

	return rootModelPtr.PopPtr();
}


istd::IChangeable* COrderControllerComp::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray& objectId,
			QString& name,
			QString& description,
			QString &errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_deviceCollectionCompPtr.IsValid()){
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
		//		istd::TDelPtr<prolifedata::IOrderInfo> orderPtr = m_orderPtr.CreateInstance();
		istd::TDelPtr<prolifedata::CIdentifiableOrderInfo> orderPtr = new prolifedata::CIdentifiableOrderInfo();

		if (!orderPtr.IsValid()){
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		orderPtr->SetObjectUuid(objectId);

		QByteArray orderId;
		if (itemModel.ContainsKey("OrderId")){
			orderId = itemModel.GetData("OrderId").toByteArray().trimmed();
		}

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
		}

		if (orderId.isEmpty()){
			errorMessage = QT_TR_NOOP("ERP Order-ID can not be empty");
			return nullptr;
		}

		QByteArray purchaseOrderId;
		if (itemModel.ContainsKey("PurchaseId")){
			purchaseOrderId = itemModel.GetData("PurchaseId").toByteArray().trimmed();
		}

		iprm::CTextParam valueParam;
		valueParam.SetText(orderId);

		iprm::CEnableableParam isEqualParam;
		isEqualParam.SetEnabled(true);

		iprm::CParamsSet valueParamsSet;
		valueParamsSet.SetEditableParameter("Value", &valueParam);
		valueParamsSet.SetEditableParameter("IsEqual", &isEqualParam);

		iprm::CParamsSet paramsSet1;
		paramsSet1.SetEditableParameter("OrderId", &valueParamsSet);

		iprm::CParamsSet filterParam;
		filterParam.SetEditableParameter("ObjectFilter", &paramsSet1);

		// Check Order-ID exists
		imtbase::ICollectionInfo::Ids collectionIds1 = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
		if (!collectionIds1.isEmpty()){
			QByteArray orderObjectId = collectionIds1[0];
			if (objectId != orderObjectId){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_objectCollectionCompPtr->GetObjectData(orderObjectId, dataPtr)){
					prolifedata::CIdentifiableOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(dataPtr.GetPtr());
					if (orderInfoPtr != nullptr){
						QByteArray currentOrderId = orderInfoPtr->GetOrderId().toLower();
						if (currentOrderId == orderId.toLower()){
							errorMessage = QT_TR_NOOP("Order ID already exists");
							return nullptr;
						}
					}
				}
			}
		}

		iprm::CTextParam valueParam2;
		valueParam2.SetText(purchaseOrderId);

		iprm::CEnableableParam isEqualParam2;
		isEqualParam2.SetEnabled(true);

		iprm::CParamsSet valueParamsSet2;
		valueParamsSet2.SetEditableParameter("Value", &valueParam2);
		valueParamsSet2.SetEditableParameter("IsEqual", &isEqualParam2);

		iprm::CParamsSet paramsSet2;
		paramsSet2.SetEditableParameter("PurchaseId", &valueParamsSet2);

		iprm::CParamsSet filterParam2;
		filterParam2.SetEditableParameter("ObjectFilter", &paramsSet2);

		// Check Purchase-ID exists
		imtbase::ICollectionInfo::Ids collectionIds2 = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam2);
		if (!collectionIds2.isEmpty()){
			QByteArray orderObjectId = collectionIds2[0];
			if (objectId != orderObjectId){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_objectCollectionCompPtr->GetObjectData(orderObjectId, dataPtr)){
					prolifedata::CIdentifiableOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(dataPtr.GetPtr());
					if (orderInfoPtr != nullptr){
						QByteArray currentPurchaseId = orderInfoPtr->GetPurchaseOrderId().toLower();
						if (purchaseOrderId != "" && currentPurchaseId == purchaseOrderId.toLower()){
							errorMessage = QT_TR_NOOP("Purchase order ID already exists");
							return nullptr;
						}
					}
				}
			}
		}

//		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
//		for (imtbase::ICollectionInfo::Id collectionId : collectionIds){
//			imtbase::IObjectCollection::DataPtr dataPtr;
//			if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
//				prolifedata::CIdentifiableOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(dataPtr.GetPtr());
//				if (orderInfoPtr != nullptr){
//					QByteArray currentObjectUuid = orderInfoPtr->GetObjectUuid();
//					if (currentObjectUuid != objectId){
//						QByteArray currentOrderId = orderInfoPtr->GetOrderId().toLower();
//						if (currentOrderId == orderId.toLower()){
//							errorMessage = QT_TR_NOOP("Order ID already exists");
//							return nullptr;
//						}

//						QByteArray currentPurchaseId = orderInfoPtr->GetPurchaseOrderId().toLower();
//						if (purchaseOrderId != "" && currentPurchaseId == purchaseOrderId.toLower()){
//							errorMessage = QT_TR_NOOP("Purchase order ID already exists");
//							return nullptr;
//						}
//					}
//				}
//			}
//		}

		QByteArray customerId;
		if (itemModel.ContainsKey("CustomerId")){
			customerId = itemModel.GetData("CustomerId").toByteArray();
		}

		if (customerId.isEmpty()){
			errorMessage = QT_TR_NOOP("Customer can not be empty!");
			return nullptr;
		}

		QByteArray description;
		if (itemModel.ContainsKey("Description")){
			description = itemModel.GetData("Description").toByteArray();
		}

		if (itemModel.ContainsKey("OrderStatus")){
			QString status = itemModel.GetData("OrderStatus").toString();
			if (status == "None"){
				orderPtr->SetOrderStatus(prolifedata::IOrderInfo::OrderStatus::OS_NONE);
			}
			else if (status == "Created"){
				orderPtr->SetOrderStatus(prolifedata::IOrderInfo::OrderStatus::OS_CREATED);
			}
			else if (status == "InProgress"){
				orderPtr->SetOrderStatus(prolifedata::IOrderInfo::OrderStatus::OS_IN_PROGRESS);
			}
			else if (status == "Canceled"){
				orderPtr->SetOrderStatus(prolifedata::IOrderInfo::OrderStatus::OS_CANCELED);
			}
			else if (status == "OnHold"){
				orderPtr->SetOrderStatus(prolifedata::IOrderInfo::OrderStatus::OS_ON_HOLD);
			}
			else if (status == "Finished"){
				orderPtr->SetOrderStatus(prolifedata::IOrderInfo::OrderStatus::OS_FINISHED);
			}
			else if (status == "Closed"){
				orderPtr->SetOrderStatus(prolifedata::IOrderInfo::OrderStatus::OS_CLOSED);
			}
		}

		orderPtr->SetOrderId(orderId);
		orderPtr->SetPurchaseOrderId(purchaseOrderId);
		orderPtr->SetCustomerId(customerId);
		orderPtr->SetDescription(description);

		imtbase::IObjectCollection* productCollectionPtr = orderPtr->GetProducts();
		if (productCollectionPtr == nullptr){
			return nullptr;
		}

		imtbase::CTreeItemModel* orderedProducts = itemModel.GetTreeItemModel("OrderProducts");
		if(orderedProducts != nullptr){
			for(int productIndex = 0; productIndex < orderedProducts->GetItemsCount(); productIndex++){
				QByteArray productCategory;
				if(orderedProducts->ContainsKey("CategoryId", productIndex)){
					productCategory = orderedProducts->GetData("CategoryId", productIndex).toByteArray();
				}

				if (productCategory == "Software"){
					InsertSoftwareProductToProductCollection(*orderedProducts, productIndex, *productCollectionPtr, objectId);
				}
				else if (productCategory == "Hardware"){
					InsertHardwareProductToProductCollection(*orderedProducts, productIndex, *productCollectionPtr, objectId);
				}
			}
		}

		return orderPtr.PopPtr();
	}
	errorMessage = QObject::tr("Can not create order: %1").arg(QString(objectId));

	return nullptr;
}


void COrderControllerComp::InsertSoftwareProductToProductCollection(
			const imtbase::CTreeItemModel& softwareProductModel,
			int modelIndex,
			imtbase::IObjectCollection& productCollection,
			const QByteArray& orderUuid) const
{
	istd::TDelPtr<prolifedata::COrderedIdentifiableSoftwareInstanceInfo> softwareInstancePtr;
	softwareInstancePtr.SetPtr(new prolifedata::COrderedIdentifiableSoftwareInstanceInfo);

	softwareInstancePtr->SetOrderId(orderUuid);

	QByteArray uuidId;
	if (softwareProductModel.ContainsKey("Id", modelIndex)){
		uuidId = softwareProductModel.GetData("Id", modelIndex).toByteArray();
		if (uuidId.isEmpty()){
			uuidId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
		}

		softwareInstancePtr->SetObjectUuid(uuidId);
	}

	if (softwareProductModel.ContainsKey("ProductId", modelIndex)){
		QByteArray productId = softwareProductModel.GetData("ProductId", modelIndex).toByteArray();

		softwareInstancePtr->SetupProductInstance(productId, "", "");
	}

	if (softwareProductModel.ContainsKey("SerialNumber", modelIndex)){
		QByteArray serialNumber = softwareProductModel.GetData("SerialNumber", modelIndex).toByteArray();

		softwareInstancePtr->SetSerialNumber(serialNumber);
	}

	if (softwareProductModel.ContainsKey("ActiveLicenses", modelIndex)){
		imtbase::CTreeItemModel* activeLicenses = softwareProductModel.GetTreeItemModel("ActiveLicenses", modelIndex);
		if (activeLicenses != nullptr){
			for (int i = 0; i < activeLicenses->GetItemsCount(); i++){
				QByteArray licenseId;
				if (activeLicenses->ContainsKey("Id", i)){
					licenseId = activeLicenses->GetData("Id", i).toByteArray();
				}

				QDateTime expirationDate;
				if (activeLicenses->ContainsKey("Expiration", i)){
					QString dateExpirationStr = activeLicenses->GetData("Expiration", i).toString();
					expirationDate = QDateTime::fromString(dateExpirationStr, "yyyy-MM-dd");
				}

				softwareInstancePtr->AddLicense(licenseId, expirationDate);
			}
		}
	}

//	productCollection.InsertNewObject(QByteArray("Software"), "", "", softwareInstancePtr.PopPtr(), uuidId);

	if (m_softwareInstanceCollectionCompPtr.IsValid()){
		m_softwareInstanceCollectionCompPtr->InsertNewObject(QByteArray("Software"), "", "", softwareInstancePtr.PopPtr(), uuidId);
	}
}


void COrderControllerComp::InsertHardwareProductToProductCollection(
			const imtbase::CTreeItemModel& hardwareProductModel,
			int modelIndex, imtbase::IObjectCollection& productCollection,
			const QByteArray& orderUuid) const
{
	istd::TDelPtr<prolifedata::COrderedIdentifiableDeviceInfo> deviceInstancePtr;
	deviceInstancePtr.SetPtr(new prolifedata::COrderedIdentifiableDeviceInfo);

	deviceInstancePtr->SetOrderId(orderUuid);

	QByteArray productId;
	if (hardwareProductModel.ContainsKey("ProductId", modelIndex)){
		productId = hardwareProductModel.GetData("ProductId", modelIndex).toByteArray();

		deviceInstancePtr->SetDeviceType(productId);
	}

	if (hardwareProductModel.ContainsKey("ModelTypeId", modelIndex)){
		QByteArray modelTypeId = hardwareProductModel.GetData("ModelTypeId", modelIndex).toByteArray();

		deviceInstancePtr->SetConfigurationType(modelTypeId);
	}

	QByteArray deviceId = hardwareProductModel.GetData("DeviceId", modelIndex).toByteArray();
	deviceInstancePtr->SetObjectUuid(deviceId);

	if (hardwareProductModel.ContainsKey("IsNewDevice", modelIndex)){
		m_deviceCollectionCompPtr->InsertNewObject("DocumentInfo", "", "", deviceInstancePtr.GetPtr(), deviceId);
	}
	else{
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_deviceCollectionCompPtr->GetObjectData(deviceId, dataPtr)){
			prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(dataPtr.GetPtr());
			if (deviceInfoPtr != nullptr){
				deviceInfoPtr->SetOrderId(orderUuid);

				m_deviceCollectionCompPtr->SetObjectData(deviceId, *deviceInfoPtr);
			}
		}
	}

//	productCollection.InsertNewObject(QByteArray("Hardware"), "", "", hardwareInstancePtr.PopPtr(), uuidId);
}


void COrderControllerComp::InsertSoftwareProductToModel(
			const imtbase::IIdentifiable& identifiable,
			imtbase::CTreeItemModel& softwareProductModel) const
{
	const imtlic::IProductInstanceInfo* softwareProductPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(&identifiable);
	if (softwareProductPtr != nullptr){
		int modelIndex = softwareProductModel.InsertNewItem();

		softwareProductModel.SetData("Id", identifiable.GetObjectUuid(), modelIndex);
		softwareProductModel.SetData("ProductId", softwareProductPtr->GetProductId(), modelIndex);
		softwareProductModel.SetData("CategoryId", softwareProductPtr->GetFactoryId(), modelIndex);
		softwareProductModel.SetData("SerialNumber", softwareProductPtr->GetSerialNumber(), modelIndex);

		imtbase::CTreeItemModel* activeLicenses = softwareProductModel.AddTreeModel("ActiveLicenses", modelIndex);
		const imtbase::ICollectionInfo& licenseInstances = softwareProductPtr->GetLicenseInstances();
		imtbase::ICollectionInfo::Ids activeLicenseIds = licenseInstances.GetElementIds();
		for (const QByteArray& activeLicenseId : activeLicenseIds){
			const imtlic::ILicenseInstance* licenseInstancePtr = softwareProductPtr->GetLicenseInstance(activeLicenseId);
			if (licenseInstancePtr != nullptr){
				int productIndex = activeLicenses->InsertNewItem();

				QString licenseName = licenseInstancePtr->GetLicenseName();

				activeLicenses->SetData("Id", activeLicenseId, productIndex);
				activeLicenses->SetData("Name", licenseName, productIndex);

				QDate date = licenseInstancePtr->GetExpiration().date();
				QString licenseExpiration = date.toString("yyyy-MM-dd");
				activeLicenses->SetData("Expiration", licenseExpiration, productIndex);
			}
		}
	}
}


void COrderControllerComp::InsertHardwareProductToModel(
			const imtbase::IIdentifiable& identifiable,
			imtbase::CTreeItemModel& hardwareProductModel) const
{
	const prolifedata::CIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::CIdentifiableDeviceInfo*>(&identifiable);
	if (deviceInfoPtr != nullptr){
		int modelIndex = hardwareProductModel.InsertNewItem();

		hardwareProductModel.SetData("Id", deviceInfoPtr->GetObjectUuid(), modelIndex);
		hardwareProductModel.SetData("ProductId", deviceInfoPtr->GetDeviceType(), modelIndex);
		hardwareProductModel.SetData("CategoryId", QByteArray("Hardware"), modelIndex);
		hardwareProductModel.SetData("ModelTypeId", deviceInfoPtr->GetConfigurationType(), modelIndex);
		hardwareProductModel.SetData("DeviceId", deviceInfoPtr->GetObjectUuid(), modelIndex);
//		hardwareProductModel.SetData("MacAddress", deviceInfoPtr->GetMacAddress(), modelIndex);
//		hardwareProductModel.SetData("SerialNumber", deviceInfoPtr->GetSerialNumber(), modelIndex);
	}

//	const imtlic::CIdentifiableHardwareInstanceInfo* hardwareProductPtr = dynamic_cast<const imtlic::CIdentifiableHardwareInstanceInfo*>(&identifiable);
//	if (hardwareProductPtr != nullptr){
//		int modelIndex = hardwareProductModel.InsertNewItem();

//		hardwareProductModel.SetData("Id", identifiable.GetObjectUuid(), modelIndex);
//		hardwareProductModel.SetData("DeviceId", hardwareProductPtr->GetObjectUuid(), modelIndex);
//		hardwareProductModel.SetData("ProductId", hardwareProductPtr->GetProductId(), modelIndex);
//		hardwareProductModel.SetData("CategoryId", hardwareProductPtr->GetFactoryId(), modelIndex);
//		hardwareProductModel.SetData("ModelTypeId", hardwareProductPtr->GetModelTypeId(), modelIndex);
//	}
}


} // namespace prolifegql


