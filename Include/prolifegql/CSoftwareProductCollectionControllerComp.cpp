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


namespace prolifegql
{


// protected methods

// reimplemented (sdl::prolife::Licenses::V1_0::CSoftwareProductCollectionControllerCompBase)

bool CSoftwareProductCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::prolife::Licenses::V1_0::CSoftwareProductsListGqlRequest& softwareProductsListRequest,
			sdl::prolife::Licenses::V1_0::CSoftwareProductItem& representationObject,
			QString& errorMessage) const
{
	sdl::prolife::Licenses::V1_0::SoftwareProductsListRequestInfo requestInfo = softwareProductsListRequest.GetRequestInfo();

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

	if (requestInfo.items.isTypeIdRequested){
		representationObject.SetTypeId(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isIdRequested){
		representationObject.SetId(objectId);
	}

	if (requestInfo.items.isNameRequested){
		QString productName = objectCollectionIterator.GetElementInfo("ProductName").toString();
		representationObject.SetName(productName);

		QByteArray serialNumber = objectCollectionIterator.GetElementInfo("SerialNumber").toByteArray();
		if (!serialNumber.isEmpty()){
			representationObject.SetName(productName + " (" + serialNumber + ")");
		}
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.SetDescription(metaInfo->GetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION).toString());
	}

	if (requestInfo.items.isOrderIdRequested){
		representationObject.SetOrderId(objectCollectionIterator.GetElementInfo("OrderId").toString());
	}

	if (requestInfo.items.isPurchaseOrderIdRequested){
		representationObject.SetPurchaseOrderId(objectCollectionIterator.GetElementInfo("PurchaseOrderId").toString());
	}

	if (requestInfo.items.isOrderUuidRequested){
		representationObject.SetOrderUuid(objectCollectionIterator.GetElementInfo("OrderUuid").toString());
	}

	if (requestInfo.items.isHardwareUuidRequested){
		representationObject.SetHardwareUuid(objectCollectionIterator.GetElementInfo("HardwareUuid").toString());
	}

	if (requestInfo.items.isProductIdRequested){
		representationObject.SetProductId(objectCollectionIterator.GetElementInfo("ProductId").toString());
	}

	if (requestInfo.items.isProductNameRequested){
		representationObject.SetProductName(objectCollectionIterator.GetElementInfo("ProductName").toString());
	}

	if (requestInfo.items.isProductUuidRequested){
		representationObject.SetProductUuid(objectCollectionIterator.GetElementInfo("ProductUuid").toString());
	}

	if (requestInfo.items.isSerialNumberRequested){
		representationObject.SetSerialNumber(objectCollectionIterator.GetElementInfo("SerialNumber").toString());
	}

	if (requestInfo.items.isSerialNumberRequested){
		representationObject.SetSerialNumber(objectCollectionIterator.GetElementInfo("SerialNumber").toString());
	}

	if (requestInfo.items.isExpirationRequested){
		representationObject.SetExpiration(objectCollectionIterator.GetElementInfo("Expiration").toString());
	}

	if (requestInfo.items.isIsPairedRequested){
		representationObject.SetIsPaired(objectCollectionIterator.GetElementInfo("IsPaired").toBool());
	}

	if (requestInfo.items.isInUseRequested){
		representationObject.SetInUse(objectCollectionIterator.GetElementInfo("InUse").toBool());
	}

	if (requestInfo.items.isDeviceIdRequested){
		representationObject.SetDeviceId(objectCollectionIterator.GetElementInfo("DeviceId").toString());
	}

	if (requestInfo.items.isLicenseNameRequested){
		representationObject.SetLicenseName(objectCollectionIterator.GetElementInfo("LicenseName").toString());
	}

	if (requestInfo.items.isLicenseIdRequested){
		representationObject.SetLicenseId(objectCollectionIterator.GetElementInfo("LicenseId").toString());
	}

	if (requestInfo.items.isLicenseUuidRequested){
		representationObject.SetLicenseUuid(objectCollectionIterator.GetElementInfo("LicenseUuid").toString());
	}

	if (requestInfo.items.isCustomerUuidRequested){
		representationObject.SetCustomerUuid(objectCollectionIterator.GetElementInfo("CustomerUuid").toString());
	}

	if (requestInfo.items.isCustomerRequested){
		representationObject.SetCustomer(objectCollectionIterator.GetElementInfo("Customer").toString());
	}

	if (requestInfo.items.isProjectRequested){
		representationObject.SetProject(objectCollectionIterator.GetElementInfo("Project").toString());
	}

	if (requestInfo.items.isStatusRequested){
		QByteArray hardwareMacAddress = objectCollectionIterator.GetElementInfo("DeviceId").toByteArray();
		bool isPaired = !hardwareMacAddress.isEmpty();
		if (isPaired){
			representationObject.SetStatus("IsPaired");
		}
		else{
			representationObject.SetStatus("NotPaired");
		}

		if (isPaired){
			bool isUse = objectCollectionIterator.GetElementInfo("InUse").toBool();
			if (isUse){
				representationObject.SetStatus("InUse");
			}
		}
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


istd::IChangeable* CSoftwareProductCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::prolife::Licenses::V1_0::CSoftwareProductData& softwareProductDataRepresentation,
			QByteArray& newObjectId,
			QString& name,
			QString& description,
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

	QByteArray softwareUuid = softwareProductDataRepresentation.GetId();
	if (softwareUuid.isEmpty()){
		softwareUuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray productId = softwareProductDataRepresentation.GetProductId();
	if (productId.isEmpty()){
		errorMessage = QString("Product cannot be empty!");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return nullptr;
	}

	QByteArray serialNumber = softwareProductDataRepresentation.GetSerialNumber();
	if (!serialNumber.isEmpty()){
		bool ok = prolifedata::CheckSoftwareSerialNumberExists(softwareUuid, serialNumber, *m_objectCollectionCompPtr);
		if (!ok){
			errorMessage = QString("Serial Number already exists");
			SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

			return nullptr;
		}
	}

	softwareInfoPtr->SetSerialNumber(serialNumber);

	QString project = softwareProductDataRepresentation.GetProject();
	softwareInfoPtr->SetProject(project.toUtf8());

	QByteArray orderUuid = softwareProductDataRepresentation.GetOrderUuid();
	softwareInfoPtr->SetOrderId(orderUuid);

	QByteArray customerUuid;
	imtbase::IObjectCollection::DataPtr orderDataPtr;
	if (m_orderCollectionCompPtr->GetObjectData(orderUuid, orderDataPtr)){
		prolifedata::IOrderInfo* productOrderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(orderDataPtr.GetPtr());
		if (productOrderInfoPtr != nullptr){
			customerUuid = productOrderInfoPtr->GetCustomerId();

			istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
			objectLinkPtr.SetPtr(new imtbase::CObjectLink());

			objectLinkPtr->SetObjectUuid(softwareUuid);
			objectLinkPtr->SetFactoryId("SoftwareInfo");

			imtbase::IObjectCollection* productCollectionPtr = productOrderInfoPtr->GetProducts();
			if (productCollectionPtr != nullptr){
				productCollectionPtr->InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), softwareUuid);

				istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
				if (m_orderOperationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext("Update", orderUuid, *productOrderInfoPtr);
				}

				m_orderCollectionCompPtr->SetObjectData(orderUuid, *productOrderInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr());
			}
		}
	}

	softwareInfoPtr->SetupProductInstance(productId, "", customerUuid);

	QByteArray licenseUuid = softwareProductDataRepresentation.GetLicenseUuid();
	QString expiration = softwareProductDataRepresentation.GetExpiration();

	softwareInfoPtr->AddLicense(licenseUuid, QDateTime::fromString(expiration, "yyyy-MM-dd"));

	name = productId;

	if (m_productCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_productCollectionCompPtr->GetObjectData(productId, dataPtr)){
			imtlic::IProductInfo* remoteProductInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
			if (remoteProductInfoPtr != nullptr){
				name = remoteProductInfoPtr->GetName();
			}
		}
	}

	if (!serialNumber.isEmpty()){
		name += " (" + serialNumber + ")";
	}

	return softwareInstancePtr.PopPtr();
}


