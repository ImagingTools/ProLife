#include <prolifegql/COrderControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>
#include <iprm/CEnableableParam.h>

// ImtCore includes
#include <imtbase/CObjectLink.h>
#include <imtlic/CHardwareInstanceInfo.h>
#include <imtgql/imtgql.h>

// ProLife includes
#include <prolifedata/TOrderedWrap.h>
#include <prolifedata/COrderInfo.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>
#include <prolifedata/IHardwareProductBinding.h>
#include <prolifegql/CDeviceControllerComp.h>
#include <prolifegql/CSoftwareProductControllerComp.h>


namespace prolifegql
{


imtbase::CTreeItemModel* COrderControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid() || !m_softwareInstanceCollectionCompPtr.IsValid() || !m_deviceCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage, "COrderControllerComp");

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
			errorMessage = QString("Unable to get an product instance");
			SendErrorMessage(0, errorMessage, "COrderControllerComp");

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
		case prolifedata::IOrderInfo::OrderStatus::OS_ON_HOLD:
		case prolifedata::IOrderInfo::OrderStatus::OS_FINISHED:
		case prolifedata::IOrderInfo::OrderStatus::OS_CREATED:
			dataModelPtr->SetData("OrderStatus", "Created");
			break;
		case prolifedata::IOrderInfo::OrderStatus::OS_IN_PROGRESS:
			dataModelPtr->SetData("OrderStatus", "InProgress");
			break;
		case prolifedata::IOrderInfo::OrderStatus::OS_CANCELED:
			dataModelPtr->SetData("OrderStatus", "Canceled");
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

		return rootModelPtr.PopPtr();
	}

	errorMessage = QT_TR_NOOP("Error when trying to get an order. There is no such order.");
	SendErrorMessage(0, errorMessage, "COrderControllerComp");
	errorMessage = imtgql::GetTranslation(m_translationManagerCompPtr.GetPtr(), gqlRequest, errorMessage.toUtf8(), "prolifegql::COrderControllerComp");

	return nullptr;
}


