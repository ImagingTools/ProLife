#include <prolifegql/CDeviceCollectionControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CEnableableParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <iqt/iqt.h>
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

// reimplemented (sdl::prolife::Sensors::V1_0::CDeviceCollectionControllerCompBase)

bool CDeviceCollectionControllerComp::CreateRepresentationFromObject(
			const ::imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::prolife::Sensors::V1_0::CDevicesListGqlRequest& devicesListRequest,
			sdl::prolife::Sensors::CDeviceItem::V1_0& representationObject,
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

	sdl::prolife::Sensors::V1_0::DevicesListRequestInfo requestInfo = devicesListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.Id = std::make_unique<QByteArray>(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		representationObject.TypeId = std::make_unique<QByteArray>(collectionObjectId);
	}

	if (requestInfo.items.isNameRequested){
		QByteArray deviceType = objectCollectionIterator.GetElementInfo("DeviceType").toByteArray();
		representationObject.Name = std::make_unique<QString>(deviceType);

		QByteArray macAddress = objectCollectionIterator.GetElementInfo("MacAddress").toByteArray();
		if (!macAddress.isEmpty()){
			representationObject.Name = std::make_unique<QString>(deviceType + " (" + macAddress + ")");
		}
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.Description = std::make_unique<QString>(metaInfo->GetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION).toString());
	}

	if (requestInfo.items.isPurchaseOrderIdRequested){
		representationObject.PurchaseOrderId = std::make_unique<QByteArray>(objectCollectionIterator.GetElementInfo("PurchaseOrderId").toByteArray());
	}

	if (requestInfo.items.isCustomerRequested){
		representationObject.Customer = std::make_unique<QByteArray>(objectCollectionIterator.GetElementInfo("Customer").toByteArray());
	}

	if (requestInfo.items.isMacAddressRequested){
		representationObject.MacAddress = std::make_unique<QString>(deviceInfoPtr->GetMacAddress());
	}

	if (requestInfo.items.isSerialNumberRequested){
		representationObject.SerialNumber = std::make_unique<QString>(deviceInfoPtr->GetSerialNumber());
	}

	if (requestInfo.items.isProjectRequested){
		representationObject.Project = std::make_unique<QString>(objectCollectionIterator.GetElementInfo("Project").toString());
	}

	if (requestInfo.items.isDeviceTypeRequested){
		representationObject.DeviceType = std::make_unique<QString>(objectCollectionIterator.GetElementInfo("DeviceType").toString());
	}

	if (requestInfo.items.isProductUuidRequested){
		representationObject.ProductUuid = std::make_unique<QByteArray>(objectCollectionIterator.GetElementInfo("ProductUuid").toByteArray());
	}

	if (requestInfo.items.isLicenseUuidRequested){
		representationObject.LicenseUuid = std::make_unique<QByteArray>(objectCollectionIterator.GetElementInfo("LicenseUuid").toByteArray());
	}

	if (requestInfo.items.isLicenseIdRequested){
		representationObject.LicenseId = std::make_unique<QByteArray>(objectCollectionIterator.GetElementInfo("LicenseId").toByteArray());
	}

	if (requestInfo.items.isLicenseNameRequested){
		representationObject.LicenseName = std::make_unique<QString>(objectCollectionIterator.GetElementInfo("LicenseName").toString());
	}

	if (requestInfo.items.isOrderIdRequested){
		representationObject.OrderId = std::make_unique<QByteArray>(objectCollectionIterator.GetElementInfo("OrderId").toByteArray());
	}

	if (requestInfo.items.isSoftwareLinksCountRequested){
		representationObject.SoftwareLinksCount = std::make_unique<int>(objectCollectionIterator.GetElementInfo("SoftwareLinksCount").toInt());
	}

	if (requestInfo.items.isOrderUuidRequested){
		representationObject.OrderUuid = std::make_unique<QByteArray>(objectCollectionIterator.GetElementInfo("OrderUuid").toByteArray());
	}

	if (requestInfo.items.isStatusRequested){
		prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
		QString statusName = prolifedata::GetNameFromDeviceProductionStatus(status);
		representationObject.Status = std::make_unique<QString>(statusName);
	}

	if (requestInfo.items.isStatusIdRequested){
		prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
		QByteArray statusId = prolifedata::GetIdFromDeviceProductionStatus(status);
		representationObject.StatusId = std::make_unique<QString>(statusId);
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.Added = std::make_unique<QString>(added);
	}

	if (requestInfo.items.isLastModifiedRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.LastModified = std::make_unique<QString>(lastModified);
	}

	return true;
}


