#include <prolifegql/COrderControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtlic/CProductInstanceCollection.h>
#include <imtlic/CHardwareInstanceInfo.h>

// ProLife includes
#include <prolifedata/IOrderedProductInfo.h>
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

	QByteArray objectId;

	const QList<imtgql::CGqlObject>* inputParams = gqlRequest.GetParams();

	if (inputParams != nullptr){
		objectId = GetObjectIdFromInputParams(*inputParams);
	}

	dataModel->SetData("Name", "");
	dataModel->SetData("Id", objectId);

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		//		prolifedata::IOrderInfo* orderPtr = dynamic_cast<prolifedata::IOrderInfo*>(dataPtr.GetPtr());
		prolifedata::CIdentifiableOrderInfo* orderPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(dataPtr.GetPtr());
		if (orderPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an product instance");
			return nullptr;
		}

		imtbase::CTreeItemModel* productsModel = dataModel->AddTreeModel("OrderProducts");

		QByteArray objectUuid = orderPtr->GetObjectUuid();
		QByteArray orderId = orderPtr->GetOrderId();
		QByteArray customerId = orderPtr->GetCustomerId();
		QString description = orderPtr->GetDescription();
		prolifedata::IOrderInfo::OrderStatus status = orderPtr->GetOrderStatus();

		QString name = m_objectCollectionCompPtr->GetElementInfo(objectId, idoc::IDocumentMetaInfo::MIT_TITLE).toString();

		dataModel->SetData("Id", objectUuid);
		dataModel->SetData("Name", name);
		dataModel->SetData("OrderId", orderId);
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
										productIndex = productsModel->InsertNewItem();

										productsModel->SetData("Id", QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8(), productIndex);
										productsModel->SetData("CategoryId", "Pair", productIndex);

										imtbase::CTreeItemModel* hardwareProductModelPtr = productsModel->AddTreeModel("HardwareProduct", productIndex);

										hardwareProductModelPtr->SetData("CategoryId", hardwareProductPtr->GetFactoryId());
										hardwareProductModelPtr->SetData("ProductId", hardwareProductPtr->GetProductId());
										hardwareProductModelPtr->SetData("PairId", softwareId);
										hardwareProductModelPtr->SetData("Id", orderedProductId);

										QByteArray deviceId = hardwareProductPtr->GetDeviceId();
										if (!deviceId.isEmpty()){
											hardwareProductModelPtr->SetData("DeviceId", deviceId);
										}

										pairFounded = true;

										break;
									}
								}
							}
						}

						QByteArray productId = softwareProductPtr->GetProductId();
						QByteArray categoryId = softwareProductPtr->GetFactoryId();
						QByteArray productInstance = softwareProductPtr->GetProductInstanceId();

						imtbase::CTreeItemModel* softwareProductModelPtr = productsModel;
						if (pairFounded){
							softwareProductModelPtr = productsModel->AddTreeModel("SoftwareProduct", productIndex);
							productIndex = 0;
						}
						else{
							productIndex = productsModel->InsertNewItem();
						}

						softwareProductModelPtr->SetData("Id", objectUuid, productIndex);
						softwareProductModelPtr->SetData("ProductId", productId, productIndex);
						softwareProductModelPtr->SetData("CategoryId", categoryId, productIndex);

						imtbase::CTreeItemModel* activeLicenses = softwareProductModelPtr->AddTreeModel("ActiveLicenses", productIndex);
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

					const imtlic::IHardwareInstanceInfo* hardwareProductPtr = dynamic_cast<const imtlic::IHardwareInstanceInfo*>(dataPtr.GetPtr());
					if (hardwareProductPtr != nullptr){
						QByteArray softwareId = hardwareProductPtr->GetSoftwareId();
						if (softwareId.isEmpty()){
							int productIndex = productsModel->InsertNewItem();

							QByteArray deviceId = hardwareProductPtr->GetDeviceId();
							QByteArray productId = hardwareProductPtr->GetProductId();
							QByteArray categoryId = hardwareProductPtr->GetFactoryId();

							productsModel->SetData("PairId", softwareId, productIndex);
							productsModel->SetData("CategoryId", categoryId, productIndex);
							productsModel->SetData("ProductId", productId, productIndex);
							productsModel->SetData("Id", objectUuid, productIndex);

							if (!deviceId.isEmpty()){
								productsModel->SetData("DeviceId", deviceId, productIndex);
							}
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

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		//		istd::TDelPtr<prolifedata::IOrderInfo> orderPtr = m_orderPtr.CreateInstance();
		istd::TDelPtr<prolifedata::CIdentifiableOrderInfo> orderPtr = new prolifedata::CIdentifiableOrderInfo();

		if (!orderPtr.IsValid()){
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

		if (itemModel.ContainsKey("Id")){
			QByteArray id = itemModel.GetData("Id").toByteArray();
			if (!id.isEmpty()){
				objectId = id;
			}
		}

		if (objectId.isEmpty()){
			objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
		}

		orderPtr->SetObjectUuid(objectId);

		QByteArray orderId;
		if (itemModel.ContainsKey("OrderId")){
			orderId = itemModel.GetData("OrderId").toByteArray().trimmed();
		}

		if (itemModel.ContainsKey("Name")){
			name = itemModel.GetData("Name").toString();
		}

		if (orderId.isEmpty()){
			errorMessage = QT_TR_NOOP("Order-ID can not be empty!");
			return nullptr;
		}

		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds();
		for (imtbase::ICollectionInfo::Id collectionId : collectionIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
				prolifedata::CIdentifiableOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(dataPtr.GetPtr());
				if (orderInfoPtr != nullptr){
					QByteArray currentObjectUuid = orderInfoPtr->GetObjectUuid();
					if (currentObjectUuid != objectId){
						QByteArray currentOrderId = orderInfoPtr->GetOrderId();
						if (currentOrderId == orderId){
							errorMessage = QT_TR_NOOP("Order-ID already exists!");
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

	QByteArray deviceId;
	if (hardwareProductModel.ContainsKey("DeviceId", modelIndex)){
		deviceId = hardwareProductModel.GetData("DeviceId", modelIndex).toByteArray();

		if (deviceId.isEmpty()){
			istd::TDelPtr<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>> devicePtr = new prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>();

			devicePtr->SetOrderId(orderId);
			devicePtr->SetDeviceType(productId);

			QByteArray deviceUuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
			devicePtr->SetObjectUuid(deviceUuid);

			m_deviceCollectionCompPtr->InsertNewObject("DeviceInfo", "", "", devicePtr.GetPtr(), deviceUuid);

			deviceId = deviceUuid;
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
	}

	hardwareInstancePtr->SetDeviceId(deviceId);

	if (hardwareProductModel.ContainsKey("PairId", modelIndex)){
		QByteArray pairId = hardwareProductModel.GetData("PairId", modelIndex).toByteArray();

		hardwareInstancePtr->SetSoftwareId(pairId);
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


} // namespace prolifegql