istd::IChangeable* COrderControllerComp::CreateObject(
			const imtgql::CGqlRequest& gqlRequest,
			QByteArray& objectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage, "COrderControllerComp");

		return nullptr;
	}

	if (!m_deviceCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage, "COrderControllerComp");

		return nullptr;
	}

	QByteArray itemData;
	const imtgql::CGqlObject* gqlInputParamsPtr = gqlRequest.GetParam("input");
	if (gqlInputParamsPtr != nullptr){
		objectId = gqlInputParamsPtr->GetFieldArgumentValue("Id").toByteArray();
		itemData = gqlInputParamsPtr->GetFieldArgumentValue("Item").toByteArray();
	}

	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	if (!itemData.isEmpty()){
		istd::TDelPtr<prolifedata::CIdentifiableOrderInfo> orderPtr = new prolifedata::CIdentifiableOrderInfo();
		Q_ASSERT(orderPtr.IsValid());

		imtbase::CTreeItemModel itemModel;
		if (!itemModel.CreateFromJson(itemData)){
			errorMessage = QString("Unable to create model from json: '%1'").arg(itemData);
			SendErrorMessage(0, errorMessage, "COrderControllerComp");

			return nullptr;
		}

		orderPtr->SetObjectUuid(objectId);

		QByteArray orderId;
		if (itemModel.ContainsKey("OrderId")){
			orderId = itemModel.GetData("OrderId").toByteArray().trimmed();
		}

		name = orderId;

		if (orderId.isEmpty()){
			errorMessage = QT_TR_NOOP("Delivery-ID cannot be empty");
			SendErrorMessage(0, errorMessage, "COrderControllerComp");
			errorMessage = imtgql::GetTranslation(m_translationManagerCompPtr.GetPtr(), gqlRequest, errorMessage.toUtf8(), "prolifegql::COrderControllerComp");

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
							errorMessage = QT_TR_NOOP("Delivery-ID already exists");
							SendErrorMessage(0, errorMessage, "COrderControllerComp");
							errorMessage = imtgql::GetTranslation(m_translationManagerCompPtr.GetPtr(), gqlRequest, errorMessage.toUtf8(), "prolifegql::COrderControllerComp");

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
			SendErrorMessage(0, errorMessage, "COrderControllerComp");
			errorMessage = imtgql::GetTranslation(m_translationManagerCompPtr.GetPtr(), gqlRequest, errorMessage.toUtf8(), "prolifegql::COrderControllerComp");

			return nullptr;
		}

		QByteArray description;
		if (itemModel.ContainsKey("Description")){
			description = itemModel.GetData("Description").toByteArray();
		}

		if (itemModel.ContainsKey("OrderStatus")){
			QString status = itemModel.GetData("OrderStatus").toString();
			if (	status == "None" ||
					status == "Created" ||
					status == "OnHold" ||
					status == "Finished" ){
				orderPtr->SetOrderStatus(prolifedata::IOrderInfo::OrderStatus::OS_CREATED);
			}
			else if (status == "InProgress"){
				orderPtr->SetOrderStatus(prolifedata::IOrderInfo::OrderStatus::OS_IN_PROGRESS);
			}
			else if (status == "Canceled"){
				orderPtr->SetOrderStatus(prolifedata::IOrderInfo::OrderStatus::OS_CANCELED);
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
			errorMessage = QString("Unable to create an order object. Product collection is nullptr.");
			SendErrorMessage(0, errorMessage, "COrderControllerComp");

			return nullptr;
		}

		imtbase::CTreeItemModel* orderedProducts = itemModel.GetTreeItemModel("OrderProducts");
		if(orderedProducts != nullptr){
			bool result = CheckProducts(objectId, *orderedProducts, errorMessage);
			if (!result){
				return nullptr;
			}

			for(int productIndex = 0; productIndex < orderedProducts->GetItemsCount(); productIndex++){
				QByteArray productCategory;
				if(orderedProducts->ContainsKey("CategoryId", productIndex)){
					productCategory = orderedProducts->GetData("CategoryId", productIndex).toByteArray();
				}

				if (productCategory == "Software"){
					InsertSoftwareProductToProductCollection(gqlRequest, *orderedProducts, productIndex, *productCollectionPtr, objectId, errorMessage);

					if (!errorMessage.isEmpty()){
						return nullptr;
					}
				}
				else if (productCategory == "Hardware"){
					InsertHardwareProductToProductCollection(gqlRequest, *orderedProducts, productIndex, *productCollectionPtr, objectId, errorMessage);

					if (!errorMessage.isEmpty()){
						return nullptr;
					}
				}
			}
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			prolifedata::CIdentifiableOrderInfo* oldOrderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(dataPtr.GetPtr());
			if (oldOrderInfoPtr != nullptr){
				QByteArrayList addedProducts;
				QByteArrayList removedProducts;
				QByteArrayList updatedProducts;

				GenerateDifferences(*oldOrderInfoPtr, *orderPtr, addedProducts, removedProducts, updatedProducts);

				imtbase::IObjectCollection* oldProductCollectionPtr = oldOrderInfoPtr->GetProducts();
				if (oldProductCollectionPtr == nullptr){
					return nullptr;
				}

				for (const QByteArray& id : removedProducts){
					imtbase::ICollectionInfo::Id typeId = oldProductCollectionPtr->GetObjectTypeId(id);

					imtbase::IObjectCollection::DataPtr productDataPtr;
					if (typeId == QByteArray("HardwareInfo")){
						if (m_deviceCollectionCompPtr->GetObjectData(id, productDataPtr)){
							prolifedata::COrderedIdentifiableDeviceInfo* hardwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(productDataPtr.GetPtr());
							if (hardwareInfoPtr != nullptr){
								hardwareInfoPtr->SetOrderId("");

								imtbase::IOperationContext* operationContextPtr = nullptr;

								if (m_deviceOperationContextControllerCompPtr.IsValid()){
									operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, id, hardwareInfoPtr);
								}

								if (!m_deviceCollectionCompPtr->SetObjectData(id, *hardwareInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
									return nullptr;
								}
							}
						}
					}
					else if (typeId == QByteArray("SoftwareInfo")){
						if (m_softwareInstanceCollectionCompPtr->GetObjectData(id, productDataPtr)){
							prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(productDataPtr.GetPtr());
							if (softwareInfoPtr != nullptr){
								softwareInfoPtr->SetOrderId("");

								imtbase::IOperationContext* operationContextPtr = nullptr;

								if (m_softwareOperationContextControllerCompPtr.IsValid()){
									operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, id, softwareInfoPtr);
								}

								if (!m_softwareInstanceCollectionCompPtr->SetObjectData(id, *softwareInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
									return nullptr;
								}
							}
						}
					}
				}
			}
		}

		return orderPtr.PopPtr();
	}

	errorMessage = QString(QT_TR_NOOP("Can not create order: %1")).arg(QString(objectId));
	SendErrorMessage(0, errorMessage, "COrderControllerComp");

	errorMessage = imtgql::GetTranslation(m_translationManagerCompPtr.GetPtr(), gqlRequest, errorMessage.toUtf8(), "prolifegql::COrderControllerComp");

	return nullptr;
}