istd::IChangeable* CDeviceCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::prolife::Sensors::CDeviceData::V1_0& deviceDataRepresentation,
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

	if (deviceDataRepresentation.Id){
		newObjectId = *deviceDataRepresentation.Id;
	}

	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	deviceInfoPtr->SetObjectUuid(newObjectId);

	prolifedata::COrderedIdentifiableDeviceInfo* oldDeviceInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr oldDeviceDataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(newObjectId, oldDeviceDataPtr)){
		oldDeviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(oldDeviceDataPtr.GetPtr());
	}

	QString macAddress;
	if (deviceDataRepresentation.MacAddress){
		macAddress = *deviceDataRepresentation.MacAddress;
	}

	if (!macAddress.isEmpty()){
		bool ok = prolifedata::CheckDeviceMacAddressExists(newObjectId, macAddress.toUtf8(), *m_objectCollectionCompPtr);
		if (!ok){
			errorMessage = QString("MAC-Address already exists");
			SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

			return nullptr;
		}
	}

	deviceInfoPtr->SetMacAddress(macAddress.toUtf8());

	QString serialNumber;
	if (deviceDataRepresentation.SerialNumber){
		macAddress = *deviceDataRepresentation.SerialNumber;
	}

	if (!serialNumber.isEmpty()){
		bool ok = prolifedata::CheckDeviceSerialNumberExists(newObjectId, serialNumber.toUtf8(), *m_objectCollectionCompPtr);
		if (!ok){
			errorMessage = QT_TR_NOOP("Serial Number already exists");
			SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

			return nullptr;
		}
	}

	deviceInfoPtr->SetSerialNumber(serialNumber.toUtf8());

	QString project;
	if (deviceDataRepresentation.Project){
		project = *deviceDataRepresentation.Project;
		deviceInfoPtr->SetProject(project.toUtf8());
	}

	QString orderId;
	if (deviceDataRepresentation.OrderId){
		orderId = *deviceDataRepresentation.OrderId;
		deviceInfoPtr->SetOrderId(orderId.toUtf8());
	}

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
				if (oldProductCollectionPtr->GetElementIds().contains(newObjectId)){
					oldProductCollectionPtr->RemoveElement(newObjectId);

					istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
					if (m_orderOperationContextControllerCompPtr.IsValid()){
						operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext("Update", oldOrderId, *oldOrderInfoPtr);
					}

					if (!m_orderCollectionCompPtr->SetObjectData(oldOrderId, *oldOrderInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
						errorMessage = QString("Unable to update an order info").toUtf8();
						SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

						return nullptr;
					}
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

						objectLinkPtr->SetObjectUuid(newObjectId);
						objectLinkPtr->SetFactoryId("HardwareInfo");

						orderProductCollectionPtr->InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), newObjectId);

						istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
						if (m_orderOperationContextControllerCompPtr.IsValid()){
							operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext("Update", orderId.toUtf8(), *orderInfoPtr);
						}

						if (!m_orderCollectionCompPtr->SetObjectData(orderId.toUtf8(), *orderInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
							errorMessage = QString("Unable to update an order info").toUtf8();
							SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

							return nullptr;
						}
					}
				}
			}
		}
	}

	if (deviceDataRepresentation.Description){
		description = *deviceDataRepresentation.Description;
		deviceInfoPtr->SetDescription(description);
	}

	if (deviceDataRepresentation.ProductionStatus){
		QString status = *deviceDataRepresentation.ProductionStatus;
		prolifedata::IDeviceInfo::DeviceProductionStatus productionStatus = prolifedata::GetProductionStatusFromId(status.toUtf8());
		deviceInfoPtr->SetDeviceProductionStatus(productionStatus);
	}

	if (deviceDataRepresentation.LicenseName){
		QString licenseName = *deviceDataRepresentation.LicenseName;
		deviceInfoPtr->SetConfigurationType(licenseName.toUtf8());
	}

	QString deviceType;
	if (deviceDataRepresentation.DeviceType){
		deviceType = *deviceDataRepresentation.DeviceType;
	}

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
			const sdl::prolife::Sensors::V1_0::CDeviceItemGqlRequest& deviceItemRequest,
			sdl::prolife::Sensors::CDeviceDataPayload::V1_0& representationPayload,
			QString& errorMessage) const
{
	const prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableDeviceInfo*>(&data);
	if (deviceInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return false;
	}

	sdl::prolife::Sensors::V1_0::DeviceItemRequestArguments arguments = deviceItemRequest.GetRequestedArguments();

	sdl::prolife::Sensors::CDeviceData::V1_0 deviceData;

	QByteArray id;
	if (arguments.input.Id){
		id = *arguments.input.Id;
	}

	deviceData.Id = std::make_unique<QByteArray>(id);

	QByteArray macAddress = deviceInfoPtr->GetMacAddress();
	deviceData.MacAddress = std::make_unique<QString>(macAddress);

	QByteArray serialNumber = deviceInfoPtr->GetSerialNumber();
	deviceData.SerialNumber = std::make_unique<QString>(serialNumber);

	QByteArray orderId = deviceInfoPtr->GetOrderId();
	deviceData.OrderId = std::make_unique<QString>(orderId);

	QString description = deviceInfoPtr->GetDescription();
	deviceData.Description = std::make_unique<QString>(description);

	prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
	QByteArray statusId = prolifedata::GetIdFromDeviceProductionStatus(status);
	deviceData.ProductionStatus = std::make_unique<QString>(statusId);

	QByteArray project = deviceInfoPtr->GetProject();
	deviceData.Project = std::make_unique<QString>(project);

	QByteArray configurationType = deviceInfoPtr->GetConfigurationType();
	deviceData.LicenseName = std::make_unique<QString>(configurationType);

	QByteArray productUuid = deviceInfoPtr->GetDeviceType();
	deviceData.DeviceType = std::make_unique<QString>(productUuid);

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

	deviceData.Name = std::make_unique<QString>(name);

	representationPayload.DeviceData = std::make_unique<sdl::prolife::Sensors::CDeviceData::V1_0>(deviceData);

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
		errorMessage = QString("No object-ID could not be extracted from the request");
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

	return BaseClass::DeleteObject(gqlRequest, errorMessage);
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
		const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
		if (gqlContextPtr != nullptr){
			languageId = gqlContextPtr->GetLanguageId();
		}

		QString elementNameTr = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), name.toUtf8(), languageId, "prolifegql::CDeviceCollectionControllerComp");

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
							imtbase::IObjectCollection::DataPtr licenseDataPtr;
							if (m_licenseCollectionCompPtr->GetObjectData(licenseId, licenseDataPtr)){
								imtlic::ILicenseDefinition* licenseDefinitionPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
								if (licenseDefinitionPtr != nullptr){
									QString licenseName = licenseDefinitionPtr->GetLicenseName();
									QByteArray licenseDefinitionId = licenseDefinitionPtr->GetLicenseId();

									int childrenIndex = childrenModelPtr->InsertNewItem();
									childrenModelPtr->SetData("Value", licenseName + " (" + licenseDefinitionId + ")", childrenIndex);
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
		QString elementNameTr = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("No Licenses"), languageId, "prolifegql::CDeviceCollectionControllerComp");

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

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
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


bool CDeviceCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& rawGqlRequest,
			const sdl::prolife::Sensors::V1_0::CDeviceUpdateGqlRequest& deviceUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	return false;
}


} // namespace prolifegql


