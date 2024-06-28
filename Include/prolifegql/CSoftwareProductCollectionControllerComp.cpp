#include <prolifegql/CSoftwareProductCollectionControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtlic/CHardwareInstanceInfo.h>
#include <imtgql/imtgql.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>


namespace prolifegql
{


imtbase::CTreeItemModel* CSoftwareProductCollectionControllerComp::DeleteObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = "No collection component was set";
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

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

	imtbase::IOperationContext* operationContextPtr = nullptr;

	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_REMOVE, gqlRequest);
	}

	if (m_objectCollectionCompPtr->RemoveElement(objectId, operationContextPtr)){
		istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

		imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
		imtbase::CTreeItemModel* notificationModel = dataModelPtr->AddTreeModel("removedNotification");

		notificationModel->SetData("Id", objectId);

		return rootModelPtr.PopPtr();
	}

	errorMessage = QString(QT_TR_NOOP("Can't remove object: %1")).arg(QString(objectId));
	SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

	errorMessage = imtgql::GetTranslation(m_translationManagerCompPtr.GetPtr(), gqlRequest, errorMessage.toUtf8(), "prolifegql::CSoftwareProductCollectionControllerComp");

	return nullptr;
}


bool CSoftwareProductCollectionControllerComp::SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& errorMessage) const
{
	if (objectCollectionIterator == nullptr){
		errorMessage = QString("Object collection iterator is invalid.");
		SendErrorMessage(0, errorMessage, "CSoftwareProductCollectionControllerComp");

		return false;
	}

	bool retVal = true;

	QByteArray collectionId = objectCollectionIterator->GetObjectId();
	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productOrderInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr orderDataPtr;
		if (objectCollectionIterator->GetObjectData(orderDataPtr)){
			productOrderInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(orderDataPtr.GetPtr());
		}

		if (productOrderInfoPtr != nullptr){
			QByteArray serialNumber = productOrderInfoPtr->GetSerialNumber();
			QByteArray productId = productOrderInfoPtr->GetProductId();
			QByteArray orderUuid = productOrderInfoPtr->GetOrderId();

			QByteArray hardwareMacAddress = objectCollectionIterator->GetElementInfo("DeviceId").toByteArray();
			QByteArray hardwareUuid = objectCollectionIterator->GetElementInfo("DeviceUuid").toByteArray();

			QByteArray licenseId ;
			imtbase::ICollectionInfo::Ids licenseIds = productOrderInfoPtr->GetLicenseInstances().GetElementIds();
			if (!licenseIds.isEmpty()){
				licenseId = licenseIds[0];
			}

			for (const QByteArray& informationId : informationIds){
				QVariant elementInformation;
				if (informationId == "Id"){
					elementInformation = collectionId;
				}
				if (informationId == "Name"){
					QString productName = objectCollectionIterator->GetElementInfo("ProductName").toString();

					elementInformation = productName;

					if (!serialNumber.isEmpty()){
						elementInformation = productName + " (" + serialNumber + ")";
					}
				}
				else if (informationId == "OrderId"){
					elementInformation = objectCollectionIterator->GetElementInfo("OrderId").toByteArray();
				}
				else if (informationId == "PurchaseOrderId"){
					elementInformation = objectCollectionIterator->GetElementInfo("PurchaseOrderId").toByteArray();
				}
				else if (informationId == "Customer"){
					elementInformation = objectCollectionIterator->GetElementInfo("Customer").toByteArray();
				}
				else if (informationId == "OrderUuid"){
					elementInformation = orderUuid;
				}
				else if (informationId == "HardwareUuid"){
					elementInformation = hardwareUuid;
				}
				else if (informationId == "ProductId"){
					elementInformation = objectCollectionIterator->GetElementInfo("ProductId").toByteArray();
				}
				else if (informationId == "ProductName"){
					elementInformation = objectCollectionIterator->GetElementInfo("ProductName").toByteArray();
				}
				else if (informationId == "ProductUuid"){
					elementInformation = objectCollectionIterator->GetElementInfo("ProductUuid").toByteArray();
				}
				else if (informationId == "SerialNumber"){
					elementInformation = serialNumber;
				}
				else if (informationId == "Expiration"){
					const imtlic::ILicenseInstance* licenseInstancePtr = productOrderInfoPtr->GetLicenseInstance(licenseId);
					if (licenseInstancePtr != nullptr){
						QDateTime expirationDate = licenseInstancePtr->GetExpiration();
						if (expirationDate.isValid()){
							elementInformation = expirationDate.toString("yyyy-MM-dd");
						}
					}
				}
				else if (informationId == "IsPaired"){
					elementInformation = !hardwareMacAddress.isEmpty();
				}
				else if (informationId == "InUse"){
					bool isUse = objectCollectionIterator->GetElementInfo("InUse").toBool();
					if (isUse){
						elementInformation = "InUse";
					}
				}
				else if (informationId == "Status"){
					bool isPaired = !hardwareMacAddress.isEmpty();
					if (isPaired){
						elementInformation = "IsPaired";
					}
					else{
						elementInformation = "NotPaired";
					}

					if (isPaired){
						bool isUse = objectCollectionIterator->GetElementInfo("InUse").toBool();
						if (isUse){
							elementInformation = "InUse";
						}
					}
				}
				else if (informationId == "DeviceId"){
					if (!hardwareMacAddress.isEmpty()){
						elementInformation = hardwareMacAddress;
					}
					else{
						elementInformation = "";
					}
				}
				else if (informationId == "LicenseName"){
					elementInformation = objectCollectionIterator->GetElementInfo("LicenseName").toByteArray();
				}
				else if (informationId == "LicenseId"){
					elementInformation = objectCollectionIterator->GetElementInfo("LicenseId").toByteArray();
				}
				else if (informationId == "LicenseUuid"){
					elementInformation = objectCollectionIterator->GetElementInfo("LicenseUuid").toByteArray();
				}
				else if (informationId == "CustomerUuid"){
					elementInformation = objectCollectionIterator->GetElementInfo("CustomerUuid").toByteArray();;
				}
				else if (informationId == "Project"){
					elementInformation = objectCollectionIterator->GetElementInfo("Project").toByteArray();;
				}
				else if(informationId == "Added"){
					QDateTime addedTime =  objectCollectionIterator->GetElementInfo("added").toDateTime();
					addedTime.setTimeSpec(Qt::UTC);

					elementInformation = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
				}
				else if(informationId == "LastModified"){
					QDateTime lastTime =  objectCollectionIterator->GetElementInfo("lastmodified").toDateTime();
					lastTime.setTimeSpec(Qt::UTC);

					elementInformation = lastTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
				}

				if (elementInformation.isNull()){
					elementInformation = "";
				}

				retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
			}
		}
	}

	return retVal;
}


void CSoftwareProductCollectionControllerComp::SetObjectFilter(
			const imtgql::CGqlRequest& gqlRequest,
			const imtbase::CTreeItemModel& objectFilterModel,
			iprm::CParamsSet& filterParams) const
{
	if (!m_accountCollectionCompPtr.IsValid() || !m_orderCollectionCompPtr.IsValid()){
		return;
	}

	imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
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


