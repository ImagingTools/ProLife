#include <prolifegql/COrderControllerComp.h>


// ImtCore includes
#include <imtlic/CHardwareInstanceInfo.h>

// ProLife includes
#include <prolifedata/TOrderedWrap.h>
#include <prolifedata/COrderInfo.h>
#include <prolifedata/CDeviceInfo.h>


namespace prolifegql
{


imtbase::CTreeItemModel* COrderControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModel = new imtbase::CTreeItemModel();

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);

	dataModel->SetData("Name", "");
	dataModel->SetData("Id", objectId);

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

		dataModel->SetData("Id", objectUuid);
		dataModel->SetData("Name", orderId);
		dataModel->SetData("OrderId", orderId);
		dataModel->SetData("PurchaseId", purchaseOrderId);
		dataModel->SetData("CustomerId", customerId);
		dataModel->SetData("Description", description);

		switch (status){
		case prolifedata::IOrderInfo::OrderStatus::OS_NONE:
			dataModel->SetData("OrderStatus", "None");
			break;
		case prolifedata::IOrderInfo::OrderStatus::OS_CREATED:
			dataModel->SetData("OrderStatus", "Created");
			break;
		case prolifedata::IOrderInfo::OrderStatus::OS_IN_PROGRESS:
			dataModel->SetData("OrderStatus", "InProgress");
			break;
		case prolifedata::IOrderInfo::OrderStatus::OS_CANCELED:
			dataModel->SetData("OrderStatus", "Canceled");
			break;
		case prolifedata::IOrderInfo::OrderStatus::OS_ON_HOLD:
			dataModel->SetData("OrderStatus", "OnHold");
			break;
		case prolifedata::IOrderInfo::OrderStatus::OS_FINISHED:
			dataModel->SetData("OrderStatus", "Finished");
			break;
		case prolifedata::IOrderInfo::OrderStatus::OS_CLOSED:
			dataModel->SetData("OrderStatus", "Closed");
			break;
		}

		imtbase::CTreeItemModel* productsModel = dataModel->AddTreeModel("OrderProducts");

		imtbase::IObjectCollection* productCollectionPtr = orderPtr->GetProducts();
		if (productCollectionPtr == nullptr){
			return nullptr;
		}

		imtbase::ICollectionInfo::Ids orderedProductsIds = productCollectionPtr->GetElementIds();
		for(const QByteArray& objectId : orderedProductsIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (productCollectionPtr->GetObjectData(objectId, dataPtr)){
				const imtbase::IIdentifiable* productIdentifiablePtr = dynamic_cast<const imtbase::IIdentifiable*>(dataPtr.GetPtr());
				if (productIdentifiablePtr != nullptr){
					QByteArray objectUuid = productIdentifiablePtr->GetObjectUuid();
					int productIndex = -1;

					QByteArray hardwareId;
					const imtlic::IProductInstanceInfo* softwareProductPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
					if (softwareProductPtr != nullptr){
						bool pairFounded = false;
						for(const QByteArray& orderedProductId : orderedProductsIds){
							imtbase::IObjectCollection::DataPtr hardwareDataPtr;
							if (productCollectionPtr->GetObjectData(orderedProductId, hardwareDataPtr)){
								const imtlic::IHardwareInstanceInfo* hardwareProductPtr = dynamic_cast<const imtlic::IHardwareInstanceInfo*>(hardwareDataPtr.GetPtr());
								if (hardwareProductPtr != nullptr){
									QByteArray softwareId = hardwareProductPtr->GetSoftwareId();
									if (objectUuid == softwareId){
										pairFounded = true;
										break;
									}
								}
							}
						}

						if (!pairFounded){
							productIndex = productsModel->InsertNewItem();
							InsertSoftwareProductToModel(*dynamic_cast<const imtbase::IIdentifiable*>(softwareProductPtr), *productsModel, productIndex);
						}
					}

					const imtlic::IHardwareInstanceInfo* hardwareProductPtr = dynamic_cast<const imtlic::IHardwareInstanceInfo*>(dataPtr.GetPtr());
					if (hardwareProductPtr != nullptr){
						productIndex = productsModel->InsertNewItem();

						QByteArray softwareId = hardwareProductPtr->GetSoftwareId();
						imtbase::IObjectCollection::DataPtr softwareDataPtr;
						if (productCollectionPtr->GetObjectData(softwareId, softwareDataPtr)){
							const imtlic::IProductInstanceInfo* softwareProductPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
							if (softwareProductPtr != nullptr){
								productsModel->SetData("Id", QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8(), productIndex);
								productsModel->SetData("CategoryId", "Pair", productIndex);

								imtbase::CTreeItemModel* hardwareProductModelPtr = productsModel->AddTreeModel("HardwareProduct", productIndex);
								InsertHardwareProductToModel(*productIdentifiablePtr, *hardwareProductModelPtr, 0);

								imtbase::CTreeItemModel* softwareProductModelPtr = productsModel->AddTreeModel("SoftwareProduct", productIndex);
								InsertSoftwareProductToModel(*dynamic_cast<const imtbase::IIdentifiable*>(softwareProductPtr), *softwareProductModelPtr, 0, objectUuid);
							}
						}
						else{
							InsertHardwareProductToModel(*productIdentifiablePtr, *productsModel, productIndex);
						}
					}
				}
			}
		}
	}

	rootModel->SetExternTreeModel("data", dataModel);

	return rootModel;
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

		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
		for (imtbase::ICollectionInfo::Id collectionId : collectionIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
				prolifedata::CIdentifiableOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(dataPtr.GetPtr());
				if (orderInfoPtr != nullptr){
					QByteArray currentObjectUuid = orderInfoPtr->GetObjectUuid();
					if (currentObjectUuid != objectId){
						QByteArray currentOrderId = orderInfoPtr->GetOrderId().toLower();
						if (currentOrderId == orderId.toLower()){
							errorMessage = QT_TR_NOOP("Order ID already exists");
							return nullptr;
						}

						QByteArray currentPurchaseId = orderInfoPtr->GetPurchaseOrderId().toLower();
						if (purchaseOrderId != "" && currentPurchaseId == purchaseOrderId.toLower()){
							errorMessage = QT_TR_NOOP("Purchase order ID already exists");
							return nullptr;
						}
					}
				}
			}
		}

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

				if (productCategory == "Pair"){
					imtbase::CTreeItemModel* softwareProductModelPtr = orderedProducts->GetTreeItemModel("SoftwareProduct", productIndex);
					if (softwareProductModelPtr != nullptr){
						InsertSoftwareProductToProductCollection(*softwareProductModelPtr, 0, *productCollectionPtr);
					}

					imtbase::CTreeItemModel* hardwareProductModelPtr = orderedProducts->GetTreeItemModel("HardwareProduct", productIndex);
					if (hardwareProductModelPtr != nullptr){
						InsertHardwareProductToProductCollection(*hardwareProductModelPtr, 0, *productCollectionPtr, objectId);
					}
				}
				else if (productCategory == "Software"){
					InsertSoftwareProductToProductCollection(*orderedProducts, productIndex, *productCollectionPtr);
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


void COrderControllerComp::InsertSoftwareProductToProductCollection(const imtbase::CTreeItemModel& softwareProductModel, int modelIndex, imtbase::IObjectCollection& productCollection) const
{
	istd::TDelPtr<imtlic::CIdentifiableSoftwareInstanceInfo> softwareInstancePtr = new imtlic::CIdentifiableSoftwareInstanceInfo();

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

	productCollection.InsertNewObject(QByteArray("Software"), "", "", softwareInstancePtr.PopPtr(), uuidId);
}


void COrderControllerComp::InsertHardwareProductToProductCollection(const imtbase::CTreeItemModel& hardwareProductModel, int modelIndex, imtbase::IObjectCollection& productCollection, const QByteArray& orderId) const
{
	istd::TDelPtr<imtlic::CIdentifiableHardwareInstanceInfo> hardwareInstancePtr = new imtlic::CIdentifiableHardwareInstanceInfo();

	QByteArray productId;
	if (hardwareProductModel.ContainsKey("ProductId", modelIndex)){
		productId = hardwareProductModel.GetData("ProductId", modelIndex).toByteArray();

		hardwareInstancePtr->SetProductId(productId);
	}

	QByteArray deviceId = hardwareProductModel.GetData("DeviceId", modelIndex).toByteArray();

	if (hardwareProductModel.ContainsKey("IsNewDevice", modelIndex)){

		istd::TDelPtr<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>> devicePtr = new prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>();

		devicePtr->SetOrderId(orderId);
		devicePtr->SetDeviceType(productId);
		devicePtr->SetObjectUuid(deviceId);

		m_deviceCollectionCompPtr->InsertNewObject("DocumentInfo", "", "", devicePtr.GetPtr(), deviceId);
	}
	else{
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_deviceCollectionCompPtr->GetObjectData(deviceId, dataPtr)){
			prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(dataPtr.GetPtr());
			if (deviceInfoPtr != nullptr){
				deviceInfoPtr->SetOrderId(orderId);
				deviceInfoPtr->SetDeviceType(productId);
				m_deviceCollectionCompPtr->SetObjectData(deviceId, *deviceInfoPtr);
			}
		}
	}

	hardwareInstancePtr->SetDeviceId(deviceId);

	if (hardwareProductModel.ContainsKey("PairId", modelIndex)){
		QByteArray pairId = hardwareProductModel.GetData("PairId", modelIndex).toByteArray();

		hardwareInstancePtr->SetSoftwareId(pairId);
	}

	if (hardwareProductModel.ContainsKey("ModelTypeId", modelIndex)){
		QByteArray modelTypeId = hardwareProductModel.GetData("ModelTypeId", modelIndex).toByteArray();

		hardwareInstancePtr->SetModelTypeId(modelTypeId);
	}

	QByteArray uuidId;
	if (hardwareProductModel.ContainsKey("Id", modelIndex)){
		uuidId = hardwareProductModel.GetData("Id", modelIndex).toByteArray();
		if (uuidId.isEmpty()){
			uuidId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
		}

		hardwareInstancePtr->SetObjectUuid(uuidId);
	}


	productCollection.InsertNewObject(QByteArray("Hardware"), "", "", hardwareInstancePtr.PopPtr(), uuidId);
}


