// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

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
	if (!errorMessage.isEmpty()){
		return sdl::imtbase::ImtCollection::CVisualStatus();
	}

	if (!retVal.Version_1_0.has_value()){
		Q_ASSERT(false);
		return sdl::imtbase::ImtCollection::CVisualStatus();
	}

	QByteArray objectId;
	if (retVal.Version_1_0->objectId){
		objectId = *retVal.Version_1_0->objectId;
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

				retVal.Version_1_0->text = name;
			}
		}
	}

	return retVal;
}


sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload CDeviceCollectionControllerComp::OnGetElementMetaInfo(
			const sdl::imtbase::ImtCollection::CGetElementMetaInfoGqlRequest& getElementMetaInfoRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CGetElementMetaInfoPayload response;
	response.Version_1_0.Emplace();

	sdl::imtbase::ImtCollection::GetElementMetaInfoRequestArguments arguments = getElementMetaInfoRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		Q_ASSERT(false);
		return response;
	}

	QByteArray objectId;
	if (arguments.input.Version_1_0->elementId){
		objectId = *arguments.input.Version_1_0->elementId;
	}

	int softwareCount = 0;
	QString parameterData;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_bindingCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const prolifedata::IHardwareProductBinding* bindingInfoPtr = dynamic_cast<const prolifedata::IHardwareProductBinding*>(dataPtr.GetPtr());
		if (bindingInfoPtr != nullptr){
			QByteArrayList softwareIds = bindingInfoPtr->GetSoftwareIds();
			softwareCount = softwareIds.size();
			for (const QByteArray& softwareId : softwareIds){
				imtbase::IObjectCollection::DataPtr productDataPtr;
				if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, productDataPtr)){
					imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
					if (productInstanceInfoPtr != nullptr){
						QByteArray serialNumber = productInstanceInfoPtr->GetSerialNumber();
						const imtbase::ICollectionInfo& licenseList = productInstanceInfoPtr->GetLicenseInstances();
						imtbase::ICollectionInfo::Ids elementsIds = licenseList.GetElementIds();

						for (const QByteArray& licenseId : elementsIds){
							imtbase::IObjectCollection::DataPtr licenseDataPtr;
							if (m_licenseCollectionCompPtr->GetObjectData(licenseId, licenseDataPtr)){
								imtlic::ILicenseDefinition* licenseDefinitionPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
								if (licenseDefinitionPtr != nullptr){
									QString licenseName = licenseDefinitionPtr->GetLicenseName();
									if (serialNumber.isEmpty()){
										parameterData += licenseName + "\n";
									}
									else{
										parameterData += licenseName + " (" + serialNumber + ")\n";
									}
								}
							}
						}
					}
				}
			}
		}
	}

	sdl::imtbase::ImtCollection::CElementMetaInfo::V1_0 elementMetaInfo;
	sdl::imtbase::ImtBaseTypes::CParameter::V1_0 parameter;
	parameter.id = QByteArrayLiteral("Licenses");
	parameter.typeId = "";
	parameter.name = QStringLiteral("Licenses") + " (" + QString::number(softwareCount) + ")";

	if (parameterData.isEmpty()){
		parameterData = QStringLiteral("No Licenses");
	}
	
	parameter.data = parameterData;
	
	imtsdl::TElementList<sdl::imtbase::ImtBaseTypes::CParameter::V1_0> infoParams;
	infoParams << parameter;

	elementMetaInfo.infoParams = infoParams;
	response.Version_1_0->elementMetaInfo = elementMetaInfo;

	return response;
}


