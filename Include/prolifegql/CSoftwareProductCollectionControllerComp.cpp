#include <prolifegql/CSoftwareProductCollectionControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtbase/CObjectLink.h>
#include <imtlic/CHardwareInstanceInfo.h>
#include <imtgql/imtgql.h>
#include <imtlic/IProductInfo.h>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/COrderInfo.h>
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>
#include <prolifedata/CHardwareProductBinding.h>


namespace prolifegql
{


// protected methods

sdl::imtbase::ImtCollection::CVisualStatus CSoftwareProductCollectionControllerComp::OnGetObjectVisualStatus(
	const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CSoftwareProductCollectionControllerComp");
		return sdl::imtbase::ImtCollection::CVisualStatus();
	}

	sdl::imtbase::ImtCollection::CVisualStatus retVal = BaseClass::OnGetObjectVisualStatus(getObjectVisualStatusRequest, gqlRequest, errorMessage);
	if (!retVal.Version_1_0){
		I_CRITICAL();

		return retVal;
	}
	sdl::imtbase::ImtCollection::CVisualStatus::V1_0& response = *retVal.Version_1_0;

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(*response.ObjectId, dataPtr)){
		prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
		if (softwareInfoPtr != nullptr){
			QByteArray productId = softwareInfoPtr->GetProductId();
			QByteArray serialNumber = softwareInfoPtr->GetSerialNumber();

			QString name = serialNumber;

			if (m_productCollectionCompPtr.IsValid()){
				imtbase::IObjectCollection::DataPtr productDataPtr;
				if (m_productCollectionCompPtr->GetObjectData(productId, productDataPtr)){
					imtlic::IProductInfo* remoteProductInfoPtr = dynamic_cast<imtlic::IProductInfo*>(productDataPtr.GetPtr());
					if (remoteProductInfoPtr != nullptr){
						name = remoteProductInfoPtr->GetName();
					}
				}
			}

			if (!serialNumber.isEmpty()){
				name += " (" + serialNumber + ")";
			}

			response.Text = name;
		}
	}

	return retVal;
}


// reimplemented (sdl::prolife::Licenses::CSoftwareProductCollectionControllerCompBase)

