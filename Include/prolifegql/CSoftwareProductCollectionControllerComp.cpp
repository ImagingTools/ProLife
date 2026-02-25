#include <prolifegql/CSoftwareProductCollectionControllerComp.h>


// ACF includes
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtbase/CObjectLink.h>
#include <imtlic/CHardwareInstanceInfo.h>
#include <imtlic/IProductInfo.h>
#include <imtgql/imtgql.h>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/COrderInfo.h>
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>
#include <prolifedata/CHardwareProductBinding.h>
#include <prolifedata/CGroupFilter.h>


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
	if (!errorMessage.isEmpty()){
		return sdl::imtbase::ImtCollection::CVisualStatus();
	}
	
	if (!retVal.Version_1_0){
		I_CRITICAL();
		
		return retVal;
	}
	sdl::imtbase::ImtCollection::CVisualStatus::V1_0& response = *retVal.Version_1_0;
	
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(*response.objectId, dataPtr)){
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
			
			response.text = name;
		}
	}
	
	return retVal;
}


bool CSoftwareProductCollectionControllerComp::OnBeforeRemoveElements(
			const QByteArrayList& elementIds,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr) {
		errorMessage = QString("Unable to remove software elements. Error: request context is missing");
		return false;
	}

	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr) {
		errorMessage = QString("Unable to remove software elements. Error: user information is missing in request context");
		return false;
	}

	bool isAdmin = userInfoPtr->IsAdmin();
	if (!isAdmin){
		for (const QByteArray& objectId : elementIds){
			idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
			if (metaInfo.IsValid()){
				bool inUse = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_IN_USE).toBool();
				if (inUse){
					errorMessage = QT_TR_NOOP("It is not possible to delete this software because a license file has been created for it. Contact your system administrator.");
					SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");
					errorMessage = imtgql::GetTranslation(m_translationManagerCompPtr.GetPtr(), gqlRequest, errorMessage.toUtf8(), "prolifegql::CSoftwareProductCollectionControllerComp");
					return false;
				}
			}
		}
	}

	if (m_bindingCollectionCompPtr.IsValid()){
		for (const QByteArray& objectId : elementIds){
			imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
			fieldFilter.fieldId = "SoftwareIds";
			fieldFilter.filterValue = objectId;
			fieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FO_CONTAINS;
			
			imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
			groupFilter.fieldFilters << fieldFilter;
			
			imtbase::CComplexCollectionFilter complexFilter;
			complexFilter.SetFilterExpression(groupFilter);
			
			iprm::CParamsSet filterParam;
			filterParam.SetEditableParameter("ComplexFilter", &complexFilter);
			
			imtbase::IObjectCollection::Ids elementIds = m_bindingCollectionCompPtr->GetElementIds(0, -1, &filterParam);
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
		}
	}
	
	for (const QByteArray& objectId : elementIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			const prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productInstanceInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
			if (productInstanceInfoPtr != nullptr){
				QByteArray orderId = productInstanceInfoPtr->GetOrderId();
				if (!orderId.isEmpty()){
					if (!RemoveSoftwareFromOrder(objectId, orderId)){
						SendWarningMessage(0,
										   QString("Remove software '%1' from order '%2' failed")
											   .arg(qPrintable(objectId), qPrintable(orderId)),
										   "CDeviceCollectionControllerComp");
					}
				}
			}
		}
	}

	return true;
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
		representationObject.id = (objectId);
	}
	
	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		representationObject.typeId = (collectionObjectId);
	}
	
	if (requestInfo.items.isNameRequested){
		QString productName = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME).toString();
		representationObject.name = (productName);
		
		QString serialNumber = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_SERIAL_NUMBER).toString();
		if (!serialNumber.isEmpty()){
			representationObject.name = (productName + " (" + serialNumber + ")");
		}
	}
	
	if (requestInfo.items.isDescriptionRequested){
		representationObject.description = (metaInfo->GetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION).toString());
	}

	QString scheme = "applink";
	if (requestInfo.items.isDeliveryIdLinkRequested){
		sdl::imtbase::ImtBaseTypes::CObjectLink::V1_0 objectLink;
		objectLink.id = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_ORDER_ID).toString().toUtf8();
		objectLink.typeId = QByteArrayLiteral("Order");
		objectLink.name = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_DELIVERY_ID).toString().toUtf8();

		sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0 urlParam;
		urlParam.scheme = scheme;
		urlParam.path = QStringLiteral("Orders/Order");
		if (!(*objectLink.id).isEmpty()){
			urlParam.path = *urlParam.path + QStringLiteral("/") + *objectLink.id;
		}
		objectLink.url = urlParam;

		representationObject.deliveryIdLink = objectLink;
	}
	
	if (requestInfo.items.isPurchaseIdLinkRequested){
		sdl::imtbase::ImtBaseTypes::CObjectLink::V1_0 objectLink;
		objectLink.id = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_ORDER_ID).toString().toUtf8();
		objectLink.typeId = QByteArrayLiteral("Order");
		objectLink.name = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PURCHASE_ID).toString().toUtf8();

		sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0 urlParam;
		urlParam.scheme = scheme;
		urlParam.path = QStringLiteral("Orders/Order");
		if (!(*objectLink.id).isEmpty()){
			urlParam.path = *urlParam.path + QStringLiteral("/") + *objectLink.id;
		}
		objectLink.url = urlParam;

		representationObject.purchaseIdLink = objectLink;
	}

	if (requestInfo.items.isHardwareLinkRequested){
		QJsonArray hardwareIds = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_HARDWARE_ID).toJsonArray();
		if (hardwareIds.size() == 1){
			sdl::imtbase::ImtBaseTypes::CObjectLink::V1_0 objectLink;
			objectLink.id = hardwareIds.at(0).toString().toUtf8();
			objectLink.typeId = QByteArrayLiteral("Device");
			objectLink.name = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_HARDWARE_MAC_ADDRESS).toString();
	
			sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0 urlParam;
			urlParam.scheme = scheme;
			urlParam.path = QStringLiteral("Devices/Device");
			if (!(*objectLink.id).isEmpty()){
				urlParam.path = *urlParam.path + QStringLiteral("/") + *objectLink.id;
			}
			objectLink.url = urlParam;
	
			representationObject.hardwareLink = objectLink;
		}
	}

	if (requestInfo.items.isProductIdRequested){
		representationObject.productId = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_ID).toString();
	}

	if (requestInfo.items.isProductNameRequested){
		representationObject.productName = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_NAME).toString();
	}

	if (requestInfo.items.isProductUuidRequested){
		representationObject.productUuid = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PRODUCT_UUID).toString();
	}

	if (requestInfo.items.isSerialNumberRequested){
		representationObject.serialNumber = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_SERIAL_NUMBER).toString();
	}

	if (requestInfo.items.isIsPairedRequested){
		representationObject.isPaired = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_IS_PAIRED).toBool();
	}

	if (requestInfo.items.isInUseRequested){
		representationObject.inUse = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_IN_USE).toBool();
	}

	if (requestInfo.items.isLicenseNameRequested){
		representationObject.licenseName = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_NAME).toString();
	}

	if (requestInfo.items.isLicenseIdRequested){
		representationObject.licenseId = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_ID).toString();
	}

	if (requestInfo.items.isLicenseUuidRequested){
		representationObject.licenseUuid = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_LICENSE_UUID).toString();
	}

	if (requestInfo.items.isCustomerLinkRequested){
		sdl::imtbase::ImtBaseTypes::CObjectLink::V1_0 objectLink;
		objectLink.id = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_CUSTOMER_ID).toString().toUtf8();
		objectLink.typeId = QByteArrayLiteral("Account");
		objectLink.name = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_CUSTOMER_NAME).toString();

		sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0 urlParam;
		urlParam.scheme = scheme;
		urlParam.path = QStringLiteral("Accounts/Account");
		if (!(*objectLink.id).isEmpty()){
			urlParam.path = *urlParam.path + QStringLiteral("/") + *objectLink.id;
		}
		objectLink.url = urlParam;

		representationObject.customerLink = objectLink;
	}

	if (requestInfo.items.isProjectRequested){
		representationObject.project = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_PROJECT).toString();
	}

	if (requestInfo.items.isExpirationRequested){
		imtbase::ICollectionInfo::Ids licenseIds = softwareInfoPtr->GetLicenseInstances().GetElementIds();
		if (!licenseIds.isEmpty()){
			QByteArray licenseId = licenseIds[0];

			const imtlic::ILicenseInstance* licenseInstancePtr = softwareInfoPtr->GetLicenseInstance(licenseId);
			if (licenseInstancePtr != nullptr){
				representationObject.expiration = (licenseInstancePtr->GetExpiration().toString("dd.MM.yyyy"));
			}
		}
	}

	if (requestInfo.items.isStatusRequested){
		QByteArray hardwareMacAddress = hardwareId.toUtf8();
		bool isPaired = !hardwareMacAddress.isEmpty();
		if (isPaired){
			representationObject.status = ("IsPaired");
		}
		else{
			representationObject.status = ("NotPaired");
		}

		if (isPaired){
			bool isUse = metaInfo->GetMetaInfo(imtlic::IProductInstanceInfo::MIT_IN_USE).toBool();
			if (isUse){
				representationObject.status = ("InUse");
			}
		}
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

	if (requestInfo.items.isIsMultipleRequested){
		representationObject.isMultiple = softwareInfoPtr->IsMultiProduct();
	}

	if (requestInfo.items.isProductCountRequested){
		representationObject.productCount = softwareInfoPtr->GetProductCount();
	}

	return true;
}


