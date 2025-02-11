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

sdl::imtbase::ImtCollection::CVisualStatus CDeviceCollectionControllerComp::OnGetObjectVisualStatus(
	const sdl::imtbase::ImtCollection::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CDeviceCollectionControllerComp");
		return sdl::imtbase::ImtCollection::CVisualStatus();
	}

	sdl::imtbase::ImtCollection::CVisualStatus retVal = BaseClass::OnGetObjectVisualStatus(getObjectVisualStatusRequest, gqlRequest, errorMessage);
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(*retVal.Version_1_0->ObjectId, dataPtr)){
		prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(dataPtr.GetPtr());
		if (deviceInfoPtr != nullptr){
			QByteArray deviceType = deviceInfoPtr->GetDeviceType();
			QByteArray macAddress = deviceInfoPtr->GetMacAddress();

			QString name = deviceType;

			if (m_productCollectionCompPtr.IsValid()){
				imtbase::IObjectCollection::DataPtr productDataPtr;
				if (m_productCollectionCompPtr->GetObjectData(deviceType, productDataPtr)){
					imtlic::IProductInfo* remoteProductInfoPtr = dynamic_cast<imtlic::IProductInfo*>(productDataPtr.GetPtr());
					if (remoteProductInfoPtr != nullptr){
						name = remoteProductInfoPtr->GetName();
					}
				}
			}

			if (!macAddress.isEmpty()){
				name += " (" + macAddress + ")";
			}

			retVal.Version_1_0->Text = name;
		}
	}

	return retVal;
}

// reimplemented (sdl::prolife::Sensors::CDeviceCollectionControllerCompBase)

