#include <prolifegql/CDeviceControllerComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtCore/QTemporaryDir>

// ACF includes
#include <iprm/CTextParam.h>
#include <istd/TOptDelPtr.h>
#include <iprm/CParamsSet.h>
#include <istd/TOptDelPtr.h>

// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/ILicenseDefinition.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/IFeatureInfo.h>
#include <imtauth/CUserInfo.h>

// ProLife includes
#include <prolifedata/CHardwareProductBinding.h>
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>
#include <prolifedata/CSoftwareTransferInfo.h>


namespace prolifegql
{


// protected methods

sdl::prolife::Sensors::CDeviceBindingData CDeviceControllerComp::OnGetDeviceBinding(
	const sdl::prolife::Sensors::CGetDeviceBindingGqlRequest& getDeviceBindingRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::prolife::Sensors::CDeviceBindingData retVal;
	
	auto inputArguments = getDeviceBindingRequest.GetRequestedArguments();
	if (!inputArguments.input.Version_1_0){
		I_CRITICAL();
		
		return retVal;
	}
	
	sdl::prolife::Sensors::CDeviceBindingData::V1_0& response = retVal.Version_1_0.emplace();
	
	QByteArray deviceId;
	if (inputArguments.input.Version_1_0->id){
		deviceId = *inputArguments.input.Version_1_0->id;
	}
	
	istd::TOptDelPtr<prolifedata::CHardwareProductBinding> hardwareProductBindingPtr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_deviceBindingCollectionCompPtr->GetObjectData(deviceId, dataPtr)){
		hardwareProductBindingPtr.SetCastedOrRemove(dataPtr.GetPtr(), false);
	}
	
	if (!hardwareProductBindingPtr.IsValid()){
		prolifedata::CHardwareProductBinding* deviceBindingPtr = new prolifedata::CHardwareProductBinding();
		deviceBindingPtr->SetHardwareId(deviceId);
		
		hardwareProductBindingPtr.SetPtr(deviceBindingPtr, true);
		
		m_deviceBindingCollectionCompPtr->InsertNewObject("HardwareBinding", "", "", deviceBindingPtr, deviceId);
	}

	if (m_deviceCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr deviceDataPtr;
		if (m_deviceCollectionCompPtr->GetObjectData(deviceId, deviceDataPtr)){
			const prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
			if (deviceInfoPtr != nullptr){
				response.project = deviceInfoPtr->GetProject();
			}
		}
	}

	QByteArrayList softwareIds = hardwareProductBindingPtr->GetSoftwareIds();
	if (!softwareIds.isEmpty()){
		if (m_softwareProductCollectionCompPtr.IsValid()){
			QByteArray softwareId = softwareIds[0];
			imtbase::IObjectCollection::DataPtr softwareDataPtr;
			if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, softwareDataPtr)){
				imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
				if (productInstanceInfoPtr != nullptr){
					QByteArray productId = productInstanceInfoPtr->GetProductId();
					response.productUuid = productId;
				}
			}
		}
	}
	
	response.id = deviceId;
	response.softwareIds = softwareIds.join(';');
	
	return retVal;
}