istd::IChangeableUniquePtr CSoftwareProductCollectionControllerComp::CreateObjectFromRepresentation(
	const sdl::prolife::Licenses::CSoftwareProductData::V1_0& softwareProductDataRepresentation,
	QByteArray& newObjectId,
	QString& errorMessage) const
{
	if (!m_softwareInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_softwareInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");
		
		return nullptr;
	}
	
	istd::TUniqueInterfacePtr<imtlic::IProductInstanceInfo> softwareInstancePtr = m_softwareInfoFactCompPtr.CreateInstance();
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
	
	if (softwareProductDataRepresentation.id){
		newObjectId = *softwareProductDataRepresentation.id;
	}
	
	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	
	softwareInfoPtr->SetObjectUuid(newObjectId);
	
	if (!FillObjectFromRepresentation(softwareProductDataRepresentation, *softwareInfoPtr, newObjectId, errorMessage)){
		errorMessage = QString("Unable to create software. Error: '%1'").arg(errorMessage);
		
		return nullptr;
	}
	
	if (softwareProductDataRepresentation.orderUuid){
		QString orderId = *softwareProductDataRepresentation.orderUuid;
		if (!orderId.isEmpty()){
			if (!AddSoftwareToOrder(newObjectId, orderId.toUtf8())){
				errorMessage = QString("Unable to add software. Error: Add software to order failed");
				return nullptr;
			}
		}
	}

	istd::IChangeableUniquePtr retVal;
	retVal.MoveCastedPtr<imtlic::IProductInstanceInfo>(softwareInstancePtr);

	return retVal;
}


