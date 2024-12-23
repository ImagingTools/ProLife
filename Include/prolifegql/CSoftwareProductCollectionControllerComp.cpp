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
			sdl::prolife::Licenses::CSoftwareProductItem::V1_0& representationObject,
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

	if (requestInfo.items.isIdRequested){
		representationObject.Id = std::make_optional<QByteArray>(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		representationObject.TypeId = std::make_optional<QByteArray>(collectionObjectId);
	}

	if (requestInfo.items.isNameRequested){
		QString productName = objectCollectionIterator.GetElementInfo("ProductName").toString();
		representationObject.Name = std::make_optional<QString>(productName);

		QByteArray serialNumber = objectCollectionIterator.GetElementInfo("SerialNumber").toByteArray();
		if (!serialNumber.isEmpty()){
			representationObject.Name = std::make_optional<QString>(productName + " (" + serialNumber + ")");
		}
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.Description = std::make_optional<QString>(metaInfo->GetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION).toString());
	}

	if (requestInfo.items.isOrderIdRequested){
		representationObject.OrderId = std::make_optional<QString>(objectCollectionIterator.GetElementInfo("OrderId").toString());
	}

	if (requestInfo.items.isPurchaseOrderIdRequested){
		representationObject.PurchaseOrderId = std::make_optional<QString>(objectCollectionIterator.GetElementInfo("PurchaseOrderId").toString());
	}

	if (requestInfo.items.isOrderUuidRequested){
		representationObject.OrderUuid = std::make_optional<QString>(objectCollectionIterator.GetElementInfo("OrderUuid").toString());
	}

	if (requestInfo.items.isHardwareUuidRequested){
		representationObject.HardwareUuid = std::make_optional<QString>(objectCollectionIterator.GetElementInfo("HardwareUuid").toString());
	}

	if (requestInfo.items.isProductIdRequested){
		representationObject.ProductId = std::make_optional<QString>(objectCollectionIterator.GetElementInfo("ProductId").toString());
	}

	if (requestInfo.items.isProductNameRequested) {
		representationObject.ProductName = std::make_optional<QString>(
			objectCollectionIterator.GetElementInfo("ProductName").toString()
			);
	}

	if (requestInfo.items.isProductUuidRequested) {
		representationObject.ProductUuid = std::make_optional<QString>(
			objectCollectionIterator.GetElementInfo("ProductUuid").toString()
			);
	}

	if (requestInfo.items.isSerialNumberRequested) {
		representationObject.SerialNumber = std::make_optional<QString>(
			objectCollectionIterator.GetElementInfo("SerialNumber").toString()
			);
	}

	if (requestInfo.items.isExpirationRequested) {
		representationObject.Expiration = std::make_optional<QString>(
			objectCollectionIterator.GetElementInfo("Expiration").toString()
			);
	}

	if (requestInfo.items.isIsPairedRequested) {
		representationObject.IsPaired = std::make_optional<bool>(objectCollectionIterator.GetElementInfo("IsPaired").toBool());
	}

	if (requestInfo.items.isInUseRequested) {
		representationObject.InUse = std::make_optional<bool>(objectCollectionIterator.GetElementInfo("InUse").toBool());
	}

	if (requestInfo.items.isDeviceIdRequested) {
		representationObject.DeviceId = std::make_optional<QString>(
			objectCollectionIterator.GetElementInfo("DeviceId").toString()
			);
	}

	if (requestInfo.items.isLicenseNameRequested) {
		representationObject.LicenseName = std::make_optional<QString>(
			objectCollectionIterator.GetElementInfo("LicenseName").toString()
			);
	}

	if (requestInfo.items.isLicenseIdRequested) {
		representationObject.LicenseId = std::make_optional<QString>(
			objectCollectionIterator.GetElementInfo("LicenseId").toString()
			);
	}

	if (requestInfo.items.isLicenseUuidRequested) {
		representationObject.LicenseUuid = std::make_optional<QString>(
			objectCollectionIterator.GetElementInfo("LicenseUuid").toString()
			);
	}

	if (requestInfo.items.isCustomerUuidRequested) {
		representationObject.CustomerUuid = std::make_optional<QString>(
			objectCollectionIterator.GetElementInfo("CustomerUuid").toString()
			);
	}

	if (requestInfo.items.isCustomerRequested) {
		representationObject.Customer = std::make_optional<QString>(
			objectCollectionIterator.GetElementInfo("Customer").toString()
			);
	}

	if (requestInfo.items.isProjectRequested) {
		representationObject.Project = std::make_optional<QString>(
			objectCollectionIterator.GetElementInfo("Project").toString()
			);
	}


	if (requestInfo.items.isStatusRequested){
		QByteArray hardwareMacAddress = objectCollectionIterator.GetElementInfo("DeviceId").toByteArray();
		bool isPaired = !hardwareMacAddress.isEmpty();
		if (isPaired){
			representationObject.Status = std::make_optional<QString>("IsPaired");
		}
		else{
			representationObject.Status = std::make_optional<QString>("NotPaired");
		}

		if (isPaired){
			bool isUse = objectCollectionIterator.GetElementInfo("InUse").toBool();
			if (isUse){
				representationObject.Status = std::make_optional<QString>("InUse");
			}
		}
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


istd::IChangeable* CSoftwareProductCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::prolife::Licenses::CSoftwareProductData::V1_0& softwareProductDataRepresentation,
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

	if (softwareProductDataRepresentation.Id){
		newObjectId = *softwareProductDataRepresentation.Id;
	}

	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	softwareInfoPtr->SetObjectUuid(newObjectId);

	QByteArray productId;
	if (softwareProductDataRepresentation.ProductId){
		productId = *softwareProductDataRepresentation.ProductId;
	}

	if (productId.isEmpty()){
		errorMessage = QString("Product cannot be empty!");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return nullptr;
	}

	QByteArray serialNumber;
	if (softwareProductDataRepresentation.SerialNumber){
		serialNumber = *softwareProductDataRepresentation.SerialNumber;
	}

	if (!serialNumber.isEmpty()){
		bool ok = prolifedata::CheckSoftwareSerialNumberExists(newObjectId, serialNumber, *m_objectCollectionCompPtr);
		if (!ok){
			errorMessage = QString("Serial Number already exists");
			SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

			return nullptr;
		}
	}

	softwareInfoPtr->SetSerialNumber(serialNumber);

	if (softwareProductDataRepresentation.Project){
		QString project = *softwareProductDataRepresentation.Project;
		softwareInfoPtr->SetProject(project.toUtf8());
	}

	QByteArray orderUuid;
	if (softwareProductDataRepresentation.OrderUuid){
		orderUuid = *softwareProductDataRepresentation.OrderUuid;
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

			objectLinkPtr->SetObjectUuid(newObjectId);
			objectLinkPtr->SetFactoryId("SoftwareInfo");

			imtbase::IObjectCollection* productCollectionPtr = productOrderInfoPtr->GetProducts();
			if (productCollectionPtr != nullptr){
				productCollectionPtr->InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), newObjectId);

				istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
				if (m_orderOperationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext("Update", orderUuid, *productOrderInfoPtr);
				}

				m_orderCollectionCompPtr->SetObjectData(orderUuid, *productOrderInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr());
			}
		}
	}

	softwareInfoPtr->SetupProductInstance(productId, "", customerUuid);

	QByteArray licenseUuid;
	if (softwareProductDataRepresentation.LicenseUuid){
		licenseUuid = *softwareProductDataRepresentation.LicenseUuid;
	}

	QString expiration = *softwareProductDataRepresentation.Expiration;

	softwareInfoPtr->AddLicense(licenseUuid, QDateTime::fromString(expiration, "dd.MM.yyyy"));

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
			sdl::prolife::Licenses::CSoftwareProductDataPayload::V1_0& representationPayload,
			QString& errorMessage) const
{
	const prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(&data);
	if (softwareInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return false;
	}

	sdl::prolife::Licenses::V1_0::SoftwareProductItemRequestArguments arguments = softwareProductItemRequest.GetRequestedArguments();

	sdl::prolife::Licenses::CSoftwareProductData::V1_0 softwareProductData;

	QByteArray id;
	if (arguments.input.Id){
		id = *arguments.input.Id;
	}

	softwareProductData.Id = std::make_optional<QByteArray>(id);

	QByteArray productId = softwareInfoPtr->GetProductId();
	softwareProductData.ProductId = std::make_optional<QByteArray>(productId);

	QByteArray factoryId = softwareInfoPtr->GetFactoryId();
	softwareProductData.CategoryId = std::make_optional<QByteArray>(factoryId);

	QByteArray serialNumber = softwareInfoPtr->GetSerialNumber();
	softwareProductData.SerialNumber = std::make_optional<QByteArray>(serialNumber);

	QByteArray project = softwareInfoPtr->GetProject();
	softwareProductData.Project = std::make_optional<QString>(project);

	bool inUse = softwareInfoPtr->IsInUse();
	softwareProductData.InUse = std::make_optional<bool>(inUse);

	QByteArray orderId = softwareInfoPtr->GetOrderId();
	softwareProductData.OrderUuid = std::make_optional<QByteArray>(orderId);

	imtbase::ICollectionInfo::Ids licenseIds = softwareInfoPtr->GetLicenseInstances().GetElementIds();
	if (!licenseIds.isEmpty()){
		QByteArray licenseId = licenseIds[0];

		const imtlic::ILicenseInstance* licenseInstancePtr = softwareInfoPtr->GetLicenseInstance(licenseId);
		if (licenseInstancePtr != nullptr){
			softwareProductData.LicenseUuid = std::make_optional<QByteArray>(licenseInstancePtr->GetLicenseId());
			softwareProductData.Expiration = std::make_optional<QString>(licenseInstancePtr->GetExpiration().toString("dd.MM.yyyy"));
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

	softwareProductData.Name = std::make_optional<QString>(name);

	representationPayload.SoftwareProductData = std::make_optional<sdl::prolife::Licenses::CSoftwareProductData::V1_0>(softwareProductData);

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


bool CSoftwareProductCollectionControllerComp::UpdateObjectFromRepresentationRequest(const imtgql::CGqlRequest& rawGqlRequest, const sdl::prolife::Licenses::V1_0::CSoftwareProductUpdateGqlRequest& softwareProductUpdateRequest, istd::IChangeable& object, QString& errorMessage) const
{
	return false;
}


} // namespace prolifegql