bool CSoftwareProductCollectionControllerComp::CreateRepresentationFromObject(
	const imtbase::IObjectCollectionIterator& objectCollectionIterator,
	const sdl::prolife::Licenses::CSoftwareProductsListGqlRequest& softwareProductsListRequest,
	sdl::prolife::Licenses::CSoftwareProductItem::V1_0& representationObject,
	QString& errorMessage) const
{
	sdl::prolife::Licenses::SoftwareProductsListRequestInfo requestInfo = softwareProductsListRequest.GetRequestInfo();

	QByteArray objectId = objectCollectionIterator.GetObjectId();

	prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		softwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
	}

	if (softwareInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return false;
	}

	idoc::MetaInfoPtr metaInfo = objectCollectionIterator.GetDataMetaInfo();
	if (!metaInfo.IsValid()){
		errorMessage = QString("Unable to create representation from object '%1'. Error: Meta info is invalid").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");
		
		return false;
	}
	
	QString hardwareId = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_HARDWARE_ID).toString();

	if (requestInfo.items.isIdRequested){
		representationObject.Id = (objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		representationObject.TypeId = (collectionObjectId);
	}

	if (requestInfo.items.isNameRequested){
		QString productName = objectCollectionIterator.GetElementInfo("ProductName").toString();
		representationObject.Name = (productName);

		QByteArray serialNumber = objectCollectionIterator.GetElementInfo("SerialNumber").toByteArray();
		if (!serialNumber.isEmpty()){
			representationObject.Name = (productName + " (" + serialNumber + ")");
		}
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.Description = (metaInfo->GetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION).toString());
	}

	if (requestInfo.items.isDeliveryIdRequested){
		representationObject.DeliveryId = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_DELIVERY_ID).toString();
	}

	if (requestInfo.items.isPurchaseIdRequested){
		representationObject.PurchaseId = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PURCHASE_ID).toString();
	}

	if (requestInfo.items.isOrderUuidRequested){
		representationObject.OrderUuid = softwareInfoPtr->GetOrderId();
	}

	if (requestInfo.items.isMacAddressRequested){
		representationObject.MacAddress = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_HARDWARE_MAC_ADDRESS).toString();
	}

	if (requestInfo.items.isProductIdRequested){
		representationObject.ProductId = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_ID).toString();
	}

	if (requestInfo.items.isProductNameRequested) {
		representationObject.ProductName = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME).toString();
	}

	if (requestInfo.items.isProductUuidRequested) {
		representationObject.ProductUuid = (
			objectCollectionIterator.GetElementInfo("ProductUuid").toString()
			);
	}

	if (requestInfo.items.isSerialNumberRequested) {
		representationObject.SerialNumber = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_SERIAL_NUMBER).toString();
	}

	if (requestInfo.items.isExpirationRequested) {
		representationObject.Expiration = (
			objectCollectionIterator.GetElementInfo("Expiration").toString()
			);
	}

	if (requestInfo.items.isIsPairedRequested) {
		representationObject.IsPaired = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_IS_PAIRED).toBool();
	}

	if (requestInfo.items.isInUseRequested) {
		representationObject.InUse = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_IN_USE).toBool();
	}

	if (requestInfo.items.isHardwareIdRequested) {
		representationObject.HardwareId = hardwareId;
	}

	if (requestInfo.items.isLicenseNameRequested) {
		representationObject.LicenseName = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_NAME).toString();
	}

	if (requestInfo.items.isLicenseIdRequested) {
		representationObject.LicenseId = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_ID).toString();
	}

	if (requestInfo.items.isLicenseUuidRequested) {
		representationObject.LicenseUuid = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_UUID).toString();
	}

	if (requestInfo.items.isCustomerIdRequested) {
		representationObject.CustomerId = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_CUSTOMER_ID).toString();
	}

	if (requestInfo.items.isCustomerNameRequested) {
		representationObject.CustomerName = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_CUSTOMER_NAME).toString();
	}

	if (requestInfo.items.isProjectRequested) {
		representationObject.Project = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PROJECT).toString();
	}

	if (requestInfo.items.isStatusRequested){
		QByteArray hardwareMacAddress = hardwareId.toUtf8();
		bool isPaired = !hardwareMacAddress.isEmpty();
		if (isPaired){
			representationObject.Status = ("IsPaired");
		}
		else{
			representationObject.Status = ("NotPaired");
		}

		if (isPaired){
			bool isUse = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_IN_USE).toBool();
			if (isUse){
				representationObject.Status = ("InUse");
			}
		}
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