void CSoftwareProductCollectionControllerComp::PopulateBoundCountInTree(sdl::prolife::Licenses::CLicenseTreeNode::V1_0& node) const
{
	if (!node.id.HasValue()){
		return;
	}

	// Calculate bound count for this node
	int boundCount = 0;
	if (m_bindingCollectionCompPtr.IsValid()){
		imtbase::IComplexCollectionFilter::FieldFilter arrayFieldFilter;
		arrayFieldFilter.fieldId = "SoftwareIds";
		arrayFieldFilter.filterValue = QVariantList({node.id.GetValue()});
		arrayFieldFilter.filterOperation = imtbase::IComplexCollectionFilter::FieldOperation::FO_ARRAY_HAS_ANY;

		imtbase::CComplexCollectionFilter arrayComplexFilter;
		arrayComplexFilter.AddFieldFilter(arrayFieldFilter);

		iprm::CParamsSet arrayFilterParam;
		arrayFilterParam.SetEditableParameter("ComplexFilter", &arrayComplexFilter);

		QByteArrayList hardwareBindingIds = m_bindingCollectionCompPtr->GetElementIds(0, -1, &arrayFilterParam);
		boundCount = hardwareBindingIds.size();
	}

	node.boundCount = boundCount;

	if (node.children.HasValue()){
		QList<sdl::prolife::Licenses::CLicenseTreeNode::V1_0> childList = node.children.GetValue().ToList();
		for (int i = 0; i < childList.size(); ++i){
			PopulateBoundCountInTree(childList[i]);
		}

		node.children.Emplace().FromList(childList);
	}
}


