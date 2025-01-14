#include <prolifegql/CDeviceControllerComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtCore/QTemporaryDir>

// ACF includes
#include <iprm/CTextParam.h>
#include <istd/TOptDelPtr.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/ILicenseDefinition.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/IFeatureInfo.h>

// ProLife includes
#include <prolifedata/CHardwareProductBinding.h>
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>


namespace prolifegql
{


// protected methods

sdl::imtbase::ImtCollection::CVisualStatus::V1_0 CDeviceControllerComp::OnGetObjectVisualStatus(
	const sdl::prolife::Sensors::V1_0::CGetObjectVisualStatusGqlRequest& /*getObjectVisualStatusRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::imtbase::ImtCollection::CVisualStatus::V1_0();
}


sdl::prolife::Sensors::CDevicesListPayload::V1_0 CDeviceControllerComp::OnDevicesList(
	const sdl::prolife::Sensors::V1_0::CDevicesListGqlRequest& /*devicesListRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::prolife::Sensors::CDevicesListPayload::V1_0();
}


sdl::prolife::Sensors::CDeviceDataPayload::V1_0 CDeviceControllerComp::OnDeviceItem(
	const sdl::prolife::Sensors::V1_0::CDeviceItemGqlRequest& /*deviceItemRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::prolife::Sensors::CDeviceDataPayload::V1_0();
}


sdl::prolife::Sensors::CDeviceBindingData::V1_0 CDeviceControllerComp::OnGetDeviceBinding(
	const sdl::prolife::Sensors::V1_0::CGetDeviceBindingGqlRequest& getDeviceBindingRequest,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	sdl::prolife::Sensors::CDeviceBindingData::V1_0 response;

	QByteArray deviceId = *getDeviceBindingRequest.GetRequestedArguments().input.Id;

	istd::TOptDelPtr<prolifedata::CHardwareProductBinding> hardwareProductBindingPtr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_deviceBindingCollectionCompPtr->GetObjectData(deviceId, dataPtr)){
		hardwareProductBindingPtr.SetCastedOrRemove(dataPtr.GetPtr(), false);
	}

	if (!hardwareProductBindingPtr.IsValid()){
		prolifedata::CHardwareProductBinding* deviceBindingPtr = new prolifedata::CHardwareProductBinding();
		deviceBindingPtr->SetHardwareId(deviceId);

		hardwareProductBindingPtr.SetPtr(deviceBindingPtr, true);

		m_deviceBindingCollectionCompPtr->InsertNewObject("", "", "", deviceBindingPtr, deviceId);
	}

	QByteArrayList softwareIds = hardwareProductBindingPtr->GetSoftwareIds();
	if (!softwareIds.isEmpty()){
		if (m_softwareProductCollectionCompPtr.IsValid()){
			QByteArray softwareId = softwareIds[0];
			imtbase::IObjectCollection::DataPtr softwareDataPtr;
			if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, softwareDataPtr)){
				imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
				if (productInstanceInfoPtr != nullptr){
					QByteArray project = productInstanceInfoPtr->GetProject();
					response.Project = project;

					QByteArray productId = productInstanceInfoPtr->GetProductId();
					response.ProductUuid = productId;
				}
			}
		}
	}

	response.Id = deviceId;
	response.SoftwareIds = softwareIds.join(';');

	return response;
}


sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0 CDeviceControllerComp::OnDeviceUpdate(
	const sdl::prolife::Sensors::V1_0::CDeviceUpdateGqlRequest& /*deviceUpdateRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0();
}


sdl::imtbase::ImtCollection::CAddedNotificationPayload::V1_0 CDeviceControllerComp::OnDeviceAdd(
	const sdl::prolife::Sensors::V1_0::CDeviceAddGqlRequest& /*deviceAddRequest*/,
	const ::imtgql::CGqlRequest& /*gqlRequest*/,
	QString& /*errorMessage*/) const
{
	return sdl::imtbase::ImtCollection::CAddedNotificationPayload::V1_0();
}


sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0 CDeviceControllerComp::OnUpdateDeviceBinding(
	const sdl::prolife::Sensors::V1_0::CUpdateDeviceBindingGqlRequest& updateDeviceBindingRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	sdl::imtbase::ImtCollection::CUpdatedNotificationPayload::V1_0 response;

	QByteArray deviceId = *updateDeviceBindingRequest.GetRequestedArguments().input.DeviceId;
	sdl::prolife::Sensors::CDeviceBindingData::V1_0 deviceBindingData = *updateDeviceBindingRequest.GetRequestedArguments().input.Item;
	QString project = *updateDeviceBindingRequest.GetRequestedArguments().input.Project;

	prolifedata::CHardwareProductBinding* deviceBindingInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_deviceBindingCollectionCompPtr->GetObjectData(deviceId, dataPtr)){
		deviceBindingInfoPtr = dynamic_cast<prolifedata::CHardwareProductBinding*>(dataPtr.GetPtr());
	}

