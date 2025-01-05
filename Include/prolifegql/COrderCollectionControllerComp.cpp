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
	const QList<sdl::prolife::Orders::COrderedProduct::V1_0>& products,
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

	for (const sdl::prolife::Orders::COrderedProduct::V1_0& product : products){
		QByteArray objectUuid = *product.Id;
		QByteArray productUuid = *product.ProductUuid;
		QByteArray categoryId = *product.CategoryId;
		QString productName = GetProductName(productUuid);

		if (categoryId == "Software"){
			QByteArray serialNumber = *product.SerialNumber;

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
			QByteArray macAddress = *product.MacAddress;
			if (!macAddress.isEmpty()){
				bool ok = prolifedata::CheckDeviceMacAddressExists(objectUuid, macAddress, *m_deviceCollectionCompPtr);
				if (!ok){
					errorMessage = QString(QT_TR_NOOP("It is not possible to save the product '%1' because MAC address '%2' already exists")).arg(productName).arg(qPrintable(macAddress));
					return false;
				}
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

			QByteArray serialNumber = *product.SerialNumber;
			if (!serialNumber.isEmpty()){
				bool serialNumberIsValid = prolifedata::CheckDeviceSerialNumberExists(objectUuid, serialNumber, *m_deviceCollectionCompPtr);
				if (!serialNumberIsValid){
					errorMessage = QString("It is not possible to save the product '%1' because Serial Number: '%2' already exists").arg(productName).arg(serialNumber);
					return false;
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
	QByteArrayList& /*updatedProducts*/) const
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


// reimplemented (sdl::prolife::Orders::V1_0::COrderCollectionControllerCompBase)

bool COrderCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::prolife::Orders::V1_0::COrdersListGqlRequest& ordersListRequest,
			sdl::prolife::Orders::COrderItem::V1_0& representationObject,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Unable to create representation from object. Error: Attribute 'm_objectCollectionCompPtr' was not set", "COrderCollectionControllerComp");
		return false;
	}

	sdl::prolife::Orders::V1_0::OrdersListRequestInfo requestInfo = ordersListRequest.GetRequestInfo();

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

	if (requestInfo.items.isIdRequested){
		representationObject.Id = std::make_optional<QByteArray>(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		representationObject.TypeId = std::make_optional<QByteArray>(collectionObjectId);
	}

	if (requestInfo.items.isNameRequested){
		representationObject.Name = std::make_optional<QString>(orderInfoPtr->GetOrderId());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.Description = std::make_optional<QString>(orderInfoPtr->GetDescription());
	}

	if (requestInfo.items.isStatusRequested){
		representationObject.Status = std::make_optional<QString>(prolifedata::GetNameFromOrderStatus(orderInfoPtr->GetOrderStatus()));
	}

	if (requestInfo.items.isOrderIdRequested){
		representationObject.OrderId = std::make_optional<QByteArray>(orderInfoPtr->GetOrderId());
	}

	if (requestInfo.items.isOrderCustomerRequested){
		representationObject.OrderCustomer = std::make_optional<QString>(objectCollectionIterator.GetElementInfo("OrderCustomer").toString());
	}

	if (requestInfo.items.isPurchaseIdRequested){
		representationObject.PurchaseId = std::make_optional<QByteArray>(orderInfoPtr->GetPurchaseOrderId());
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.Added = std::make_optional<QString>(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.LastModified = std::make_optional<QString>(lastModified);
	}

	return true;
}


istd::IChangeable* COrderCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::prolife::Orders::COrderData::V1_0& orderDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_orderInfoFactCompPtr.IsValid()){
		Q_ASSERT_X(false, "Error: Attribute 'm_orderInfoFactCompPtr' was not set", "COrderCollectionControllerComp");
		return nullptr;
	}

	istd::TDelPtr<prolifedata::CIdentifiableOrderInfo> orderInfoPtr;
	orderInfoPtr.SetCastedOrRemove(m_orderInfoFactCompPtr.CreateInstance());
	if (!orderInfoPtr.IsValid()){
		errorMessage = QString("Unable to cast order instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return nullptr;
	}

	if (!FillObjectFromRepresentation(orderDataRepresentation, *orderInfoPtr, newObjectId, errorMessage)){
		errorMessage = QString("Unable to create order from representatiom. Error: '%1'").arg(errorMessage);
		return nullptr;
	}

	return orderInfoPtr.PopPtr();
}


bool COrderCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::prolife::Orders::V1_0::COrderItemGqlRequest& orderItemRequest,
			sdl::prolife::Orders::COrderDataPayload::V1_0& representationPayload,
			QString& errorMessage) const
{
	prolifedata::CIdentifiableOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(&const_cast<istd::IChangeable&>(data));
	if (orderInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return false;
	}

	sdl::prolife::Orders::V1_0::OrderItemRequestArguments arguments = orderItemRequest.GetRequestedArguments();
	sdl::prolife::Orders::COrderData::V1_0 orderData;

	QByteArray id;
	if (arguments.input.Id){
		id = *arguments.input.Id;
	}
	orderData.Id = std::make_optional<QByteArray>(id);

	QByteArray orderId = orderInfoPtr->GetOrderId();
	orderData.Name = std::make_optional<QString>(orderId);
	orderData.OrderId = std::make_optional<QByteArray>(orderId);

	QByteArray purchaseOrderId = orderInfoPtr->GetPurchaseOrderId();
	orderData.PurchaseId = std::make_optional<QByteArray>(purchaseOrderId);

	QByteArray customerId = orderInfoPtr->GetCustomerId();
	orderData.CustomerId = std::make_optional<QByteArray>(customerId);

	QString description = orderInfoPtr->GetDescription();
	orderData.Description = std::make_optional<QString>(description);

	prolifedata::IOrderInfo::OrderStatus status = orderInfoPtr->GetOrderStatus();
	QByteArray orderStatus = prolifedata::GetIdFromOrderStatus(status);
	orderData.OrderStatus = std::make_optional<QByteArray>(orderStatus);

	imtbase::IObjectCollection* productCollectionPtr = orderInfoPtr->GetProducts();
	if (productCollectionPtr == nullptr){
		return false;
	}

	QList<sdl::prolife::Orders::COrderedProduct::V1_0> products;

	imtbase::ICollectionInfo::Ids orderedProductIds = productCollectionPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& productId : orderedProductIds){
		imtbase::ICollectionInfo::Id typeId = productCollectionPtr->GetObjectTypeId(productId);

		imtbase::IObjectCollection::DataPtr productDataPtr;
		if (typeId == QByteArray("SoftwareInfo")){
			if (m_softwareInstanceCollectionCompPtr->GetObjectData(productId, productDataPtr)){
				const imtlic::IProductInstanceInfo* softwareProductPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
				if (softwareProductPtr != nullptr){
					sdl::prolife::Orders::COrderedProduct::V1_0 softwareProduct;
					QByteArray productUuid = softwareProductPtr->GetProductId();

					softwareProduct.Id = std::make_optional<QByteArray>(productId);
					softwareProduct.CategoryId = std::make_optional<QByteArray>(softwareProductPtr->GetFactoryId());
					softwareProduct.ProductUuid = std::make_optional<QByteArray>(softwareProductPtr->GetProductId());
					softwareProduct.SerialNumber = std::make_optional<QByteArray>(softwareProductPtr->GetSerialNumber());
					softwareProduct.InUse = std::make_optional<bool>(softwareProductPtr->IsInUse());
					softwareProduct.IsNew = std::make_optional<bool>(false);

					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
						const imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
						if (productInfoPtr != nullptr){
							softwareProduct.ProductName = std::make_optional<QString>(productInfoPtr->GetName());
						}
					}

					const imtbase::ICollectionInfo& licenseInstances = softwareProductPtr->GetLicenseInstances();
					imtbase::ICollectionInfo::Ids activeLicenseIds = licenseInstances.GetElementIds();
					if (!activeLicenseIds.isEmpty()){
						QByteArray activeLicenseId = activeLicenseIds[0];
						const imtlic::ILicenseInstance* licenseInstancePtr = softwareProductPtr->GetLicenseInstance(activeLicenseId);
						if (licenseInstancePtr != nullptr){
							softwareProduct.LicenseUuid = std::make_optional<QByteArray>(activeLicenseId);

							imtbase::IObjectCollection::DataPtr licenseDataPtr;
							if (m_licenseDefinitionCollectionCompPtr->GetObjectData(activeLicenseId, licenseDataPtr)){
								const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
								if (licenseInfoPtr != nullptr){
									softwareProduct.LicenseName = std::make_optional<QString>(licenseInfoPtr->GetLicenseName());
									softwareProduct.LicenseUuid = std::make_optional<QByteArray>(licenseInfoPtr->GetLicenseId());
								}
							}

							QDate date = licenseInstancePtr->GetExpiration().date();
							QString licenseExpiration = date.toString("yyyy-MM-dd");
							softwareProduct.Expiration = std::make_optional<QString>(licenseExpiration);
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
					sdl::prolife::Orders::COrderedProduct::V1_0 hardwareProduct;

					QByteArray productUuid = deviceInfoPtr->GetDeviceType();
					QByteArray licenseDefinitionUuid = deviceInfoPtr->GetConfigurationType();

					hardwareProduct.Id = std::make_optional<QByteArray>(productId);
					hardwareProduct.ProductUuid = std::make_optional<QByteArray>(productUuid);
					hardwareProduct.CategoryId = std::make_optional<QByteArray>(QByteArray("Hardware"));
					hardwareProduct.LicenseUuid = std::make_optional<QByteArray>(licenseDefinitionUuid);
					hardwareProduct.IsNew = std::make_optional<bool>(false);

					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
						const imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
						if (productInfoPtr != nullptr){
							hardwareProduct.ProductName = std::make_optional<QString>(productInfoPtr->GetName());
						}
					}

					imtbase::IObjectCollection::DataPtr licenseDataPtr;
					if (m_licenseDefinitionCollectionCompPtr->GetObjectData(licenseDefinitionUuid, licenseDataPtr)){
						const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
						if (licenseInfoPtr != nullptr){
							hardwareProduct.LicenseName = std::make_optional<QString>(licenseInfoPtr->GetLicenseName());
							hardwareProduct.LicenseId = std::make_optional<QByteArray>(licenseInfoPtr->GetLicenseId());
						}
					}

					imtbase::IObjectCollection::DataPtr deviceDataPtr;
					if (m_deviceCollectionCompPtr->GetObjectData(productId, deviceDataPtr)){
						const prolifedata::IDeviceInfo* deviceInfoInstancePtr = dynamic_cast<prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
						if (deviceInfoInstancePtr != nullptr){
							hardwareProduct.MacAddress = std::make_optional<QByteArray>(deviceInfoInstancePtr->GetMacAddress());
							hardwareProduct.SerialNumber = std::make_optional<QByteArray>(deviceInfoInstancePtr->GetSerialNumber());
						}
					}

					imtbase::IObjectCollection::DataPtr bindingDataPtr;
					if (m_bindingCollectionCompPtr->GetObjectData(productId, bindingDataPtr)){
						const prolifedata::IHardwareProductBinding* bindingInfoPtr = dynamic_cast<const prolifedata::IHardwareProductBinding*>(bindingDataPtr.GetPtr());
						if (bindingInfoPtr != nullptr){
							hardwareProduct.InUse = std::make_optional<bool>(false);

							QByteArrayList softwareIds = bindingInfoPtr->GetSoftwareIds();
							for (const QByteArray& softwareId : softwareIds){
								imtbase::IObjectCollection::DataPtr softwareDataPtr;
								if (m_softwareInstanceCollectionCompPtr->GetObjectData(softwareId, softwareDataPtr)){
									const imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
									if (productInstanceInfoPtr != nullptr){
										bool isUse = productInstanceInfoPtr->IsInUse();
										if (isUse){
											hardwareProduct.InUse = std::make_optional<bool>(true);
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

	orderData.OrderProducts = std::make_optional<QList<sdl::prolife::Orders::COrderedProduct::V1_0>>(products);
	representationPayload.OrderData = std::make_optional<sdl::prolife::Orders::COrderData::V1_0>(orderData);

	return true;
}


bool COrderCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::prolife::Orders::V1_0::COrderUpdateGqlRequest& orderUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	sdl::prolife::Orders::COrderData::V1_0 orderData = *orderUpdateRequest.GetRequestedArguments().input.Item;
	QByteArray objectId = *orderUpdateRequest.GetRequestedArguments().input.Id;

	prolifedata::CIdentifiableOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(&const_cast<istd::IChangeable&>(object));
	if (orderInfoPtr == nullptr){
		errorMessage = QString("Object is invalid");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return false;
	}

	orderInfoPtr->ResetData();

	if (!FillObjectFromRepresentation(orderData, object, objectId, errorMessage)){
		errorMessage = QString("Unable to create order from representatiom. Error: '%1'").arg(errorMessage);
		return false;
	}

	return true;
}


void COrderCollectionControllerComp::SetObjectFilter(
	const imtgql::CGqlRequest& gqlRequest,
	const imtbase::CTreeItemModel& objectFilterModel,
	iprm::CParamsSet& filterParams) const
{
	BaseClass::SetObjectFilter(gqlRequest, objectFilterModel, filterParams);

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
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


// private methods

bool COrderCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::prolife::Orders::COrderData::V1_0& orderDataRepresentation,
			istd::IChangeable& object,
			QByteArray& objectId,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Error: Attribute 'm_objectCollectionCompPtr' was not set", "COrderCollectionControllerComp");
		return false;
	}

	if (!m_deviceCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Error: Attribute 'm_deviceCollectionCompPtr' was not set", "COrderCollectionControllerComp");
		return false;
	}

	if (!m_softwareInstanceCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Error: Attribute 'm_softwareInstanceCollectionCompPtr' was not set", "COrderCollectionControllerComp");
		return false;
	}

	prolifedata::CIdentifiableOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(&const_cast<istd::IChangeable&>(object));
	if (orderInfoPtr == nullptr){
		errorMessage = QString("Object is invalid");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return false;
	}

	if (orderDataRepresentation.Id){
		objectId = *orderDataRepresentation.Id;
	}

	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray orderUuid = objectId;
	orderInfoPtr->SetObjectUuid(orderUuid);

	QString orderId;
	if (orderDataRepresentation.OrderId){
		orderId = *orderDataRepresentation.OrderId;
	}

	if (orderId.isEmpty()){
		errorMessage = QString("Delivery-ID cannot be empty");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return false;
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
				prolifedata::CIdentifiableOrderInfo* objectPrderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(dataPtr.GetPtr());
				if (objectPrderInfoPtr != nullptr){
					QByteArray currentOrderId = objectPrderInfoPtr->GetOrderId().toLower();
					if (currentOrderId == orderId.toLower()){
						errorMessage = QString("Delivery-ID already exists");
						SendErrorMessage(0, errorMessage, "COrderControllerComp");

						return false;
					}
				}
			}
		}
	}

	orderInfoPtr->SetOrderId(orderId.toUtf8());

	QByteArray customerId;
	if (orderDataRepresentation.CustomerId){
		customerId = *orderDataRepresentation.CustomerId;
		orderInfoPtr->SetCustomerId(customerId);
	}

	if (customerId.isEmpty()){
		errorMessage = QString("Customer can not be empty!");
		SendErrorMessage(0, errorMessage, "COrderControllerComp");

		return false;
	}

	if (orderDataRepresentation.PurchaseId){
		QByteArray purchaseOrderId = *orderDataRepresentation.PurchaseId;
		orderInfoPtr->SetPurchaseOrderId(purchaseOrderId);
	}

	if (orderDataRepresentation.Description){
		QString description = *orderDataRepresentation.Description;
		orderInfoPtr->SetDescription(description);
	}

	if (orderDataRepresentation.OrderStatus){
		QByteArray status = *orderDataRepresentation.OrderStatus;
		orderInfoPtr->SetOrderStatus(prolifedata::GetOrderStatusFromId(status));
	}

	imtbase::IObjectCollection* productCollectionPtr = orderInfoPtr->GetProducts();
	if (productCollectionPtr == nullptr){
		Q_ASSERT(false);
		return false;
	}

	QList<sdl::prolife::Orders::COrderedProduct::V1_0> products;
	if (orderDataRepresentation.OrderProducts){
		products = *orderDataRepresentation.OrderProducts;
	}

	bool ok = CheckProducts(orderUuid, products, errorMessage);
	if (!ok){
		return false;
	}

	for (const sdl::prolife::Orders::COrderedProduct::V1_0& product : products){
		QByteArray orderProductUuid = *product.Id;
		QByteArray categoryId = *product.CategoryId;
		QByteArray productUuid = *product.ProductUuid;
		QByteArray serialNumber = *product.SerialNumber;
		QByteArray licenseUuid = *product.LicenseUuid;
		bool isNew = *product.IsNew;

		istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
		objectLinkPtr.SetPtr(new imtbase::CObjectLink());

		objectLinkPtr->SetObjectUuid(orderUuid);

		if (categoryId == "Software"){
			istd::TDelPtr<prolifedata::COrderedIdentifiableSoftwareInstanceInfo> softwareInstancePtr;
			softwareInstancePtr.SetPtr(new prolifedata::COrderedIdentifiableSoftwareInstanceInfo);

			softwareInstancePtr->SetObjectUuid(orderProductUuid);
			softwareInstancePtr->SetupProductInstance(productUuid, "", "");
			softwareInstancePtr->SetSerialNumber(serialNumber);

			QString expiration = *product.Expiration;
			QDateTime expirationDate = QDateTime::fromString(expiration, "yyyy-MM-dd");
			softwareInstancePtr->AddLicense(licenseUuid, expirationDate);

			if (isNew){
				istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
				if (m_softwareOperationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Create", orderProductUuid, *softwareInstancePtr.GetPtr());
				}

				QByteArray result = m_softwareInstanceCollectionCompPtr->InsertNewObject(QByteArray("Software"), "", "", softwareInstancePtr.PopPtr(), orderProductUuid, nullptr, nullptr, operationContextPtr.GetPtr());
				if (result.isEmpty()){
					errorMessage = QString("Unable to insert new software product with ID: '%1'").arg(orderProductUuid);
					return false;
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
								return false;
							}

							if (!isInUse && !productInfoPtr->IsEqual(*softwareInstancePtr)){
								istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
								if (m_softwareOperationContextControllerCompPtr.IsValid()){
									operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Update", orderProductUuid, *softwareInstancePtr.GetPtr());
								}

								if (!m_softwareInstanceCollectionCompPtr->SetObjectData(orderProductUuid, *softwareInstancePtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
									errorMessage = QString("Unable to update a software product with ID: '%1'").arg(orderProductUuid);

									return false;
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
			deviceInstancePtr->SetMacAddress(*product.MacAddress);
			deviceInstancePtr->SetSerialNumber(serialNumber);

			if (isNew){
				istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
				if (m_deviceOperationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext("Create", orderProductUuid, *deviceInstancePtr.GetPtr());
				}

				m_deviceCollectionCompPtr->InsertNewObject("DocumentInfo", "", "", deviceInstancePtr.GetPtr(), orderProductUuid, nullptr, nullptr, operationContextPtr.GetPtr());
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

							istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
							if (m_deviceOperationContextControllerCompPtr.IsValid()){
								operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext("Update", orderProductUuid, *deviceInstancePtr.GetPtr());
							}

							m_deviceCollectionCompPtr->SetObjectData(orderProductUuid, *deviceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr());
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
				return false;
			}

			for (const QByteArray& id : removedProducts){
				imtbase::ICollectionInfo::Id typeId = oldProductCollectionPtr->GetObjectTypeId(id);

				imtbase::IObjectCollection::DataPtr productDataPtr;
				if (typeId == QByteArray("HardwareInfo")){
					if (m_deviceCollectionCompPtr->GetObjectData(id, productDataPtr)){
						prolifedata::COrderedIdentifiableDeviceInfo* hardwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(productDataPtr.GetPtr());
						if (hardwareInfoPtr != nullptr){
							hardwareInfoPtr->SetOrderId("");

							istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
							if (m_deviceOperationContextControllerCompPtr.IsValid()){
								operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext("Update", id, *hardwareInfoPtr);
							}

							if (!m_deviceCollectionCompPtr->SetObjectData(id, *hardwareInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
								return false;
							}
						}
					}
				}
				else if (typeId == QByteArray("SoftwareInfo")){
					if (m_softwareInstanceCollectionCompPtr->GetObjectData(id, productDataPtr)){
						prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(productDataPtr.GetPtr());
						if (softwareInfoPtr != nullptr){
							softwareInfoPtr->SetOrderId("");

							istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
							if (m_softwareOperationContextControllerCompPtr.IsValid()){
								operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Update", id, *softwareInfoPtr);
							}

							if (!m_softwareInstanceCollectionCompPtr->SetObjectData(id, *softwareInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
								return false;
							}
						}
					}
				}
			}
		}
	}

	return true;
}


} // namespace prolifegql