bool CDeviceCollectionControllerComp::CreateRepresentationFromObject(
	const ::imtbase::IObjectCollectionIterator& objectCollectionIterator,
	const sdl::prolife::Sensors::CDevicesListGqlRequest& devicesListRequest,
	sdl::prolife::Sensors::CDeviceItem::V1_0& representationObject,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CDeviceCollectionControllerComp");
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

	sdl::prolife::Sensors::DevicesListRequestInfo requestInfo = devicesListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.Id = std::make_optional<QByteArray>(objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		representationObject.TypeId = std::make_optional<QByteArray>(collectionObjectId);
	}

	if (requestInfo.items.isNameRequested){
		QByteArray deviceType = objectCollectionIterator.GetElementInfo("DeviceType").toByteArray();
		representationObject.Name = std::make_optional<QString>(deviceType);

		QByteArray macAddress = objectCollectionIterator.GetElementInfo("MacAddress").toByteArray();
		if (!macAddress.isEmpty()){
			representationObject.Name = std::make_optional<QString>(deviceType + " (" + macAddress + ")");
		}
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.Description = std::make_optional<QString>(metaInfo->GetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION).toString());
	}

	if (requestInfo.items.isPurchaseOrderIdRequested){
		representationObject.PurchaseOrderId = std::make_optional<QByteArray>(objectCollectionIterator.GetElementInfo("PurchaseOrderId").toByteArray());
	}

	if (requestInfo.items.isCustomerRequested){
		representationObject.Customer = std::make_optional<QByteArray>(objectCollectionIterator.GetElementInfo("Customer").toByteArray());
	}

	if (requestInfo.items.isMacAddressRequested){
		representationObject.MacAddress = std::make_optional<QString>(deviceInfoPtr->GetMacAddress());
	}

	if (requestInfo.items.isSerialNumberRequested){
		representationObject.SerialNumber = std::make_optional<QString>(deviceInfoPtr->GetSerialNumber());
	}

	if (requestInfo.items.isProjectRequested){
		representationObject.Project = std::make_optional<QString>(objectCollectionIterator.GetElementInfo("Project").toString());
	}

	if (requestInfo.items.isDeviceTypeRequested){
		representationObject.DeviceType = std::make_optional<QString>(objectCollectionIterator.GetElementInfo("DeviceType").toString());
	}

	if (requestInfo.items.isProductUuidRequested){
		representationObject.ProductUuid = std::make_optional<QByteArray>(objectCollectionIterator.GetElementInfo("ProductUuid").toByteArray());
	}

	if (requestInfo.items.isLicenseUuidRequested){
		representationObject.LicenseUuid = std::make_optional<QByteArray>(objectCollectionIterator.GetElementInfo("LicenseUuid").toByteArray());
	}

	if (requestInfo.items.isLicenseIdRequested){
		representationObject.LicenseId = std::make_optional<QByteArray>(objectCollectionIterator.GetElementInfo("LicenseId").toByteArray());
	}

	if (requestInfo.items.isLicenseNameRequested){
		representationObject.LicenseName = std::make_optional<QString>(objectCollectionIterator.GetElementInfo("LicenseName").toString());
	}

	if (requestInfo.items.isOrderIdRequested){
		representationObject.OrderId = std::make_optional<QByteArray>(objectCollectionIterator.GetElementInfo("OrderId").toByteArray());
	}

	if (requestInfo.items.isSoftwareLinksCountRequested){
		representationObject.SoftwareLinksCount = std::make_optional<int>(objectCollectionIterator.GetElementInfo("SoftwareLinksCount").toInt());
	}

	if (requestInfo.items.isOrderUuidRequested){
		representationObject.OrderUuid = std::make_optional<QByteArray>(objectCollectionIterator.GetElementInfo("OrderUuid").toByteArray());
	}

	if (requestInfo.items.isStatusRequested){
		prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
		QString statusName = prolifedata::GetNameFromDeviceProductionStatus(status);
		representationObject.Status = std::make_optional<QString>(statusName);
	}

	if (requestInfo.items.isStatusIdRequested){
		prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
		QByteArray statusId = prolifedata::GetIdFromDeviceProductionStatus(status);
		representationObject.StatusId = std::make_optional<QString>(statusId);
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


istd::IChangeable* CDeviceCollectionControllerComp::CreateObjectFromRepresentation(
	const sdl::prolife::Sensors::CDeviceData::V1_0& deviceDataRepresentation,
	QByteArray& newObjectId,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CDeviceCollectionControllerComp");
		return nullptr;
	}

	if (!m_deviceInfoFactCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'DeviceInfoFactory' was not set", "CDeviceCollectionControllerComp");
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

	prolifedata::COrderedIdentifiableDeviceInfo* oldDeviceInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr oldDeviceDataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(newObjectId, oldDeviceDataPtr)){
		oldDeviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(oldDeviceDataPtr.GetPtr());
	}

	if (deviceDataRepresentation.Id){
		newObjectId = *deviceDataRepresentation.Id;
	}

	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	deviceInfoPtr->SetObjectUuid(newObjectId);

	if (!FillObjectFromRepresentation(deviceDataRepresentation, *deviceInfoPtr, newObjectId, errorMessage)){
		errorMessage = QString("Unable to create device from representaion. Error: '%1'").arg(errorMessage);

		return nullptr;
	}

	QString orderId = *deviceDataRepresentation.OrderId;
	if (!orderId.isEmpty()){
		if (!AddDeviceToOrder(newObjectId, orderId.toUtf8())){
			errorMessage = QString("Unable to add device. Error: Add device to order failed");
			return nullptr;
		}
	}

	return deviceInstancePtr.PopPtr();
}


bool CDeviceCollectionControllerComp::CreateRepresentationFromObject(
	const istd::IChangeable& data,
	const sdl::prolife::Sensors::CDeviceItemGqlRequest& deviceItemRequest,
	sdl::prolife::Sensors::CDeviceDataPayload::V1_0& representationPayload,
	QString& errorMessage) const
{
	const prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableDeviceInfo*>(&data);
	if (deviceInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return false;
	}

	sdl::prolife::Sensors::DeviceItemRequestArguments arguments = deviceItemRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		I_CRITICAL();

		return false;
	}

	sdl::prolife::Sensors::CDeviceData::V1_0 deviceData;

	QByteArray id;
	if (arguments.input.Version_1_0->Id){
		id = *arguments.input.Version_1_0->Id;
	}

	deviceData.Id = std::make_optional<QByteArray>(id);

	QByteArray macAddress = deviceInfoPtr->GetMacAddress();
	deviceData.MacAddress = std::make_optional<QString>(macAddress);

	QByteArray serialNumber = deviceInfoPtr->GetSerialNumber();
	deviceData.SerialNumber = std::make_optional<QString>(serialNumber);

	QByteArray orderId = deviceInfoPtr->GetOrderId();
	deviceData.OrderId = std::make_optional<QString>(orderId);

	QString description = deviceInfoPtr->GetDescription();
	deviceData.Description = std::make_optional<QString>(description);

	prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
	QByteArray statusId = prolifedata::GetIdFromDeviceProductionStatus(status);
	deviceData.ProductionStatus = std::make_optional<QString>(statusId);

	QByteArray project = deviceInfoPtr->GetProject();
	deviceData.Project = std::make_optional<QString>(project);

	QByteArray configurationType = deviceInfoPtr->GetConfigurationType();
	deviceData.LicenseName = std::make_optional<QString>(configurationType);

	QByteArray productUuid = deviceInfoPtr->GetDeviceType();
	deviceData.DeviceType = std::make_optional<QString>(productUuid);

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

	deviceData.Name = std::make_optional<QString>(name);

	representationPayload.DeviceData = std::make_optional<sdl::prolife::Sensors::CDeviceData::V1_0>(deviceData);

	return true;
}