void COrderControllerComp::InsertSoftwareProductToModel(const imtbase::IIdentifiable& identifiable, imtbase::CTreeItemModel& softwareProductModel, int modelIndex, const QByteArray& pairId) const
{
	const imtlic::IProductInstanceInfo* softwareProductPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(&identifiable);
	if (softwareProductPtr != nullptr){
		softwareProductModel.SetData("Id", identifiable.GetObjectUuid(), modelIndex);
		softwareProductModel.SetData("PairId", pairId, modelIndex);
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


void COrderControllerComp::InsertHardwareProductToModel(const imtbase::IIdentifiable& identifiable, imtbase::CTreeItemModel& hardwareProductModel, int modelIndex) const
{
	const imtlic::IHardwareInstanceInfo* hardwareProductPtr = dynamic_cast<const imtlic::IHardwareInstanceInfo*>(&identifiable);
	if (hardwareProductPtr != nullptr){
		hardwareProductModel.SetData("Id", identifiable.GetObjectUuid(), modelIndex);
		hardwareProductModel.SetData("PairId", hardwareProductPtr->GetSoftwareId(), modelIndex);
		hardwareProductModel.SetData("DeviceId", hardwareProductPtr->GetDeviceId(), modelIndex);
		hardwareProductModel.SetData("ProductId", hardwareProductPtr->GetProductId(), modelIndex);
		hardwareProductModel.SetData("CategoryId", hardwareProductPtr->GetFactoryId(), modelIndex);
		hardwareProductModel.SetData("ModelTypeId", hardwareProductPtr->GetModelTypeId(), modelIndex);
	}
}


} // namespace prolifegql