	if (deviceBindingInfoPtr == nullptr){
		errorMessage = QString("Unable to update device binding. Error: Device is invalid");
		return response;
	}

	response.Id = deviceId;

	QByteArrayList newHardwareBindingSoftwareIds = deviceBindingData.SoftwareIds->split(';');
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

	CreateDeviceOperationContext(deviceId, project.toUtf8(), addedLicenses, removedLicenses);

	if (m_softwareProductCollectionCompPtr.IsValid()){
		// Update project for all software

		for (const QByteArray& softwareId : newHardwareBindingSoftwareIds){
			imtbase::IObjectCollection::DataPtr softwareDataPtr;
			if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, softwareDataPtr)){
				imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
				if (productInstanceInfoPtr != nullptr){
					productInstanceInfoPtr->SetProject(project.toUtf8());

					istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
					if (m_softwareOperationContextControllerCompPtr.IsValid()){
						operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Update", softwareId, *productInstanceInfoPtr);
					}

					if (!m_softwareProductCollectionCompPtr->SetObjectData(softwareId, *productInstanceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
						errorMessage = QString("Unable to update software instance object.");
						SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
						return response;
					}
				}
			}
		}
	}

	deviceBindingInfoPtr->SetHardwareId(deviceId);
	deviceBindingInfoPtr->SetSoftwareIds(newHardwareBindingSoftwareIds);

	CreateSoftwareOperationContext(deviceId, project.toUtf8(), addedLicenses, removedLicenses);

	if (!m_deviceBindingCollectionCompPtr->SetObjectData(deviceId, *deviceBindingInfoPtr)){
		errorMessage = QString("Unable to update hardware binding info");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	return response;
}


