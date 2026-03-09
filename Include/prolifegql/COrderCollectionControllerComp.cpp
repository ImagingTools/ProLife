// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#include <prolifegql/COrderCollectionControllerComp.h>


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
#include <prolifedata/CGroupFilter.h>


namespace prolifegql
{


// protected methods

bool COrderCollectionControllerComp::CheckProducts(
	const QByteArray& orderUuid,
	const imtsdl::TElementList<sdl::prolife::Orders::COrderedProduct::V1_0>& products,
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

	for (const istd::TSharedNullable<sdl::prolife::Orders::COrderedProduct::V1_0>& product : products){
		QByteArray objectUuid;
		if (product->id){
			objectUuid = *product->id;
		}

		QByteArray productUuid;
		if (product->productUuid){
			productUuid = *product->productUuid;
		}

		QByteArray categoryId;
		if (product->categoryId){
			categoryId = *product->categoryId;
		}

		QString productName = GetProductName(productUuid);

		bool isNew = false;
		if (product->isNew){
			isNew = *product->isNew;
		}

		if (categoryId == "Software"){
			QByteArray serialNumber;
			if (product->serialNumber){
				serialNumber = *product->serialNumber;
			}

			if (isNew){
				bool ok = prolifedata::CheckSoftwareSerialNumberExists(objectUuid, serialNumber, *m_softwareInstanceCollectionCompPtr);
				if (!ok){
					errorMessage = QString(QT_TR_NOOP("It is not possible to save the product '%1' because serial number '%2' already exists")).arg(productName, qPrintable(serialNumber));
					return false;
				}
			}

			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_softwareInstanceCollectionCompPtr->GetObjectData(objectUuid, dataPtr)){
				prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
				if (softwareInfoPtr != nullptr){
					QByteArray currentOrderId = softwareInfoPtr->GetOrderId();
					if (!currentOrderId.isEmpty() && orderUuid != currentOrderId){
						QByteArray softwareSerialNumber = softwareInfoPtr->GetSerialNumber();
						errorMessage = QString("It is not possible to add a product that is linked to another order. Software product '%1' with ID '%2'").arg(productName, qPrintable(softwareSerialNumber));
						return false;
					}
				}
			}
		}
		else if (categoryId == "Hardware"){
			if (product->macAddress && isNew){
				QByteArray macAddress = *product->macAddress;
				if (!macAddress.isEmpty()){
					bool ok = prolifedata::CheckDeviceMacAddressExists(objectUuid, macAddress, *m_deviceCollectionCompPtr);
					if (!ok){
						errorMessage = QString(QT_TR_NOOP("It is not possible to save the product '%1' because MAC address '%2' already exists")).arg(productName, qPrintable(macAddress));
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
						errorMessage = QString("It is not possible to save a product that is linked to another order. Hardware product '%1' with ID '%2'").arg(productName, qPrintable(hardwareInfoPtr->GetMacAddress()));
						return false;
					}
				}
			}

			if (product->serialNumber && isNew){
				QByteArray serialNumber = *product->serialNumber;
				if (!serialNumber.isEmpty()){
					bool serialNumberIsValid = prolifedata::CheckDeviceSerialNumberExists(objectUuid, serialNumber, *m_deviceCollectionCompPtr);
					if (!serialNumberIsValid){
						errorMessage = QString("It is not possible to save the product '%1' because Serial Number: '%2' already exists").arg(productName, qPrintable(serialNumber));
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


bool COrderCollectionControllerComp::OnBeforeRemoveElements(
			const QByteArrayList& elementIds,
			const imtgql::CGqlRequest& /*gqlRequest*/,
			QString& /*errorMessage*/) const
{
	for (const QByteArray& objectId : elementIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			prolifedata::CIdentifiableOrderInfo* oldOrderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(dataPtr.GetPtr());
			if (oldOrderInfoPtr != nullptr){
				imtbase::IObjectCollection* productCollectionPtr = oldOrderInfoPtr->GetProducts();
				if (productCollectionPtr != nullptr){
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
				}
			}
		}
	}

	return true;
}


void COrderCollectionControllerComp::OnAfterSetObjectDescription(
			const QByteArray& objectId,
			const QString& description,
			const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT(false);
		return;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		prolifedata::IOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(dataPtr.GetPtr());
		if (orderInfoPtr != nullptr){
			orderInfoPtr->SetDescription(description);

			if (!m_objectCollectionCompPtr->SetObjectData(objectId, *orderInfoPtr)){
				SendWarningMessage(0, QString("Unable to set description for object '%1'. Error: Set object data failed").arg(QString::fromUtf8(objectId)));
			}
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
		representationObject.id = (objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		representationObject.typeId = (collectionObjectId);
	}

	if (requestInfo.items.isNameRequested){
		representationObject.name = (orderInfoPtr->GetOrderId());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.description = (orderInfoPtr->GetDescription());
	}

	if (requestInfo.items.isStatusRequested){
		representationObject.status = (prolifedata::GetNameFromOrderStatus(orderInfoPtr->GetOrderStatus()));
	}

	if (requestInfo.items.isOrderIdRequested){
		representationObject.orderId = (orderInfoPtr->GetOrderId());
	}

	if (requestInfo.items.isCustomerNameRequested){
		representationObject.customerName = metaInfo->GetMetaInfo(prolifedata::IOrderInfo::MIT_CUSTOMER_NAME).toString();
	}

	if (requestInfo.items.isCustomerLinkRequested){
		sdl::imtbase::ImtBaseTypes::CObjectLink::V1_0 objectLink;
		objectLink.id = metaInfo->GetMetaInfo(prolifedata::IOrderInfo::MIT_CUSTOMER_ID).toString().toUtf8();
		objectLink.typeId = QByteArrayLiteral("Account");
		objectLink.name = metaInfo->GetMetaInfo(prolifedata::IOrderInfo::MIT_CUSTOMER_NAME).toString();

		sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0 urlParam;
		urlParam.scheme = "applink";
		urlParam.path = QStringLiteral("Accounts/Account");
		if (!(*objectLink.id).isEmpty()){
			urlParam.path = *urlParam.path + QStringLiteral("/") + *objectLink.id;
		}
		objectLink.url = urlParam;

		representationObject.customerLink = objectLink;
	}

	if (requestInfo.items.isPurchaseIdRequested){
		representationObject.purchaseId = (orderInfoPtr->GetPurchaseOrderId());
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime().toUTC();

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.added = (added);
	}

	if (requestInfo.items.isTimeStampRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("Timestamp").toDateTime().toUTC();

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.timeStamp = (lastModified);
	}

	return true;
}


istd::IChangeableUniquePtr COrderCollectionControllerComp::CreateObjectFromRepresentation(
	const sdl::prolife::Orders::COrderData::V1_0& orderDataRepresentation,
	QByteArray& newObjectId,
	QString& errorMessage) const
{
	if (!m_orderInfoFactCompPtr.IsValid()){
		Q_ASSERT_X(false, "Error: Attribute 'm_orderInfoFactCompPtr' was not set", "COrderCollectionControllerComp");
		return nullptr;
	}

	istd::TUniqueInterfacePtr<prolifedata::IOrderInfo> orderInfoPtr;
	orderInfoPtr = m_orderInfoFactCompPtr.CreateInstance();
	if (!orderInfoPtr.IsValid()){
		errorMessage = QString("Unable to cast order instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return nullptr;
	}

	if (!FillObjectFromRepresentation(orderDataRepresentation, *orderInfoPtr, newObjectId, errorMessage)){
		errorMessage = QString("Unable to create order. Error: '%1'").arg(errorMessage);
		return nullptr;
	}

	imtsdl::TElementList<sdl::prolife::Orders::COrderedProduct::V1_0> products;
	if (orderDataRepresentation.orderProducts){
		products = *orderDataRepresentation.orderProducts;
	}

	for (const istd::TSharedNullable<sdl::prolife::Orders::COrderedProduct::V1_0>& product : products){
		QByteArray categoryId;
		if (product->categoryId){
			categoryId = *product->categoryId;
		}

		QByteArray productId;
		if (product->id){
			productId = *product->id;
		}

		bool isNew = false;
		if (product->isNew){
			isNew = *product->isNew;
		}

		if (categoryId == QByteArray("Hardware")){
			if (isNew){
				CreateNewHardware(*product, newObjectId);
			}
			else{
				UpdateOrderForHardware(productId, newObjectId);
			}
		}
		else if (categoryId == QByteArray("Software")){
			if (isNew){
				CreateNewSoftware(*product, newObjectId);
			}
			else{
				UpdateOrderForSoftware(productId, newObjectId);
			}
		}
	}

	istd::IChangeableUniquePtr retVal;
	retVal.MoveCastedPtr<prolifedata::IOrderInfo>(orderInfoPtr);

	return retVal;
}


bool COrderCollectionControllerComp::CreateRepresentationFromObject(
	const istd::IChangeable& data,
	const sdl::prolife::Orders::COrderItemGqlRequest& orderItemRequest,
	sdl::prolife::Orders::COrderData::V1_0& representationPayload,
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

	QByteArray id;
	if (arguments.input.Version_1_0->id){
		id = *arguments.input.Version_1_0->id;
	}
	representationPayload.id = (id);

	QByteArray orderId = orderInfoPtr->GetOrderId();
	representationPayload.name = (orderId);
	representationPayload.orderId = (orderId);

	QByteArray purchaseOrderId = orderInfoPtr->GetPurchaseOrderId();
	representationPayload.purchaseId = (purchaseOrderId);

	QByteArray customerId = orderInfoPtr->GetCustomerId();
	representationPayload.customerId = (customerId);

	QString description = orderInfoPtr->GetDescription();
	representationPayload.description = (description);

	prolifedata::IOrderInfo::OrderStatus status = orderInfoPtr->GetOrderStatus();
	QByteArray orderStatus = prolifedata::GetIdFromOrderStatus(status);
	representationPayload.orderStatus = (orderStatus);

	imtbase::IObjectCollection* productCollectionPtr = orderInfoPtr->GetProducts();
	if (productCollectionPtr == nullptr){
		return false;
	}

	imtsdl::TElementList<sdl::prolife::Orders::COrderedProduct::V1_0> products;

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

					softwareProduct.id = (productId);
					softwareProduct.categoryId = (softwareProductPtr->GetFactoryId());
					softwareProduct.productUuid = (softwareProductPtr->GetProductId());
					softwareProduct.serialNumber = (softwareProductPtr->GetSerialNumber());
					softwareProduct.inUse = softwareProductPtr->IsInUse();
					softwareProduct.isNew = false;
					softwareProduct.isMultiple = softwareProductPtr->IsMultiProduct();
					softwareProduct.productCount = softwareProductPtr->GetProductCount();

					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
						const imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
						if (productInfoPtr != nullptr){
							softwareProduct.productName = (productInfoPtr->GetName());
						}
					}

					const imtbase::ICollectionInfo& licenseInstances = softwareProductPtr->GetLicenseInstances();
					imtbase::ICollectionInfo::Ids activeLicenseIds = licenseInstances.GetElementIds();
					if (!activeLicenseIds.isEmpty()){
						QByteArray activeLicenseId = activeLicenseIds[0];
						const imtlic::ILicenseInstance* licenseInstancePtr = softwareProductPtr->GetLicenseInstance(activeLicenseId);
						if (licenseInstancePtr != nullptr){
							softwareProduct.licenseUuid = (activeLicenseId);

							imtbase::IObjectCollection::DataPtr licenseDataPtr;
							if (m_licenseDefinitionCollectionCompPtr->GetObjectData(activeLicenseId, licenseDataPtr)){
								const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
								if (licenseInfoPtr != nullptr){
									softwareProduct.licenseName = (licenseInfoPtr->GetLicenseName());
									softwareProduct.licenseUuid = (licenseInfoPtr->GetLicenseId());
								}
							}

							QDate date = licenseInstancePtr->GetExpiration().date();
							QString licenseExpiration = date.toString("yyyy-MM-dd");
							softwareProduct.expiration = (licenseExpiration);
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

					hardwareProduct.id = (productId);
					hardwareProduct.productUuid = (productUuid);
					hardwareProduct.categoryId = (QByteArray("Hardware"));
					hardwareProduct.licenseUuid = (licenseDefinitionUuid);
					hardwareProduct.isNew = std::move(false);

					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
						const imtlic::IProductInfo* productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
						if (productInfoPtr != nullptr){
							hardwareProduct.productName = (productInfoPtr->GetName());
						}
					}

					imtbase::IObjectCollection::DataPtr licenseDataPtr;
					if (m_licenseDefinitionCollectionCompPtr->GetObjectData(licenseDefinitionUuid, licenseDataPtr)){
						const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
						if (licenseInfoPtr != nullptr){
							hardwareProduct.licenseName = (licenseInfoPtr->GetLicenseName());
							hardwareProduct.licenseId = (licenseInfoPtr->GetLicenseId());
						}
					}

					imtbase::IObjectCollection::DataPtr deviceDataPtr;
					if (m_deviceCollectionCompPtr->GetObjectData(productId, deviceDataPtr)){
						const prolifedata::IDeviceInfo* deviceInfoInstancePtr = dynamic_cast<prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
						if (deviceInfoInstancePtr != nullptr){
							hardwareProduct.macAddress = (deviceInfoInstancePtr->GetMacAddress());
							hardwareProduct.serialNumber = (deviceInfoInstancePtr->GetSerialNumber());
						}
					}

					imtbase::IObjectCollection::DataPtr bindingDataPtr;
					if (m_bindingCollectionCompPtr->GetObjectData(productId, bindingDataPtr)){
						const prolifedata::IHardwareProductBinding* bindingInfoPtr = dynamic_cast<const prolifedata::IHardwareProductBinding*>(bindingDataPtr.GetPtr());
						if (bindingInfoPtr != nullptr){
							hardwareProduct.inUse = std::move(false);

							QByteArrayList softwareIds = bindingInfoPtr->GetSoftwareIds();
							for (const QByteArray& softwareId : softwareIds){
								imtbase::IObjectCollection::DataPtr softwareDataPtr;
								if (m_softwareInstanceCollectionCompPtr->GetObjectData(softwareId, softwareDataPtr)){
									const imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
									if (productInstanceInfoPtr != nullptr){
										bool isUse = productInstanceInfoPtr->IsInUse();
										if (isUse){
											hardwareProduct.inUse = std::move(true);
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

	representationPayload.orderProducts = products;

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
	
	if (!inputArguments.input.Version_1_0->item){
		I_CRITICAL();
		return false;
	}

	sdl::prolife::Orders::COrderData::V1_0 orderData = *inputArguments.input.Version_1_0->item;
	QByteArray objectId;
	if (inputArguments.input.Version_1_0->id){
		objectId = *inputArguments.input.Version_1_0->id;
	}

	prolifedata::CIdentifiableOrderInfo *orderInfoPtr =
		dynamic_cast<prolifedata::CIdentifiableOrderInfo *>(
		&const_cast<istd::IChangeable &>(object));
	if (orderInfoPtr == nullptr){
		errorMessage = QString("Object is invalid");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return false;
	}

	orderInfoPtr->ResetData();
	
	orderInfoPtr->SetObjectUuid(objectId);

	if (!FillObjectFromRepresentation(orderData, object, objectId, errorMessage)){
		errorMessage = QString("Unable to update order. Error: '%1'").arg(errorMessage);
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

	if (orderData.orderProducts){
		for (const istd::TSharedNullable<sdl::prolife::Orders::COrderedProduct::V1_0>& product : *orderData.orderProducts){
			bool isNew = false;
			if (product->isNew){
				isNew = *product->isNew;
			}

			QByteArray productId;
			if (product->id){
				productId = *product->id;
			}

			QByteArray categoryId;
			if (product->categoryId){
				categoryId = *product->categoryId;
			}

			if (categoryId == QByteArray("Hardware")){
				if (isNew){
					CreateNewHardware(*product, objectId);
				}
				else if (addedProducts.contains(productId)){
					UpdateOrderForHardware(productId, objectId);
				}
			}
			else if (categoryId == QByteArray("Software")){
				if (isNew){
					CreateNewSoftware(*product, objectId);
				}
				else if (addedProducts.contains(productId)){
					UpdateOrderForSoftware(productId, objectId);
				}
			}
		}
	}

	return true;
}


void COrderCollectionControllerComp::SetAdditionalFilters(
	const imtgql::CGqlRequest& gqlRequest,
	const imtgql::CGqlParamObject& /*viewParamsGql*/,
	iprm::CParamsSet* filterParams) const
{
	if (m_groupFilterParamJoinerCompPtr.IsValid()){
		if (!m_groupFilterParamJoinerCompPtr->JoinGroupFilterParam(gqlRequest, *filterParams)){
			SendWarningMessage(0, QString("Unable to join group filter param"), "COrderCollectionControllerComp");
		}
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

	if (orderDataRepresentation.id){
		objectId = *orderDataRepresentation.id;
	}

	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray orderUuid = objectId;
	orderInfoPtr->SetObjectUuid(orderUuid);

	QString orderId;
	if (orderDataRepresentation.orderId){
		orderId = *orderDataRepresentation.orderId;
	}

	if (orderId.isEmpty()){
		errorMessage = QString("Delivery-ID cannot be empty");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return false;
	}
	
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = "OrderId";
	fieldFilter.filterValue = orderId;
	
	imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
	groupFilter.fieldFilters << fieldFilter;
	
	imtbase::CComplexCollectionFilter complexFilter;
	complexFilter.SetFilterExpression(groupFilter);
	
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
						errorMessage = QString("Delivery-ID '%1' already exists").arg(orderId);
						SendErrorMessage(0, errorMessage, "COrderControllerComp");

						return false;
					}
				}
			}
		}
	}

	orderInfoPtr->SetOrderId(orderId.toUtf8());

	QByteArray customerId;
	if (orderDataRepresentation.customerId){
		customerId = *orderDataRepresentation.customerId;
		orderInfoPtr->SetCustomerId(customerId);
	}

	if (customerId.isEmpty()){
		errorMessage = QString("Customer cannot be empty");
		SendErrorMessage(0, errorMessage, "COrderControllerComp");

		return false;
	}

	if (orderDataRepresentation.purchaseId){
		QByteArray purchaseOrderId = *orderDataRepresentation.purchaseId;
		orderInfoPtr->SetPurchaseOrderId(purchaseOrderId);
	}

	if (orderDataRepresentation.description){
		QString description = *orderDataRepresentation.description;
		orderInfoPtr->SetDescription(description);
	}

	if (orderDataRepresentation.orderStatus){
		QByteArray status = *orderDataRepresentation.orderStatus;
		orderInfoPtr->SetOrderStatus(prolifedata::GetOrderStatusFromId(status));
	}

	imtbase::IObjectCollection* productCollectionPtr = orderInfoPtr->GetProducts();
	if (productCollectionPtr == nullptr){
		Q_ASSERT(false);
		return false;
	}

	imtsdl::TElementList<sdl::prolife::Orders::COrderedProduct::V1_0> products;
	if (orderDataRepresentation.orderProducts){
		products = *orderDataRepresentation.orderProducts;
	}

	bool ok = CheckProducts(orderUuid, products, errorMessage);
	if (!ok){
		return false;
	}

	for (const istd::TSharedNullable<sdl::prolife::Orders::COrderedProduct::V1_0>& product : products){
		istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
		objectLinkPtr.SetPtr(new imtbase::CObjectLink());

		QByteArray orderProductUuid;
		if (product->id){
			orderProductUuid = *product->id;
		}

		QByteArray categoryId;
		if (product->categoryId){
			categoryId = *product->categoryId;
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
		operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext("Update", deviceId, hardwareInfoPtr);
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
		operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Update", softwareId, softwareInfoPtr);
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
		if (product.isNew.has_value() && *product.isNew){
			QByteArray categoryId;
			if (product.categoryId){
				categoryId = *product.categoryId;
			}

			QByteArray orderProductUuid;
			if (product.id){
				orderProductUuid = *product.id;
			}

			QByteArray productUuid;
			if (product.productUuid){
				productUuid = *product.productUuid;
			}

			QByteArray serialNumber;
			if (product.serialNumber){
				serialNumber = *product.serialNumber;
			}

			QByteArray licenseUuid;
			if (product.licenseUuid){
				licenseUuid = *product.licenseUuid;
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
					operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext("Create", orderProductUuid, deviceInstancePtr.GetPtr());
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
				if (product.expiration){
					expiration = *product.expiration;
				}

				QDateTime expirationDate = QDateTime::fromString(expiration, "yyyy-MM-dd");
				softwareInstancePtr->AddLicense(licenseUuid, expirationDate);

				istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
				if (m_softwareOperationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Create", orderProductUuid, softwareInstancePtr.GetPtr());
				}

				QByteArray result = m_softwareInstanceCollectionCompPtr->InsertNewObject(QByteArray("SoftwareProduct"), "", "", softwareInstancePtr.PopPtr(), orderProductUuid, nullptr, nullptr, operationContextPtr.GetPtr());
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
	if (product.id){
		orderProductUuid = *product.id;
	}

	QByteArray productUuid;
	if (product.productUuid){
		productUuid = *product.productUuid;
	}

	QByteArray serialNumber;
	if (product.serialNumber){
		serialNumber = *product.serialNumber;
	}

	QByteArray macAddress;
	if (product.macAddress){
		macAddress = *product.macAddress;
	}

	QByteArray licenseUuid;
	if (product.licenseUuid){
		licenseUuid = *product.licenseUuid;
	}

	istd::TDelPtr<prolifedata::COrderedIdentifiableDeviceInfo> deviceInstancePtr;
	deviceInstancePtr.SetPtr(new prolifedata::COrderedIdentifiableDeviceInfo);

	deviceInstancePtr->SetObjectUuid(orderProductUuid);
	deviceInstancePtr->SetOrderId(orderId);
	deviceInstancePtr->SetDeviceType(productUuid);
	deviceInstancePtr->SetConfigurationType(licenseUuid);
	deviceInstancePtr->SetSerialNumber(serialNumber);
	deviceInstancePtr->SetMacAddress(macAddress);

	if (!macAddress.isEmpty() && !serialNumber.isEmpty()){
		deviceInstancePtr->SetDeviceProductionStatus(prolifedata::IDeviceInfo::DPS_FINISHED);
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_deviceOperationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext("Create", orderProductUuid, deviceInstancePtr.GetPtr());
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
	if (product.id){
		orderProductUuid = *product.id;
	}

	QByteArray productUuid;
	if (product.productUuid){
		productUuid = *product.productUuid;
	}

	QByteArray serialNumber;
	if (product.serialNumber){
		serialNumber = *product.serialNumber;
	}

	QByteArray licenseUuid;
	if (product.licenseUuid){
		licenseUuid = *product.licenseUuid;
	}

	bool isMultiple = false;
	if (product.isMultiple){
		isMultiple = *product.isMultiple;
	}

	int productCount = 1;
	if (product.productCount){
		productCount = *product.productCount;
	}

	istd::TDelPtr<prolifedata::COrderedIdentifiableSoftwareInstanceInfo> softwareInstancePtr;
	softwareInstancePtr.SetPtr(new prolifedata::COrderedIdentifiableSoftwareInstanceInfo);

	softwareInstancePtr->SetObjectUuid(orderProductUuid);
	softwareInstancePtr->SetupProductInstance(productUuid, "", "");
	softwareInstancePtr->SetSerialNumber(serialNumber);
	softwareInstancePtr->SetOrderId(orderId);
	softwareInstancePtr->SetMultiProduct(isMultiple);
	softwareInstancePtr->SetProductCount(productCount);

	QString expiration;
	if (product.expiration){
		expiration = *product.expiration;
	}

	QDateTime expirationDate = QDateTime::fromString(expiration, "yyyy-MM-dd");
	softwareInstancePtr->AddLicense(licenseUuid, expirationDate);

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_softwareOperationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Create", orderProductUuid, softwareInstancePtr.GetPtr());
	}

	QByteArray result = m_softwareInstanceCollectionCompPtr->InsertNewObject(QByteArray("SoftwareProduct"), "", "", softwareInstancePtr.PopPtr(), orderProductUuid, nullptr, nullptr, operationContextPtr.GetPtr());
	if (result.isEmpty()){
		return false;
	}

	return true;
}


} // namespace prolifegql

			
			
