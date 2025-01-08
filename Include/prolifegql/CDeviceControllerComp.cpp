#include <prolifegql/CDeviceControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <istd/TOptDelPtr.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/ILicenseDefinition.h>

// ProLife includes
#include <prolifedata/CHardwareProductBinding.h>
#include <prolifedata/IDeviceInfo.h>


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


} // namespace prolifegql