sdl::imtbase::ImtCollection::CUpdatedNotificationPayload CDeviceControllerComp::OnUpdateDeviceBinding(
			const sdl::prolife::Sensors::CUpdateDeviceBindingGqlRequest& updateDeviceBindingRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CUpdatedNotificationPayload retVal;

	sdl::prolife::Sensors::UpdateDeviceBindingRequestArguments inputArguments = updateDeviceBindingRequest.GetRequestedArguments();
	if (!inputArguments.input.Version_1_0){
		I_CRITICAL();

		return retVal;
	}

	sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0& response = retVal.Version_1_0.emplace();

	QByteArray deviceId;
	if (inputArguments.input.Version_1_0->deviceId){
		deviceId = *inputArguments.input.Version_1_0->deviceId;
	}

	sdl::prolife::Sensors::CDeviceBindingData::V1_0 deviceBindingData;
	if (inputArguments.input.Version_1_0->item){
		deviceBindingData = *inputArguments.input.Version_1_0->item;
	}

	QString project;
	if (inputArguments.input.Version_1_0->project){
		project = *inputArguments.input.Version_1_0->project;
	}

	istd::TOptDelPtr<prolifedata::CHardwareProductBinding> deviceBindingInfoPtr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_deviceBindingCollectionCompPtr->GetObjectData(deviceId, dataPtr)){
		deviceBindingInfoPtr.SetCastedOrRemove(dataPtr.GetPtr(), false);
	}

	if (!deviceBindingInfoPtr.IsValid()){
		prolifedata::CHardwareProductBinding* deviceBindingPtr = new prolifedata::CHardwareProductBinding();
		deviceBindingPtr->SetHardwareId(deviceId);

		deviceBindingInfoPtr.SetPtr(deviceBindingPtr, true);

		QByteArray result = m_deviceBindingCollectionCompPtr->InsertNewObject("HardwareBinding", "", "", deviceBindingPtr, deviceId);
		if (result.isEmpty()){
			SendWarningMessage(0, QString("Unable to insert hardware binding object to collection"), "CDeviceControllerComp");
		}
	}

	if (!deviceBindingInfoPtr.IsValid()){
		errorMessage = QString("Unable to update device binding. Error: Device is invalid");

		return retVal;
	}

	response.id = deviceId;

	QByteArrayList newHardwareBindingSoftwareIds;
	if (deviceBindingData.softwareIds){
		if (!deviceBindingData.softwareIds->isEmpty()){
			newHardwareBindingSoftwareIds = deviceBindingData.softwareIds->split(';');
		}
	}

	QByteArrayList hardwareBindingSoftwareIds = deviceBindingInfoPtr->GetSoftwareIds();

	QByteArrayList addedLicenses;
	QByteArrayList removedLicenses;

	for (const QByteArray& id : newHardwareBindingSoftwareIds){
		if (!hardwareBindingSoftwareIds.contains(id)){
			addedLicenses << id;
		}
	}

	for (const QByteArray& id : hardwareBindingSoftwareIds){
		if (!newHardwareBindingSoftwareIds.contains(id)){
			removedLicenses << id;
		}
	}

	// Check product-ID
	QByteArray prevProductId;
	for (const QByteArray& softwareId : newHardwareBindingSoftwareIds){
		imtbase::IObjectCollection::DataPtr softwareDataPtr;
		if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, softwareDataPtr)){
			const imtlic::IProductInstanceInfo* productInstanceInfoPtr
				= dynamic_cast<const imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
			if (productInstanceInfoPtr != nullptr){
				QByteArray productId = productInstanceInfoPtr->GetProductId();
				if (prevProductId.isEmpty()){
					prevProductId = productId;
				}
				else{
					if (prevProductId != productId){
						errorMessage = QString("Unable to update device binding. Error: Licenses must be of the same product");

						return retVal;
					}
				}
			}
		}
	}

	// Check licenses
	for (int i = 0; i < newHardwareBindingSoftwareIds.size(); i++){
		imtbase::IObjectCollection::DataPtr softwareDataPtr;
		if (m_softwareProductCollectionCompPtr->GetObjectData(newHardwareBindingSoftwareIds[i], softwareDataPtr)){
			const imtlic::IProductInstanceInfo* productInstanceInfoPtr
				= dynamic_cast<const imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
			if (productInstanceInfoPtr != nullptr){
				imtbase::ICollectionInfo::Ids licenseIds = productInstanceInfoPtr->GetLicenseInstances().GetElementIds();

				for (int j = i + 1; j < newHardwareBindingSoftwareIds.size(); j++){
					imtbase::IObjectCollection::DataPtr softwareDataPtr2;
					if (m_softwareProductCollectionCompPtr->GetObjectData(newHardwareBindingSoftwareIds[j], softwareDataPtr2)){
						const imtlic::IProductInstanceInfo* productInstanceInfoPtr2
							= dynamic_cast<const imtlic::IProductInstanceInfo*>(softwareDataPtr2.GetPtr());
						if (productInstanceInfoPtr2 != nullptr){
							imtbase::ICollectionInfo::Ids licenseIds2 = productInstanceInfoPtr2->GetLicenseInstances().GetElementIds();
							for (const imtbase::ICollectionInfo::Id& licenseId: licenseIds){
								if (licenseIds2.contains(licenseId)){
									errorMessage = QString("Unable to update device binding. Error: The same licenses are selected");
									
									return retVal;
								}
							}
						}
					}
				}
			}
		}
	}

	deviceBindingInfoPtr->SetHardwareId(deviceId);

	newHardwareBindingSoftwareIds.removeAll("");
	deviceBindingInfoPtr->SetSoftwareIds(newHardwareBindingSoftwareIds);

	if (!m_deviceBindingCollectionCompPtr->SetObjectData(deviceId, *deviceBindingInfoPtr)){
		errorMessage = QString("Unable to update hardware binding info");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

		return retVal;
	}

	CreateDeviceOperationContext(deviceId, project.toUtf8(), addedLicenses, removedLicenses);
	CreateSoftwareOperationContext(deviceId, project.toUtf8(), addedLicenses, removedLicenses);

	return retVal;
}