bool CDeviceCollectionControllerComp::OnBeforeRemoveElements(
			const QByteArrayList& elementIds,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	for (const QByteArray& objectId : elementIds){
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
								return false;
							}
						}
					}
				}
			}
		}
	}

	for (const QByteArray& objectId : elementIds){
		prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr deviceDataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(objectId, deviceDataPtr)){
			deviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(deviceDataPtr.GetPtr());
		}

		if (deviceInfoPtr != nullptr){
			QByteArray orderId = deviceInfoPtr->GetOrderId();
			if (!orderId.isEmpty()){
				if (!RemoveDeviceFromOrder(objectId, orderId)){
					SendWarningMessage(	0,
										QString("Remove device '%1' from order '%2' failed")
										.arg(qPrintable(objectId), qPrintable(orderId)),
										"CDeviceCollectionControllerComp");
				}
			}
		}
	}

	return true;
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

	sdl::prolife::Sensors::DevicesListRequestInfo requestInfo = devicesListRequest.GetRequestInfo();
	if (requestInfo.items.isIdRequested){
		representationObject.id = objectId;
	}

	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = objectCollectionIterator.GetObjectTypeId();
		representationObject.typeId = collectionObjectId;
	}

	QString productName = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_PRODUCT_NAME).toString();
	if (requestInfo.items.isNameRequested){
		representationObject.name = productName;

		QString macAddress = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_MAC_ADDRESS).toString();
		if (!macAddress.isEmpty()){
			representationObject.name = (productName + " (" + macAddress + ")");
		}
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.description = metaInfo->GetMetaInfo(imtbase::ICollectionInfo::EIT_DESCRIPTION).toString();
	}

	QString scheme = "applink";
	if (requestInfo.items.isPurchaseIdLinkRequested){
		sdl::imtbase::ImtBaseTypes::CObjectLink::V1_0 objectLink;
		objectLink.id = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_ORDER_ID).toString().toUtf8();
		objectLink.typeId = QByteArrayLiteral("Order");
		objectLink.name = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_PURCHASE_ID).toString().toUtf8();

		sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0 urlParam;
		urlParam.scheme = scheme;
		urlParam.path = QStringLiteral("Orders/Order");
		if (!(*objectLink.id).isEmpty()){
			urlParam.path = *urlParam.path + QStringLiteral("/") + *objectLink.id;
		}
		objectLink.url = urlParam;

		representationObject.purchaseIdLink = objectLink;
	}

	if (requestInfo.items.isCustomerLinkRequested){
		sdl::imtbase::ImtBaseTypes::CObjectLink::V1_0 objectLink;
		objectLink.id = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_CUSTOMER_ID).toString().toUtf8();
		objectLink.typeId = QByteArrayLiteral("Account");
		objectLink.name = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_CUSTOMER_NAME).toString().toUtf8();

		sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0 urlParam;
		urlParam.scheme = scheme;
		urlParam.path = QStringLiteral("Accounts/Account");
		if (!(*objectLink.id).isEmpty()){
			urlParam.path = *urlParam.path + QStringLiteral("/") + *objectLink.id;
		}
		objectLink.url = urlParam;

		representationObject.customerLink = objectLink;
	}

	if (requestInfo.items.isMacAddressRequested){
		representationObject.macAddress = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_MAC_ADDRESS).toString();
	}

	if (requestInfo.items.isSerialNumberRequested){
		representationObject.serialNumber = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_SERIAL_NUMBER).toString();
	}

	if (requestInfo.items.isProjectRequested){
		representationObject.project = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_PROJECT).toString();
	}

	if (requestInfo.items.isDeviceTypeRequested){
		representationObject.deviceType = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_TYPE).toString().toUtf8();
	}

	if (requestInfo.items.isProductNameRequested){
		representationObject.productName = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_PRODUCT_NAME).toString().toUtf8();
	}

	if (requestInfo.items.isProductUuidRequested){
		representationObject.productUuid = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DEVICE_TYPE).toString().toUtf8();
	}

	if (requestInfo.items.isLicenseUuidRequested){
		representationObject.licenseUuid = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_CONFIGURATION_TYPE).toString().toUtf8();
	}

	if (requestInfo.items.isLicenseIdRequested){
		representationObject.licenseId = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_LICENSE_ID).toString().toUtf8();
	}

	if (requestInfo.items.isLicenseNameRequested){
		representationObject.licenseName = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_LICENSE_NAME).toString().toUtf8();
	}

	if (requestInfo.items.isDeliveryIdLinkRequested){
		sdl::imtbase::ImtBaseTypes::CObjectLink::V1_0 objectLink;
		objectLink.id = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_ORDER_ID).toString().toUtf8();
		objectLink.typeId = QByteArrayLiteral("Order");
		objectLink.name = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_DELIVERY_ID).toString().toUtf8();

		sdl::imtbase::ImtBaseTypes::CUrlParam::V1_0 urlParam;
		urlParam.scheme = scheme;
		urlParam.path = QStringLiteral("Orders/Order");
		if (!(*objectLink.id).isEmpty()){
			urlParam.path = *urlParam.path + QStringLiteral("/") + *objectLink.id;
		}
		objectLink.url = urlParam;

		representationObject.deliveryIdLink = objectLink;
	}

	if (requestInfo.items.isInUseRequested){
		representationObject.inUse = metaInfo->GetMetaInfo(prolifedata::IDeviceInfo::MIT_IN_USE).toBool();
	}

	if (requestInfo.items.isStatusRequested){
		prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
		QString statusName = prolifedata::GetNameFromDeviceProductionStatus(status);
		representationObject.status = (statusName);
	}

	if (requestInfo.items.isStatusIdRequested){
		prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
		QByteArray statusId = prolifedata::GetIdFromDeviceProductionStatus(status);
		representationObject.statusId = (statusId);
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

	return true;
}