istd::IChangeable* CSoftwareProductCollectionControllerComp::CreateObjectFromRepresentation(
	const sdl::prolife::Licenses::CSoftwareProductData::V1_0& softwareProductDataRepresentation,
	QByteArray& newObjectId,
	QString& errorMessage) const
{
	if (!m_softwareInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_softwareInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtlic::IProductInstanceInfo> softwareInstancePtr = m_softwareInfoFactCompPtr.CreateInstance();
	if (!softwareInstancePtr.IsValid()){
		errorMessage = QString("Unable to create software instance. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return nullptr;
	}

	prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(softwareInstancePtr.GetPtr());
	if (softwareInfoPtr == nullptr){
		errorMessage = QString("Unable to cast software instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return nullptr;
	}

	if (softwareProductDataRepresentation.Id){
		newObjectId = *softwareProductDataRepresentation.Id;
	}

	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	softwareInfoPtr->SetObjectUuid(newObjectId);

	if (!FillObjectFromRepresentation(softwareProductDataRepresentation, *softwareInfoPtr, newObjectId, errorMessage)){
		errorMessage = QString("Unable to create software from representation. Error: '%1'").arg(errorMessage);

		return nullptr;
	}

	if (softwareProductDataRepresentation.OrderUuid){
		QString orderId = *softwareProductDataRepresentation.OrderUuid;
		if (!orderId.isEmpty()){
			if (!AddSoftwareToOrder(newObjectId, orderId.toUtf8())){
				errorMessage = QString("Unable to add software. Error: Add software to order failed");
				return nullptr;
			}
		}
	}

	return softwareInstancePtr.PopPtr();
}


bool CSoftwareProductCollectionControllerComp::CreateRepresentationFromObject(
	const istd::IChangeable& data,
	const sdl::prolife::Licenses::CSoftwareProductItemGqlRequest& softwareProductItemRequest,
	sdl::prolife::Licenses::CSoftwareProductDataPayload::V1_0& representationPayload,
	QString& errorMessage) const
{
	const prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(&data);
	if (softwareInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return false;
	}

	sdl::prolife::Licenses::SoftwareProductItemRequestArguments arguments = softwareProductItemRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		I_CRITICAL();

		return false;
	}

	sdl::prolife::Licenses::CSoftwareProductData::V1_0 softwareProductData;

	QByteArray id;
	if (arguments.input.Version_1_0->Id){
		id = *arguments.input.Version_1_0->Id;
	}

	softwareProductData.Id = (id);

	QByteArray productId = softwareInfoPtr->GetProductId();
	softwareProductData.ProductId = (productId);

	QByteArray factoryId = softwareInfoPtr->GetFactoryId();
	softwareProductData.CategoryId = (factoryId);

	QByteArray serialNumber = softwareInfoPtr->GetSerialNumber();
	softwareProductData.SerialNumber = (serialNumber);

	QByteArray project = softwareInfoPtr->GetProject();
	softwareProductData.Project = (project);

	bool inUse = softwareInfoPtr->IsInUse();
	softwareProductData.InUse = (inUse);

	QByteArray orderId = softwareInfoPtr->GetOrderId();
	softwareProductData.OrderUuid = (orderId);

	imtbase::ICollectionInfo::Ids licenseIds = softwareInfoPtr->GetLicenseInstances().GetElementIds();
	if (!licenseIds.isEmpty()){
		QByteArray licenseId = licenseIds[0];

		const imtlic::ILicenseInstance* licenseInstancePtr = softwareInfoPtr->GetLicenseInstance(licenseId);
		if (licenseInstancePtr != nullptr){
			softwareProductData.LicenseUuid = (licenseInstancePtr->GetLicenseId());
			softwareProductData.Expiration = (licenseInstancePtr->GetExpiration().toString("dd.MM.yyyy"));
		}
	}

	representationPayload.SoftwareProductData = std::make_optional<sdl::prolife::Licenses::CSoftwareProductData::V1_0>(softwareProductData);

	return true;
}


imtbase::CTreeItemModel* CSoftwareProductCollectionControllerComp::DeleteObject(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CSoftwareProductCollectionControllerComp");
		return nullptr;
	}

	const imtgql::CGqlObject& inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		errorMessage = QString("No object-ID could not be extracted from the request");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return nullptr;
	}

	const prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productInstanceInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		productInstanceInfoPtr =
			dynamic_cast<const prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
	}

	if (productInstanceInfoPtr == nullptr){
		errorMessage = QString("Unable to remove software '%1'. Error: Software does not exists");
		return nullptr;
	}

	bool isUse = productInstanceInfoPtr->IsInUse();
	if (isUse){
		errorMessage = QT_TR_NOOP("It is not possible to delete this sensor because a license file has been created for it. Contact your system administrator.");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		errorMessage = imtgql::GetTranslation(m_translationManagerCompPtr.GetPtr(), gqlRequest, errorMessage.toUtf8(), "prolifegql::CSoftwareProductCollectionControllerComp");

		return nullptr;
	}

	imtbase::IObjectCollection::Ids elementIds = m_bindingCollectionCompPtr->GetElementIds();
	for (const imtbase::IObjectCollection::Id& elementId: elementIds){
		imtbase::IObjectCollection::DataPtr bindingDataPtr;
		if (m_bindingCollectionCompPtr->GetObjectData(elementId, bindingDataPtr)){
			prolifedata::CHardwareProductBinding* deviceBindingInfoPtr = dynamic_cast<prolifedata::CHardwareProductBinding*>(bindingDataPtr.GetPtr());
			if (deviceBindingInfoPtr != nullptr){
				QByteArrayList softwareIds = deviceBindingInfoPtr->GetSoftwareIds();
				if (softwareIds.contains(objectId)){
					deviceBindingInfoPtr->Unbind(objectId);

					if (!m_bindingCollectionCompPtr->SetObjectData(elementId, *deviceBindingInfoPtr)){
						SendWarningMessage(0, QString("Unable to update hardware binding object after software removing"));
					}

					break;
				}
			}
		}
	}

	QByteArray orderId = productInstanceInfoPtr->GetOrderId();
	if (!orderId.isEmpty()){
		if (!RemoveSoftwareFromOrder(objectId, orderId)){
			SendWarningMessage(0,
							   QString("Remove software '%1' from order '%2' failed")
								   .arg(qPrintable(objectId), qPrintable(orderId)),
							   "CDeviceCollectionControllerComp");
		}
	}

	return BaseClass::DeleteObject(gqlRequest, errorMessage);
}