void COrderControllerComp::InsertSoftwareProductToProductCollection(
			const imtgql::CGqlRequest& gqlRequest,
			const imtbase::CTreeItemModel& softwareProductModel,
			int modelIndex,
			imtbase::IObjectCollection& productCollection,
			const QByteArray& orderUuid,
			QString& errorMessage) const
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

	QByteArray productId;
	if (softwareProductModel.ContainsKey("ProductUuid", modelIndex)){
		productId = softwareProductModel.GetData("ProductUuid", modelIndex).toByteArray();

		softwareInstancePtr->SetupProductInstance(productId, "", "");
	}

	QByteArray serialNumber;
	if (softwareProductModel.ContainsKey("SerialNumber", modelIndex)){
		serialNumber = softwareProductModel.GetData("SerialNumber", modelIndex).toByteArray();

		softwareInstancePtr->SetSerialNumber(serialNumber);
	}

	QByteArray licenseUuid;
	if (softwareProductModel.ContainsKey("LicenseUuid", modelIndex)){
		licenseUuid = softwareProductModel.GetData("LicenseUuid", modelIndex).toByteArray();
	}

	QString expiration;
	if (softwareProductModel.ContainsKey("Expiration", modelIndex)){
		expiration = softwareProductModel.GetData("Expiration", modelIndex).toString();
	}

	QDateTime expirationDate = QDateTime::fromString(expiration, "yyyy-MM-dd");
	softwareInstancePtr->AddLicense(licenseUuid, expirationDate);

	bool isNew = false;

	if (softwareProductModel.ContainsKey("IsNew", modelIndex)){
		isNew = softwareProductModel.GetData("IsNew", modelIndex).toBool();
	}

	if (isNew){
		imtbase::IOperationContext* operationContextPtr = nullptr;

		if (m_softwareOperationContextControllerCompPtr.IsValid()){
			operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_CREATE, gqlRequest);
		}

		QByteArray result = m_softwareInstanceCollectionCompPtr->InsertNewObject(QByteArray("Software"), "", "", softwareInstancePtr.PopPtr(), uuidId, nullptr, nullptr, operationContextPtr);
		if (result.isEmpty()){
			errorMessage = QString("Unable to insert new software product with ID: '%1'").arg(uuidId);

			return;
		}
	}
	else{
		if (m_softwareInstanceCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_softwareInstanceCollectionCompPtr->GetObjectData(uuidId, dataPtr)){
				prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
				if (productInfoPtr != nullptr){
					bool isInUse = productInfoPtr->IsInUse();
					if (isInUse){
						errorMessage = QString("it is not possible to add a product that is already in use");

						return;
					}

					QByteArray oldOrderId = productInfoPtr->GetOrderId();
					if (!oldOrderId.isEmpty() && oldOrderId != orderUuid){
						return;
					}

					if (!isInUse && !productInfoPtr->IsEqual(*softwareInstancePtr)){
						imtbase::IOperationContext* operationContextPtr = nullptr;

						if (m_softwareOperationContextControllerCompPtr.IsValid()){
							operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, uuidId, softwareInstancePtr.GetPtr());
						}

						if (!m_softwareInstanceCollectionCompPtr->SetObjectData(uuidId, *softwareInstancePtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
							errorMessage = QString("Unable to update a software product with ID: '%1'").arg(uuidId);

							return;
						}
					}
				}
			}
		}
	}

	istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
	objectLinkPtr.SetPtr(new imtbase::CObjectLink());

	objectLinkPtr->SetObjectUuid(uuidId);
	objectLinkPtr->SetFactoryId("SoftwareInfo");

	productCollection.InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), uuidId);
}


