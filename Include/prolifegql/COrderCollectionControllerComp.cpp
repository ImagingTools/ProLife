#include <prolifegql/COrderCollectionControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CEnableableParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/CObjectLink.h>
#include <imtlic/IProductInfo.h>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/COrderInfo.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>
#include <prolifedata/IHardwareProductBinding.h>


namespace prolifegql
{


// protected methods

bool COrderCollectionControllerComp::CheckProducts(
	const QByteArray& orderUuid,
	const QList<prolife::sdl::Orders::CProductItem>& products,
	QString& errorMessage) const
{
	if (!m_softwareInstanceCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Error: Attribute 'm_softwareInstanceCollectionCompPtr' was not set", "COrderCollectionControllerComp");
		return false;
	}

	if (!m_deviceCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Error: Attribute 'm_deviceCollectionCompPtr' was not set", "COrderCollectionControllerComp");
		return false;
	}

	for (const prolife::sdl::Orders::CProductItem& product : products){
		QByteArray objectUuid = product.GetId();
		QByteArray productUuid = product.GetProductUuid();
		QByteArray categoryId = product.GetCategoryId();
		QString productName = GetProductName(productUuid);

		if (categoryId == "Software"){
			QByteArray serialNumber = product.GetSerialNumber();

			bool ok = prolifedata::CheckSoftwareSerialNumberExists(objectUuid, serialNumber, *m_softwareInstanceCollectionCompPtr);
			if (!ok){
				errorMessage = QString(QT_TR_NOOP("It is not possible to save the product '%1' because serial number '%2' already exists")).arg(productName).arg(qPrintable(serialNumber));
				return false;
			}

			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_softwareInstanceCollectionCompPtr->GetObjectData(objectUuid, dataPtr)){
				prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
				if (softwareInfoPtr != nullptr){
					QByteArray currentOrderId = softwareInfoPtr->GetOrderId();
					if (!currentOrderId.isEmpty() && orderUuid != currentOrderId){
						QByteArray softwareSerialNumber = softwareInfoPtr->GetSerialNumber();
						errorMessage = QString("It is not possible to add a product that is linked to another order. Software product '%1' with ID '%2'").arg(productName).arg(softwareSerialNumber);
						return false;
					}
				}
			}
		}
		else if (categoryId == "Hardware"){
			QByteArray macAddress = product.GetMacAddress();
			bool ok = prolifedata::CheckDeviceMacAddressExists(objectUuid, macAddress, *m_deviceCollectionCompPtr);
			if (!ok){
				errorMessage = QString(QT_TR_NOOP("It is not possible to save the product '%1' because MAC address '%2' already exists")).arg(productName).arg(qPrintable(macAddress));
				return false;
			}

			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_deviceCollectionCompPtr->GetObjectData(objectUuid, dataPtr)){
				prolifedata::COrderedIdentifiableDeviceInfo* hardwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(dataPtr.GetPtr());
				if (hardwareInfoPtr != nullptr){
					QByteArray currentOrderId = hardwareInfoPtr->GetOrderId();
					if (!currentOrderId.isEmpty() && orderUuid != currentOrderId){
						errorMessage = QString("It is not possible to save a product that is linked to another order. Hardware product '%1' with ID '%2'").arg(productName).arg(hardwareInfoPtr->GetMacAddress());
						return false;
					}
				}
			}

			QByteArray serialNumber = product.GetSerialNumber();
			bool serialNumberIsValid = prolifedata::CheckDeviceSerialNumberExists(objectUuid, serialNumber, *m_deviceCollectionCompPtr);
			if (!serialNumberIsValid){
				errorMessage = QString("It is not possible to save the product '%1' because Serial Number: '%2' already exists").arg(productName).arg(serialNumber);
				return false;
			}
		}
		else{
			errorMessage = QString("Unknown category for product");
			return false;
		}
	}

	return true;
}


QString COrderCollectionControllerComp::GetProductName(const QByteArray& productUuid) const
{
	if (!m_productCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Error: Attribute 'm_productCollectionCompPtr' was not set", "COrderCollectionControllerComp");
		return QString();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
		const imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
		if (productInfoPtr != nullptr){
			return productInfoPtr->GetName();
		}
	}

	return QString();
}