sdl::prolife::Sensors::CTransferLicensesPayload CDeviceControllerComp::OnTransferLicenses(
			const sdl::prolife::Sensors::CTransferLicensesGqlRequest& transferLicensesRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_softwareTransferCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SoftwareTransferCollection' was not set", "CDeviceControllerComp");
		return sdl::prolife::Sensors::CTransferLicensesPayload();
	}

	if (!m_supportEmailParamCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SupportEmailParam' was not set", "CDeviceControllerComp");
		return sdl::prolife::Sensors::CTransferLicensesPayload();
	}

	sdl::prolife::Sensors::CTransferLicensesPayload retVal;
	retVal.Version_1_0.emplace();
	retVal.Version_1_0->ok = false;
	retVal.Version_1_0->limit = false;
	retVal.Version_1_0->supportEmail = m_supportEmailParamCompPtr->GetText();

	sdl::prolife::Sensors::TransferLicensesRequestArguments inputArguments = transferLicensesRequest.GetRequestedArguments();
	if (!inputArguments.input.Version_1_0){
		I_CRITICAL();
		
		return retVal;
	}
	
	QByteArray fromDeviceId;
	if (inputArguments.input.Version_1_0->fromDeviceId){
		fromDeviceId = *inputArguments.input.Version_1_0->fromDeviceId;
	}

	QByteArray toDeviceId;
	if (inputArguments.input.Version_1_0->toDeviceId){
		toDeviceId = *inputArguments.input.Version_1_0->toDeviceId;
	}

	// Get FROM device binding data
	istd::TDelPtr<prolifedata::IHardwareProductBinding> fromDeviceBindingInfoPtr = GetOrCreateDeviceBinding(fromDeviceId);
	if (!fromDeviceBindingInfoPtr.IsValid()){
		return retVal;
	}

	// Get TO device binding data
	istd::TDelPtr<prolifedata::IHardwareProductBinding> toDeviceBindingInfoPtr = GetOrCreateDeviceBinding(toDeviceId);
	if (!toDeviceBindingInfoPtr.IsValid()){
		return retVal;
	}

	// Get FROM device data
	prolifedata::IDeviceInfo* fromDeviceInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr deviceDataPtr;
	if (m_deviceCollectionCompPtr->GetObjectData(fromDeviceId, deviceDataPtr)){
		fromDeviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
	}

	if (fromDeviceInfoPtr == nullptr){
		errorMessage = QString("Unable to transfer license from '%1' to '%2'. Error: Device '%1' not exists").arg(qPrintable(fromDeviceId), qPrintable(toDeviceId));
		
		return retVal;
	}

	// Get TO device data
	prolifedata::IDeviceInfo* toDeviceInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr toDeviceDataPtr;
	if (m_deviceCollectionCompPtr->GetObjectData(toDeviceId, toDeviceDataPtr)){
		toDeviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(toDeviceDataPtr.GetPtr());
	}

	if (toDeviceInfoPtr == nullptr){
		errorMessage = QString("Unable to transfer license from '%1' to '%2'. Error: Device '%2' not exists").arg(qPrintable(fromDeviceId), qPrintable(toDeviceId));
		return retVal;
	}

	// Check TO device status is Finished
	prolifedata::IDeviceInfo::DeviceProductionStatus deviceProductionStatus = toDeviceInfoPtr->GetDeviceProductionStatus();
	if (deviceProductionStatus != prolifedata::IDeviceInfo::DPS_FINISHED){
		errorMessage = QString("Unable to transfer license from '%1' to '%2'. Error: Production status for device '%1' should be 'Finished'")
		.arg(qPrintable(fromDeviceId), qPrintable(toDeviceId));
		
		return retVal;
	}

	// Check same devices
	if (fromDeviceId == toDeviceId){
		errorMessage = QString("It is not possible to transfer licenses to the same device");
		
		return retVal;
	}

	QByteArray projectId = fromDeviceInfoPtr->GetProject();
	
	QByteArrayList fromDeviceSoftwareIds = fromDeviceBindingInfoPtr->GetSoftwareIds();
	QByteArrayList toDeviceSoftwareIds = toDeviceBindingInfoPtr->GetSoftwareIds();

	// Check TO device empty licenses
	if (!toDeviceSoftwareIds.isEmpty()){
		errorMessage = QString("Unable to transfer license from '%1' to '%2'. Error: Device '%2' already contains licenses").arg(qPrintable(fromDeviceId), qPrintable(toDeviceId));
		return retVal;
	}

	// Checking the number of license transfers
	for (const QByteArray& softwareId : fromDeviceSoftwareIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_softwareTransferCollectionCompPtr->GetObjectData(softwareId, dataPtr)){
			prolifedata::CSoftwareTransferInfo* softwareTransferInfoPtr =
				dynamic_cast<prolifedata::CSoftwareTransferInfo*>(dataPtr.GetPtr());
			if (softwareTransferInfoPtr != nullptr){
				int softwareCount = softwareTransferInfoPtr->GetTransferCount();
				int maxTransferCount = m_maxTransferCountAttrPtr.IsValid() ? *m_maxTransferCountAttrPtr : 3;
				if (softwareCount >= maxTransferCount){
					retVal.Version_1_0->limit = true;
					return retVal;
				}
			}
		}
	}

	toDeviceBindingInfoPtr->SetSoftwareIds(fromDeviceSoftwareIds);
	fromDeviceBindingInfoPtr->SetSoftwareIds(QByteArrayList());

	// Update licenses for TO device
	if (!m_deviceBindingCollectionCompPtr->SetObjectData(toDeviceId, *toDeviceBindingInfoPtr)){
		errorMessage = QString("Unable to update hardware binding info");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		
		return retVal;
	}

	// Update licenses for FROM device
	if (!m_deviceBindingCollectionCompPtr->SetObjectData(fromDeviceId, *fromDeviceBindingInfoPtr)){
		errorMessage = QString("Unable to update hardware binding info");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return retVal;
	}

	// Set operation context for TO device data
	istd::TDelPtr<imtbase::IOperationContext> toDeviceOperationContextPtr =  nullptr;
	if (m_deviceOperationContextControllerCompPtr.IsValid()){
		iprm::CTextParam fromDeviceParam;
		fromDeviceParam.SetText(fromDeviceId);

		iprm::CParamsSet paramsSet;
		paramsSet.SetEditableParameter("FromDeviceId", &fromDeviceParam);

		toDeviceOperationContextPtr.SetPtr(m_deviceOperationContextControllerCompPtr->CreateOperationContext("TransferFromDevice", toDeviceId, toDeviceInfoPtr, &paramsSet));

		if (!m_deviceCollectionCompPtr->SetObjectData(toDeviceId, *toDeviceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, toDeviceOperationContextPtr.GetPtr())){
			SendWarningMessage(0, "Unable to set operation context for device instance", "CDeviceControllerComp");
		}
	}

	iprm::CTextParam toDeviceParam;
	toDeviceParam.SetText(toDeviceId);

	iprm::CParamsSet toParamsSet;
	toParamsSet.SetEditableParameter("ToDeviceId", &toDeviceParam);

	// Set operation context for FROM device data
	istd::TDelPtr<imtbase::IOperationContext> fromDeviceOperationContextPtr =  nullptr;
	if (m_deviceOperationContextControllerCompPtr.IsValid()){
		fromDeviceOperationContextPtr.SetPtr(m_deviceOperationContextControllerCompPtr->CreateOperationContext("TransferToDevice", fromDeviceId, fromDeviceBindingInfoPtr.GetPtr(), &toParamsSet));

		fromDeviceInfoPtr->SetDeviceProductionStatus(prolifedata::IDeviceInfo::DPS_DEFECTED);
		if (!m_deviceCollectionCompPtr->SetObjectData(fromDeviceId, *fromDeviceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, fromDeviceOperationContextPtr.GetPtr())){
			SendWarningMessage(0, "Unable to set operation context for device instance", "CDeviceControllerComp");
		}
	}

	// Incrementing the number of license transfers
	for (const QByteArray& softwareId : fromDeviceSoftwareIds){
		imtbase::IObjectCollection::DataPtr softwareDataPtr;
		if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, softwareDataPtr)){
			const imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
			if (productInstanceInfoPtr != nullptr){
				istd::TDelPtr<imtbase::IOperationContext> softwareOperationContextPtr =  nullptr;
				softwareOperationContextPtr.SetPtr(m_softwareOperationContextControllerCompPtr->CreateOperationContext("TransferToDevice", softwareId, productInstanceInfoPtr, &toParamsSet));

				if (!m_softwareProductCollectionCompPtr->SetObjectData(softwareId, *productInstanceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, softwareOperationContextPtr.GetPtr())){
					SendWarningMessage(0, "Unable to set operation context for software instances", "CDeviceControllerComp");
				}
			}
		}

		istd::TOptDelPtr<prolifedata::CSoftwareTransferInfo> softwareTransferInfoPtr;
		softwareTransferInfoPtr.SetPtr(new prolifedata::CSoftwareTransferInfo, true);

		bool isNew = false;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_softwareTransferCollectionCompPtr->GetObjectData(softwareId, dataPtr)){
			softwareTransferInfoPtr.SetPtr(dynamic_cast<prolifedata::CSoftwareTransferInfo*>(dataPtr.GetPtr()), false);
		}
		else{
			softwareTransferInfoPtr.SetPtr(new prolifedata::CSoftwareTransferInfo, true);
			isNew = true;
		}

		if (softwareTransferInfoPtr.IsValid()){
			softwareTransferInfoPtr->SetSoftwareId(softwareId);

			int currentTransferCount = softwareTransferInfoPtr->GetTransferCount();
			softwareTransferInfoPtr->SetTransferCount(currentTransferCount + 1);

			if (isNew){
				QByteArray result = m_softwareTransferCollectionCompPtr->InsertNewObject("SoftwareTransferInfo", "", "", softwareTransferInfoPtr.GetPtr(), softwareId);
				if (result.isEmpty()){
					errorMessage = QString("Unable to insert software transfer info");
					SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

					return retVal;
				}
			}
			else{
				if (!m_softwareTransferCollectionCompPtr->SetObjectData(softwareId, *softwareTransferInfoPtr.GetPtr())){
					errorMessage = QString("Unable to update software transfer info");
					SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

					return retVal;
				}
			}
		}
	}

	retVal.Version_1_0->ok = true;

	return retVal;
}