// private methods

bool CSoftwareProductCollectionControllerComp::FillObjectFromRepresentation(
	const sdl::prolife::Licenses::CSoftwareProductData::V1_0& representation,
	istd::IChangeable& object,
	QByteArray& objectId,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CSoftwareProductCollectionControllerComp");
		return false;
	}

	if (!m_orderCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'OrderCollection' was not set", "CSoftwareProductCollectionControllerComp");
		return false;
	}

	prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(&object);
	if (softwareInfoPtr == nullptr){
		errorMessage = QString("Object is invalid");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return false;
	}

	QByteArray serialNumber;
	if (representation.SerialNumber){
		serialNumber = *representation.SerialNumber;
	}

	if (serialNumber.isEmpty()){
		errorMessage = QString("Serial Number cannot be empty");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return false;
	}

	if (!serialNumber.isEmpty()){
		bool ok = prolifedata::CheckSoftwareSerialNumberExists(objectId, serialNumber, *m_objectCollectionCompPtr);
		if (!ok){
			errorMessage = QString("Serial Number already exists");
			SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

			return false;
		}
	}

	softwareInfoPtr->SetSerialNumber(serialNumber);

	QByteArray productId;
	if (representation.ProductId){
		productId = *representation.ProductId;
	}

	if (productId.isEmpty()){
		errorMessage = QString("Product cannot be empty!");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return false;
	}

	bool ok = false;
	if (m_productCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr productDataPtr;
		if (m_productCollectionCompPtr->GetObjectData(productId, productDataPtr)){
			imtlic::IProductInfo* remoteProductInfoPtr = dynamic_cast<imtlic::IProductInfo*>(productDataPtr.GetPtr());
			if (remoteProductInfoPtr != nullptr){
				if (remoteProductInfoPtr->GetCategoryId() == "Software"){
					ok = true;
				}
			}
		}
	}

	if (!ok){
		errorMessage = QString("Product category should be 'Software'");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return false;
	}

	if (representation.Project){
		QString project = *representation.Project;
		softwareInfoPtr->SetProject(project.toUtf8());
	}

	QByteArray orderUuid;
	if (representation.OrderUuid){
		orderUuid = *representation.OrderUuid;
		softwareInfoPtr->SetOrderId(orderUuid);
	}

	QByteArray customerUuid;
	imtbase::IObjectCollection::DataPtr orderDataPtr;
	if (m_orderCollectionCompPtr->GetObjectData(orderUuid, orderDataPtr)){
		prolifedata::IOrderInfo* productOrderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(orderDataPtr.GetPtr());
		if (productOrderInfoPtr != nullptr){
			customerUuid = productOrderInfoPtr->GetCustomerId();

			istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
			objectLinkPtr.SetPtr(new imtbase::CObjectLink());

			objectLinkPtr->SetObjectUuid(objectId);
			objectLinkPtr->SetFactoryId("SoftwareInfo");

			imtbase::IObjectCollection* productCollectionPtr = productOrderInfoPtr->GetProducts();
			if (productCollectionPtr != nullptr){
				productCollectionPtr->InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), objectId);
			}
		}
	}

	softwareInfoPtr->SetupProductInstance(productId, "", customerUuid);

	QByteArray licenseUuid;
	if (representation.LicenseUuid){
		licenseUuid = *representation.LicenseUuid;
	}

	if (licenseUuid.isEmpty()){
		errorMessage = QString("License cannot be empty");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return false;
	}

	QString expiration = *representation.Expiration;

	softwareInfoPtr->AddLicense(licenseUuid, QDateTime::fromString(expiration, "dd.MM.yyyy"));

	return true;
}


