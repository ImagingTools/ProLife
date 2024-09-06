#include <prolifegql/CDeviceCollectionControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CEnableableParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/CObjectLink.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtgql/imtgql.h>
#include <imtlic/ILicenseDefinition.h>
#include <imtlic/IProductInfo.h>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/IOrderInfo.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/TOrderedWrap.h>
#include <prolifedata/IHardwareProductBinding.h>


namespace prolifegql
{


// protected methods

// reimplemented (prolife::sdl::Sensors::CDeviceCollectionControllerCompBase)

bool CDeviceCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const prolife::sdl::Sensors::CDevicesListGqlRequest& devicesListRequest,
			prolife::sdl::Sensors::CDeviceItem& representationObject,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to create representation from object. Error: Attribute 'm_objectCollectionCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return false;
	}

	QByteArray objectId = objectCollectionIterator.GetObjectId();

	prolifedata::CDeviceInfo* deviceInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		deviceInfoPtr = dynamic_cast<prolifedata::CDeviceInfo*>(dataPtr.GetPtr());
	}

	if (deviceInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return false;
	}

	idoc::MetaInfoPtr metaInfo = objectCollectionIterator.GetDataMetaInfo();

	prolife::sdl::Sensors::DevicesListRequestInfo requestInfo = devicesListRequest.GetRequestInfo();

	if (requestInfo.items.isTypeIdRequested){
		representationObject.SetTypeId(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isIdRequested){
		representationObject.SetId(objectId);
	}

	if (requestInfo.items.isNameRequested){
		QByteArray deviceType = objectCollectionIterator.GetElementInfo("DeviceType").toByteArray();
		representationObject.SetName(deviceType);

		QByteArray macAddress = objectCollectionIterator.GetElementInfo("MacAddress").toByteArray();
		if (!macAddress.isEmpty()){
			representationObject.SetName(deviceType + " (" + macAddress + ")");
		}
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.SetDescription(metaInfo->GetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION).toString());
	}

	if (requestInfo.items.isPurchaseOrderIdRequested){
		representationObject.SetPurchaseOrderId(objectCollectionIterator.GetElementInfo("PurchaseOrderId").toByteArray());
	}

	if (requestInfo.items.isCustomerRequested){
		representationObject.SetCustomer(objectCollectionIterator.GetElementInfo("Customer").toByteArray());
	}

	if (requestInfo.items.isMacAddressRequested){
		representationObject.SetMacAddress(deviceInfoPtr->GetMacAddress());
	}

	if (requestInfo.items.isSerialNumberRequested){
		representationObject.SetSerialNumber(deviceInfoPtr->GetSerialNumber());
	}

	if (requestInfo.items.isProjectRequested){
		representationObject.SetProject(objectCollectionIterator.GetElementInfo("Project").toByteArray());
	}

	if (requestInfo.items.isDeviceTypeRequested){
		representationObject.SetDeviceType(objectCollectionIterator.GetElementInfo("DeviceType").toByteArray());
	}

	if (requestInfo.items.isProductUuidRequested){
		representationObject.SetProductUuid(objectCollectionIterator.GetElementInfo("ProductUuid").toByteArray());
	}

	if (requestInfo.items.isLicenseUuidRequested){
		representationObject.SetLicenseUuid(objectCollectionIterator.GetElementInfo("LicenseUuid").toByteArray());
	}

	if (requestInfo.items.isLicenseIdRequested){
		representationObject.SetLicenseId(objectCollectionIterator.GetElementInfo("LicenseId").toByteArray());
	}

	if (requestInfo.items.isLicenseNameRequested){
		representationObject.SetLicenseName(objectCollectionIterator.GetElementInfo("LicenseName").toByteArray());
	}

	if (requestInfo.items.isOrderIdRequested){
		representationObject.SetOrderId(objectCollectionIterator.GetElementInfo("OrderId").toByteArray());
	}

	if (requestInfo.items.isOrderUuidRequested){
		representationObject.SetOrderUuid(objectCollectionIterator.GetElementInfo("OrderUuid").toByteArray());
	}

	if (requestInfo.items.isStatusRequested){
		deviceInfoPtr->GetDeviceProductionStatus();
		representationObject.SetStatus(objectCollectionIterator.GetElementInfo("Status").toByteArray());

		prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
		QString statusName = prolifedata::GetNameFromDeviceProductionStatus(status);
		representationObject.SetStatus(statusName);
	}

	if (requestInfo.items.isStatusIdRequested){
		prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
		QByteArray statusId = prolifedata::GetIdFromDeviceProductionStatus(status);
		representationObject.SetStatusId(statusId);
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


istd::IChangeable* CDeviceCollectionControllerComp::CreateObjectFromRepresentation(
			const prolife::sdl::Sensors::CDeviceData& deviceDataRepresentation,
			QByteArray& newObjectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	if (!m_deviceInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_deviceInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return nullptr;
	}

	istd::TDelPtr<prolifedata::IDeviceInfo> deviceInstancePtr = m_deviceInfoFactCompPtr.CreateInstance();
	if (!deviceInstancePtr.IsValid()){
		errorMessage = QString("Unable to create device instance. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return nullptr;
	}

	prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(deviceInstancePtr.GetPtr());
	if (deviceInfoPtr == nullptr){
		errorMessage = QString("Unable to cast device instance to identifable object. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return nullptr;
	}

	QByteArray deviceId = deviceDataRepresentation.GetId();
	if (deviceId.isEmpty()){
		deviceId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	deviceInfoPtr->SetObjectUuid(deviceId);
	newObjectId = deviceId;

	prolifedata::COrderedIdentifiableDeviceInfo* oldDeviceInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr oldDeviceDataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(deviceId, oldDeviceDataPtr)){
		oldDeviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(oldDeviceDataPtr.GetPtr());
	}

	QString macAddress = deviceDataRepresentation.GetMacAddress();
	if (!macAddress.isEmpty()){
		bool ok = prolifedata::CheckDeviceMacAddressExists(deviceId, macAddress.toUtf8(), *m_objectCollectionCompPtr);
		if (!ok){
			errorMessage = QString("MAC-Address already exists");
			SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

			return nullptr;
		}
	}

	deviceInfoPtr->SetMacAddress(macAddress.toUtf8());

	QString serialNumber = deviceDataRepresentation.GetSerialNumber();
	if (!serialNumber.isEmpty()){
		bool ok = prolifedata::CheckDeviceSerialNumberExists(deviceId, serialNumber.toUtf8(), *m_objectCollectionCompPtr);
		if (!ok){
			errorMessage = QT_TR_NOOP("Serial Number already exists");
			SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

			return nullptr;
		}
	}

	deviceInfoPtr->SetSerialNumber(serialNumber.toUtf8());

	QString project = deviceDataRepresentation.GetProject();
	deviceInfoPtr->SetProject(project.toUtf8());

	QString orderId = deviceDataRepresentation.GetOrderId();
	deviceInfoPtr->SetOrderId(orderId.toUtf8());

	QByteArray oldOrderId;

	if (oldDeviceInfoPtr != nullptr){
		oldOrderId = oldDeviceInfoPtr->GetOrderId();

		prolifedata::IOrderInfo* oldOrderInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr oldOrderDataPtr;
		if (m_orderCollectionCompPtr->GetObjectData(oldOrderId, oldOrderDataPtr)){
			oldOrderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(oldOrderDataPtr.GetPtr());
		}

		// Remove device from the old order
		if (oldOrderInfoPtr != nullptr){
			imtbase::IObjectCollection* oldProductCollectionPtr = oldOrderInfoPtr->GetProducts();

			if (oldProductCollectionPtr != nullptr && orderId != oldOrderId){
				if (oldProductCollectionPtr->GetElementIds().contains(deviceId)){
					oldProductCollectionPtr->RemoveElement(deviceId);

					istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;

					// if (m_orderOperationContextControllerCompPtr.IsValid()){
					// 	operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, oldOrderId, oldOrderInfoPtr);
					// }

					// if (!m_orderCollectionCompPtr->SetObjectData(oldOrderId, *oldOrderInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
					// 	errorMessage = QString("Unable to update an order info").toUtf8();
					// 	SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

					// 	return nullptr;
					// }
				}
			}
		}
	}

	if (oldOrderId != orderId){
		// Add device to the new order info
		if (m_orderCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr orderDataPtr;
			if (m_orderCollectionCompPtr->GetObjectData(orderId.toUtf8(), orderDataPtr)){
				prolifedata::IOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(orderDataPtr.GetPtr());
				if (orderInfoPtr != nullptr){
					imtbase::IObjectCollection* orderProductCollectionPtr = orderInfoPtr->GetProducts();
					if (orderProductCollectionPtr != nullptr){
						istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
						objectLinkPtr.SetPtr(new imtbase::CObjectLink());

						objectLinkPtr->SetObjectUuid(deviceId);
						objectLinkPtr->SetFactoryId("HardwareInfo");

						orderProductCollectionPtr->InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), deviceId);

						// istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;

						// if (m_orderOperationContextControllerCompPtr.IsValid()){
						// 	operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, orderId, orderInfoPtr);
						// }

						if (!m_orderCollectionCompPtr->SetObjectData(orderId.toUtf8(), *orderInfoPtr, istd::IChangeable::CM_WITHOUT_REFS/*, operationContextPtr*/)){
							errorMessage = QString("Unable to update an order info").toUtf8();
							SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

							return nullptr;
						}
					}
				}
			}
		}
	}

	QString deviceDescription = deviceDataRepresentation.GetDescription();
	deviceInfoPtr->SetDescription(deviceDescription);

	QString status = deviceDataRepresentation.GetProductionStatus();
	prolifedata::IDeviceInfo::DeviceProductionStatus productionStatus = prolifedata::GetProductionStatusFromId(status.toUtf8());
	deviceInfoPtr->SetDeviceProductionStatus(productionStatus);

	QString licenseName = deviceDataRepresentation.GetLicenseName();
	deviceInfoPtr->SetConfigurationType(licenseName.toUtf8());

	QString deviceType = deviceDataRepresentation.GetDeviceType();
	if (deviceType.isEmpty()){
		errorMessage = QString("Device type cannot be empty");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return nullptr;
	}

	deviceInfoPtr->SetDeviceType(deviceType.toUtf8());

	name = deviceType;

	if (m_productCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_productCollectionCompPtr->GetObjectData(deviceType.toUtf8(), dataPtr)){
			imtlic::IProductInfo* remoteProductInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
			if (remoteProductInfoPtr != nullptr){
				name = remoteProductInfoPtr->GetName();
			}
		}
	}

	if (!macAddress.isEmpty()){
		name += " (" + macAddress + ")";
	}

	return deviceInstancePtr.PopPtr();
}