sdl::prolife::Sensors::CCreateLicenseFilePayload CDeviceControllerComp::OnCreateLicenseFile(
			const sdl::prolife::Sensors::CCreateLicenseFileGqlRequest& createLicenseFileRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::Sensors::CCreateLicenseFilePayload retVal;
	sdl::prolife::Sensors::CreateLicenseFileRequestArguments arguments = createLicenseFileRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		I_CRITICAL();

		return retVal;
	}

	retVal.Version_1_0.emplace();

	QByteArray deviceId;
	if (arguments.input.Version_1_0->deviceId){
		deviceId = *arguments.input.Version_1_0->deviceId;
	}

	bool encrypt = true;
	if (arguments.input.Version_1_0->encrypt){
		encrypt = *arguments.input.Version_1_0->encrypt;
	}

	imtbase::IObjectCollection::DataPtr deviceDataPtr;
	if (!m_deviceCollectionCompPtr->GetObjectData(deviceId, deviceDataPtr)){
		errorMessage = QString("Unable to create license file for hardware '%1'. Error: Hardware object is invalid").arg(qPrintable(deviceId));
		SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");

		return retVal;
	}

	prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
	if (deviceInfoPtr == nullptr){
		Q_ASSERT(false);

		return retVal;
	}

	prolifedata::IDeviceInfo::DeviceProductionStatus deviceProductionStatus = deviceInfoPtr->GetDeviceProductionStatus();
	if (deviceProductionStatus != prolifedata::IDeviceInfo::DPS_FINISHED){
		errorMessage = QString("Unable to create license file for hardware '%1'. Error: Production status should be 'Finished'").arg(qPrintable(deviceId));
		SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");

		return retVal;
	}

	QByteArray macAddress = deviceInfoPtr->GetMacAddress();
	if (macAddress.isEmpty()){
		errorMessage = QString("Unable to create license file for hardware '%1'. Error: MAC Address is invalid").arg(qPrintable(deviceId));
		SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");

		return retVal;
	}

	prolifedata::IHardwareProductBinding* bindingInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr bindingDataPtr;
	if (m_deviceBindingCollectionCompPtr->GetObjectData(deviceId, bindingDataPtr)){
		bindingInfoPtr = dynamic_cast<prolifedata::IHardwareProductBinding*>(bindingDataPtr.GetPtr());
	}

	if (bindingInfoPtr == nullptr){
		errorMessage = QString("Unable to create license file for hardware '%1'. Error: There are no licenses for this sensor").arg(qPrintable(deviceId));
		SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");

		return retVal;
	}

	QByteArrayList softwareIds = bindingInfoPtr->GetSoftwareIds();
	if (softwareIds.isEmpty()){
		errorMessage = QString("Unable to create license file for hardware '%1'. Error: There are no licenses for this sensor").arg(qPrintable(deviceId));
		SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");

		return retVal;
	}

	QByteArray productUuid;
	for (int i = 0; i < softwareIds.size(); i++){
		imtbase::IObjectCollection::DataPtr softwareProductDataPtr;
		if (m_softwareProductCollectionCompPtr->GetObjectData(softwareIds[i], softwareProductDataPtr)){
			prolifedata::COrderedIdentifiableSoftwareInstanceInfo* orderProductInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(softwareProductDataPtr.GetPtr());
			if (orderProductInfoPtr != nullptr){
				QByteArray currentProductUuid = orderProductInfoPtr->GetProductId();
				if (i == 0){
					productUuid = currentProductUuid;
				}
				else if (currentProductUuid != productUuid){
					errorMessage = QString("Unable to create license file for hardware '%1'. Error: Licenses are linked to different products").arg(qPrintable(deviceId));
					SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");

					return retVal;
				}
			}
		}
	}

	QByteArray productId;
	imtbase::IObjectCollection::DataPtr productDataPtr;
	if (m_productCollectionCompPtr->GetObjectData(productUuid, productDataPtr)){
		const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(productDataPtr.GetPtr());
		if (productInfoPtr != nullptr){
			productId = productInfoPtr->GetProductId();
		}
	}

	istd::TDelPtr<imtlic::CProductInstanceInfo> productInstancePtr;
	productInstancePtr.SetPtr(new imtlic::CProductInstanceInfo);

	QByteArray customerId;
	productInstancePtr->SetupProductInstance(productId, macAddress, customerId);

	for (const QByteArray& softwareId : softwareIds){
		imtlic::IProductInstanceInfo* productInstanceInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr softwareProductDataPtr;
		if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, softwareProductDataPtr)){
			productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(softwareProductDataPtr.GetPtr());
		}

		if (productInstanceInfoPtr == nullptr){
			errorMessage = QString("Unable to create license file for hardware '%1'. Error: Linked software '%2' is invalid").arg(qPrintable(deviceId), qPrintable(softwareId));
			SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");
			
			return retVal;
		}

		const imtbase::ICollectionInfo& licenseList = productInstanceInfoPtr->GetLicenseInstances();
		for (const QByteArray& licenseCollectionId : licenseList.GetElementIds()){
			imtlic::ILicenseInstance* licenseInstancePtr = dynamic_cast<imtlic::ILicenseInstance*>(const_cast<imtlic::ILicenseInstance*>(productInstanceInfoPtr->GetLicenseInstance(licenseCollectionId)));
			if (licenseInstancePtr == nullptr){
				errorMessage = QString("Unable to create license file for hardware '%1'. Error: License '%2' is invalid").arg(qPrintable(deviceId), qPrintable(licenseCollectionId));
				SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");

				return retVal;
			}

			imtlic::ILicenseDefinition* licenseDefinitionPtr = nullptr;
			imtbase::IObjectCollection::DataPtr licenseDefinitionDataPtr;
			if (m_licenseCollectionCompPtr->GetObjectData(licenseCollectionId, licenseDefinitionDataPtr)){
				licenseDefinitionPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDefinitionDataPtr.GetPtr());
			}

			if (licenseDefinitionPtr == nullptr){
				errorMessage = QString("Unable to create license file for hardware '%1'. Error: License '%2' is invalid").arg(qPrintable(deviceId), qPrintable(licenseCollectionId));
				SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");

				return retVal;
			}

			QByteArray licenseId = licenseDefinitionPtr->GetLicenseId();
			QString licenseName = licenseDefinitionPtr->GetLicenseName();
			QDateTime expiration = licenseInstancePtr->GetExpiration();

			productInstancePtr->AddLicense(licenseId, expiration);

			imtlic::ILicenseInstance* productLicenseInstancePtr = const_cast<imtlic::ILicenseInstance*>(productInstancePtr->GetLicenseInstance(licenseId));
			if (productLicenseInstancePtr == nullptr){
				Q_ASSERT(false);

				return retVal;
			}

			productLicenseInstancePtr->SetLicenseName(licenseName);
			
			imtlic::ILicenseInstance::FeatureInfos featureInfos;

			// License together with all dependent licenses
			QByteArrayList licenses;
			licenses << licenseCollectionId;

			licenses += GetAllLicenseDependencies(licenseCollectionId);

			for (const QByteArray& dependencyId : licenses){
				imtbase::IObjectCollection::DataPtr licenseDataPtr;
				if (m_licenseCollectionCompPtr->GetObjectData(dependencyId, licenseDataPtr)){
					const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
					if (licenseInfoPtr != nullptr){
						imtlic::ILicenseDefinition::FeatureInfos licenseFeatureInfos = licenseInfoPtr->GetFeatureInfos();
						for (const imtlic::ILicenseDefinition::FeatureInfo& featureInfo : licenseFeatureInfos){
							imtbase::IObjectCollection::DataPtr featureDataPtr;
							if (m_featureCollectionCompPtr->GetObjectData(featureInfo.id, featureDataPtr)){
								const imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<const imtlic::IFeatureInfo*>(featureDataPtr.GetPtr());
								if (featureInfoPtr != nullptr){
									imtlic::ILicenseDefinition::FeatureInfo info;
									info.id = featureInfoPtr->GetFeatureId();
									info.name = featureInfoPtr->GetFeatureName();

									if (!featureInfos.contains(info) && !info.id.isEmpty()){
										featureInfos.append(info);
									}
								}
							}
						}
					}
				}
			}

			productLicenseInstancePtr->SetFeatureInfos(featureInfos);
		}
	}

	bool isAdmin = false;
	QByteArray userId;
	QString username;
	const imtgql::IGqlContext* gqlContextPtr = createLicenseFileRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<imtauth::CIdentifiableUserInfo*>(gqlContextPtr->GetUserInfo());
		if (userInfoPtr != nullptr){
			userId = userInfoPtr->GetObjectUuid();
			isAdmin = userInfoPtr->IsAdmin();
			username = userInfoPtr->GetName();
		}
	}

	if (!isAdmin){
		encrypt = true;
	}

	m_productInstanceId = macAddress.toLower();

	QTemporaryDir tempDir;
	QString filePathTmp = tempDir.path() + "/" + QUuid::createUuid().toString() + ".xml";

	ifile::IFilePersistence* filePersistencePtr = nullptr;

	if (encrypt){
		if (m_encryptionBasedPersistenceCompPtr.IsValid()){
			filePersistencePtr = m_encryptionBasedPersistenceCompPtr.GetPtr();
		}
	}
	else{
		if (m_basedPersistenceCompPtr.IsValid()){
			filePersistencePtr = m_basedPersistenceCompPtr.GetPtr();
		}
	}

	if (filePersistencePtr == nullptr){
		errorMessage = QString("Unable to create license file for hardware '%1'. Error: File persistence is invalid").arg(qPrintable(macAddress));
		SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");
		
		return retVal;
	}

	int state = filePersistencePtr->SaveToFile(*productInstancePtr, filePathTmp);
	if (state != ifile::IFilePersistence::OS_OK){
		SendErrorMessage(0, "License file could not be saved", "CDeviceControllerComp");
		
		return retVal;
	}
	
	QFile file(filePathTmp);

	if (!file.open(QIODevice::ReadOnly)){
		errorMessage = QString("Unable to create license file for hardware '%1'. Error: File '%1' could not be open")
		.arg(qPrintable(deviceId), filePathTmp);
		SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");
		
		return retVal;
	}

	QByteArray originalData = file.readAll();

	QByteArray returnedData = originalData.toBase64();

	QString name = macAddress.split(':').join('_') + "_" + "License.lic";

	retVal.Version_1_0->data = returnedData;
	retVal.Version_1_0->name = name;

	file.close();

	for (const QByteArray& softwareId : softwareIds){
		imtbase::IObjectCollection::DataPtr softwareProductDataPtr;
		if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, softwareProductDataPtr)){
			prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productInstanceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(softwareProductDataPtr.GetPtr());
			if (productInstanceInfoPtr != nullptr){
				productInstanceInfoPtr->SetInUse(true);

				istd::TDelPtr<imtbase::IOperationContext> operationContextPtr =  nullptr;
				if (m_softwareOperationContextControllerCompPtr.IsValid()){
					operationContextPtr.SetPtr(m_softwareOperationContextControllerCompPtr->CreateOperationContext(
						"CreateLicenseFile",
						softwareId,
						productInstanceInfoPtr));
				}

				if (!m_softwareProductCollectionCompPtr->SetObjectData(softwareId, *productInstanceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
					SendWarningMessage(0, "Error when trying update software instance", "CDeviceControllerComp");
				}
			}
		}
	}

	{
		if (m_userActionManagerCompPtr.IsValid()){
			imtauth::IUserRecentAction::UserInfo userInfo(userId, username);
			imtauth::IUserRecentAction::ActionTypeInfo actionTypeInfo( QByteArrayLiteral("CreateLicenseFile"), QStringLiteral("Create License File"), QStringLiteral("License file was created"));
			imtauth::IUserRecentAction::TargetInfo targetInfo(deviceId, QByteArrayLiteral("Device"), QStringLiteral("Device"), QByteArrayLiteral("Devices"), macAddress);

			m_userActionManagerCompPtr->CreateUserAction(userInfo, actionTypeInfo, targetInfo);
		}
	}

	SendInfoMessage(0, QString("License file for hardware '%1' successfully created. User: '%2'").arg(QString(macAddress), username), "CDeviceControllerComp");

	return retVal;
}