void COrderControllerComp::InsertHardwareProductToProductCollection(
			const imtgql::CGqlRequest& gqlRequest,
			const imtbase::CTreeItemModel& hardwareProductModel,
			int modelIndex, imtbase::IObjectCollection& productCollection,
			const QByteArray& orderUuid,
			QString& errorMessage) const
{
	istd::TDelPtr<prolifedata::COrderedIdentifiableDeviceInfo> deviceInstancePtr;
	deviceInstancePtr.SetPtr(new prolifedata::COrderedIdentifiableDeviceInfo);

	deviceInstancePtr->SetOrderId(orderUuid);

	QByteArray uuidId;
	if (hardwareProductModel.ContainsKey("Id", modelIndex)){
		uuidId = hardwareProductModel.GetData("Id", modelIndex).toByteArray();
		if (uuidId.isEmpty()){
			uuidId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
		}

		deviceInstancePtr->SetObjectUuid(uuidId);
	}

	QByteArray productId;
	if (hardwareProductModel.ContainsKey("ProductUuid", modelIndex)){
		productId = hardwareProductModel.GetData("ProductUuid", modelIndex).toByteArray();

		deviceInstancePtr->SetDeviceType(productId);
	}

	QByteArray modelTypeId;
	if (hardwareProductModel.ContainsKey("LicenseUuid", modelIndex)){
		modelTypeId = hardwareProductModel.GetData("LicenseUuid", modelIndex).toByteArray();

		deviceInstancePtr->SetConfigurationType(modelTypeId);
	}

	QByteArray macAddress;
	if (hardwareProductModel.ContainsKey("MacAddress", modelIndex)){
		macAddress = hardwareProductModel.GetData("MacAddress", modelIndex).toByteArray();

		deviceInstancePtr->SetMacAddress(macAddress);
	}

	bool isNew = false;

	if (hardwareProductModel.ContainsKey("IsNew", modelIndex)){
		isNew = hardwareProductModel.GetData("IsNew", modelIndex).toBool();
	}

	if (isNew){
		imtbase::IOperationContext* operationContextPtr = nullptr;

		if (m_deviceOperationContextControllerCompPtr.IsValid()){
			operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_CREATE, gqlRequest);
		}

		m_deviceCollectionCompPtr->InsertNewObject("DocumentInfo", "", "", deviceInstancePtr.GetPtr(), uuidId, nullptr, nullptr, operationContextPtr);
	}
	else{
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_deviceCollectionCompPtr->GetObjectData(uuidId, dataPtr)){
			prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(dataPtr.GetPtr());
			if (deviceInfoPtr != nullptr){
				QByteArray deviceOrderUuid = deviceInfoPtr->GetOrderId();
				QByteArray deviceType = deviceInfoPtr->GetDeviceType();
				QByteArray deviceConfigurationType = deviceInfoPtr->GetConfigurationType();

				if (deviceOrderUuid != orderUuid || deviceType != productId || deviceConfigurationType != modelTypeId){
					deviceInfoPtr->SetOrderId(orderUuid);
					deviceInfoPtr->SetDeviceType(productId);
					deviceInfoPtr->SetConfigurationType(modelTypeId);

					imtbase::IOperationContext* operationContextPtr = nullptr;

					if (m_deviceOperationContextControllerCompPtr.IsValid()){
						operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, uuidId, deviceInfoPtr);
					}

					m_deviceCollectionCompPtr->SetObjectData(uuidId, *deviceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr);
				}
			}
		}
	}

	istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
	objectLinkPtr.SetPtr(new imtbase::CObjectLink());

	objectLinkPtr->SetObjectUuid(uuidId);
	objectLinkPtr->SetFactoryId("HardwareInfo");

	productCollection.InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), uuidId);
}