bool CDeviceCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const prolife::sdl::Sensors::CDeviceItemGqlRequest& deviceItemRequest,
			prolife::sdl::Sensors::CDeviceDataPayload& representationPayload,
			QString& errorMessage) const
{
	const prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableDeviceInfo*>(&data);
	if (deviceInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return false;
	}

	prolife::sdl::Sensors::DeviceItemRequestArguments arguments = deviceItemRequest.GetRequestedArguments();

	prolife::sdl::Sensors::CDeviceData deviceData;

	QByteArray id = arguments.input.GetId();
	deviceData.SetId(id);

	QByteArray macAddress = deviceInfoPtr->GetMacAddress();
	deviceData.SetMacAddress(macAddress);

	QByteArray serialNumber = deviceInfoPtr->GetSerialNumber();
	deviceData.SetSerialNumber(serialNumber);

	QByteArray orderId = deviceInfoPtr->GetOrderId();
	deviceData.SetOrderId(orderId);

	QString description = deviceInfoPtr->GetDescription();
	deviceData.SetDescription(description);

	prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
	QByteArray statusId = prolifedata::GetIdFromDeviceProductionStatus(status);
	deviceData.SetProductionStatus(statusId);

	QByteArray project = deviceInfoPtr->GetProject();
	deviceData.SetProject(project);

	QByteArray configurationType = deviceInfoPtr->GetConfigurationType();
	deviceData.SetLicenseName(configurationType);

	QByteArray productUuid = deviceInfoPtr->GetDeviceType();
	deviceData.SetDeviceType(productUuid);

	QString name = productUuid;

	if (m_productCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr productDataPtr;
		if (m_productCollectionCompPtr->GetObjectData(productUuid, productDataPtr)){
			imtlic::IProductInfo* remoteProductInfoPtr = dynamic_cast<imtlic::IProductInfo*>(productDataPtr.GetPtr());
			if (remoteProductInfoPtr != nullptr){
				name = remoteProductInfoPtr->GetName();
			}
		}
	}

	if (!macAddress.isEmpty()){
		name = name + " (" + macAddress + ")";
	}

	deviceData.SetName(name);

	representationPayload.SetDeviceData(deviceData);

	return true;
}


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

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;

	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_REMOVE, gqlRequest);
	}

	if (m_objectCollectionCompPtr->RemoveElement(objectId, operationContextPtr.GetPtr())){
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