bool CSoftwareProductCollectionControllerComp::RemoveSoftwareFromOrder(const QByteArray& softwareId, const QByteArray& orderId) const
{
	if (!m_orderCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'OrderCollection' was not set", "CSoftwareProductCollectionControllerComp");
		return false;
	}

	prolifedata::IOrderInfo* oldOrderInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr oldOrderDataPtr;
	if (m_orderCollectionCompPtr->GetObjectData(orderId, oldOrderDataPtr)){
		oldOrderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(oldOrderDataPtr.GetPtr());
	}

	if (oldOrderInfoPtr == nullptr){
		SendErrorMessage(0,
						 QString("Unable to remove software '%1' from order '%2'. Error: Order does not exists")
							 .arg(qPrintable(softwareId), qPrintable(orderId)),
						 "CSoftwareProductCollectionControllerComp");
		return false;
	}

	imtbase::IObjectCollection* productCollectionPtr = oldOrderInfoPtr->GetProducts();
	if (productCollectionPtr == nullptr){
		SendErrorMessage(0,
						 QString("Unable to remove software '%1' from order '%2'. Error: Product collection from order is invalid")
							 .arg(qPrintable(softwareId), qPrintable(orderId)),
						 "CSoftwareProductCollectionControllerComp");
		return false;
	}

	QByteArrayList elementIds = productCollectionPtr->GetElementIds();
	if (!elementIds.contains(softwareId)){
		SendErrorMessage(0,
						 QString("Unable to remove software '%1' from order '%2'. Error: The software does not exist in this order")
							 .arg(qPrintable(softwareId), qPrintable(orderId)),
						 "CSoftwareProductCollectionControllerComp");
		return false;
	}

	if (!productCollectionPtr->RemoveElement(softwareId)){
		SendErrorMessage(0,
						 QString("Unable to remove software '%1' from order '%2'. Error: Removing element from product collection failed")
							 .arg(qPrintable(softwareId), qPrintable(orderId)),
						 "CSoftwareProductCollectionControllerComp");
		return false;
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_orderOperationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext("Update", orderId, *oldOrderInfoPtr);
	}

	if (!m_orderCollectionCompPtr->SetObjectData(orderId, *oldOrderInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
		SendErrorMessage(0,
						 QString("Unable to remove software '%1' from order '%2'. Error: Updating an order in a collection failed")
							 .arg(qPrintable(softwareId), qPrintable(orderId)),
						 "CSoftwareProductCollectionControllerComp");
		return false;
	}

	return true;
}


bool CSoftwareProductCollectionControllerComp::AddSoftwareToOrder(const QByteArray& softwareId, const QByteArray& orderId) const
{
	if (!m_orderCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'OrderCollection' was not set", "CSoftwareProductCollectionControllerComp");
		return false;
	}

	prolifedata::IOrderInfo* orderInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr orderDataPtr;
	if (m_orderCollectionCompPtr->GetObjectData(orderId, orderDataPtr)){
		orderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(orderDataPtr.GetPtr());
	}

	if (orderInfoPtr == nullptr){
		SendErrorMessage(0,
						 QString("Unable to add software '%1' to order '%2'. Error: Order does not exists")
							 .arg(qPrintable(softwareId), qPrintable(orderId)),
						 "CSoftwareProductCollectionControllerComp");
		return false;
	}

	imtbase::IObjectCollection* productCollectionPtr = orderInfoPtr->GetProducts();
	if (orderInfoPtr == nullptr){
		SendErrorMessage(0,
						 QString("Unable to add software '%1' to order '%2'. Error: Product collection from order is invalid")
							 .arg(qPrintable(softwareId), qPrintable(orderId)),
						 "CSoftwareProductCollectionControllerComp");
		return false;
	}

	QByteArrayList elementIds = productCollectionPtr->GetElementIds();
	if (elementIds.contains(softwareId)){
		SendErrorMessage(0,
						 QString("Unable to add software '%1' to order '%2'. Error: The software already exists in this order")
							 .arg(qPrintable(softwareId), qPrintable(orderId)),
						 "CSoftwareProductCollectionControllerComp");
		return false;
	}

	istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
	objectLinkPtr.SetPtr(new imtbase::CObjectLink());

	objectLinkPtr->SetObjectUuid(softwareId);
	objectLinkPtr->SetFactoryId("SoftwareInfo");

	QByteArray objectId = productCollectionPtr->InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), softwareId);
	if (objectId.isEmpty()){
		SendErrorMessage(0,
						 QString("Unable to add software '%1' to order '%2'. Error: Adding an order in a collection failed")
							 .arg(qPrintable(softwareId), qPrintable(orderId)),
						 "CSoftwareProductCollectionControllerComp");
		return false;
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_orderOperationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext("Update", orderId, *orderInfoPtr);
	}

	if (!m_orderCollectionCompPtr->SetObjectData(orderId, *orderInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
		SendErrorMessage(0,
						 QString("Unable to remove software '%1' from order '%2'. Error: Updating an order in a collection failed")
							 .arg(qPrintable(softwareId), qPrintable(orderId)),
						 "CSoftwareProductCollectionControllerComp");
		return false;
	}

	return true;
}


