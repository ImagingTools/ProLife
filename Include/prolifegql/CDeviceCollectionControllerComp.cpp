#include <prolifegql/CDeviceCollectionControllerComp.h>


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iprm/CTextParam.h>
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/CCollectionFilter.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtdb/CSqlDatabaseObjectCollectionComp.h>
#include <imtlic/ILicenseDefinition.h>
#include <imtgql/imtgql.h>

// ProLife includes
#include <prolifedata/IOrderInfo.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/TOrderedWrap.h>
#include <prolifedata/CHardwareProductBinding.h>


namespace prolifegql
{


// protected methods

// reimplemented (imtgql::CObjectCollectionControllerCompBase)

imtbase::CTreeItemModel* CDeviceCollectionControllerComp::DeleteObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid() || !m_bindingCollectionCompPtr.IsValid() || !m_softwareProductCollectionCompPtr.IsValid()){
		errorMessage = QString("No collection component was set");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return nullptr;
	}

	const imtgql::CGqlObject& inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		errorMessage = QObject::tr("No object-ID could not be extracted from the request");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_bindingCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const prolifedata::IHardwareProductBinding* bindingInfoPtr = dynamic_cast<const prolifedata::IHardwareProductBinding*>(dataPtr.GetPtr());
		if (bindingInfoPtr != nullptr){
			QByteArrayList softwareIds = bindingInfoPtr->GetSoftwareIds();
			for (const QByteArray& softwareId : softwareIds){
				imtbase::IObjectCollection::DataPtr softwareDataPtr;
				if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, softwareDataPtr)){
					const imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
					if (productInstanceInfoPtr != nullptr){
						bool isUse = productInstanceInfoPtr->IsInUse();
						if (isUse){
							errorMessage = QT_TR_NOOP("It is not possible to delete this sensor because a license file has been created for it. Contact your system administrator.");
							SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");
							errorMessage = imtgql::GetTranslation(m_translationManagerCompPtr.GetPtr(), gqlRequest, errorMessage.toUtf8(), "prolifegql::CDeviceCollectionControllerComp");

							return nullptr;
						}
					}
				}
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
	SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

	errorMessage = imtgql::GetTranslation(m_translationManagerCompPtr.GetPtr(), gqlRequest, errorMessage.toUtf8(), "prolifegql::CDeviceCollectionControllerComp");

	return nullptr;
}