void COrderCollectionControllerComp::GenerateDifferences(
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


// reimplemented (prolife::sdl::Orders::COrderCollectionControllerCompBase)

bool COrderCollectionControllerComp::CreateRepresentationFromObject(
	const imtbase::IObjectCollectionIterator& objectCollectionIterator,
	const prolife::sdl::Orders::COrdersListGqlRequest& ordersListRequest,
	prolife::sdl::Orders::COrderItem& representationObject,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Unable to create representation from object. Error: Attribute 'm_objectCollectionCompPtr' was not set", "COrderCollectionControllerComp");
		return false;
	}

	prolife::sdl::Orders::OrdersListRequestInfo requestInfo = ordersListRequest.GetRequestInfo();

	QByteArray objectId = objectCollectionIterator.GetObjectId();

	prolifedata::COrderInfo* orderInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		orderInfoPtr = dynamic_cast<prolifedata::COrderInfo*>(dataPtr.GetPtr());
	}

	if (orderInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return false;
	}

	idoc::MetaInfoPtr metaInfo = objectCollectionIterator.GetDataMetaInfo();

	if (requestInfo.items.isTypeIdRequested){
		representationObject.SetTypeId(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isIdRequested){
		representationObject.SetId(objectId);
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.SetDescription(objectCollectionIterator.GetElementInfo("Description").toString());
	}

	if (requestInfo.items.isStatusRequested){
		representationObject.SetStatus(prolifedata::GetNameFromOrderStatus(orderInfoPtr->GetOrderStatus()));
	}

	if (requestInfo.items.isOrderIdRequested){
		representationObject.SetOrderId(objectCollectionIterator.GetElementInfo("OrderId").toByteArray());
	}

	if (requestInfo.items.isOrderCustomerRequested){
		representationObject.SetOrderCustomer(objectCollectionIterator.GetElementInfo("OrderCustomer").toByteArray());
	}

	if (requestInfo.items.isPurchaseIdRequested){
		representationObject.SetPurchaseId(objectCollectionIterator.GetElementInfo("PurchaseId").toByteArray());
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.SetAdded(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.SetLastModified(lastModified);
	}

	return true;
}


istd::IChangeable* COrderCollectionControllerComp::CreateObjectFromRepresentation(
	const prolife::sdl::Orders::COrderData& orderDataRepresentation,
	QByteArray& newObjectId,
	QString& name,
	QString& description,
	QString& errorMessage) const
{
	if (!m_orderInfoFactCompPtr.IsValid()){
		Q_ASSERT_X(false, "Error: Attribute 'm_orderInfoFactCompPtr' was not set", "COrderCollectionControllerComp");
		return nullptr;
	}

	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Error: Attribute 'm_objectCollectionCompPtr' was not set", "COrderCollectionControllerComp");
		return nullptr;
	}

	if (!m_deviceCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Error: Attribute 'm_deviceCollectionCompPtr' was not set", "COrderCollectionControllerComp");
		return nullptr;
	}

	if (!m_softwareInstanceCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Error: Attribute 'm_softwareInstanceCollectionCompPtr' was not set", "COrderCollectionControllerComp");
		return nullptr;
	}

	istd::TDelPtr<prolifedata::IOrderInfo> orderInstancePtr = m_orderInfoFactCompPtr.CreateInstance();
	if (!orderInstancePtr.IsValid()){
		errorMessage = QString("Unable to create order instance. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return nullptr;
	}

	prolifedata::CIdentifiableOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(orderInstancePtr.GetPtr());
	if (orderInfoPtr == nullptr){
		errorMessage = QString("Unable to cast order instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return nullptr;
	}

	QByteArray orderUuid = orderDataRepresentation.GetId();
	if (orderUuid.isEmpty()){
		orderUuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	orderInfoPtr->SetObjectUuid(orderUuid);
	newObjectId = orderUuid;

	QString orderId = orderDataRepresentation.GetOrderId();
	if (orderId.isEmpty()){
		errorMessage = QString("Delivery-ID cannot be empty");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return nullptr;
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
		if (orderUuid != orderObjectId){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_objectCollectionCompPtr->GetObjectData(orderObjectId, dataPtr)){
				prolifedata::CIdentifiableOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(dataPtr.GetPtr());
				if (orderInfoPtr != nullptr){
					QByteArray currentOrderId = orderInfoPtr->GetOrderId().toLower();
					if (currentOrderId == orderId.toLower()){
						errorMessage = QString("Delivery-ID already exists");
						SendErrorMessage(0, errorMessage, "COrderControllerComp");

						return nullptr;
					}
				}
			}
		}
	}

	orderInfoPtr->SetOrderId(orderId.toUtf8());
	name = orderId;

	QByteArray customerId = orderDataRepresentation.GetCustomerId();
	orderInfoPtr->SetCustomerId(customerId);
	if (customerId.isEmpty()){
		errorMessage = QString("Customer can not be empty!");
		SendErrorMessage(0, errorMessage, "COrderControllerComp");

		return nullptr;
	}

	QByteArray purchaseOrderId = orderDataRepresentation.GetPurchaseId();
	orderInfoPtr->SetPurchaseOrderId(purchaseOrderId);

	QString orderDescription = orderDataRepresentation.GetDescription();
	orderInfoPtr->SetDescription(orderDescription);
	description = orderDescription;

	QByteArray status = orderDataRepresentation.GetOrderStatus();
	orderInfoPtr->SetOrderStatus(prolifedata::GetOrderStatusFromId(status));

	imtbase::IObjectCollection* productCollectionPtr = orderInfoPtr->GetProducts();
	if (productCollectionPtr == nullptr){
		Q_ASSERT(false);
		return nullptr;
	}

	QList<prolife::sdl::Orders::CProductItem> products = orderDataRepresentation.GetOrderProducts();

	bool ok = CheckProducts(orderUuid, products, errorMessage);
	if (!ok){
		return nullptr;
	}

	for (const prolife::sdl::Orders::CProductItem& product : products){
		QByteArray orderProductUuid = product.GetId();
		QByteArray categoryId = product.GetCategoryId();
		QByteArray productUuid = product.GetProductUuid();
		QByteArray serialNumber = product.GetSerialNumber();
		QByteArray licenseUuid = product.GetLicenseUuid();
		bool isNew = product.GetIsNew();

		istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
		objectLinkPtr.SetPtr(new imtbase::CObjectLink());

		objectLinkPtr->SetObjectUuid(orderUuid);

		if (categoryId == "Software"){
			istd::TDelPtr<prolifedata::COrderedIdentifiableSoftwareInstanceInfo> softwareInstancePtr;
			softwareInstancePtr.SetPtr(new prolifedata::COrderedIdentifiableSoftwareInstanceInfo);

			softwareInstancePtr->SetObjectUuid(orderProductUuid);
			softwareInstancePtr->SetupProductInstance(productUuid, "", "");
			softwareInstancePtr->SetSerialNumber(serialNumber);

			QString expiration = product.GetExpiration();
			QDateTime expirationDate = QDateTime::fromString(expiration, "yyyy-MM-dd");
			softwareInstancePtr->AddLicense(licenseUuid, expirationDate);

			if (isNew){
				QByteArray result = m_softwareInstanceCollectionCompPtr->InsertNewObject(QByteArray("Software"), "", "", softwareInstancePtr.PopPtr(), orderProductUuid, nullptr, nullptr/*, operationContextPtr*/);
				if (result.isEmpty()){
					errorMessage = QString("Unable to insert new software product with ID: '%1'").arg(orderProductUuid);
					return nullptr;
				}
			}
			else{
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_softwareInstanceCollectionCompPtr->GetObjectData(orderProductUuid, dataPtr)){
					prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
					if (productInfoPtr != nullptr){
						bool isInUse = productInfoPtr->IsInUse();
						if (!isInUse){
							QByteArray oldOrderId = productInfoPtr->GetOrderId();
							if (!oldOrderId.isEmpty() && oldOrderId != orderUuid){
								return nullptr;
							}

							if (!isInUse && !productInfoPtr->IsEqual(*softwareInstancePtr)){
								if (!m_softwareInstanceCollectionCompPtr->SetObjectData(orderProductUuid, *softwareInstancePtr, istd::IChangeable::CM_WITHOUT_REFS/*, operationContextPtr*/)){
									errorMessage = QString("Unable to update a software product with ID: '%1'").arg(orderProductUuid);

									return nullptr;
								}
							}
						}
					}
				}
			}

			objectLinkPtr->SetFactoryId("SoftwareInfo");
		}
		else if (categoryId == "Hardware"){
			istd::TDelPtr<prolifedata::COrderedIdentifiableDeviceInfo> deviceInstancePtr;
			deviceInstancePtr.SetPtr(new prolifedata::COrderedIdentifiableDeviceInfo);

			deviceInstancePtr->SetObjectUuid(orderProductUuid);
			deviceInstancePtr->SetOrderId(orderUuid);
			deviceInstancePtr->SetDeviceType(productUuid);
			deviceInstancePtr->SetConfigurationType(licenseUuid);
			deviceInstancePtr->SetMacAddress(product.GetMacAddress());
			deviceInstancePtr->SetSerialNumber(serialNumber);

			if (isNew){
				m_deviceCollectionCompPtr->InsertNewObject("DocumentInfo", "", "", deviceInstancePtr.GetPtr(), orderProductUuid, nullptr, nullptr/*, operationContextPtr*/);
			}
			else{
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_deviceCollectionCompPtr->GetObjectData(orderProductUuid, dataPtr)){
					prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(dataPtr.GetPtr());
					if (deviceInfoPtr != nullptr){
						QByteArray deviceOrderUuid = deviceInfoPtr->GetOrderId();
						QByteArray deviceType = deviceInfoPtr->GetDeviceType();
						QByteArray deviceConfigurationType = deviceInfoPtr->GetConfigurationType();

						if (deviceOrderUuid != orderUuid || deviceType != productUuid || deviceConfigurationType != licenseUuid){
							deviceInfoPtr->SetOrderId(orderUuid);
							deviceInfoPtr->SetDeviceType(productUuid);
							deviceInfoPtr->SetConfigurationType(licenseUuid);

							m_deviceCollectionCompPtr->SetObjectData(orderProductUuid, *deviceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS/*, operationContextPtr*/);
						}
					}
				}
			}

			objectLinkPtr->SetFactoryId("HardwareInfo");
		}

		productCollectionPtr->InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), orderProductUuid);
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(orderUuid, dataPtr)){
		prolifedata::CIdentifiableOrderInfo* oldOrderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(dataPtr.GetPtr());
		if (oldOrderInfoPtr != nullptr){
			QByteArrayList addedProducts;
			QByteArrayList removedProducts;
			QByteArrayList updatedProducts;

			GenerateDifferences(*oldOrderInfoPtr, *orderInfoPtr, addedProducts, removedProducts, updatedProducts);

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

							if (!m_deviceCollectionCompPtr->SetObjectData(id, *hardwareInfoPtr, istd::IChangeable::CM_WITHOUT_REFS/*, operationContextPtr*/)){
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

							if (!m_softwareInstanceCollectionCompPtr->SetObjectData(id, *softwareInfoPtr, istd::IChangeable::CM_WITHOUT_REFS/*, operationContextPtr*/)){
								return nullptr;
							}
						}
					}
				}
			}
		}
	}

	return orderInstancePtr.PopPtr();
}


