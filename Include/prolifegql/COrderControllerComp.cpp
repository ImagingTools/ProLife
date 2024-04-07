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
		itemModel.CreateFromJson(itemData);

		orderPtr->SetObjectUuid(objectId);

		QByteArray orderId;
		if (itemModel.ContainsKey("OrderId")){
			orderId = itemModel.GetData("OrderId").toByteArray().trimmed();
		}

		name = orderId;

		if (orderId.isEmpty()){
			errorMessage = QT_TR_NOOP("ERP Order-ID can not be empty");
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
							errorMessage = QT_TR_NOOP("Order ID already exists");
							SendErrorMessage(0, errorMessage, "COrderControllerComp");
							errorMessage = imtgql::GetTranslation(m_translationManagerCompPtr.GetPtr(), gqlRequest, errorMessage.toUtf8(), "prolifegql::COrderControllerComp");

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
			errorMessage = QString("Unable to create an order object. Product collection is nullptr.");
			SendErrorMessage(0, errorMessage, "COrderControllerComp");

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
					InsertSoftwareProductToProductCollection(gqlRequest, *orderedProducts, productIndex, *productCollectionPtr, objectId, errorMessage);
					if (!errorMessage.isEmpty()){
						return nullptr;
					}
				}
				else if (productCategory == "Hardware"){
					InsertHardwareProductToProductCollection(gqlRequest, *orderedProducts, productIndex, *productCollectionPtr, objectId, errorMessage);
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

				for (const QByteArray& id : removedProducts){
					imtbase::IObjectCollection::DataPtr productDataPtr;
					if (m_softwareInstanceCollectionCompPtr->GetObjectData(id, productDataPtr)){
						prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(productDataPtr.GetPtr());
						if (productInfoPtr != nullptr){
							productInfoPtr->SetOrderId("");

							imtbase::IOperationContext* operationContextPtr = nullptr;

							if (m_softwareOperationContextControllerCompPtr.IsValid()){
								operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, id, productInfoPtr);
							}

							m_softwareInstanceCollectionCompPtr->SetObjectData(id, *productInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr);
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

	iprm::CTextParam valueParam;
	valueParam.SetText(serialNumber);

	iprm::CParamsSet paramsSet;
	paramsSet.SetEditableParameter("SerialNumber", &valueParam);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

	imtbase::IObjectCollection::Ids collectionIds = m_softwareInstanceCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty() && !serialNumber.isEmpty()){
		QByteArray objectId = collectionIds[0];
		if (objectId != uuidId){
			errorMessage = QString(QT_TR_NOOP("Serial number: %1 already exists.")).arg(qPrintable(serialNumber));
			SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");
			errorMessage = imtgql::GetTranslation(m_translationManagerCompPtr.GetPtr(), gqlRequest, errorMessage.toUtf8(), "prolifegql::COrderControllerComp");

			return;
		}
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

	istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
	objectLinkPtr.SetPtr(new imtbase::CObjectLink());

	objectLinkPtr->SetObjectUuid(uuidId);
	objectLinkPtr->SetFactoryId("SoftwareInfo");

	productCollection.InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), uuidId);

	if (m_softwareInstanceCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_softwareInstanceCollectionCompPtr->GetObjectData(uuidId, dataPtr)){
			prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
			if (productInfoPtr != nullptr){
				bool isInUse = productInfoPtr->IsInUse();
				if (!isInUse && !productInfoPtr->IsEqual(*softwareInstancePtr)){
					imtbase::ICollectionInfo::Ids ids = softwareInstancePtr->GetLicenseInstances().GetElementIds();
					imtbase::ICollectionInfo::Ids currentIds = productInfoPtr->GetLicenseInstances().GetElementIds();

					bool changed = false;
					if (!ids.isEmpty() && !currentIds.isEmpty()){
						const imtlic::ILicenseInstance* licensePtr = softwareInstancePtr->GetLicenseInstance(ids[0]);
						const imtlic::ILicenseInstance* currentLicensePtr = productInfoPtr->GetLicenseInstance(currentIds[0]);

						if (licensePtr->GetExpiration().toString("yyyy-MM-dd") != currentLicensePtr->GetExpiration().toString("yyyy-MM-dd")){
							changed = true;
						}
					}

					if (!changed){
						QByteArray currentProductId = productInfoPtr->GetProductId();
						QByteArray currentSerialNumber = productInfoPtr->GetSerialNumber();

						if (productId != currentProductId || serialNumber != currentSerialNumber || ids != currentIds){
							changed = true;
						}
					}

					if (changed){
						imtbase::IOperationContext* operationContextPtr = nullptr;

						if (m_softwareOperationContextControllerCompPtr.IsValid()){
							operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, uuidId, softwareInstancePtr.GetPtr());
						}

						m_softwareInstanceCollectionCompPtr->SetObjectData(uuidId, *softwareInstancePtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr);
					}
				}
			}
		}
		else{
			imtbase::IOperationContext* operationContextPtr = nullptr;

			if (m_softwareOperationContextControllerCompPtr.IsValid()){
				operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_CREATE, gqlRequest);
			}

			m_softwareInstanceCollectionCompPtr->InsertNewObject(QByteArray("Software"), "", "", softwareInstancePtr.PopPtr(), uuidId, nullptr, nullptr, operationContextPtr);
		}
	}
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

	QByteArray deviceUuid = hardwareProductModel.GetData("DeviceId", modelIndex).toByteArray();
	deviceInstancePtr->SetObjectUuid(deviceUuid);

	if (hardwareProductModel.ContainsKey("IsNewDevice", modelIndex)){
		imtbase::IOperationContext* operationContextPtr = nullptr;

		if (m_deviceOperationContextControllerCompPtr.IsValid()){
			operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_CREATE, gqlRequest);
		}

		m_deviceCollectionCompPtr->InsertNewObject("DocumentInfo", "", "", deviceInstancePtr.GetPtr(), deviceUuid, nullptr, nullptr, operationContextPtr);
	}
	else{
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_deviceCollectionCompPtr->GetObjectData(deviceUuid, dataPtr)){
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
						operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, deviceUuid, deviceInfoPtr);
					}

					m_deviceCollectionCompPtr->SetObjectData(deviceUuid, *deviceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr);
				}
			}
		}
	}

	istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
	objectLinkPtr.SetPtr(new imtbase::CObjectLink());

	objectLinkPtr->SetObjectUuid(deviceUuid);
	objectLinkPtr->SetFactoryId("HardwareInfo");

	productCollection.InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), deviceUuid);
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

//				QString licenseName = licenseInstancePtr->GetLicenseName();
//				softwareProductModel.SetData("LicenseName", licenseName, modelIndex);

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
		hardwareProductModel.SetData("DeviceId", objectUuid, modelIndex);

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


} // namespace prolifegql