bool CDeviceCollectionControllerComp::SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& errorMessage) const
{
	bool retVal = true;

	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (objectCollectionIterator->GetObjectData(dataPtr)){
			deviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(dataPtr.GetPtr());
		}

		idoc::MetaInfoPtr metaInfo = objectCollectionIterator->GetDataMetaInfo();

		if (deviceInfoPtr != nullptr){
			QByteArray collectionId = objectCollectionIterator->GetObjectId();

			for (QByteArray informationId : informationIds){
				QVariant elementInformation;

				if (informationId == "TypeId"){
					elementInformation = m_objectCollectionCompPtr->GetObjectTypeId(collectionId);
				}
				else if(informationId == "Id"){
					elementInformation = collectionId;
				}
				else if(informationId == "Name"){
					QByteArray deviceType = objectCollectionIterator->GetElementInfo("DeviceType").toByteArray();
					QByteArray macAddress = deviceInfoPtr->GetMacAddress();

					elementInformation = deviceType;
					if (!macAddress.isEmpty()){
						elementInformation = deviceType + " (" + macAddress + ")";
					}
				}
				else if(informationId == "Description"){
					elementInformation = metaInfo->GetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
				}
				else if(informationId == "PurchaseOrderId"){
					elementInformation = objectCollectionIterator->GetElementInfo("PurchaseOrderId");
				}
				else if(informationId == "Article"){
					elementInformation = objectCollectionIterator->GetElementInfo("LicenseId");
				}
				else if(informationId == "Customer"){
					elementInformation = objectCollectionIterator->GetElementInfo("Customer");
				}
				else if(informationId == "MacAddress"){
					elementInformation = deviceInfoPtr->GetMacAddress();
				}
				else if(informationId == "SerialNumber"){
					elementInformation = deviceInfoPtr->GetSerialNumber();
				}
				else if(informationId == "Project"){
					elementInformation = deviceInfoPtr->GetProject();
				}
				else if(informationId == "DeviceType"){
					elementInformation = objectCollectionIterator->GetElementInfo("DeviceType");
				}
				else if(informationId == "ProductUuid"){
					elementInformation = objectCollectionIterator->GetElementInfo("ProductUuid");
				}
				else if(informationId == "LicenseUuid"){
					elementInformation = objectCollectionIterator->GetElementInfo("LicenseUuid");
				}
				else if(informationId == "LicenseId"){
					elementInformation = objectCollectionIterator->GetElementInfo("LicenseId");
				}
				else if(informationId == "LicenseName"){
					elementInformation = objectCollectionIterator->GetElementInfo("LicenseName");
				}
				else if(informationId == "OrderId"){
					elementInformation = objectCollectionIterator->GetElementInfo("OrderId");
				}
				else if(informationId == "OrderUuid"){
					elementInformation = deviceInfoPtr->GetOrderId();
				}
				else if(informationId == "Status"){
					int status = deviceInfoPtr->GetDeviceProductionStatus();
					switch (status){
					case prolifedata::IDeviceInfo::DPS_NONE:
						elementInformation = "None";
						break;
					case prolifedata::IDeviceInfo::DPS_ACCEPTED:
						elementInformation = "Accepted";
						break;
					case prolifedata::IDeviceInfo::DPS_IN_PROGRESS:
						elementInformation = "In Progress";
						break;
					case prolifedata::IDeviceInfo::DPS_CANCELED:
						elementInformation = "Canceled";
						break;
					case prolifedata::IDeviceInfo::DPS_ON_HOLD:
						elementInformation = "Hold";
						break;
					case prolifedata::IDeviceInfo::DPS_FINISHED:
						elementInformation = "Finished";
						break;
					}
				}
				else if(informationId == "StatusId"){
					int status = deviceInfoPtr->GetDeviceProductionStatus();
					switch (status){
					case prolifedata::IDeviceInfo::DPS_NONE:
						elementInformation = "None";
						break;
					case prolifedata::IDeviceInfo::DPS_ACCEPTED:
						elementInformation = "Accepted";
						break;
					case prolifedata::IDeviceInfo::DPS_IN_PROGRESS:
						elementInformation = "InProgress";
						break;
					case prolifedata::IDeviceInfo::DPS_CANCELED:
						elementInformation = "Canceled";
						break;
					case prolifedata::IDeviceInfo::DPS_ON_HOLD:
						elementInformation = "OnHold";
						break;
					case prolifedata::IDeviceInfo::DPS_FINISHED:
						elementInformation = "Finished";
						break;
					}
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

			QVariant  softwareLinksCount = objectCollectionIterator->GetElementInfo("SoftwareLinksCount").toInt();
			retVal = retVal && model.SetData("SoftwareLinksCount", softwareLinksCount, itemIndex);

			return true;
		}
	}

	return false;
}


imtbase::CTreeItemModel* CDeviceCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_bindingCollectionCompPtr.IsValid() || !m_licenseCollectionCompPtr.IsValid() || !m_softwareProductCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error.");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return nullptr;
	}

	QByteArray objectId;
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel);
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	int index = dataModelPtr->InsertNewItem();

	QString name = QT_TR_NOOP("Licenses");

	QByteArray languageId;
	if (m_translationManagerCompPtr.IsValid()){
		imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
		if (gqlContextPtr != nullptr){
			languageId = gqlContextPtr->GetLanguageId();
		}

		QString elementNameTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), name.toUtf8(), languageId, "prolifegql::CDeviceCollectionControllerComp");

		name = elementNameTr;
	}

	dataModelPtr->SetData("Name", name, index);
	imtbase::CTreeItemModel* childrenModelPtr = dataModelPtr->AddTreeModel("Children", index);

	bool ok = false;

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_bindingCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const prolifedata::IHardwareProductBinding* bindingInfoPtr = dynamic_cast<const prolifedata::IHardwareProductBinding*>(dataPtr.GetPtr());
		if (bindingInfoPtr != nullptr){
			QByteArrayList softwareIds = bindingInfoPtr->GetSoftwareIds();

			ok = !softwareIds.isEmpty();

			for (const QByteArray& softwareId : softwareIds){
				imtbase::IObjectCollection::DataPtr productDataPtr;
				if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, productDataPtr)){
					imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
					if (productInstanceInfoPtr != nullptr){
						QByteArray productId = productInstanceInfoPtr->GetProductId();
						QByteArray serialNumber = productInstanceInfoPtr->GetSerialNumber();

						const imtbase::ICollectionInfo& licenseList = productInstanceInfoPtr->GetLicenseInstances();
						imtbase::ICollectionInfo::Ids elementsIds = licenseList.GetElementIds();

						for (const QByteArray& licenseId : elementsIds){
							imtbase::IObjectCollection::DataPtr dataPtr;
							if (m_licenseCollectionCompPtr->GetObjectData(licenseId, dataPtr)){
								imtlic::ILicenseDefinition* licenseDefinitionPtr = dynamic_cast<imtlic::ILicenseDefinition*>(dataPtr.GetPtr());
								if (licenseDefinitionPtr != nullptr){
									QString licenseName = licenseDefinitionPtr->GetLicenseName();
									QByteArray licenseId = licenseDefinitionPtr->GetLicenseId();

									int childrenIndex = childrenModelPtr->InsertNewItem();
									childrenModelPtr->SetData("Value", licenseName + " (" + licenseId + ")", childrenIndex);
								}
							}
						}
					}
				}
			}
		}
	}

	if (!ok){
		int childrenIndex = childrenModelPtr->InsertNewItem();
		QString elementNameTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("No Licenses"), languageId, "prolifegql::CDeviceCollectionControllerComp");

		childrenModelPtr->SetData("Value", elementNameTr, childrenIndex);
	}

	return rootModelPtr.PopPtr();
}


void CDeviceCollectionControllerComp::SetObjectFilter(
			const imtgql::CGqlRequest& gqlRequest,
			const imtbase::CTreeItemModel& objectFilterModel,
			iprm::CParamsSet& filterParams) const
{
	BaseClass::SetObjectFilter(gqlRequest, objectFilterModel, filterParams);

	imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return;
	}

	bool isAdmin = false;
	QByteArray userId;
	QByteArrayList userGroupIds;
	QByteArrayList userPermissions;

	imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr != nullptr){
		userPermissions = userInfoPtr->GetPermissions();
		userId = userInfoPtr->GetId();
		isAdmin = userInfoPtr->IsAdmin();
		userGroupIds = userInfoPtr->GetGroups();
	}

	bool filterByGroup = false;
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
		if (!userGroupIds.isEmpty()){
			groups = userGroupIds.join(';');
		}
		groupParamPtr->SetText(groups);

		iprm::CParamsSet* paramsSetPtr = new iprm::CParamsSet();

		paramsSetPtr->SetEditableParameter("UserParam", userParamPtr, true);
		paramsSetPtr->SetEditableParameter("GroupParam", groupParamPtr, true);

		filterParams.SetEditableParameter("Groups", paramsSetPtr, true);
	}
}


} // namespace prolifegql