void COrderControllerComp::InsertSoftwareProductToModel(
			const imtbase::IIdentifiable& identifiable,
			imtbase::CTreeItemModel& softwareProductModel) const
{
	const imtlic::IProductInstanceInfo* softwareProductPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(&identifiable);
	if (softwareProductPtr != nullptr){
		int modelIndex = softwareProductModel.InsertNewItem();

		QByteArray productUuid = softwareProductPtr->GetProductId();

		softwareProductModel.SetData("Id", identifiable.GetObjectUuid(), modelIndex);
		softwareProductModel.SetData("ProductUuid", productUuid, modelIndex);
		softwareProductModel.SetData("CategoryId", softwareProductPtr->GetFactoryId(), modelIndex);
		softwareProductModel.SetData("SerialNumber", softwareProductPtr->GetSerialNumber(), modelIndex);
		softwareProductModel.SetData("InUse", softwareProductPtr->IsInUse(), modelIndex);
		softwareProductModel.SetData("IsNew", false, modelIndex);

		if (m_productCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
				const imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
				if (productInfoPtr != nullptr){
					softwareProductModel.SetData("ProductName", productInfoPtr->GetName(), modelIndex);
				}
			}
		}

		const imtbase::ICollectionInfo& licenseInstances = softwareProductPtr->GetLicenseInstances();
		imtbase::ICollectionInfo::Ids activeLicenseIds = licenseInstances.GetElementIds();

		if (!activeLicenseIds.isEmpty()){
			QByteArray activeLicenseId = activeLicenseIds[0];
			const imtlic::ILicenseInstance* licenseInstancePtr = softwareProductPtr->GetLicenseInstance(activeLicenseId);
			if (licenseInstancePtr != nullptr){
				softwareProductModel.SetData("LicenseUuid", activeLicenseId, modelIndex);

				if (m_licenseDefinitionCollectionCompPtr.IsValid()){
					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_licenseDefinitionCollectionCompPtr->GetObjectData(activeLicenseId, dataPtr)){
						const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(dataPtr.GetPtr());
						if (licenseInfoPtr != nullptr){
							softwareProductModel.SetData("LicenseName", licenseInfoPtr->GetLicenseName(), modelIndex);
							softwareProductModel.SetData("LicenseId", licenseInfoPtr->GetLicenseId(), modelIndex);
						}
					}
				}

				QDate date = licenseInstancePtr->GetExpiration().date();
				QString licenseExpiration = date.toString("yyyy-MM-dd");
				softwareProductModel.SetData("Expiration", licenseExpiration, modelIndex);
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

		QByteArray objectUuid = deviceInfoPtr->GetObjectUuid();
		QByteArray productUuid = deviceInfoPtr->GetDeviceType();
		QByteArray licenseDefinitionUuid = deviceInfoPtr->GetConfigurationType();

		hardwareProductModel.SetData("Id", objectUuid, modelIndex);
		hardwareProductModel.SetData("ProductUuid", productUuid, modelIndex);
		hardwareProductModel.SetData("CategoryId", QByteArray("Hardware"), modelIndex);
		hardwareProductModel.SetData("LicenseUuid", licenseDefinitionUuid, modelIndex);
		hardwareProductModel.SetData("IsNew", false, modelIndex);

		if (m_productCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
				const imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
				if (productInfoPtr != nullptr){
					hardwareProductModel.SetData("ProductName", productInfoPtr->GetName(), modelIndex);
				}
			}
		}

		if (m_licenseDefinitionCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_licenseDefinitionCollectionCompPtr->GetObjectData(licenseDefinitionUuid, dataPtr)){
				const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(dataPtr.GetPtr());
				if (licenseInfoPtr != nullptr){
					hardwareProductModel.SetData("LicenseName", licenseInfoPtr->GetLicenseName(), modelIndex);
					hardwareProductModel.SetData("LicenseId", licenseInfoPtr->GetLicenseId(), modelIndex);
				}
			}
		}

		if (m_deviceCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_deviceCollectionCompPtr->GetObjectData(objectUuid, dataPtr)){
				const prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(dataPtr.GetPtr());
				if (deviceInfoPtr != nullptr){
					QByteArray macAddress = deviceInfoPtr->GetMacAddress();
					QByteArray serialNumber = deviceInfoPtr->GetSerialNumber();

					hardwareProductModel.SetData("MacAddress", macAddress, modelIndex);
					hardwareProductModel.SetData("SerialNumber", serialNumber, modelIndex);
				}
			}
		}

		if (m_bindingCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_bindingCollectionCompPtr->GetObjectData(objectUuid, dataPtr)){
				const prolifedata::IHardwareProductBinding* bindingInfoPtr = dynamic_cast<const prolifedata::IHardwareProductBinding*>(dataPtr.GetPtr());
				if (bindingInfoPtr != nullptr){
					QByteArrayList softwareIds = bindingInfoPtr->GetSoftwareIds();

					hardwareProductModel.SetData("InUse", false, modelIndex);

					for (const QByteArray& softwareId : softwareIds){
						imtbase::IObjectCollection::DataPtr softwareDataPtr;
						if (m_softwareInstanceCollectionCompPtr->GetObjectData(softwareId, softwareDataPtr)){
							const imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
							if (productInstanceInfoPtr != nullptr){
								bool isUse = productInstanceInfoPtr->IsInUse();
								if (isUse){
									hardwareProductModel.SetData("InUse", true, modelIndex);

									break;
								}
							}
						}
					}
				}
			}
		}
	}
}