bool CSoftwareProductCollectionControllerComp::CreateRepresentationFromObject(
	const istd::IChangeable& data,
	const sdl::prolife::Licenses::CSoftwareProductItemGqlRequest& softwareProductItemRequest,
	sdl::prolife::Licenses::CSoftwareProductData::V1_0& representationPayload,
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
	
	QByteArray id;
	if (arguments.input.Version_1_0->id){
		id = *arguments.input.Version_1_0->id;
	}
	
	representationPayload.id = (id);
	
	QByteArray productId = softwareInfoPtr->GetProductId();
	representationPayload.productId = (productId);
	
	QByteArray factoryId = softwareInfoPtr->GetFactoryId();
	representationPayload.categoryId = (factoryId);
	
	QByteArray serialNumber = softwareInfoPtr->GetSerialNumber();
	representationPayload.serialNumber = (serialNumber);
	
	QByteArray project = softwareInfoPtr->GetProject();
	representationPayload.project = (project);
	
	bool inUse = softwareInfoPtr->IsInUse();
	representationPayload.inUse = (inUse);
	
	QByteArray orderId = softwareInfoPtr->GetOrderId();
	representationPayload.orderUuid = (orderId);

	representationPayload.customerId = softwareInfoPtr->GetCustomerId();

	imtbase::ICollectionInfo::Ids licenseIds = softwareInfoPtr->GetLicenseInstances().GetElementIds();
	if (!licenseIds.isEmpty()){
		QByteArray licenseId = licenseIds[0];
		
		const imtlic::ILicenseInstance* licenseInstancePtr = softwareInfoPtr->GetLicenseInstance(licenseId);
		if (licenseInstancePtr != nullptr){
			representationPayload.licenseUuid = (licenseInstancePtr->GetLicenseId());
			representationPayload.expiration = (licenseInstancePtr->GetExpiration().toString("dd.MM.yyyy"));
		}
	}

	representationPayload.internalUse = softwareInfoPtr->IsInternalUse();
	representationPayload.isMultiple = softwareInfoPtr->IsMultiProduct();
	representationPayload.productCount = softwareInfoPtr->GetProductCount();
	representationPayload.parentInstanceId = softwareInfoPtr->GetParentInstanceId();

	// Check if this license has a parent
	QByteArray parentInstanceId = softwareInfoPtr->GetParentInstanceId();
	representationPayload.hasParent = !parentInstanceId.isEmpty();

	// Check if this license has children
	if (m_objectCollectionCompPtr.IsValid()){
		imtbase::IComplexCollectionFilter::FieldFilter childFilter;
		childFilter.fieldId = "ParentInstanceId";
		childFilter.filterValue = id;

		imtbase::CComplexCollectionFilter childComplexFilter;
		childComplexFilter.AddFieldFilter(childFilter);

		iprm::CParamsSet childFilterParam;
		childFilterParam.SetEditableParameter("ComplexFilter", &childComplexFilter);

		int childCount = m_objectCollectionCompPtr->GetElementsCount(&childFilterParam);
		representationPayload.hasChildren = (childCount > 0);
	}
	else {
		representationPayload.hasChildren = false;
	}

	// Build hierarchical license tree from UserActions
	if (m_userActionManagerCompPtr.IsValid()){
		QString treeError;
		sdl::prolife::Licenses::CLicenseTreeNode::V1_0 rootNode = prolifedata::BuildLicenseTreeFromActions(
			id,
			*m_objectCollectionCompPtr.GetPtr(),
			*m_userActionManagerCompPtr.GetPtr(),
			treeError,
			true);

		if (rootNode.id.HasValue()){
			// Populate boundCount for all nodes in the tree
			PopulateBoundCountInTree(rootNode);
			representationPayload.licenseTree = rootNode;
		}
	}
	// If tree building fails or UserActionManager unavailable, we just don't populate the field (it's optional)

	return true;
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
	if (representation.serialNumber){
		serialNumber = *representation.serialNumber;
	}
	
	if (serialNumber.isEmpty()){
		errorMessage = QString("Serial Number cannot be empty");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");
		
		return false;
	}
	
	if (!serialNumber.isEmpty()){
		bool ok = prolifedata::CheckSoftwareSerialNumberExists(objectId, serialNumber, *m_objectCollectionCompPtr);
		if (!ok){
			errorMessage = QString("Serial Number '%1' already exists").arg(serialNumber);
			SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

			return false;
		}
	}
	
	softwareInfoPtr->SetSerialNumber(serialNumber);
	
	QByteArray productId;
	if (representation.productId){
		productId = *representation.productId;
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
	
	if (representation.project){
		QString project = *representation.project;
		softwareInfoPtr->SetProject(project.toUtf8());
	}
	
	QByteArray orderUuid;
	if (representation.orderUuid){
		orderUuid = *representation.orderUuid;
		softwareInfoPtr->SetOrderId(orderUuid);
	}

	QByteArray customerUuid;
	if (representation.customerId){
		customerUuid = *representation.customerId;
	}

	if (!orderUuid.isEmpty()){
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
	}

	softwareInfoPtr->SetupProductInstance(productId, "", customerUuid);
	
	QByteArray licenseUuid;
	if (representation.licenseUuid){
		licenseUuid = *representation.licenseUuid;
	}
	
	if (licenseUuid.isEmpty()){
		errorMessage = QString("License cannot be empty");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");
		
		return false;
	}
	
	QString expiration = *representation.expiration;
	
	softwareInfoPtr->AddLicense(licenseUuid, QDateTime::fromString(expiration, "dd.MM.yyyy"));

	if (representation.inUse){
		softwareInfoPtr->SetInUse(*representation.inUse);
	}

	if (representation.internalUse){
		softwareInfoPtr->SetInternalUse(*representation.internalUse);
	}

	// Check if license has children or parent - if so, isMultiple and productCount cannot be changed
	bool hasChildren = false;
	bool hasParent = false;

	QByteArray parentInstanceId = softwareInfoPtr->GetParentInstanceId();
	hasParent = !parentInstanceId.isEmpty();

	if (m_objectCollectionCompPtr.IsValid()){
		imtbase::IComplexCollectionFilter::FieldFilter childFilter;
		childFilter.fieldId = "ParentInstanceId";
		childFilter.filterValue = objectId;

		imtbase::CComplexCollectionFilter childComplexFilter;
		childComplexFilter.AddFieldFilter(childFilter);

		iprm::CParamsSet childFilterParam;
		childFilterParam.SetEditableParameter("ComplexFilter", &childComplexFilter);

		int childCount = m_objectCollectionCompPtr->GetElementsCount(&childFilterParam);
		hasChildren = (childCount > 0);
	}

	bool canModifyProductSettings = !hasChildren && !hasParent;

	if (canModifyProductSettings){
		if (representation.isMultiple){
			softwareInfoPtr->SetMultiProduct(*representation.isMultiple);
		}

		int productCount = 1;

		if (representation.productCount){
			if (*representation.productCount > 0){
				productCount = *representation.productCount;
			}
		}

		softwareInfoPtr->SetProductCount(productCount);
	}
	// If hasChildren or hasParent is true, we silently ignore changes to isMultiple and productCount

	if (representation.parentInstanceId){
		softwareInfoPtr->SetParentInstanceId(*representation.parentInstanceId);
	}

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
	
	QByteArrayList removedIds;
	removedIds << softwareId;
	if (!productCollectionPtr->RemoveElements(removedIds)){
		SendErrorMessage(0,
						 QString("Unable to remove software '%1' from order '%2'. Error: Removing element from product collection failed")
							 .arg(qPrintable(softwareId), qPrintable(orderId)),
						 "CSoftwareProductCollectionControllerComp");
		return false;
	}
	
	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_orderOperationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext("Update", orderId, oldOrderInfoPtr);
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
		operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext("Update", orderId, orderInfoPtr);
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


void CSoftwareProductCollectionControllerComp::SetAdditionalFilters(
	const imtgql::CGqlRequest& gqlRequest,
	const imtgql::CGqlParamObject& /*viewParamsGql*/,
	iprm::CParamsSet* filterParams) const
{
	if (m_groupFilterParamJoinerCompPtr.IsValid()){
		if (!m_groupFilterParamJoinerCompPtr->JoinGroupFilterParam(gqlRequest, *filterParams)){
			SendWarningMessage(0, QString("Unable to join group filter param"), "CSoftwareProductCollectionControllerComp");
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CSoftwareProductCollectionControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_timeFilterParamRepresentationControllerPtr.SetPtr(new imtserverapp::CTimeFilterParamRepresentationController("LicenseCreationTimeFilter"));
	RegisterFilterToSelectionParams(m_licenseCreationTimeParam, *m_timeFilterParamRepresentationControllerPtr.GetPtr());
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
	
	if (!requestArguments.input.Version_1_0->item){
		I_CRITICAL();
		return false;
	}
	
	const sdl::prolife::Licenses::CSoftwareProductData::V1_0& softwareData = *requestArguments.input.Version_1_0->item;
	
	QByteArray objectId;
	if (requestArguments.input.Version_1_0->id){
		objectId = *softwareProductUpdateRequest.GetRequestedArguments().input.Version_1_0->id;
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
		errorMessage = QString("Unable to update software. Error: '%1'").arg(errorMessage);
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