bool COrderCollectionControllerComp::CreateRepresentationFromObject(
	const istd::IChangeable& data,
	const prolife::sdl::Orders::COrderItemGqlRequest& orderItemRequest,
	prolife::sdl::Orders::COrderDataPayload& representationPayload,
	QString& errorMessage) const
{
	istd::IChangeable objectPtr = static_cast<istd::IChangeable>(data);
	prolifedata::CIdentifiableOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(&objectPtr);
	if (orderInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return false;
	}

	prolife::sdl::Orders::OrderItemRequestArguments arguments = orderItemRequest.GetRequestedArguments();
	prolife::sdl::Orders::COrderData orderData;

	QByteArray id = arguments.input.GetId();
	orderData.SetId(id);

	QByteArray orderId = orderInfoPtr->GetOrderId();
	orderData.SetName(orderId);
	orderData.SetOrderId(orderId);

	QByteArray purchaseOrderId = orderInfoPtr->GetPurchaseOrderId();
	orderData.SetPurchaseId(purchaseOrderId);

	QByteArray customerId = orderInfoPtr->GetCustomerId();
	orderData.SetCustomerId(customerId);

	QString description = orderInfoPtr->GetDescription();
	orderData.SetDescription(description);

	prolifedata::IOrderInfo::OrderStatus status = orderInfoPtr->GetOrderStatus();
	QByteArray orderStatus = prolifedata::GetIdFromOrderStatus(status);
	orderData.SetOrderStatus(orderStatus);

	imtbase::IObjectCollection* productCollectionPtr = orderInfoPtr->GetProducts();
	if (productCollectionPtr == nullptr){
		return false;
	}

	QList<prolife::sdl::Orders::CProductItem> products;

	imtbase::ICollectionInfo::Ids orderedProductIds = productCollectionPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& productId : orderedProductIds){
		imtbase::ICollectionInfo::Id typeId = productCollectionPtr->GetObjectTypeId(productId);

		imtbase::IObjectCollection::DataPtr productDataPtr;
		if (typeId == QByteArray("SoftwareInfo")){
			if (m_softwareInstanceCollectionCompPtr->GetObjectData(productId, productDataPtr)){
				const imtlic::IProductInstanceInfo* softwareProductPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
				if (softwareProductPtr != nullptr){
					prolife::sdl::Orders::CProductItem softwareProduct;
					QByteArray productUuid = softwareProductPtr->GetProductId();

					softwareProduct.SetId(productId);
					softwareProduct.SetCategoryId(softwareProductPtr->GetFactoryId());
					softwareProduct.SetProductUuid(softwareProductPtr->GetProductId());
					softwareProduct.SetSerialNumber(softwareProductPtr->GetSerialNumber());
					softwareProduct.SetInUse(softwareProductPtr->IsInUse());
					softwareProduct.SetIsNew(false);

					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
						const imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
						if (productInfoPtr != nullptr){
							softwareProduct.SetProductName(productInfoPtr->GetName());
						}
					}

					const imtbase::ICollectionInfo& licenseInstances = softwareProductPtr->GetLicenseInstances();
					imtbase::ICollectionInfo::Ids activeLicenseIds = licenseInstances.GetElementIds();
					if (!activeLicenseIds.isEmpty()){
						QByteArray activeLicenseId = activeLicenseIds[0];
						const imtlic::ILicenseInstance* licenseInstancePtr = softwareProductPtr->GetLicenseInstance(activeLicenseId);
						if (licenseInstancePtr != nullptr){
							softwareProduct.SetLicenseUuid(activeLicenseId);

							imtbase::IObjectCollection::DataPtr dataPtr;
							if (m_licenseDefinitionCollectionCompPtr->GetObjectData(activeLicenseId, dataPtr)){
								const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(dataPtr.GetPtr());
								if (licenseInfoPtr != nullptr){
									softwareProduct.SetLicenseName(licenseInfoPtr->GetLicenseName());
									softwareProduct.SetLicenseId(licenseInfoPtr->GetLicenseId());
								}
							}

							QDate date = licenseInstancePtr->GetExpiration().date();
							QString licenseExpiration = date.toString("yyyy-MM-dd");

							softwareProduct.SetExpiration(licenseExpiration);
						}
					}

					products.append(softwareProduct);
				}
			}
		}
		else if (typeId == QByteArray("HardwareInfo")){
			if (m_deviceCollectionCompPtr->GetObjectData(productId, productDataPtr)){
				const prolifedata::CIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::CIdentifiableDeviceInfo*>(productDataPtr.GetPtr());
				if (deviceInfoPtr != nullptr){
					prolife::sdl::Orders::CProductItem hardwareProduct;

					QByteArray productUuid = deviceInfoPtr->GetDeviceType();
					QByteArray licenseDefinitionUuid = deviceInfoPtr->GetConfigurationType();

					hardwareProduct.SetId(productId);
					hardwareProduct.SetProductUuid(productUuid);
					hardwareProduct.SetCategoryId(QByteArray("Hardware"));
					hardwareProduct.SetLicenseUuid(licenseDefinitionUuid);
					hardwareProduct.SetIsNew(false);

					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
						const imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
						if (productInfoPtr != nullptr){
							hardwareProduct.SetProductName(productInfoPtr->GetName());
						}
					}

					imtbase::IObjectCollection::DataPtr licenseDataPtr;
					if (m_licenseDefinitionCollectionCompPtr->GetObjectData(licenseDefinitionUuid, licenseDataPtr)){
						const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
						if (licenseInfoPtr != nullptr){
							hardwareProduct.SetLicenseName(licenseInfoPtr->GetLicenseName());
							hardwareProduct.SetLicenseId(licenseInfoPtr->GetLicenseId());
						}
					}

					imtbase::IObjectCollection::DataPtr deviceDataPtr;
					if (m_deviceCollectionCompPtr->GetObjectData(productId, deviceDataPtr)){
						const prolifedata::IDeviceInfo* deviceInfoInstancePtr = dynamic_cast<prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
						if (deviceInfoInstancePtr != nullptr){
							hardwareProduct.SetMacAddress(deviceInfoInstancePtr->GetMacAddress());
							hardwareProduct.SetSerialNumber(deviceInfoInstancePtr->GetSerialNumber());
						}
					}

					imtbase::IObjectCollection::DataPtr bindingDataPtr;
					if (m_bindingCollectionCompPtr->GetObjectData(productId, bindingDataPtr)){
						const prolifedata::IHardwareProductBinding* bindingInfoPtr = dynamic_cast<const prolifedata::IHardwareProductBinding*>(bindingDataPtr.GetPtr());
						if (bindingInfoPtr != nullptr){
							hardwareProduct.SetInUse(false);

							QByteArrayList softwareIds = bindingInfoPtr->GetSoftwareIds();
							for (const QByteArray& softwareId : softwareIds){
								imtbase::IObjectCollection::DataPtr softwareDataPtr;
								if (m_softwareInstanceCollectionCompPtr->GetObjectData(softwareId, softwareDataPtr)){
									const imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
									if (productInstanceInfoPtr != nullptr){
										bool isUse = productInstanceInfoPtr->IsInUse();
										if (isUse){
											hardwareProduct.SetInUse(true);
											break;
										}
									}
								}
							}
						}
					}

					products.append(hardwareProduct);
				}
			}
		}
	}

	representationPayload.SetOrderData(orderData);

	return true;
}