sdl::prolife::Sensors::CTransferLicensesPayload::V1_0 CDeviceControllerComp::OnTransferLicenses(
	const sdl::prolife::Sensors::V1_0::CTransferLicensesGqlRequest& transferLicensesRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	sdl::prolife::Sensors::CTransferLicensesPayload::V1_0 response;

	QByteArray fromDeviceId = *transferLicensesRequest.GetRequestedArguments().input.FromDeviceId;
	QByteArray toDeviceId = *transferLicensesRequest.GetRequestedArguments().input.ToDeviceId;

	istd::TDelPtr<prolifedata::IHardwareProductBinding> fromDeviceBindingInfoPtr = GetOrCreateDeviceBinding(fromDeviceId);
	if (!fromDeviceBindingInfoPtr.IsValid()){
		return response;
	}

	istd::TDelPtr<prolifedata::IHardwareProductBinding> toDeviceBindingInfoPtr = GetOrCreateDeviceBinding(toDeviceId);
	if (!toDeviceBindingInfoPtr.IsValid()){
		return response;
	}

	prolifedata::IDeviceInfo* fromDeviceInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr deviceDataPtr;
	if (m_deviceCollectionCompPtr->GetObjectData(fromDeviceId, deviceDataPtr)){
		fromDeviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
	}

	if (fromDeviceInfoPtr == nullptr){
		errorMessage = QString("Unable to transfer license from '%1' to '%2'. Error: Device '%1' not exists").arg(qPrintable(fromDeviceId)).arg(qPrintable(toDeviceId));
		return response;
	}

	QByteArray projectId = fromDeviceInfoPtr->GetProject();

	QByteArrayList fromDeviceSoftwareIds = fromDeviceBindingInfoPtr->GetSoftwareIds();
	QByteArrayList toDeviceSoftwareIds = toDeviceBindingInfoPtr->GetSoftwareIds();

	if (!toDeviceSoftwareIds.isEmpty()){
		errorMessage = QString("Unable to transfer license from '%1' to '%2'. Error: Device '%2' already contains licenses").arg(qPrintable(fromDeviceId)).arg(qPrintable(toDeviceId));
		return response;
	}

	CreateDeviceOperationContext(fromDeviceId, "", QByteArrayList(), fromDeviceSoftwareIds);
	CreateDeviceOperationContext(toDeviceId, projectId, fromDeviceSoftwareIds, QByteArrayList());

	CreateSoftwareOperationContext(toDeviceId, projectId, fromDeviceSoftwareIds, QByteArrayList());
	CreateSoftwareOperationContext(fromDeviceId, projectId, QByteArrayList(), fromDeviceSoftwareIds);

	toDeviceBindingInfoPtr->SetSoftwareIds(fromDeviceSoftwareIds);
	fromDeviceBindingInfoPtr->SetSoftwareIds(QByteArrayList());

	if (!m_deviceBindingCollectionCompPtr->SetObjectData(toDeviceId, *toDeviceBindingInfoPtr)){
		errorMessage = QString("Unable to update hardware binding info");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	if (!m_deviceBindingCollectionCompPtr->SetObjectData(fromDeviceId, *fromDeviceBindingInfoPtr)){
		errorMessage = QString("Unable to update hardware binding info");
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");
		return response;
	}

	return response;
}


sdl::prolife::Sensors::CCreateLicenseFilePayload::V1_0 CDeviceControllerComp::OnCreateLicenseFile(
	const sdl::prolife::Sensors::V1_0::CCreateLicenseFileGqlRequest& createLicenseFileRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	sdl::prolife::Sensors::CCreateLicenseFilePayload::V1_0 response;

	sdl::prolife::Sensors::V1_0::CreateLicenseFileRequestArguments arguments = createLicenseFileRequest.GetRequestedArguments();
	QByteArray deviceId;
	if (arguments.input.DeviceId){
		deviceId = *arguments.input.DeviceId;
	}

	bool encrypt = true;
	if (arguments.input.Encrypt){
		encrypt = *arguments.input.Encrypt;
	}

	imtbase::IObjectCollection::DataPtr deviceDataPtr;
	if (!m_deviceCollectionCompPtr->GetObjectData(deviceId, deviceDataPtr)){
		errorMessage = QString("Unable to create license file for hardware '%1'. Error: Hardware object is invalid").arg(qPrintable(deviceId));
		SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");

		return response;
	}

	prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
	if (deviceInfoPtr == nullptr){
		Q_ASSERT(false);
		return response;
	}

	QByteArray macAddress = deviceInfoPtr->GetMacAddress();
	if (macAddress.isEmpty()){
		errorMessage = QString("Unable to create license file for hardware '%1'. Error: MAC Address is invalid").arg(qPrintable(deviceId));
		SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");

		return response;
	}

	prolifedata::IHardwareProductBinding* bindingInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr bindingDataPtr;
	if (m_deviceBindingCollectionCompPtr->GetObjectData(deviceId, bindingDataPtr)){
		bindingInfoPtr = dynamic_cast<prolifedata::IHardwareProductBinding*>(bindingDataPtr.GetPtr());
	}

	if (bindingInfoPtr == nullptr){
		errorMessage = QString("Unable to create license file for hardware '%1'. Error: There are no licenses for this sensor").arg(qPrintable(deviceId));
		SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");

		return response;
	}

	QByteArrayList softwareIds = bindingInfoPtr->GetSoftwareIds();
	if (softwareIds.isEmpty()){
		errorMessage = QString("Unable to create license file for hardware '%1'. Error: There are no licenses for this sensor").arg(qPrintable(deviceId));
		SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");

		return response;
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

					return response;
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
			errorMessage = QString("Unable to create license file for hardware '%1'. Error: Linked software '%2' is invalid").arg(qPrintable(deviceId)).arg(qPrintable(softwareId));
			SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");
			return response;
		}

		const imtbase::ICollectionInfo& licenseList = productInstanceInfoPtr->GetLicenseInstances();
		for (const QByteArray& licenseCollectionId : licenseList.GetElementIds()){
			imtlic::ILicenseInstance* licenseInstancePtr = dynamic_cast<imtlic::ILicenseInstance*>(const_cast<imtlic::ILicenseInstance*>(productInstanceInfoPtr->GetLicenseInstance(licenseCollectionId)));
			if (licenseInstancePtr == nullptr){
				errorMessage = QString("Unable to create license file for hardware '%1'. Error: License '%2' is invalid").arg(qPrintable(deviceId)).arg(qPrintable(licenseCollectionId));
				SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");
				return response;
			}

			imtlic::ILicenseDefinition* licenseDefinitionPtr = nullptr;
			imtbase::IObjectCollection::DataPtr licenseDefinitionDataPtr;
			if (m_licenseCollectionCompPtr->GetObjectData(licenseCollectionId, licenseDefinitionDataPtr)){
				licenseDefinitionPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDefinitionDataPtr.GetPtr());
			}

			if (licenseDefinitionPtr == nullptr){
				errorMessage = QString("Unable to create license file for hardware '%1'. Error: License '%2' is invalid").arg(qPrintable(deviceId)).arg(qPrintable(licenseCollectionId));
				SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");
				return response;
			}

			QByteArray licenseId = licenseDefinitionPtr->GetLicenseId();
			QString licenseName = licenseDefinitionPtr->GetLicenseName();
			QDateTime expiration = licenseInstancePtr->GetExpiration();

			productInstancePtr->AddLicense(licenseId, expiration);

			imtlic::ILicenseInstance* productLicenseInstancePtr = const_cast<imtlic::ILicenseInstance*>(productInstancePtr->GetLicenseInstance(licenseId));
			if (productLicenseInstancePtr == nullptr){
				Q_ASSERT(false);
				return response;
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
	QString username;
	const imtgql::IGqlContext* gqlContextPtr = createLicenseFileRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			isAdmin = userInfoPtr->IsAdmin();
			username = userInfoPtr->GetName();
		}
	}

	if (!isAdmin){
		encrypt = true;
	}

	m_productInstanceId = macAddress;

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
		return response;
	}

	int state = filePersistencePtr->SaveToFile(*productInstancePtr, filePathTmp);
	if (state != ifile::IFilePersistence::OS_OK){
		SendErrorMessage(0, "License file could not be saved", "CDeviceControllerComp");

		return response;
	}

	QFile file(filePathTmp);

	if (!file.open(QIODevice::ReadOnly)){
		errorMessage = QString("Unable to create license file for hardware '%1'. Error: File '%1' could not be open").arg(qPrintable(deviceId)).arg(qPrintable(filePathTmp));
		SendCriticalMessage(0, errorMessage, "CDeviceControllerComp");

		return response;
	}

	QByteArray originalData = file.readAll();

	QByteArray returnedData = originalData.toBase64();

	QString name = macAddress.split(':').join('_') + "_" + "License.lic";

	response.Data = returnedData;
	response.Name = name;

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
						*productInstanceInfoPtr));
				}

				if (!m_softwareProductCollectionCompPtr->SetObjectData(softwareId, *productInstanceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
					SendWarningMessage(0, "Error when trying update software instance", "CDeviceControllerComp");
				}
			}
		}
	}

	{
		istd::TDelPtr<imtbase::IOperationContext> operationContextPtr =  nullptr;
		if (m_deviceOperationContextControllerCompPtr.IsValid()){
			operationContextPtr.SetPtr(m_softwareOperationContextControllerCompPtr->CreateOperationContext(
				"CreateLicenseFile",
				deviceId,
				*deviceDataPtr.GetPtr()));
		}

		if (!m_deviceCollectionCompPtr->SetObjectData(deviceId, *deviceDataPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
			SendWarningMessage(0, "Error when trying update hardware product", "CDeviceControllerComp");
		}
	}

	SendInfoMessage(0, QString("License file for hardware '%1' successfully created. User: '%2'").arg(QString(macAddress)).arg(username), "CDeviceControllerComp");

	return response;
}