void CSoftwareProductCollectionControllerComp::SetObjectFilter(
	const imtgql::CGqlRequest& gqlRequest,
	const imtbase::CTreeItemModel& objectFilterModel,
	iprm::CParamsSet& filterParams) const
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		SendErrorMessage(0, QString("Unable to create an object filter. GraphQL context is nullptr."), "CSoftwareProductCollectionControllerComp");

		return;
	}

	bool filterByGroup = true;

	QByteArray userId;
	QByteArrayList userGroupIds;
	imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr != nullptr){
		userGroupIds = userInfoPtr->GetGroups();
		userId = userInfoPtr->GetId();

		if (userInfoPtr->IsAdmin()){
			filterByGroup = false;
		}
		else{
			if (m_checkPermissionCompPtr.IsValid()){
				QByteArrayList userPermissions = userInfoPtr->GetPermissions();

				QByteArrayList permissions;
				permissions << *m_permissionIdAttrPtr;
				filterByGroup = !m_checkPermissionCompPtr->CheckPermission(userPermissions, permissions);
			}
		}
	}

	if (filterByGroup){
		iprm::CTextParam* userParamPtr = new iprm::CTextParam();
		userParamPtr->SetText(userId);

		iprm::CTextParam* groupParamPtr = new iprm::CTextParam();
		QByteArray groups;
		if (!userGroupIds.isEmpty()){
			groups = userGroupIds.join(';');
		}
		groupParamPtr->SetText(groups);

		iprm::CParamsSet* paramsSetPtr = new iprm::CParamsSet();

		paramsSetPtr->SetEditableParameter("UserParam", userParamPtr, true);
		paramsSetPtr->SetEditableParameter("GroupParam", groupParamPtr, true);

		filterParams.SetEditableParameter("Groups", paramsSetPtr, true);
	}

	if (objectFilterModel.ContainsKey("BindingFilter")){
		imtbase::CTreeItemModel* bindingFilterPtr = objectFilterModel.GetTreeItemModel("BindingFilter");
		if (bindingFilterPtr != nullptr){
			iprm::CParamsSet* paramsSetPtr = new iprm::CParamsSet();
			QStringList keys = bindingFilterPtr->GetKeys(0);

			for (const QString& key : keys){
				QByteArray value = bindingFilterPtr->GetData(key.toUtf8()).toByteArray();

				iprm::CIdParam* textParamPtr = new iprm::CIdParam();
				textParamPtr->SetId(value);

				paramsSetPtr->SetEditableParameter(key.toUtf8(), textParamPtr, true);
			}

			filterParams.SetEditableParameter("BindingFilter", paramsSetPtr, true);
		}
	}

	if (objectFilterModel.ContainsKey("LicenseFilter")){
		QString licenseFilter = objectFilterModel.GetData("LicenseFilter").toString();
		if (!licenseFilter.isEmpty()){
			iprm::CIdParam* textParamPtr = new iprm::CIdParam();
			textParamPtr->SetId(licenseFilter.toUtf8());

			filterParams.SetEditableParameter("LicenseFilter", textParamPtr, true);
		}
	}

	if (objectFilterModel.ContainsKey("CustomerUuid")){
		QString filter = objectFilterModel.GetData("CustomerUuid").toString();
		if (!filter.isEmpty()){
			iprm::CIdParam* textParamPtr = new iprm::CIdParam();
			textParamPtr->SetId(filter.toUtf8());

			filterParams.SetEditableParameter("CustomerUuid", textParamPtr, true);
		}
	}
}