bool CDeviceCollectionControllerComp::UpdateObjectFromRepresentationRequest(
	const imtgql::CGqlRequest& /*rawGqlRequest*/,
	const sdl::prolife::Sensors::CDeviceUpdateGqlRequest& deviceUpdateRequest,
	istd::IChangeable& object,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CDeviceCollectionControllerComp");
		return false;
	}

	sdl::prolife::Sensors::DeviceUpdateRequestArguments inputArguments = deviceUpdateRequest.GetRequestedArguments();
	if (!inputArguments.input.Version_1_0){
		I_CRITICAL();

		return false;
	}

	sdl::prolife::Sensors::CDeviceData::V1_0 deviceData = *inputArguments.input.Version_1_0->Item;
	QByteArray objectId = *inputArguments.input.Version_1_0->Id;

	prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(&object);
	Q_ASSERT(deviceInfoPtr != nullptr);

	deviceInfoPtr->ResetData();

	if (!FillObjectFromRepresentation(deviceData, *deviceInfoPtr, objectId, errorMessage)){
		errorMessage = QString("Unable to update device. Error: '%1'").arg(errorMessage);
		return false;
	}

	prolifedata::COrderedIdentifiableDeviceInfo* oldDeviceInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr oldDeviceDataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, oldDeviceDataPtr)){
		oldDeviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(oldDeviceDataPtr.GetPtr());
	}

	if (deviceInfoPtr != nullptr && oldDeviceInfoPtr != nullptr){
		QByteArray newOrderId = deviceInfoPtr->GetOrderId();
		QByteArray oldOrderId = oldDeviceInfoPtr->GetOrderId();

		if (newOrderId.isEmpty() && !oldOrderId.isEmpty()){
			if (!RemoveDeviceFromOrder(objectId, oldOrderId)){
				errorMessage = QString("Unable to update device. Error: Remove device from order failed");
				return false;
			}
		}
		else if (!newOrderId.isEmpty() && oldOrderId.isEmpty()){
			if (!AddDeviceToOrder(objectId, newOrderId)){
				errorMessage = QString("Unable to add device. Error: Add device to order failed");
				return false;
			}
		}
		else if (!newOrderId.isEmpty() && !oldOrderId.isEmpty() && newOrderId != oldOrderId){
			if (!AddDeviceToOrder(objectId, newOrderId)){
				errorMessage = QString("Unable to add device. Error: Add device to order failed");
				return false;
			}

			if (!RemoveDeviceFromOrder(objectId, oldOrderId)){
				errorMessage = QString("Unable to update device. Error: Remove device from order failed");
				return false;
			}
		}
	}

	return true;
}


imtbase::CTreeItemModel* CDeviceCollectionControllerComp::DeleteObject(
	const imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ObjectCollection' was not set", "CDeviceCollectionControllerComp");
		return nullptr;
	}

	if (!m_bindingCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'BindingCollection' was not set", "CDeviceCollectionControllerComp");
		return nullptr;
	}

	if (!m_softwareProductCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SoftwareProductCollection' was not set", "CDeviceCollectionControllerComp");
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

	prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr deviceDataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, deviceDataPtr)){
		deviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(deviceDataPtr.GetPtr());
	}

	if (deviceInfoPtr != nullptr){
		QByteArray orderId = deviceInfoPtr->GetOrderId();
		if (!orderId.isEmpty()){
			if (!RemoveDeviceFromOrder(objectId, orderId)){
				SendWarningMessage(0,
								   QString("Remove device '%1' from order '%2' failed")
									   .arg(qPrintable(objectId), qPrintable(orderId)),
								   "CDeviceCollectionControllerComp");
			}
		}
	}

	return BaseClass::DeleteObject(gqlRequest, errorMessage);
}


