#include <prolifegql/COrderControllerComp.h>


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/ICollectionInfo.h>
#include <imtgui/CObjectCollectionViewDelegate.h>
#include <imtlic/CFeaturePackageCollectionUtility.h>
#include <imtlic/CLicenseInstance.h>
#include <imtlic/CProductInstanceCollection.h>
#include <imtlic/CLicensedHardwareInstanceInfo.h>

// ProLife includes
#include <prolifedata/IOrderedProductInfo.h>


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

//	dataModel->SetData("Id", "");
//	dataModel->SetData("AccountId", "");

	QByteArray objectId;

	const QList<imtgql::CGqlObject>* inputParams = gqlRequest.GetParams();

	if (inputParams != nullptr){
		objectId = GetObjectIdFromInputParams(*inputParams);
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		prolifedata::IOrderInfo* orderPtr = dynamic_cast<prolifedata::IOrderInfo*>(dataPtr.GetPtr());

		if (orderPtr == nullptr){
			errorMessage = QT_TR_NOOP("Unable to get an product instance");
			return nullptr;
		}

		imtbase::CTreeItemModel* productsModel = dataModel->AddTreeModel("OrderProducts");

		QByteArray orderId = orderPtr->GetOrderId();
		QByteArray customerId = orderPtr->GetCustomerId();
		QByteArray description = orderPtr->GetDescription();
		QByteArray orderStatus = orderPtr->GetStatus();

		QString name = m_objectCollectionCompPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString();
		dataModel->SetData("Name", name);
		dataModel->SetData("Id", objectId);
		dataModel->SetData("OrderId", orderId);
		dataModel->SetData("CustomerId", customerId);
		dataModel->SetData("OrderStatus", orderStatus);
		dataModel->SetData("Description", description);

		imtbase::IObjectCollection* productCollectionPtr = orderPtr->GetProducts();
		if (productCollectionPtr == nullptr){
			return nullptr;
		}
		imtbase::ICollectionInfo::Ids orderedProductsIds = productCollectionPtr->GetElementIds();

		for(const QByteArray& objectId : orderedProductsIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (productCollectionPtr->GetObjectData(objectId, dataPtr)){
				const imtlic::IProductInstanceInfo* productPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
				if(productPtr != nullptr){
					int productIndex = productsModel->InsertNewItem();
					QByteArray productId = productPtr->GetProductId();
					QByteArray categoryId = productPtr->GetFactoryId();
					QByteArray productInstance = productPtr->GetProductInstanceId();

					productsModel->SetData("Id", objectId, productIndex);
					productsModel->SetData("ProductId", productId, productIndex);
					productsModel->SetData("CategoryId", categoryId, productIndex);
					productsModel->SetData("MacAddress", productInstance, productIndex);

					if (productPtr->GetFactoryId() == "Software"){
						imtbase::CTreeItemModel* activeLicenses = productsModel->AddTreeModel("ActiveLicenses", productIndex);
						const imtbase::ICollectionInfo& licenseInstances = productPtr->GetLicenseInstances();

						imtbase::ICollectionInfo::Ids activeLicenseIds = licenseInstances.GetElementIds();
						for (const QByteArray& activeLicenseId : activeLicenseIds){
							const imtlic::ILicenseInstance* licenseInstancePtr = productPtr->GetLicenseInstance(activeLicenseId);
							if (licenseInstancePtr != nullptr){
								int productIndex = activeLicenses->InsertNewItem();

								QString licenseName = licenseInstancePtr->GetLicenseName();

								activeLicenses->SetData("LicenseId", activeLicenseId, productIndex);
								activeLicenses->SetData("Name", name, productIndex);

								QDate date = licenseInstancePtr->GetExpiration().date();
								QString licenseExpiration = date.toString("yyyy-MM-dd");
								activeLicenses->SetData("Expiration", licenseExpiration, productIndex);
							}
							else{
//								Q_ASSERT(false);
							}
						}
					}
					else{
						const imtlic::CLicensedHardwareInstanceInfo* hardwareInstance = dynamic_cast<const imtlic::CLicensedHardwareInstanceInfo*>(productPtr);
						if (hardwareInstance != nullptr){
							QByteArray partStatus = hardwareInstance->GetStatus();
							QByteArray pairId = hardwareInstance->GetSoftwareId();
							QByteArray serialNumber = hardwareInstance->GetSerialNumber();

							productsModel->SetData("Status", partStatus, productIndex);
							productsModel->SetData("PairId", pairId, productIndex);
							productsModel->SetData("SerialNumber", serialNumber, productIndex);
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
	if (inputParams.isEmpty()){
		return nullptr;
	}

	QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	if (!itemData.isEmpty()){
		istd::TDelPtr<prolifedata::IOrderInfo> orderPtr = m_orderPtr.CreateInstance();
		if (!orderPtr.IsValid()) {
			return nullptr;
		}

		imtbase::CTreeItemModel itemModel;
		itemModel.CreateFromJson(itemData);

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

//		objectId = orderId;

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

				if (itemModel.ContainsKey("Id", productIndex)){
					uuidId = orderedProducts->GetData("Id").toByteArray();
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
					istd::TDelPtr<imtlic::CProductInstanceInfo> softwareInstance = new imtlic::CProductInstanceInfo();

					softwareInstance->SetupProductInstance(productId, macAddress, "");

					imtbase::CTreeItemModel* activeLicenses = orderedProducts->GetTreeItemModel("ActiveLicenses");
					if (activeLicenses != nullptr){
						for (int i = 0; i < activeLicenses->GetItemsCount(); i++){
							QByteArray licenseId;
							if (activeLicenses->ContainsKey("LicenseId", i)){
								licenseId = activeLicenses->GetData("LicenseId", i).toByteArray();
							}

							QDateTime expirationDate;
							if (activeLicenses->ContainsKey("Expiration", i)){
								QString dateExpirationStr = activeLicenses->GetData("Expiration", i).toString();
								expirationDate = QDateTime::fromString(dateExpirationStr, "yyyy-MM-dd");
							}

							softwareInstance->AddLicense(licenseId, expirationDate);
						}
					}

					productCollectionPtr->InsertNewObject(productCategory,"","", softwareInstance.PopPtr(), uuidId);
				}
				else{
					istd::TDelPtr<imtlic::CLicensedHardwareInstanceInfo> hardwareInstance = new imtlic::CLicensedHardwareInstanceInfo();

					hardwareInstance->SetupProductInstance(productId, macAddress, "");
					hardwareInstance->SetSoftwareId(pairId);
					hardwareInstance->SetSerialNumber(serialNumber);
					hardwareInstance->SetStatus(productStatus);
					productCollectionPtr->InsertNewObject(productCategory,"","", hardwareInstance.PopPtr(), uuidId);
				}

			}
		}


		return orderPtr.PopPtr();
	}
	errorMessage = QObject::tr("Can not create order: %1").arg(QString(objectId));


	return nullptr;
}


} // namespace prolifegql