sdl::prolife::Sensors::CDecryptLicenseFilePayload CDeviceControllerComp::OnDecryptLicenseFile(
			const sdl::prolife::Sensors::CDecryptLicenseFileGqlRequest& decryptLicenseFileRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::prolife::Sensors::CDecryptLicenseFilePayload retVal;
	sdl::prolife::Sensors::CDecryptLicenseFilePayload::V1_0& response = retVal.Version_1_0.emplace();

	if (!m_encryptionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'Encryption' was not set", "CDeviceControllerComp");

		return retVal;
	}

	const imtgql::IGqlContext* gqlContextPtr = decryptLicenseFileRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		errorMessage = QString("Unable to decrypt license file. Error: GraphQL context is invalid");
		return retVal;
	}

	imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to decrypt license file. Error: User info is invalid");
		return retVal;
	}

	if (!userInfoPtr->IsAdmin()){
		errorMessage = QString("Unable to decrypt license file. Error: Permission denied");
		return retVal;
	}

	sdl::prolife::Sensors::DecryptLicenseFileRequestArguments arguments = decryptLicenseFileRequest.GetRequestedArguments();

	if (!arguments.input.Version_1_0){
		I_CRITICAL();
		
		return retVal;
	}

	QByteArray encryptedData;
	if (arguments.input.Version_1_0->fileData){
		encryptedData = *arguments.input.Version_1_0->fileData;
	}

	QByteArray encryptionKey;
	if (arguments.input.Version_1_0->key){
		encryptionKey = *arguments.input.Version_1_0->key;
	}

	m_productInstanceId = encryptionKey.toLower();

	encryptedData = QByteArray::fromBase64(encryptedData);

	QByteArray decryptedData;
	if (!m_encryptionCompPtr->DecryptData(encryptedData, imtcrypt::IEncryption::EA_AES, *this, decryptedData)){
		errorMessage = QString("Unable to decrypt license file. Error: Decryption data failed");
		return retVal;
	}

	QString name = encryptionKey.split(':').join('_') + "_" + "Decrypted.lic";
	response.decryptedData = decryptedData.toBase64();
	response.fileName = name;

	return retVal;
}