imtbase::CTreeItemModel* CDeviceCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_licenseCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'LicenseCollection' was not set", "CDeviceCollectionControllerComp");
		return nullptr;
	}

	if (!m_bindingCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'BindingCollection' was not set", "CDeviceCollectionControllerComp");
		return nullptr;
	}

	if (!m_softwareProductCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SoftwareProductCollection' was not set", "CDeviceCollectionControllerComp");
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


// private methods

bool CDeviceCollectionControllerComp::FillObjectFromRepresentation(
	const sdl::prolife::Sensors::CDeviceData::V1_0& representation,
	istd::IChangeable& object,
	QByteArray& objectId,
	QString& errorMessage) const
{
	prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(&object);
	if (deviceInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return false;
	}

	QString macAddress;
	if (representation.MacAddress){
		macAddress = *representation.MacAddress;
	}

	if (!macAddress.isEmpty()){
		bool ok = prolifedata::CheckDeviceMacAddressExists(objectId, macAddress.toUtf8(), *m_objectCollectionCompPtr);
		if (!ok){
			errorMessage = QString("MAC-Address already exists");
			SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

			return false;
		}
	}

	deviceInfoPtr->SetMacAddress(macAddress.toUtf8());

	QString serialNumber;
	if (representation.SerialNumber){
		serialNumber = *representation.SerialNumber;
	}

	if (!serialNumber.isEmpty()){
		bool ok = prolifedata::CheckDeviceSerialNumberExists(objectId, serialNumber.toUtf8(), *m_objectCollectionCompPtr);
		if (!ok){
			errorMessage = QT_TR_NOOP("Serial Number already exists");
			SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

			return false;
		}
	}

	deviceInfoPtr->SetSerialNumber(serialNumber.toUtf8());

	QString project;
	if (representation.Project){
		project = *representation.Project;
		deviceInfoPtr->SetProject(project.toUtf8());
	}

	QString orderId;
	if (representation.OrderId){
		orderId = *representation.OrderId;
		deviceInfoPtr->SetOrderId(orderId.toUtf8());
	}

	if (representation.Description){
		deviceInfoPtr->SetDescription(*representation.Description);
	}

	if (representation.ProductionStatus){
		QString status = *representation.ProductionStatus;
		prolifedata::IDeviceInfo::DeviceProductionStatus productionStatus = prolifedata::GetProductionStatusFromId(status.toUtf8());
		deviceInfoPtr->SetDeviceProductionStatus(productionStatus);
	}

	QString licenseName;
	if (representation.LicenseName){
		licenseName = *representation.LicenseName;
	}

	if (licenseName.isEmpty()){
		errorMessage = QString("Configuration cannot be empty");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return false;
	}

	deviceInfoPtr->SetConfigurationType(licenseName.toUtf8());

	QString deviceType;
	if (representation.DeviceType){
		deviceType = *representation.DeviceType;
	}

	if (deviceType.isEmpty()){
		errorMessage = QString("Device type cannot be empty");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return false;
	}

	deviceInfoPtr->SetDeviceType(deviceType.toUtf8());

	return true;
}


bool CDeviceCollectionControllerComp::RemoveDeviceFromOrder(const QByteArray& deviceId, const QByteArray& orderId) const
{
	if (!m_orderCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'OrderCollection' was not set", "CDeviceCollectionControllerComp");
		return false;
	}

	prolifedata::IOrderInfo* oldOrderInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr oldOrderDataPtr;
	if (m_orderCollectionCompPtr->GetObjectData(orderId, oldOrderDataPtr)){
		oldOrderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(oldOrderDataPtr.GetPtr());
	}

	if (oldOrderInfoPtr == nullptr){
		SendErrorMessage(0,
						 QString("Unable to remove device '%1' from order '%2'. Error: Order does not exists")
							 .arg(qPrintable(deviceId), qPrintable(orderId)),
						 "CDeviceCollectionControllerComp");
		return false;
	}

	imtbase::IObjectCollection* productCollectionPtr = oldOrderInfoPtr->GetProducts();
	if (productCollectionPtr == nullptr){
		SendErrorMessage(0,
						 QString("Unable to remove device '%1' from order '%2'. Error: Product collection from order is invalid")
							 .arg(qPrintable(deviceId), qPrintable(orderId)),
						 "CDeviceCollectionControllerComp");
		return false;
	}

	QByteArrayList elementIds = productCollectionPtr->GetElementIds();
	if (!elementIds.contains(deviceId)){
		SendErrorMessage(0,
						 QString("Unable to remove device '%1' from order '%2'. Error: The device does not exist in this order")
							 .arg(qPrintable(deviceId), qPrintable(orderId)),
						 "CDeviceCollectionControllerComp");
		return false;
	}

	if (!productCollectionPtr->RemoveElement(deviceId)){
		SendErrorMessage(0,
						 QString("Unable to remove device '%1' from order '%2'. Error: Removing element from product collection failed")
							 .arg(qPrintable(deviceId), qPrintable(orderId)),
						 "CDeviceCollectionControllerComp");
		return false;
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_orderOperationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext("Update", orderId, *oldOrderInfoPtr);
	}

	if (!m_orderCollectionCompPtr->SetObjectData(orderId, *oldOrderInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
		SendErrorMessage(0,
						 QString("Unable to remove device '%1' from order '%2'. Error: Updating an order in a collection failed")
							 .arg(qPrintable(deviceId), qPrintable(orderId)),
						 "CDeviceCollectionControllerComp");
		return false;
	}

	return true;
}


bool CDeviceCollectionControllerComp::AddDeviceToOrder(const QByteArray& deviceId, const QByteArray& orderId) const
{
	if (!m_orderCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'OrderCollection' was not set", "CDeviceCollectionControllerComp");
		return false;
	}

	prolifedata::IOrderInfo* orderInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr orderDataPtr;
	if (m_orderCollectionCompPtr->GetObjectData(orderId, orderDataPtr)){
		orderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(orderDataPtr.GetPtr());
	}

	if (orderInfoPtr == nullptr){
		SendErrorMessage(0,
						 QString("Unable to add device '%1' to order '%2'. Error: Order does not exists")
							 .arg(qPrintable(deviceId), qPrintable(orderId)),
						 "CDeviceCollectionControllerComp");
		return false;
	}

	imtbase::IObjectCollection* productCollectionPtr = orderInfoPtr->GetProducts();
	if (orderInfoPtr == nullptr){
		SendErrorMessage(0,
						 QString("Unable to add device '%1' to order '%2'. Error: Product collection from order is invalid")
							 .arg(qPrintable(deviceId), qPrintable(orderId)),
						 "CDeviceCollectionControllerComp");
		return false;
	}

	QByteArrayList elementIds = productCollectionPtr->GetElementIds();
	if (elementIds.contains(deviceId)){
		SendErrorMessage(0,
						 QString("Unable to add device '%1' to order '%2'. Error: The device already exists in this order")
							 .arg(qPrintable(deviceId), qPrintable(orderId)),
						 "CDeviceCollectionControllerComp");
		return false;
	}

	istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
	objectLinkPtr.SetPtr(new imtbase::CObjectLink());

	objectLinkPtr->SetObjectUuid(deviceId);
	objectLinkPtr->SetFactoryId("HardwareInfo");

	QByteArray objectId = productCollectionPtr->InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), deviceId);
	if (objectId.isEmpty()){
		SendErrorMessage(0,
						 QString("Unable to add device '%1' to order '%2'. Error: Adding an order in a collection failed")
							 .arg(qPrintable(deviceId), qPrintable(orderId)),
						 "CDeviceCollectionControllerComp");
		return false;
	}

	istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
	if (m_orderOperationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext("Update", orderId, *orderInfoPtr);
	}

	if (!m_orderCollectionCompPtr->SetObjectData(orderId, *orderInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
		SendErrorMessage(0,
						 QString("Unable to remove device '%1' from order '%2'. Error: Updating an order in a collection failed")
							 .arg(qPrintable(deviceId), qPrintable(orderId)),
						 "CDeviceCollectionControllerComp");
		return false;
	}

	return true;
}


} // namespace prolifegql


