#include <prolifegql/CDeviceCollectionControllerComp.h>


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
#include <prolifedata/CGroupFilter.h>


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
	
	QByteArray objectId;
	if (retVal.Version_1_0->ObjectId){
		objectId = *retVal.Version_1_0->ObjectId;
	}
	
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(dataPtr.GetPtr());
		if (deviceInfoPtr != nullptr){
			QByteArray deviceType = deviceInfoPtr->GetDeviceType();
			QByteArray macAddress = deviceInfoPtr->GetMacAddress();
			
			idoc::MetaInfoPtr metaInfoPtr = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);
			if (metaInfoPtr.IsValid()){
				QString productName = metaInfoPtr->GetMetaInfo(prolifedata::IDeviceInfo::MIT_PRODUCT_NAME).toString();
				
				QString name = productName;
				if (name.isEmpty()){
					name = deviceType;
				}
				
				if (!macAddress.isEmpty()){
					name += " (" + macAddress + ")";
				}
				
				retVal.Version_1_0->Text = name;
			}
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
	QByteArray objectId = objectCollectionIterator.GetObjectId();

	const prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		deviceInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableDeviceInfo*>(dataPtr.GetPtr());
	}

	if (deviceInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return false;
	}

	idoc::MetaInfoPtr metaInfo = objectCollectionIterator.GetDataMetaInfo();
	if (!metaInfo.IsValid()){
		errorMessage = QString("Unable to create representation from object '%1'. Error: Meta info is invalid").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");
		
		return false;
	}
	
	QByteArray deviceType = deviceInfoPtr->GetDeviceType();

	sdl::prolife::Sensors::DevicesListRequestInfo requestInfo = devicesListRequest.GetRequestInfo();

	if (requestInfo.items.isIdRequested){
		representationObject.Id = objectId;
	}

	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = objectCollectionIterator.GetObjectTypeId();
		representationObject.TypeId = collectionObjectId;
	}
	
	QString productName = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_PRODUCT_NAME).toString();
	if (requestInfo.items.isNameRequested){
		representationObject.Name = productName;
		
		QString macAddress = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_MAC_ADDRESS).toString();
		if (!macAddress.isEmpty()){
			representationObject.Name = (deviceType + " (" + macAddress + ")");
		}
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.Description = metaInfo->GetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
	}

	if (requestInfo.items.isPurchaseIdRequested){
		representationObject.PurchaseId = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_PURCHASE_ID).toString().toUtf8();
	}

	if (requestInfo.items.isCustomerNameRequested){
		representationObject.CustomerName = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_CUSTOMER_NAME).toString().toUtf8();
	}

	if (requestInfo.items.isMacAddressRequested){
		representationObject.MacAddress = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_MAC_ADDRESS).toString();
	}

	if (requestInfo.items.isSerialNumberRequested){
		representationObject.SerialNumber = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_SERIAL_NUMBER).toString();
	}

	if (requestInfo.items.isProjectRequested){
		representationObject.Project = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_PROJECT).toString();
	}

	if (requestInfo.items.isDeviceTypeRequested){
		representationObject.DeviceType = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_PRODUCT_NAME).toString().toUtf8();
	}

	if (requestInfo.items.isProductUuidRequested){
		representationObject.ProductUuid = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_TYPE).toString().toUtf8();
	}

	if (requestInfo.items.isLicenseUuidRequested){
		representationObject.LicenseUuid = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_CONFIGURATION_TYPE).toString().toUtf8();
	}

	if (requestInfo.items.isLicenseIdRequested){
		representationObject.LicenseId = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_LICENSE_ID).toString().toUtf8();
	}

	if (requestInfo.items.isLicenseNameRequested){
		representationObject.LicenseName = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_LICENSE_NAME).toString().toUtf8();
	}

	if (requestInfo.items.isDeliveryIdRequested){
		representationObject.DeliveryId = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DELIVERY_ID).toString().toUtf8();
	}

	if (requestInfo.items.isSoftwareLinksCountRequested){
		representationObject.SoftwareLinksCount = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_COUNT_BINDED_LICENSES).toInt();
	}

	if (requestInfo.items.isOrderUuidRequested){
		representationObject.OrderUuid = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_ORDER_ID).toString().toUtf8();
	}

	if (requestInfo.items.isStatusRequested){
		prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
		QString statusName = prolifedata::GetNameFromDeviceProductionStatus(status);
		representationObject.Status = (statusName);
	}

	if (requestInfo.items.isStatusIdRequested){
		prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
		QByteArray statusId = prolifedata::GetIdFromDeviceProductionStatus(status);
		representationObject.StatusId = (statusId);
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
	sdl::prolife::Sensors::CDeviceData::V1_0& representationPayload,
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

	QByteArray id;
	if (arguments.input.Version_1_0->Id){
		id = *arguments.input.Version_1_0->Id;
	}

	representationPayload.Id = (id);

	QByteArray macAddress = deviceInfoPtr->GetMacAddress();
	representationPayload.MacAddress = (macAddress);

	QByteArray serialNumber = deviceInfoPtr->GetSerialNumber();
	representationPayload.SerialNumber = (serialNumber);

	QByteArray orderId = deviceInfoPtr->GetOrderId();
	representationPayload.OrderId = (orderId);

	QString description = deviceInfoPtr->GetDescription();
	representationPayload.Description = (description);

	prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
	QByteArray statusId = prolifedata::GetIdFromDeviceProductionStatus(status);
	representationPayload.ProductionStatus = (statusId);

	QByteArray project = deviceInfoPtr->GetProject();
	representationPayload.Project = (project);

	QByteArray configurationType = deviceInfoPtr->GetConfigurationType();
	representationPayload.LicenseName = (configurationType);

	QByteArray productUuid = deviceInfoPtr->GetDeviceType();
	representationPayload.DeviceType = (productUuid);

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
	
	if (!inputArguments.input.Version_1_0->Item){
		I_CRITICAL();
		
		return false;
	}
	
	sdl::prolife::Sensors::CDeviceData::V1_0 deviceData = *inputArguments.input.Version_1_0->Item;
	
	QByteArray objectId;
	if (inputArguments.input.Version_1_0->Id){
		objectId = *inputArguments.input.Version_1_0->Id;
	}

	prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(&object);
	Q_ASSERT(deviceInfoPtr != nullptr);

	deviceInfoPtr->ResetData();
	
	deviceInfoPtr->SetObjectUuid(objectId);
	
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


imtbase::CTreeItemModel* CDeviceCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
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


void CDeviceCollectionControllerComp::SetAdditionalFilters(
			const imtgql::CGqlRequest& gqlRequest,
			const imtgql::CGqlObject& /*viewParamsGql*/,
			iprm::CParamsSet* filterParams) const
{
	if (m_groupFilterParamJoinerCompPtr.IsValid()){
		if (!m_groupFilterParamJoinerCompPtr->JoinGroupFilterParam(gqlRequest, *filterParams)){
			SendWarningMessage(0, QString("Unable to join group filter param"), "CDeviceCollectionControllerComp");
		}
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
		operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext("Update", orderId, oldOrderInfoPtr);
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
		operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext("Update", orderId, orderInfoPtr);
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