bool CSoftwareProductCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::prolife::Licenses::V1_0::CSoftwareProductItemGqlRequest& softwareProductItemRequest,
			sdl::prolife::Licenses::V1_0::CSoftwareProductDataPayload& representationPayload,
			QString& errorMessage) const
{
	const prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(&data);
	if (softwareInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return false;
	}

	sdl::prolife::Licenses::V1_0::SoftwareProductItemRequestArguments arguments = softwareProductItemRequest.GetRequestedArguments();

	sdl::prolife::Licenses::V1_0::CSoftwareProductData softwareProductData;

	QByteArray id = arguments.input.GetId();
	softwareProductData.SetId(id);

	QByteArray productId = softwareInfoPtr->GetProductId();
	softwareProductData.SetProductId(productId);

	QByteArray factoryId = softwareInfoPtr->GetFactoryId();
	softwareProductData.SetCategoryId(factoryId);

	QByteArray serialNumber = softwareInfoPtr->GetSerialNumber();
	softwareProductData.SetSerialNumber(serialNumber);

	QByteArray project = softwareInfoPtr->GetProject();
	softwareProductData.SetProject(project);

	bool inUse = softwareInfoPtr->IsInUse();
	softwareProductData.SetInUse(inUse);

	QByteArray orderId = softwareInfoPtr->GetOrderId();
	softwareProductData.SetOrderUuid(orderId);

	imtbase::ICollectionInfo::Ids licenseIds = softwareInfoPtr->GetLicenseInstances().GetElementIds();
	if (!licenseIds.isEmpty()){
		QByteArray licenseId = licenseIds[0];

		const imtlic::ILicenseInstance* licenseInstancePtr = softwareInfoPtr->GetLicenseInstance(licenseId);
		if (licenseInstancePtr != nullptr){
			softwareProductData.SetLicenseUuid(licenseInstancePtr->GetLicenseId());
			softwareProductData.SetExpiration(licenseInstancePtr->GetExpiration().toString("yyyy-MM-dd"));
		}
	}

	QString name = productId;

	if (m_productCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_productCollectionCompPtr->GetObjectData(productId, dataPtr)){
			imtlic::IProductInfo* remoteProductInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
			if (remoteProductInfoPtr != nullptr){
				name = remoteProductInfoPtr->GetName();
			}
		}
	}

	if (!serialNumber.isEmpty()){
		name += " (" + serialNumber + ")";
	}

	softwareProductData.SetName(name);

	representationPayload.SetSoftwareProductData(softwareProductData);

	return true;
}


imtbase::CTreeItemModel* CSoftwareProductCollectionControllerComp::DeleteObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = "No collection component was set";
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return nullptr;
	}

	const imtgql::CGqlObject& inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		errorMessage = QString("No object-ID could not be extracted from the request");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
		if (productInstanceInfoPtr != nullptr){
			bool isUse = productInstanceInfoPtr->IsInUse();
			if (isUse){
				errorMessage = QT_TR_NOOP("It is not possible to delete this sensor because a license file has been created for it. Contact your system administrator.");
				SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

				errorMessage = imtgql::GetTranslation(m_translationManagerCompPtr.GetPtr(), gqlRequest, errorMessage.toUtf8(), "prolifegql::CSoftwareProductCollectionControllerComp");

				return nullptr;
			}
		}
	}

	return BaseClass::DeleteObject(gqlRequest, errorMessage);
}


void CSoftwareProductCollectionControllerComp::SetObjectFilter(
			const imtgql::CGqlRequest& gqlRequest,
			const imtbase::CTreeItemModel& objectFilterModel,
			iprm::CParamsSet& filterParams) const
{
	if (!m_accountCollectionCompPtr.IsValid() || !m_orderCollectionCompPtr.IsValid()){
		return;
	}

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


} // namespace prolifegql


