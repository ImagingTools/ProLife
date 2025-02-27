#include <prolifegql/COrderCollectionControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
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
			QByteArray serialNumber;
			if (product.SerialNumber){
				serialNumber = *product.SerialNumber;
			}

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
			if (product.MacAddress){
				QByteArray macAddress = *product.MacAddress;
				if (!macAddress.isEmpty()){
					bool ok = prolifedata::CheckDeviceMacAddressExists(objectUuid, macAddress, *m_deviceCollectionCompPtr);
					if (!ok){
						errorMessage = QString(QT_TR_NOOP("It is not possible to save the product '%1' because MAC address '%2' already exists")).arg(productName).arg(qPrintable(macAddress));
						return false;
					}
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

			if (product.SerialNumber){
				QByteArray serialNumber = *product.SerialNumber;
				if (!serialNumber.isEmpty()){
					bool serialNumberIsValid = prolifedata::CheckDeviceSerialNumberExists(objectUuid, serialNumber, *m_deviceCollectionCompPtr);
					if (!serialNumberIsValid){
						errorMessage = QString("It is not possible to save the product '%1' because Serial Number: '%2' already exists").arg(productName).arg(serialNumber);
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


// reimplemented (sdl::prolife::Orders::COrderCollectionControllerCompBase)

bool COrderCollectionControllerComp::CreateRepresentationFromObject(
	const imtbase::IObjectCollectionIterator& objectCollectionIterator,
	const sdl::prolife::Orders::COrdersListGqlRequest& ordersListRequest,
	sdl::prolife::Orders::COrderItem::V1_0& representationObject,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Unable to create representation from object. Error: Attribute 'm_objectCollectionCompPtr' was not set", "COrderCollectionControllerComp");
		return false;
	}

	sdl::prolife::Orders::OrdersListRequestInfo requestInfo = ordersListRequest.GetRequestInfo();

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
		representationObject.Id = (objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		representationObject.TypeId = (collectionObjectId);
	}

	if (requestInfo.items.isNameRequested){
		representationObject.Name = (orderInfoPtr->GetOrderId());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.Description = (orderInfoPtr->GetDescription());
	}

	if (requestInfo.items.isStatusRequested){
		representationObject.Status = (prolifedata::GetNameFromOrderStatus(orderInfoPtr->GetOrderStatus()));
	}

	if (requestInfo.items.isOrderIdRequested){
		representationObject.OrderId = (orderInfoPtr->GetOrderId());
	}

	if (requestInfo.items.isCustomerNameRequested){
		if (metaInfo.IsValid()){
			representationObject.CustomerName = metaInfo->GetMetaInfo(prolifedata::IOrderInfo::MIT_CUSTOMER_NAME).toString();
		}
	}

	if (requestInfo.items.isPurchaseIdRequested){
		representationObject.PurchaseId = (orderInfoPtr->GetPurchaseOrderId());
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime().toUTC();

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.Added = (added);
	}

	if (requestInfo.items.isTimeStampRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("Timestamp").toDateTime().toUTC();

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.TimeStamp = (lastModified);
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

	QList<sdl::prolife::Orders::COrderedProduct::V1_0> products;
	if (orderDataRepresentation.OrderProducts){
		products = *orderDataRepresentation.OrderProducts;
	}

	for (const sdl::prolife::Orders::COrderedProduct::V1_0& product : products){
		QByteArray categoryId;
		if (product.CategoryId){
			categoryId = *product.CategoryId;
		}

		QByteArray productId;
		if (product.Id){
			productId = *product.Id;
		}

		bool isNew = false;
		if (product.IsNew){
			isNew = *product.IsNew;
		}

		if (categoryId == QByteArray("Hardware")){
			if (isNew){
				CreateNewHardware(product, newObjectId);
			}
			else{
				UpdateOrderForHardware(productId, newObjectId);
			}
		}
		else if (categoryId == QByteArray("Software")){
			if (isNew){
				CreateNewSoftware(product, newObjectId);
			}
			else{
				UpdateOrderForSoftware(productId, newObjectId);
			}
		}
	}

	return orderInfoPtr.PopPtr();
}


bool COrderCollectionControllerComp::CreateRepresentationFromObject(
	const istd::IChangeable& data,
	const sdl::prolife::Orders::COrderItemGqlRequest& orderItemRequest,
	sdl::prolife::Orders::COrderDataPayload::V1_0& representationPayload,
	QString& errorMessage) const
{
	prolifedata::CIdentifiableOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(&const_cast<istd::IChangeable&>(data));
	if (orderInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return false;
	}

	sdl::prolife::Orders::OrderItemRequestArguments arguments = orderItemRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		I_CRITICAL();

		return false;
	}

	sdl::prolife::Orders::COrderData::V1_0 orderData;

	QByteArray id;
	if (arguments.input.Version_1_0->Id){
		id = *arguments.input.Version_1_0->Id;
	}
	orderData.Id = (id);

	QByteArray orderId = orderInfoPtr->GetOrderId();
	orderData.Name = (orderId);
	orderData.OrderId = (orderId);

	QByteArray purchaseOrderId = orderInfoPtr->GetPurchaseOrderId();
	orderData.PurchaseId = (purchaseOrderId);

	QByteArray customerId = orderInfoPtr->GetCustomerId();
	orderData.CustomerId = (customerId);

	QString description = orderInfoPtr->GetDescription();
	orderData.Description = (description);

	prolifedata::IOrderInfo::OrderStatus status = orderInfoPtr->GetOrderStatus();
	QByteArray orderStatus = prolifedata::GetIdFromOrderStatus(status);
	orderData.OrderStatus = (orderStatus);

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

					softwareProduct.Id = (productId);
					softwareProduct.CategoryId = (softwareProductPtr->GetFactoryId());
					softwareProduct.ProductUuid = (softwareProductPtr->GetProductId());
					softwareProduct.SerialNumber = (softwareProductPtr->GetSerialNumber());
					softwareProduct.InUse = std::make_optional<bool>(softwareProductPtr->IsInUse());
					softwareProduct.IsNew = std::make_optional<bool>(false);

					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
						const imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
						if (productInfoPtr != nullptr){
							softwareProduct.ProductName = (productInfoPtr->GetName());
						}
					}

					const imtbase::ICollectionInfo& licenseInstances = softwareProductPtr->GetLicenseInstances();
					imtbase::ICollectionInfo::Ids activeLicenseIds = licenseInstances.GetElementIds();
					if (!activeLicenseIds.isEmpty()){
						QByteArray activeLicenseId = activeLicenseIds[0];
						const imtlic::ILicenseInstance* licenseInstancePtr = softwareProductPtr->GetLicenseInstance(activeLicenseId);
						if (licenseInstancePtr != nullptr){
							softwareProduct.LicenseUuid = (activeLicenseId);

							imtbase::IObjectCollection::DataPtr licenseDataPtr;
							if (m_licenseDefinitionCollectionCompPtr->GetObjectData(activeLicenseId, licenseDataPtr)){
								const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
								if (licenseInfoPtr != nullptr){
									softwareProduct.LicenseName = (licenseInfoPtr->GetLicenseName());
									softwareProduct.LicenseUuid = (licenseInfoPtr->GetLicenseId());
								}
							}

							QDate date = licenseInstancePtr->GetExpiration().date();
							QString licenseExpiration = date.toString("yyyy-MM-dd");
							softwareProduct.Expiration = (licenseExpiration);
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

					hardwareProduct.Id = (productId);
					hardwareProduct.ProductUuid = (productUuid);
					hardwareProduct.CategoryId = (QByteArray("Hardware"));
					hardwareProduct.LicenseUuid = (licenseDefinitionUuid);
					hardwareProduct.IsNew = std::make_optional<bool>(false);

					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
						const imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
						if (productInfoPtr != nullptr){
							hardwareProduct.ProductName = (productInfoPtr->GetName());
						}
					}

					imtbase::IObjectCollection::DataPtr licenseDataPtr;
					if (m_licenseDefinitionCollectionCompPtr->GetObjectData(licenseDefinitionUuid, licenseDataPtr)){
						const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
						if (licenseInfoPtr != nullptr){
							hardwareProduct.LicenseName = (licenseInfoPtr->GetLicenseName());
							hardwareProduct.LicenseId = (licenseInfoPtr->GetLicenseId());
						}
					}

					imtbase::IObjectCollection::DataPtr deviceDataPtr;
					if (m_deviceCollectionCompPtr->GetObjectData(productId, deviceDataPtr)){
						const prolifedata::IDeviceInfo* deviceInfoInstancePtr = dynamic_cast<prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
						if (deviceInfoInstancePtr != nullptr){
							hardwareProduct.MacAddress = (deviceInfoInstancePtr->GetMacAddress());
							hardwareProduct.SerialNumber = (deviceInfoInstancePtr->GetSerialNumber());
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
	const sdl::prolife::Orders::COrderUpdateGqlRequest& orderUpdateRequest,
	istd::IChangeable& object,
	QString& errorMessage) const
{
	sdl::prolife::Orders::OrderUpdateRequestArguments inputArguments = orderUpdateRequest.GetRequestedArguments();
	if (!inputArguments.input.Version_1_0){
		I_CRITICAL();

		return false;
	}

	sdl::prolife::Orders::COrderData::V1_0 orderData = *inputArguments.input.Version_1_0->Item;
	QByteArray objectId = *orderUpdateRequest.GetRequestedArguments().input.Version_1_0->Id;

	prolifedata::CIdentifiableOrderInfo *orderInfoPtr =
		dynamic_cast<prolifedata::CIdentifiableOrderInfo *>(
		&const_cast<istd::IChangeable &>(object));
	if (orderInfoPtr == nullptr) {
		errorMessage = QString("Object is invalid");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return false;
	}

	orderInfoPtr->ResetData();

	if (!FillObjectFromRepresentation(orderData, object, objectId, errorMessage)){
		if (errorMessage.isEmpty()){
			errorMessage = QString("Unable to create order from representatiom");
		}

		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return false;
	}

	prolifedata::CIdentifiableOrderInfo* oldOrderInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		oldOrderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(dataPtr.GetPtr());
	}

	if (oldOrderInfoPtr == nullptr){
		Q_ASSERT(false);
		return false;
	}

	QByteArrayList addedProducts;
	QByteArrayList removedProducts;
	QByteArrayList updatedProducts;

	GenerateDifferences(*oldOrderInfoPtr, *orderInfoPtr, addedProducts, removedProducts, updatedProducts);

	imtbase::IObjectCollection* productCollectionPtr = oldOrderInfoPtr->GetProducts();
	if (productCollectionPtr == nullptr){
		Q_ASSERT(false);
		return false;
	}

	for (const QByteArray& id : removedProducts){
		imtbase::ICollectionInfo::Id typeId = productCollectionPtr->GetObjectTypeId(id);
		if (typeId == QByteArray("HardwareInfo")){
			UpdateOrderForHardware(id, "");
		}
		else if (typeId == QByteArray("SoftwareInfo")){
			UpdateOrderForSoftware(id, "");
		}
	}

	if (orderData.OrderProducts){
		for (const sdl::prolife::Orders::COrderedProduct::V1_0& product : *orderData.OrderProducts){
			bool isNew = false;
			if (product.IsNew){
				isNew = *product.IsNew;
			}

			QByteArray productId;
			if (product.Id){
				productId = *product.Id;
			}

			QByteArray categoryId;
			if (product.CategoryId){
				categoryId = *product.CategoryId;
			}

			if (categoryId == QByteArray("Hardware")){
				if (isNew){
					CreateNewHardware(product, objectId);
				}
				else if (addedProducts.contains(productId)){
					UpdateOrderForHardware(productId, objectId);
				}
			}
			else if (categoryId == QByteArray("Software")){
				if (isNew){
					CreateNewSoftware(product, objectId);
				}
				else if (addedProducts.contains(productId)){
					UpdateOrderForSoftware(productId, objectId);
				}
			}
		}
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


imtbase::CTreeItemModel* COrderCollectionControllerComp::DeleteObject(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	const imtgql::CGqlObject& inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		errorMessage = QString("Unable to delete an order with empty ID");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return nullptr;
	}

	prolifedata::CIdentifiableOrderInfo* oldOrderInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		oldOrderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(dataPtr.GetPtr());
	}

	if (oldOrderInfoPtr == nullptr){
		errorMessage = QString("Unable to delete an order with ID '%1'. Order does not exists");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return nullptr;
	}

	imtbase::IObjectCollection* productCollectionPtr = oldOrderInfoPtr->GetProducts();
	if (productCollectionPtr == nullptr){
		Q_ASSERT(false);
		return nullptr;
	}

	imtbase::ICollectionInfo::Ids elementIds = productCollectionPtr->GetElementIds();

	for (const imtbase::ICollectionInfo::Id& id : elementIds){
		imtbase::ICollectionInfo::Id typeId = productCollectionPtr->GetObjectTypeId(id);
		if (typeId == QByteArray("HardwareInfo")){
			if (!UpdateOrderForHardware(id, "")){
				SendWarningMessage(0,
								   QString("Unable to remove order '%1' from device '%2'")
										  .arg(qPrintable(objectId), qPrintable(objectId)),
								   "COrderCollectionControllerComp");
			}
		}
		else if (typeId == QByteArray("SoftwareInfo")){
			if (!UpdateOrderForSoftware(id, "")){
				SendWarningMessage(0,
								   QString("Unable to remove order '%1' from software '%2'")
									.arg(qPrintable(objectId), qPrintable(objectId)),
								   "COrderCollectionControllerComp");
			}
		}
	}

	return BaseClass::DeleteObject(gqlRequest, errorMessage);
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
	
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "OrderId";
	fieldFilter.filterValue = orderId;
	
	imtbase::IComplexCollectionFilter::GroupFilter groupFilter;
	groupFilter.fieldFilters << fieldFilter;
	
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFieldsFilter(groupFilter);
	
	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ComplexFilter", &complexFilter);

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
		istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
		objectLinkPtr.SetPtr(new imtbase::CObjectLink());

		QByteArray orderProductUuid;
		if (product.Id){
			orderProductUuid = *product.Id;
		}

		QByteArray categoryId;
		if (product.CategoryId){
			categoryId = *product.CategoryId;
		}

		objectLinkPtr->SetObjectUuid(orderProductUuid);

		if (categoryId == "Software"){
			objectLinkPtr->SetFactoryId("SoftwareInfo");
		}
		else if (categoryId == "Hardware"){
			objectLinkPtr->SetFactoryId("HardwareInfo");
		}

		QByteArray result = productCollectionPtr->InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), orderProductUuid);
		if (result.isEmpty()){
			errorMessage = QString("Product '%1' cannot be inserted to collection for order '%2'").arg(qPrintable(orderProductUuid), qPrintable(orderUuid));
			return false;
		}
	}

	return true;
}


bool COrderCollectionControllerComp::UpdateOrderForHardware(const QByteArray& deviceId, const QByteArray& orderId) const
{
	imtbase::IObjectCollection::DataPtr productDataPtr;
	prolifedata::COrderedIdentifiableDeviceInfo* hardwareInfoPtr = nullptr;
	if (m_deviceCollectionCompPtr->GetObjectData(deviceId, productDataPtr)){
		hardwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(productDataPtr.GetPtr());
	}

	if (hardwareInfoPtr == nullptr){
		return false;
	}

	hardwareInfoPtr->SetOrderId(orderId);

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_deviceOperationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext("Update", deviceId, *hardwareInfoPtr);
	}

	if (!m_deviceCollectionCompPtr->SetObjectData(deviceId, *hardwareInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
		return false;
	}

	return true;
}


bool COrderCollectionControllerComp::UpdateOrderForSoftware(const QByteArray& softwareId, const QByteArray& orderId) const
{
	imtbase::IObjectCollection::DataPtr productDataPtr;

	prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = nullptr;
	if (m_softwareInstanceCollectionCompPtr->GetObjectData(softwareId, productDataPtr)){
		softwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(productDataPtr.GetPtr());
	}

	if (softwareInfoPtr == nullptr){
		return false;
	}

	softwareInfoPtr->SetOrderId(orderId);

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_softwareOperationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Update", softwareId, *softwareInfoPtr);
	}

	if (!m_softwareInstanceCollectionCompPtr->SetObjectData(softwareId, *softwareInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
		return false;
	}

	return true;
}


bool COrderCollectionControllerComp::CheckNewProducts(
	QList<sdl::prolife::Orders::COrderedProduct::V1_0> orderProducts,
	const QByteArray& orderId) const
{
	for (const sdl::prolife::Orders::COrderedProduct::V1_0& product : orderProducts){
		if (product.IsNew.has_value() && *product.IsNew){
			QByteArray categoryId;
			if (product.CategoryId){
				categoryId = *product.CategoryId;
			}

			QByteArray orderProductUuid;
			if (product.Id){
				orderProductUuid = *product.Id;
			}

			QByteArray productUuid;
			if (product.ProductUuid){
				productUuid = *product.ProductUuid;
			}

			QByteArray serialNumber;
			if (product.SerialNumber){
				serialNumber = *product.SerialNumber;
			}

			QByteArray licenseUuid;
			if (product.LicenseUuid){
				licenseUuid = *product.LicenseUuid;
			}

			if (categoryId == "Hardware"){
				istd::TDelPtr<prolifedata::COrderedIdentifiableDeviceInfo> deviceInstancePtr;
				deviceInstancePtr.SetPtr(new prolifedata::COrderedIdentifiableDeviceInfo);

				deviceInstancePtr->SetObjectUuid(orderProductUuid);
				deviceInstancePtr->SetOrderId(orderId);
				deviceInstancePtr->SetDeviceType(productUuid);
				deviceInstancePtr->SetConfigurationType(licenseUuid);
				deviceInstancePtr->SetSerialNumber(serialNumber);

				istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
				if (m_deviceOperationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext("Create", orderProductUuid, *deviceInstancePtr.GetPtr());
				}

				QByteArray result = m_deviceCollectionCompPtr->InsertNewObject("Device", "", "", deviceInstancePtr.GetPtr(), orderProductUuid, nullptr, nullptr, operationContextPtr.GetPtr());
				if (result.isEmpty()){
					return false;
				}
			}
			else if (categoryId == "Software"){
				istd::TDelPtr<prolifedata::COrderedIdentifiableSoftwareInstanceInfo> softwareInstancePtr;
				softwareInstancePtr.SetPtr(new prolifedata::COrderedIdentifiableSoftwareInstanceInfo);

				softwareInstancePtr->SetObjectUuid(orderProductUuid);
				softwareInstancePtr->SetupProductInstance(productUuid, "", "");
				softwareInstancePtr->SetSerialNumber(serialNumber);
				softwareInstancePtr->SetOrderId(orderId);

				QString expiration;
				if (product.Expiration){
					expiration = *product.Expiration;
				}

				QDateTime expirationDate = QDateTime::fromString(expiration, "yyyy-MM-dd");
				softwareInstancePtr->AddLicense(licenseUuid, expirationDate);

				istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
				if (m_softwareOperationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Create", orderProductUuid, *softwareInstancePtr.GetPtr());
				}

				QByteArray result = m_softwareInstanceCollectionCompPtr->InsertNewObject(QByteArray("Software"), "", "", softwareInstancePtr.PopPtr(), orderProductUuid, nullptr, nullptr, operationContextPtr.GetPtr());
				if (result.isEmpty()){
					return false;
				}
			}
		}
	}

	return true;
}


bool COrderCollectionControllerComp::CreateNewHardware(
	const sdl::prolife::Orders::COrderedProduct::V1_0& product,
	const QByteArray& orderId) const
{
	QByteArray orderProductUuid;
	if (product.Id){
		orderProductUuid = *product.Id;
	}

	QByteArray productUuid;
	if (product.ProductUuid){
		productUuid = *product.ProductUuid;
	}

	QByteArray serialNumber;
	if (product.SerialNumber){
		serialNumber = *product.SerialNumber;
	}

	QByteArray macAddress;
	if (product.MacAddress){
		macAddress = *product.MacAddress;
	}

	QByteArray licenseUuid;
	if (product.LicenseUuid){
		licenseUuid = *product.LicenseUuid;
	}

	istd::TDelPtr<prolifedata::COrderedIdentifiableDeviceInfo> deviceInstancePtr;
	deviceInstancePtr.SetPtr(new prolifedata::COrderedIdentifiableDeviceInfo);

	deviceInstancePtr->SetObjectUuid(orderProductUuid);
	deviceInstancePtr->SetOrderId(orderId);
	deviceInstancePtr->SetDeviceType(productUuid);
	deviceInstancePtr->SetConfigurationType(licenseUuid);
	deviceInstancePtr->SetSerialNumber(serialNumber);
	deviceInstancePtr->SetMacAddress(macAddress);

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_deviceOperationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext("Create", orderProductUuid, *deviceInstancePtr.GetPtr());
	}

	QByteArray result = m_deviceCollectionCompPtr->InsertNewObject("Device", "", "", deviceInstancePtr.GetPtr(), orderProductUuid, nullptr, nullptr, operationContextPtr.GetPtr());
	if (result.isEmpty()){
		return false;
	}

	return true;
}


bool COrderCollectionControllerComp::CreateNewSoftware(
	const sdl::prolife::Orders::COrderedProduct::V1_0& product,
	const QByteArray& orderId) const
{
	QByteArray orderProductUuid;
	if (product.Id){
		orderProductUuid = *product.Id;
	}

	QByteArray productUuid;
	if (product.ProductUuid){
		productUuid = *product.ProductUuid;
	}

	QByteArray serialNumber;
	if (product.SerialNumber){
		serialNumber = *product.SerialNumber;
	}

	QByteArray licenseUuid;
	if (product.LicenseUuid){
		licenseUuid = *product.LicenseUuid;
	}

	istd::TDelPtr<prolifedata::COrderedIdentifiableSoftwareInstanceInfo> softwareInstancePtr;
	softwareInstancePtr.SetPtr(new prolifedata::COrderedIdentifiableSoftwareInstanceInfo);

	softwareInstancePtr->SetObjectUuid(orderProductUuid);
	softwareInstancePtr->SetupProductInstance(productUuid, "", "");
	softwareInstancePtr->SetSerialNumber(serialNumber);
	softwareInstancePtr->SetOrderId(orderId);

	QString expiration;
	if (product.Expiration){
		expiration = *product.Expiration;
	}

	QDateTime expirationDate = QDateTime::fromString(expiration, "yyyy-MM-dd");
	softwareInstancePtr->AddLicense(licenseUuid, expirationDate);

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_softwareOperationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Create", orderProductUuid, *softwareInstancePtr.GetPtr());
	}

	QByteArray result = m_softwareInstanceCollectionCompPtr->InsertNewObject(QByteArray("Software"), "", "", softwareInstancePtr.PopPtr(), orderProductUuid, nullptr, nullptr, operationContextPtr.GetPtr());
	if (result.isEmpty()){
		return false;
	}

	return true;
}


} // namespace prolifegql