void COrderControllerComp::GenerateDifferences(
			prolifedata::IOrderInfo& currentOrder,
			prolifedata::IOrderInfo& newOrder,
			QByteArrayList& addedProducts,
			QByteArrayList& removedProducts,
			QByteArrayList& updatedProducts) const
{
	imtbase::IObjectCollection* currentProductCollectionPtr = currentOrder.GetProducts();
	imtbase::IObjectCollection* newProductCollectionPtr = newOrder.GetProducts();

	imtbase::ICollectionInfo::Ids currentCollectionIds = currentProductCollectionPtr->GetElementIds();
	imtbase::ICollectionInfo::Ids newCollectionIds = newProductCollectionPtr->GetElementIds();

	for (const QByteArray& id : newCollectionIds){
		if (!currentCollectionIds.contains(id)){
			addedProducts.push_back(id);
		}
	}

	for (const QByteArray& id : std::as_const(currentCollectionIds)){
		if (!newCollectionIds.contains(id)){
			removedProducts.push_back(id);
		}
	}
}


bool COrderControllerComp::CheckProducts(
			const QByteArray& orderUuid,
			imtbase::CTreeItemModel& productsModel,
			QString& errorMessage) const
{
	if (!m_deviceCollectionCompPtr.IsValid()){
		return false;
	}

	if (!m_softwareInstanceCollectionCompPtr.IsValid()){
		return false;
	}

	for(int productIndex = 0; productIndex < productsModel.GetItemsCount(); productIndex++){
		QByteArray objectUuid;
		if(productsModel.ContainsKey("Id", productIndex)){
			objectUuid = productsModel.GetData("Id", productIndex).toByteArray();
		}

		QByteArray productUuid;
		if(productsModel.ContainsKey("ProductUuid", productIndex)){
			productUuid = productsModel.GetData("ProductUuid", productIndex).toByteArray();
		}

		QByteArray productCategory;
		if(productsModel.ContainsKey("CategoryId", productIndex)){
			productCategory = productsModel.GetData("CategoryId", productIndex).toByteArray();
		}

		QString productName = GetProductName(productUuid);

		if (productCategory == "Software"){
			QByteArray serialNumber;
			if(productsModel.ContainsKey("SerialNumber", productIndex)){
				serialNumber = productsModel.GetData("SerialNumber", productIndex).toByteArray();
			}

			iprm::CTextParam valueParam;
			valueParam.SetText(serialNumber);

			iprm::CParamsSet paramsSet;
			paramsSet.SetEditableParameter("SerialNumber", &valueParam);

			iprm::CParamsSet filterParam;
			filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

			imtbase::IObjectCollection::Ids collectionIds = m_softwareInstanceCollectionCompPtr->GetElementIds(0, -1, &filterParam);
			if (!collectionIds.isEmpty() && !serialNumber.isEmpty()){
				QByteArray objectId = collectionIds[0];
				if (objectId != objectUuid){
					errorMessage = QString(QT_TR_NOOP("it is not possible to save the product '%1' because serial number '%2' already exists"))
								.arg(productName)
								.arg(qPrintable(serialNumber));

					return false;
				}
			}

			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_softwareInstanceCollectionCompPtr->GetObjectData(objectUuid, dataPtr)){
				prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
				if (softwareInfoPtr != nullptr){
					QByteArray currentOrderId = softwareInfoPtr->GetOrderId();
					if (!currentOrderId.isEmpty() && orderUuid != currentOrderId){
						QByteArray serialNumber = softwareInfoPtr->GetSerialNumber();

						errorMessage = QString("It is not possible to add a product that is linked to another order. Software product '%1' with ID '%2'").arg(productName).arg(serialNumber);

						return false;
					}
				}
			}
		}
		else if (productCategory == "Hardware"){
			QByteArray macAddress;
			if(productsModel.ContainsKey("MacAddress", productIndex)){
				macAddress = productsModel.GetData("MacAddress", productIndex).toByteArray();
			}

			if (!macAddress.isEmpty()){
				iprm::CTextParam valueParam;
				valueParam.SetText(macAddress);

				iprm::CEnableableParam isEqualParam;
				isEqualParam.SetEnabled(true);

				iprm::CParamsSet valueParamsSet;
				valueParamsSet.SetEditableParameter("Value", &valueParam);
				valueParamsSet.SetEditableParameter("IsEqual", &isEqualParam);

				iprm::CParamsSet paramsSet1;
				paramsSet1.SetEditableParameter("MacAddress", &valueParamsSet);

				iprm::CParamsSet filterParam;
				filterParam.SetEditableParameter("ObjectFilter", &paramsSet1);

				imtbase::ICollectionInfo::Ids collectionIds = m_deviceCollectionCompPtr->GetElementIds(0, -1, &filterParam);
				if (!collectionIds.isEmpty()){
					QByteArray id = collectionIds[0];
					if (objectUuid != id){
						imtbase::IObjectCollection::DataPtr dataPtr;
						if (m_deviceCollectionCompPtr->GetObjectData(id, dataPtr)){
							prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(dataPtr.GetPtr());
							if (deviceInfoPtr != nullptr){
								QByteArray currentMacAddress = deviceInfoPtr->GetMacAddress().toLower();
								if (currentMacAddress == macAddress.toLower()){
									errorMessage = QString(QT_TR_NOOP("it is not possible to save the product '%1' because MAC address '%2' already exists"))
												.arg(productName)
												.arg(qPrintable(macAddress));

									return false;
								}
							}
						}
					}
				}
			}

			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_deviceCollectionCompPtr->GetObjectData(objectUuid, dataPtr)){
				prolifedata::COrderedIdentifiableDeviceInfo* hardwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(dataPtr.GetPtr());
				if (hardwareInfoPtr != nullptr){
					QByteArray currentOrderId = hardwareInfoPtr->GetOrderId();
					if (!currentOrderId.isEmpty() && orderUuid != currentOrderId){
						QByteArray macAddress = hardwareInfoPtr->GetMacAddress();

						errorMessage = QString("It is not possible to save a product that is linked to another order. Hardware product '%1' with ID '%2'").arg(productName).arg(macAddress);

						return false;
					}
				}
			}
		}
		else{
			errorMessage = QString("Unknown category for product");

			return false;
		}
	}

	return true;
}


QString COrderControllerComp::GetProductName(const QByteArray& productUuid) const
{
	if (m_productCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
			const imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
			if (productInfoPtr != nullptr){
				return productInfoPtr->GetName();
			}
		}
	}

	return QString();
}


} // namespace prolifegql