void COrderCollectionControllerComp::SetObjectFilter(
	const imtgql::CGqlRequest& gqlRequest,
	const imtbase::CTreeItemModel& objectFilterModel,
	iprm::CParamsSet& filterParams) const
{
	BaseClass::SetObjectFilter(gqlRequest, objectFilterModel, filterParams);

	imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return;
	}

	imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		return;
	}

	QByteArray userId = userInfoPtr->GetId();
	QByteArrayList groupIds = userInfoPtr->GetGroups();
	QByteArrayList userPermissions = userInfoPtr->GetPermissions();

	bool isAdmin = userInfoPtr->IsAdmin();
	bool filterByGroup = true;

	if (m_checkPermissionCompPtr.IsValid()){
		QByteArrayList permissions;
		permissions << *m_permissionIdAttrPtr;

		filterByGroup = !m_checkPermissionCompPtr->CheckPermission(userPermissions, permissions);
	}

	if (isAdmin){
		filterByGroup = false;
	}

	if (filterByGroup){
		iprm::CTextParam* userParamPtr = new iprm::CTextParam();
		userParamPtr->SetText(userId);

		iprm::CTextParam* groupParamPtr = new iprm::CTextParam();
		QByteArray groups;
		if (!groupIds.isEmpty()){
			groups = groupIds.join(';');
		}
		groupParamPtr->SetText(groups);

		iprm::CParamsSet* paramsSetPtr = new iprm::CParamsSet();

		paramsSetPtr->SetEditableParameter("UserParam", userParamPtr, true);
		paramsSetPtr->SetEditableParameter("GroupParam", groupParamPtr, true);

		filterParams.SetEditableParameter("Groups", paramsSetPtr, true);
	}
}


} // namespace prolifegql