istd::IChangeableUniquePtr CDeviceCollectionControllerComp::CreateObjectFromRepresentation(
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

	istd::TUniqueInterfacePtr<prolifedata::IDeviceInfo> deviceInstancePtr = m_deviceInfoFactCompPtr.CreateInstance();
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

	if (deviceDataRepresentation.id){
		newObjectId = *deviceDataRepresentation.id;
	}

	if (newObjectId.isEmpty()){
		newObjectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	deviceInfoPtr->SetObjectUuid(newObjectId);

	if (!FillObjectFromRepresentation(deviceDataRepresentation, *deviceInfoPtr, newObjectId, errorMessage)){
		errorMessage = QString("Unable to create sensor. Error: '%1'").arg(errorMessage);

		return nullptr;
	}

	QString orderId = *deviceDataRepresentation.orderId;
	if (!orderId.isEmpty()){
		if (!AddDeviceToOrder(newObjectId, orderId.toUtf8())){
			errorMessage = QString("Unable to add sensor. Error: Add device to order failed");
			return nullptr;
		}
	}

	istd::IChangeableUniquePtr retVal;
	retVal.MoveCastedPtr<prolifedata::IDeviceInfo>(deviceInstancePtr);

	return retVal;
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
	if (arguments.input.Version_1_0->id){
		id = *arguments.input.Version_1_0->id;
	}

	representationPayload.id = (id);

	QByteArray macAddress = deviceInfoPtr->GetMacAddress();
	representationPayload.macAddress = (macAddress);

	QByteArray serialNumber = deviceInfoPtr->GetSerialNumber();
	representationPayload.serialNumber = (serialNumber);

	QByteArray orderId = deviceInfoPtr->GetOrderId();
	representationPayload.orderId = (orderId);

	QString description = deviceInfoPtr->GetDescription();
	representationPayload.description = (description);

	prolifedata::IDeviceInfo::DeviceProductionStatus status = deviceInfoPtr->GetDeviceProductionStatus();
	QByteArray statusId = prolifedata::GetIdFromDeviceProductionStatus(status);
	representationPayload.productionStatus = (statusId);

	QByteArray project = deviceInfoPtr->GetProject();
	representationPayload.project = (project);

	QByteArray configurationType = deviceInfoPtr->GetConfigurationType();
	representationPayload.licenseName = (configurationType);

	QByteArray productUuid = deviceInfoPtr->GetDeviceType();
	representationPayload.deviceType = (productUuid);

	imtsdl::TElementList<sdl::prolife::Sensors::CSoftwareBindingInfo::V1_0> softwareBindingInfoList;

	QByteArrayList softwareIds = GetBindedSoftware(id);
	for (const QByteArray& softwareId : softwareIds){
		sdl::prolife::Sensors::CSoftwareBindingInfo::V1_0 softwareBindingInfo;
		if (GetSoftwareInfo(softwareId, softwareBindingInfo)){
			softwareBindingInfoList << softwareBindingInfo;
		}
	}

	representationPayload.softwareBindingInfos = softwareBindingInfoList;
	representationPayload.internalUse = deviceInfoPtr->IsInternalUse();

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

	if (!inputArguments.input.Version_1_0->item){
		I_CRITICAL();

		return false;
	}

	sdl::prolife::Sensors::CDeviceData::V1_0 deviceData = *inputArguments.input.Version_1_0->item;

	QByteArray objectId;
	if (inputArguments.input.Version_1_0->id){
		objectId = *inputArguments.input.Version_1_0->id;
	}

	prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(&object);
	Q_ASSERT(deviceInfoPtr != nullptr);

	deviceInfoPtr->ResetData();

	deviceInfoPtr->SetObjectUuid(objectId);

	if (!FillObjectFromRepresentation(deviceData, *deviceInfoPtr, objectId, errorMessage)){
		errorMessage = QString("Unable to update sensor. Error: '%1'").arg(errorMessage);
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


void CDeviceCollectionControllerComp::SetAdditionalFilters(
			const imtgql::CGqlRequest& gqlRequest,
			const imtgql::CGqlParamObject& /*viewParamsGql*/,
			iprm::CParamsSet* filterParams) const
{
	if (m_groupFilterParamJoinerCompPtr.IsValid()){
		if (!m_groupFilterParamJoinerCompPtr->JoinGroupFilterParam(gqlRequest, *filterParams)){
			SendWarningMessage(0, QString("Unable to join group filter param"), "CDeviceCollectionControllerComp");
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CDeviceCollectionControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_timeFilterParamRepresentationControllerPtr.SetPtr(new imtserverapp::CTimeFilterParamRepresentationController("LicenseCreationTimeFilter"));
	RegisterFilterToSelectionParams(m_licenseCreationTimeParam, *m_timeFilterParamRepresentationControllerPtr.GetPtr());
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
	if (representation.macAddress){
		macAddress = *representation.macAddress;
	}

	if (!macAddress.isEmpty()){
		bool ok = prolifedata::CheckDeviceMacAddressExists(objectId, macAddress.toUtf8(), *m_objectCollectionCompPtr);
		if (!ok){
			errorMessage = QString("MAC-Address '%1' already exists").arg(macAddress);
			SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

			return false;
		}
	}

	deviceInfoPtr->SetMacAddress(macAddress.toUtf8());

	QString serialNumber;
	if (representation.serialNumber){
		serialNumber = *representation.serialNumber;
	}

	if (!serialNumber.isEmpty()){
		bool ok = prolifedata::CheckDeviceSerialNumberExists(objectId, serialNumber.toUtf8(), *m_objectCollectionCompPtr);
		if (!ok){
			errorMessage = QT_TR_NOOP(QString("Serial Number '%1' already exists").arg(serialNumber));
			SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

			return false;
		}
	}

	deviceInfoPtr->SetSerialNumber(serialNumber.toUtf8());

	QString project;
	if (representation.project){
		project = *representation.project;
		deviceInfoPtr->SetProject(project.toUtf8());
	}

	QString orderId;
	if (representation.orderId){
		orderId = *representation.orderId;
		deviceInfoPtr->SetOrderId(orderId.toUtf8());
	}

	if (representation.description){
		deviceInfoPtr->SetDescription(*representation.description);
	}

	if (representation.productionStatus){
		QString status = *representation.productionStatus;
		prolifedata::IDeviceInfo::DeviceProductionStatus productionStatus = prolifedata::GetProductionStatusFromId(status.toUtf8());
		deviceInfoPtr->SetDeviceProductionStatus(productionStatus);
	}

	QString licenseName;
	if (representation.licenseName){
		licenseName = *representation.licenseName;
	}

	if (licenseName.isEmpty()){
		errorMessage = QString("Configuration cannot be empty");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return false;
	}

	deviceInfoPtr->SetConfigurationType(licenseName.toUtf8());

	QString deviceType;
	if (representation.deviceType){
		deviceType = *representation.deviceType;
	}

	if (deviceType.isEmpty()){
		errorMessage = QString("Device type cannot be empty");
		SendErrorMessage(0, errorMessage, "CDeviceCollectionControllerComp");

		return false;
	}

	deviceInfoPtr->SetDeviceType(deviceType.toUtf8());

	if (representation.internalUse){
		deviceInfoPtr->SetInternalUse(*representation.internalUse);
	}

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

	QByteArrayList removedIds;
	removedIds << deviceId;
	if (!productCollectionPtr->RemoveElements(removedIds)){
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


QByteArrayList CDeviceCollectionControllerComp::GetBindedSoftware(const QByteArray& deviceId) const
{
	if (!m_bindingCollectionCompPtr.IsValid()){
		return QByteArrayList();
	}

	const prolifedata::IHardwareProductBinding* bindingInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_bindingCollectionCompPtr->GetObjectData(deviceId, dataPtr)){
		bindingInfoPtr = dynamic_cast<const prolifedata::IHardwareProductBinding*>(dataPtr.GetPtr());
	}

	if (bindingInfoPtr == nullptr){
		return QByteArrayList();
	}

	return bindingInfoPtr->GetSoftwareIds();
}


bool CDeviceCollectionControllerComp::GetSoftwareInfo(const QByteArray& softwareId, sdl::prolife::Sensors::CSoftwareBindingInfo::V1_0& softwareInfo) const
{
	if (!m_softwareProductCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection::DataPtr productDataPtr;
	if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, productDataPtr)){
		imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
		if (productInstanceInfoPtr != nullptr){
			imtbase::ICollectionInfo::Ids elementsIds = productInstanceInfoPtr->GetLicenseInstances().GetElementIds();
			if (!elementsIds.isEmpty()){
				QByteArray licenseId = elementsIds[0];

				imtbase::IObjectCollection::DataPtr licenseDataPtr;
				if (m_licenseCollectionCompPtr->GetObjectData(licenseId, licenseDataPtr)){
					imtlic::ILicenseDefinition* licenseDefinitionPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
					if (licenseDefinitionPtr != nullptr){
						QString licenseName = licenseDefinitionPtr->GetLicenseName();
						QByteArray licenseDefinitionId = licenseDefinitionPtr->GetLicenseId();

						softwareInfo.id = softwareId;
						softwareInfo.softwareId = productInstanceInfoPtr->GetSerialNumber();
						softwareInfo.softwareName = licenseName + " (" + licenseDefinitionId + ")";

						return true;
					}
				}
			}
		}
	}

	return false;
}


} // namespace prolifegql