sdl::prolife::Sensors::CDecryptLicenseFilePayload::V1_0 CDeviceControllerComp::OnDecryptLicenseFile(
	const sdl::prolife::Sensors::V1_0::CDecryptLicenseFileGqlRequest& decryptLicenseFileRequest,
	const ::imtgql::CGqlRequest& gqlRequest,
	QString& errorMessage) const
{
	sdl::prolife::Sensors::CDecryptLicenseFilePayload::V1_0 response;

	if (!m_encryptionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'Encryption' was not set", "CDeviceControllerComp");
		return response;
	}

	const imtgql::IGqlContext* gqlContextPtr = decryptLicenseFileRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		errorMessage = QString("Unable to decrypt license file. Error: GraphQL context is invalid");
		return response;
	}

	imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to decrypt license file. Error: User info is invalid");
		return response;
	}

	if (!userInfoPtr->IsAdmin()){
		errorMessage = QString("Unable to decrypt license file. Error: Permission denied");
		return response;
	}

	sdl::prolife::Sensors::V1_0::DecryptLicenseFileRequestArguments arguments = decryptLicenseFileRequest.GetRequestedArguments();

	QByteArray encryptedData;
	if (arguments.input.FileData){
		encryptedData = *arguments.input.FileData;
	}

	QByteArray encryptionKey;
	if (arguments.input.Key){
		encryptionKey = *arguments.input.Key;
	}

	m_productInstanceId = encryptionKey;

	encryptedData = QByteArray::fromBase64(encryptedData);

	QByteArray decryptedData;
	if (!m_encryptionCompPtr->DecryptData(encryptedData, imtcrypt::IEncryption::EA_AES, *this, decryptedData)){
		errorMessage = QString("Unable to decrypt license file. Error: Decryption data failed");
		return response;
	}

	QString name = encryptionKey.split(':').join('_') + "_" + "Decrypted.lic";
	response.DecryptedData = decryptedData.toBase64();
	response.FileName = name;

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
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_deviceBindingCollectionCompPtr->GetObjectData(deviceId, dataPtr)){
		istd::TDelPtr<prolifedata::IHardwareProductBinding> deviceBindingInfoPtr;
		deviceBindingInfoPtr.SetCastedOrRemove(dataPtr.GetPtr()->CloneMe());

		return deviceBindingInfoPtr.PopPtr();
	}

	istd::TDelPtr<prolifedata::CHardwareProductBinding> deviceBindingInfoPtr;
	deviceBindingInfoPtr.SetPtr(new prolifedata::CHardwareProductBinding());
	deviceBindingInfoPtr->SetHardwareId(deviceId);

	m_deviceBindingCollectionCompPtr->InsertNewObject("", "", "", deviceBindingInfoPtr.GetPtr(), deviceId);

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
					operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext("BindLicense", deviceId, *deviceInfoPtr, &paramsSet);
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
							operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Bind", id, *productInstanceInfoPtr, &paramsSet);
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
						operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Bind", id, *productInstanceInfoPtr, &paramsSet);
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

	for (const QByteArray& licenseId : dependencies){
		retVal << GetAllLicenseDependencies(licenseId);
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