bool CSoftwareProductCollectionControllerComp::UpdateObjectFromRepresentationRequest(
	const imtgql::CGqlRequest& /*rawGqlRequest*/,
	const sdl::prolife::Licenses::CSoftwareProductUpdateGqlRequest& softwareProductUpdateRequest,
	istd::IChangeable& object,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CSoftwareProductCollectionControllerComp");
		return false;
	}

	sdl::prolife::Licenses::SoftwareProductUpdateRequestArguments requestArguments = softwareProductUpdateRequest.GetRequestedArguments();
	if (!requestArguments.input.Version_1_0){
		I_CRITICAL();
		return false;
	}
	
	if (!requestArguments.input.Version_1_0->Item){
		I_CRITICAL();
		return false;
	}

	const sdl::prolife::Licenses::CSoftwareProductData::V1_0& softwareData = *requestArguments.input.Version_1_0->Item;
	
	QByteArray objectId;
	if (requestArguments.input.Version_1_0->Id){
		objectId = *softwareProductUpdateRequest.GetRequestedArguments().input.Version_1_0->Id;
	}
	
	prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(&object);
	if (softwareInfoPtr == nullptr){
		errorMessage = QString("Object is invalid");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return false;
	}

	softwareInfoPtr->ResetData();
	
	// ResetData clear UUID 
	softwareInfoPtr->SetObjectUuid(objectId);

	if (!FillObjectFromRepresentation(softwareData, object, objectId, errorMessage)){
		errorMessage = QString("Unable to update software from representation. Error: '%1'").arg(errorMessage);
		return false;
	}

	prolifedata::COrderedIdentifiableSoftwareInstanceInfo* oldSoftwareInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr oldSoftwareDataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, oldSoftwareDataPtr)){
		oldSoftwareInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(oldSoftwareDataPtr.GetPtr());
	}

	if (softwareInfoPtr != nullptr && oldSoftwareInfoPtr != nullptr){
		QByteArray newOrderId = softwareInfoPtr->GetOrderId();
		QByteArray oldOrderId = oldSoftwareInfoPtr->GetOrderId();

		if (newOrderId.isEmpty() && !oldOrderId.isEmpty()){
			if (!RemoveSoftwareFromOrder(objectId, oldOrderId)){
				errorMessage = QString("Unable to update software. Error: Remove software from order failed");
				return false;
			}
		}
		else if (!newOrderId.isEmpty() && oldOrderId.isEmpty()){
			if (!AddSoftwareToOrder(objectId, newOrderId)){
				errorMessage = QString("Unable to add software. Error: Add software to order failed");
				return false;
			}
		}
		else if (!newOrderId.isEmpty() && !oldOrderId.isEmpty() && newOrderId != oldOrderId){
			if (!AddSoftwareToOrder(objectId, newOrderId)){
				errorMessage = QString("Unable to add software. Error: Add software to order failed");
				return false;
			}

			if (!RemoveSoftwareFromOrder(objectId, oldOrderId)){
				errorMessage = QString("Unable to update software. Error: Remove software from order failed");
				return false;
			}
		}
	}

	return true;
}


} // namespace prolifegql


