#include <prolifegql/COrderControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtgui/CObjectCollectionViewDelegate.h>
#include <imtlic/CFeaturePackageCollectionUtility.h>
#include <imtlic/CLicenseInstance.h>
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

		QString name = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();

		dataModel->SetData("Id", objectUuid);
		dataModel->SetData("Name", name);
		dataModel->SetData("OrderId", orderId);
		dataModel->SetData("CustomerId", customerId);
		dataModel->SetData("Description", description);

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

					int productIndex = productsModel->InsertNewItem();

					productsModel->SetData("Id", objectUuid, productIndex);

					const imtlic::IProductInstanceInfo* softwareProductPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
					if (softwareProductPtr != nullptr){
						QByteArray productId = softwareProductPtr->GetProductId();
						QByteArray categoryId = softwareProductPtr->GetFactoryId();
						QByteArray productInstance = softwareProductPtr->GetProductInstanceId();

						productsModel->SetData("ProductId", productId, productIndex);
						productsModel->SetData("CategoryId", categoryId, productIndex);
						productsModel->SetData("MacAddress", productInstance, productIndex);

						imtbase::CTreeItemModel* activeLicenses = productsModel->AddTreeModel("ActiveLicenses", productIndex);
						const imtbase::ICollectionInfo& licenseInstances = softwareProductPtr->GetLicenseInstances();

						imtbase::ICollectionInfo::Ids activeLicenseIds = licenseInstances.GetElementIds();
						for (const QByteArray& activeLicenseId : activeLicenseIds){
							const imtlic::ILicenseInstance* licenseInstancePtr = softwareProductPtr->GetLicenseInstance(activeLicenseId);
							if (licenseInstancePtr != nullptr){
								int productIndex = activeLicenses->InsertNewItem();

								QString licenseName = licenseInstancePtr->GetLicenseName();

								activeLicenses->SetData("Id", activeLicenseId, productIndex);
								activeLicenses->SetData("Name", name, productIndex);

								QDate date = licenseInstancePtr->GetExpiration().date();
								QString licenseExpiration = date.toString("yyyy-MM-dd");
								activeLicenses->SetData("Expiration", licenseExpiration, productIndex);
							}
						}
					}

					const imtlic::IHardwareInstanceInfo* hardwareProductPtr = dynamic_cast<const imtlic::IHardwareInstanceInfo*>(dataPtr.GetPtr());
					if (hardwareProductPtr != nullptr){
						QByteArray categoryId = hardwareProductPtr->GetFactoryId();
						QByteArray softwareId = hardwareProductPtr->GetSoftwareId();
						QByteArray deviceId = hardwareProductPtr->GetDeviceId();
						QByteArray productId = hardwareProductPtr->GetProductId();

						productsModel->SetData("PairId", softwareId, productIndex);
						productsModel->SetData("DeviceId", deviceId, productIndex);
						productsModel->SetData("CategoryId", categoryId, productIndex);
						productsModel->SetData("ProductId", productId, productIndex);
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
			orderId = itemModel.GetData("OrderId").toByteArray();
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
				QByteArray uuidId;

				if (orderedProducts->ContainsKey("Id", productIndex)){
					uuidId = orderedProducts->GetData("Id", productIndex).toByteArray();
					if (uuidId.isEmpty()){
						uuidId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
					}
				}

				QByteArray productCategory = "Software";
				if(orderedProducts->ContainsKey("CategoryId", productIndex)){
					productCategory = orderedProducts->GetData("CategoryId", productIndex).toByteArray();
				}

				QByteArray productId;
				if(orderedProducts->ContainsKey("ProductId", productIndex)){
					productId = orderedProducts->GetData("ProductId", productIndex).toByteArray();
				}

				QByteArray productStatus;
				if(orderedProducts->ContainsKey("ProductStatus", productIndex)){
					productStatus = orderedProducts->GetData("ProductStatus", productIndex).toByteArray();
				}

				QByteArray macAddress;
				if(orderedProducts->ContainsKey("MacAddress", productIndex)){
					macAddress = orderedProducts->GetData("MacAddress", productIndex).toByteArray();
				}

				QByteArray serialNumber;
				if(orderedProducts->ContainsKey("SerialNumber", productIndex)){
					serialNumber = orderedProducts->GetData("SerialNumber", productIndex).toByteArray();
				}

				QByteArray pairId;
				if(orderedProducts->ContainsKey("PairId", productIndex)){
					pairId = orderedProducts->GetData("PairId", productIndex).toByteArray();
				}

				if (productCategory == "Software"){
					istd::TDelPtr<imtlic::CIdentifiableSoftwareInstanceInfo> softwareInstancePtr = new imtlic::CIdentifiableSoftwareInstanceInfo();

					softwareInstancePtr->SetObjectUuid(uuidId);
					softwareInstancePtr->SetupProductInstance(productId, "", "");

					imtbase::CTreeItemModel* activeLicenses = orderedProducts->GetTreeItemModel("ActiveLicenses", productIndex);
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

					productCollectionPtr->InsertNewObject(productCategory,"","", softwareInstancePtr.PopPtr(), uuidId);
				}
				else{
					QByteArray deviceId;
					if(orderedProducts->ContainsKey("DeviceId", productIndex)){
						deviceId = orderedProducts->GetData("DeviceId", productIndex).toByteArray();
					}

					if (deviceId.isEmpty()){
						istd::TDelPtr<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>> devicePtr = new prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>();

						devicePtr->SetOrderId(objectId);
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
								deviceInfoPtr->SetOrderId(objectId);
								deviceInfoPtr->SetDeviceType(productId);
								m_deviceCollectionCompPtr->SetObjectData(deviceId, *deviceInfoPtr);
							}
						}
					}

					istd::TDelPtr<imtlic::CIdentifiableHardwareInstanceInfo> hardwareInstancePtr = new imtlic::CIdentifiableHardwareInstanceInfo();

					hardwareInstancePtr->SetObjectUuid(uuidId);
					hardwareInstancePtr->SetProductId(productId);
					hardwareInstancePtr->SetSoftwareId(pairId);
					hardwareInstancePtr->SetDeviceId(deviceId);

					productCollectionPtr->InsertNewObject(productCategory, "", "", hardwareInstancePtr.PopPtr(), uuidId);
				}
			}
		}

		return orderPtr.PopPtr();
	}
	errorMessage = QObject::tr("Can not create order: %1").arg(QString(objectId));


	return nullptr;
}


} // namespace prolifegql