sdl::prolife::Sensors::CRequestTransferLicensesPayload CDeviceControllerComp::OnRequestTransferLicenses(
			const sdl::prolife::Sensors::CRequestTransferLicensesGqlRequest& requestTransferLicensesRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Sensors::CRequestTransferLicensesPayload response;
	response.Version_1_0.emplace();
	response.Version_1_0->result = false;

	if (!m_smtpMessageCreatorCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SmtpMessageCreator' was not set", "CDeviceControllerComp");
		return response;
	}

	if (!m_smtpClientCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SmtpClient' was not set", "CDeviceControllerComp");
		return response;
	}

	if (!m_supportEmailParamCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SupportEmailParam' was not set", "CDeviceControllerComp");
		return response;
	}

	sdl::prolife::Sensors::RequestTransferLicensesRequestArguments arguments = requestTransferLicensesRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		errorMessage = QString("Unable to request transfer license. Error: Request invalid");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	QByteArray fromDeviceId;
	if (arguments.input.Version_1_0->fromDeviceId){
		fromDeviceId = *arguments.input.Version_1_0->fromDeviceId;
	}

	QByteArray toDeviceId;
	if (arguments.input.Version_1_0->toDeviceId){
		toDeviceId = *arguments.input.Version_1_0->toDeviceId;
	}

	istd::TDelPtr<prolifedata::IHardwareProductBinding> fromDeviceBindingInfoPtr = GetOrCreateDeviceBinding(fromDeviceId);
	if (!fromDeviceBindingInfoPtr.IsValid()){
		errorMessage = QString("Unable to request transfer license. Error: From device is invalid");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	istd::TDelPtr<prolifedata::IHardwareProductBinding> toDeviceBindingInfoPtr = GetOrCreateDeviceBinding(toDeviceId);
	if (!toDeviceBindingInfoPtr.IsValid()){
		errorMessage = QString("Unable to request transfer license. Error: To device is invalid");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	QByteArrayList fromDeviceSoftwareIds = fromDeviceBindingInfoPtr->GetSoftwareIds();

	bool ok = false;

	// Checking that there are expired licenses among them
	for (const QByteArray& softwareId : fromDeviceSoftwareIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_softwareTransferCollectionCompPtr->GetObjectData(softwareId, dataPtr)){
			prolifedata::CSoftwareTransferInfo* softwareTransferInfoPtr = dynamic_cast<prolifedata::CSoftwareTransferInfo*>(dataPtr.GetPtr());
			if (softwareTransferInfoPtr != nullptr){
				int softwareCount = softwareTransferInfoPtr->GetTransferCount();
				int maxTransferCount = m_maxTransferCountAttrPtr.IsValid() ? *m_maxTransferCountAttrPtr : 3;
				
				if (softwareCount >= maxTransferCount){
					ok = true;
				}
			}
		}
	}

	if (!ok){
		errorMessage = QString("Unable to request transfer license. Error: There are no licenses for which the limit has been reached");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	QByteArrayList toDeviceSoftwareIds = toDeviceBindingInfoPtr->GetSoftwareIds();
	if (!toDeviceSoftwareIds.isEmpty()){
		errorMessage = QString("Unable to request transfer license. Error: To device already contains licenses");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		errorMessage = QString("Unable to request transfer license. Error: GraphQL context from request is invalid");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to request transfer license. Error: User info from GraphQL context is invalid");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	QString userName = userInfoPtr->GetName();

	istd::TDelPtr<imtmail::ISmtpMessage> messagePtr = m_smtpMessageCreatorCompPtr->CreateMessage();
	if (!messagePtr.IsValid()){
		errorMessage = QString("Unable to request transfer license. Error: User info from GraphQL context is invalid");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	messagePtr->SetSubject(QString("Transferring licenses for user '%1'").arg(userName));
	messagePtr->SetBody(QString("User '%1' requests a license transfer because the limit has been exceeded").arg(userName));

	QString supportEmail = m_supportEmailParamCompPtr->GetText();
	messagePtr->SetTo(supportEmail);

	if (!m_smtpClientCompPtr->SendEmail(*messagePtr.GetPtr())){
		errorMessage = QString("Unable to request transfer license. Error when trying to send a message");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	// Set limit exceeded
	for (const QByteArray& softwareId : fromDeviceSoftwareIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_softwareTransferCollectionCompPtr->GetObjectData(softwareId, dataPtr)){
			prolifedata::CSoftwareTransferInfo* softwareTransferInfoPtr = dynamic_cast<prolifedata::CSoftwareTransferInfo*>(dataPtr.GetPtr());
			if (softwareTransferInfoPtr != nullptr){
				softwareTransferInfoPtr->SetTransferLimitExceeded(true);

				if (!m_softwareTransferCollectionCompPtr->SetObjectData(softwareId, *softwareTransferInfoPtr)){
					errorMessage = QString("Unable to update object for software transfer info");
					SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

					return response;
				}
			}
		}
	}

	response.Version_1_0->result = true;

	return response;
}


sdl::prolife::Sensors::CResetTransferCounterPayload CDeviceControllerComp::OnResetTransferCounter(
			const sdl::prolife::Sensors::CResetTransferCounterGqlRequest& resetTransferCounterRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::prolife::Sensors::CResetTransferCounterPayload response;
	response.Version_1_0.emplace();
	response.Version_1_0->result = false;

	if (!m_softwareTransferCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'SoftwareTransferCollection' was not set", "CDeviceControllerComp");
		return response;
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		errorMessage = QString("Unable to reset transfer counter. Error: GraphQL context from request is invalid");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to reset transfer counter. Error: User info from GraphQL context is invalid");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	sdl::prolife::Sensors::ResetTransferCounterRequestArguments arguments = resetTransferCounterRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		errorMessage = QString("Unable to reset transfer counter. Error: Request invalid");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	QByteArray hardwareId;
	if (arguments.input.Version_1_0->hardwareId){
		hardwareId = *arguments.input.Version_1_0->hardwareId;
	}

	istd::TDelPtr<prolifedata::IHardwareProductBinding> fromDeviceBindingInfoPtr = GetOrCreateDeviceBinding(hardwareId);
	if (!fromDeviceBindingInfoPtr.IsValid()){
		errorMessage = QString("Unable to reset transfer counter. Error: Hardware is invalid");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	QByteArrayList softwareIds = fromDeviceBindingInfoPtr->GetSoftwareIds();
	if (softwareIds.isEmpty()){
		errorMessage = QString("Unable to reset transfer counter. Error: There are no licenses for this sensor");
		return response;
	}

	for (const QByteArray& softwareId : softwareIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_softwareTransferCollectionCompPtr->GetObjectData(softwareId, dataPtr)){
			prolifedata::CSoftwareTransferInfo* softwareTransferInfoPtr = dynamic_cast<prolifedata::CSoftwareTransferInfo*>(dataPtr.GetPtr());
			if (softwareTransferInfoPtr != nullptr){
				softwareTransferInfoPtr->SetTransferCount(0);
				m_softwareTransferCollectionCompPtr->SetObjectData(softwareId, *softwareTransferInfoPtr);
			}
		}
	}

	response.Version_1_0->result = true;

	return response;
}


// reimplemented (imtcrypt::IEncryptionKeysProvider)

QByteArray CDeviceControllerComp::GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KeyType type) const
{
	QByteArray retVal;

	if (type == KT_PASSWORD){
		return m_productInstanceId;
	}
	else if (type == KT_INIT_VECTOR){
		if (m_vectorKeyCompPtr.IsValid()){
			retVal = m_vectorKeyCompPtr->GetId();
		}
	}

	return retVal;
}


// private methods

prolifedata::IHardwareProductBinding* CDeviceControllerComp::GetOrCreateDeviceBinding(const QByteArray& deviceId) const
{
	if (deviceId.isEmpty()){
		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_deviceBindingCollectionCompPtr->GetObjectData(deviceId, dataPtr)){
		istd::TUniqueInterfacePtr<prolifedata::IHardwareProductBinding> deviceBindingInfoPtr;
		deviceBindingInfoPtr.MoveCastedPtr(dataPtr.GetPtr()->CloneMe());

		return deviceBindingInfoPtr.PopInterfacePtr();
	}

	istd::TDelPtr<prolifedata::CHardwareProductBinding> deviceBindingInfoPtr;
	deviceBindingInfoPtr.SetPtr(new prolifedata::CHardwareProductBinding());
	deviceBindingInfoPtr->SetHardwareId(deviceId);

	m_deviceBindingCollectionCompPtr->InsertNewObject("HardwareBinding", "", "", deviceBindingInfoPtr.GetPtr(), deviceId);

	return deviceBindingInfoPtr.PopPtr();
}


void CDeviceControllerComp::CreateDeviceOperationContext(const QByteArray& deviceId, const QByteArray& project, QByteArrayList addedLicenses, QByteArrayList removedLicenses) const
{
	if (m_deviceCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr deviceDataPtr;
		if (m_deviceCollectionCompPtr->GetObjectData(deviceId, deviceDataPtr)){
			prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
			if (deviceInfoPtr != nullptr){
				deviceInfoPtr->SetProject(project);

				iprm::CTextParam addedTextParam;
				if (!addedLicenses.isEmpty()){
					addedTextParam.SetText(addedLicenses.join(';'));
				}

				iprm::CTextParam removedTextParam;
				if (!removedLicenses.isEmpty()){
					removedTextParam.SetText(removedLicenses.join(';'));
				}

				iprm::CParamsSet paramsSet;
				paramsSet.SetEditableParameter("AddedProductIds", &addedTextParam);
				paramsSet.SetEditableParameter("RemovedProductIds", &removedTextParam);

				istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
				if (m_deviceOperationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext("BindLicense", deviceId, deviceInfoPtr, &paramsSet);
				}

				if (!m_deviceCollectionCompPtr->SetObjectData(deviceId, *deviceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
					SendErrorMessage(0, "Unable to create device operation context. Error: Set object data failed", "CDeviceControllerComp");
				}
			}
		}
	}
}


void CDeviceControllerComp::CreateSoftwareOperationContext(const QByteArray& deviceId, const QByteArray& project, QByteArrayList addedLicenses, QByteArrayList removedLicenses) const
{
	if (m_softwareProductCollectionCompPtr.IsValid()){
		for (const QByteArray& id : addedLicenses){
			imtbase::IObjectCollection::DataPtr softwareDataPtr;
			if (m_softwareProductCollectionCompPtr->GetObjectData(id, softwareDataPtr)){
				imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
				if (productInstanceInfoPtr != nullptr){
					if (!productInstanceInfoPtr->IsInUse()){
						productInstanceInfoPtr->SetProject(project);

						iprm::CTextParam textParam;
						textParam.SetText(deviceId);

						iprm::CParamsSet paramsSet;
						paramsSet.SetEditableParameter("AddedHardwareId", &textParam);

						istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
						if (m_softwareOperationContextControllerCompPtr.IsValid()){
							operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Bind", id, productInstanceInfoPtr, &paramsSet);
						}

						if (!m_softwareProductCollectionCompPtr->SetObjectData(id, *productInstanceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
							return;
						}
					}
				}
			}
		}

		for (const QByteArray& id : removedLicenses){
			imtbase::IObjectCollection::DataPtr softwareDataPtr;
			if (m_softwareProductCollectionCompPtr->GetObjectData(id, softwareDataPtr)){
				imtlic::IProductInstanceInfo* productInstanceInfoPtr =  dynamic_cast<imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
				if (productInstanceInfoPtr != nullptr){
					productInstanceInfoPtr->SetInUse(false);
					productInstanceInfoPtr->SetProject("");

					iprm::CTextParam textParam;
					textParam.SetText(deviceId);

					iprm::CParamsSet paramsSet;
					paramsSet.SetEditableParameter("RemovedHardwareId", &textParam);

					istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
					if (m_softwareOperationContextControllerCompPtr.IsValid()){
						operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Bind", id, productInstanceInfoPtr, &paramsSet);
					}

					if (!m_softwareProductCollectionCompPtr->SetObjectData(id, *productInstanceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
						return;
					}
				}
			}
		}
	}
}


QByteArrayList CDeviceControllerComp::GetAllLicenseDependencies(const QByteArray& licenseId) const
{
	QByteArrayList retVal;
	if (!m_licenseCollectionCompPtr.IsValid()){
		return retVal;
	}

	const imtlic::ILicenseDefinition* licenseInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr licenseDataPtr;
	if (m_licenseCollectionCompPtr->GetObjectData(licenseId, licenseDataPtr)){
		licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
	}

	if (licenseInfoPtr == nullptr){
		return retVal;
	}

	QByteArrayList dependencies = licenseInfoPtr->GetDependencies();

	for (const QByteArray& depId : dependencies){
		retVal << GetAllLicenseDependencies(depId);
	}

	retVal << dependencies;

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
	QSet<QByteArray> set(retVal.cbegin(), retVal.cend());
	retVal = set.values();
#else
	QSet<QByteArray> set(retVal.toSet());
	retVal = set.values();
#endif

	return retVal;
}


} // namespace prolifegql


