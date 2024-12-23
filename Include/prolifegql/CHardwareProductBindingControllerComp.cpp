#include <prolifegql/CHardwareProductBindingControllerComp.h>


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


imtbase::CTreeItemModel* CHardwareProductBindingControllerComp::GetObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to get data object. Error: Attribute 'm_objectCollectionCompPtr' was not set").toUtf8();
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to get data object. Error: GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CObjectCollectionControllerCompBase");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	QByteArray objectTypeId = GetObjectTypeIdFromRequest(gqlRequest);

	istd::TOptDelPtr<prolifedata::CHardwareProductBinding> hardwareProductBindingPtr;

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		hardwareProductBindingPtr.SetCastedOrRemove(dataPtr.GetPtr(), false);
	}

	if (!hardwareProductBindingPtr.IsValid()){
		prolifedata::CHardwareProductBinding* infoPtr = new prolifedata::CHardwareProductBinding();
		infoPtr->SetHardwareId(objectId);

		hardwareProductBindingPtr.SetPtr(infoPtr, true);

		m_objectCollectionCompPtr->InsertNewObject("", "", "", infoPtr, objectId);
	}

	if (!hardwareProductBindingPtr.IsValid()){
		errorMessage = QString("Unable to get hardware object '%1'. Error: Object is invalid.").arg(qPrintable(objectId));
		return nullptr;
	}

	return BaseClass::GetObject(gqlRequest, errorMessage);
}


bool CHardwareProductBindingControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& /*objectCollectionIterator*/,
			const sdl::prolife::SensorBinding::V1_0::CGetSensorBindingListGqlRequest& /*getSensorBindingListRequest*/,
			sdl::prolife::SensorBinding::CSensorBindingItem::V1_0& /*representationObject*/,
			QString& /*errorMessage*/) const
{
	return true;
}


istd::IChangeable* CHardwareProductBindingControllerComp::CreateObjectFromRepresentation(
			const sdl::prolife::SensorBinding::CSensorBindingData::V1_0& sensorBindingDataRepresentation,
			QByteArray& newObjectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	istd::TDelPtr<prolifedata::CHardwareProductBinding> hardwareProductBindingPtr;
	hardwareProductBindingPtr.SetPtr(new prolifedata::CHardwareProductBinding());

	if (sensorBindingDataRepresentation.Id){
		newObjectId = *sensorBindingDataRepresentation.Id;
	}

	if (newObjectId.isEmpty()){
		errorMessage = QString(QT_TR_NOOP("Unable to create object with empty ID"));
		SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

		return nullptr;
	}

	hardwareProductBindingPtr->SetHardwareId(newObjectId);

	QByteArray softwareLinkInfos;
	if (sensorBindingDataRepresentation.SoftwareIds){
		softwareLinkInfos = *sensorBindingDataRepresentation.SoftwareIds;
	}

	QByteArrayList softwareIds = softwareLinkInfos.split(';');
	softwareIds.removeAll("");
	hardwareProductBindingPtr->SetSoftwareIds(softwareIds);

	return hardwareProductBindingPtr.PopPtr();
}


bool CHardwareProductBindingControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::prolife::SensorBinding::V1_0::CGetSensorBindingGqlRequest& getSensorBindingRequest,
			sdl::prolife::SensorBinding::CSensorBindingDataPayload::V1_0& representationPayload,
			QString& errorMessage) const
{
	const prolifedata::IHardwareProductBinding* productBindingPtr = dynamic_cast<const prolifedata::IHardwareProductBinding*>(&data);
	if (productBindingPtr == nullptr){
		errorMessage = QString("Unable to create a hardware binding object. Error: Object is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

		return false;
	}

	sdl::prolife::SensorBinding::V1_0::GetSensorBindingRequestArguments arguments = getSensorBindingRequest.GetRequestedArguments();

	sdl::prolife::SensorBinding::CSensorBindingData::V1_0 sensorBindingData;

	QByteArray hardwareId = productBindingPtr->GetHardwareId();
	sensorBindingData.Id = std::make_optional<QByteArray>(hardwareId);

	QByteArrayList softwareIds = productBindingPtr->GetSoftwareIds();
	sensorBindingData.SoftwareIds = std::make_optional<QByteArray>(softwareIds.join(';'));

	if (!softwareIds.isEmpty()){
		if (m_softwareProductCollectionCompPtr.IsValid()){
			QByteArray softwareId = softwareIds[0];

			imtbase::IObjectCollection::DataPtr softwareDataPtr;
			if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, softwareDataPtr)){
				imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
				if (productInstanceInfoPtr != nullptr){
					QByteArray project = productInstanceInfoPtr->GetProject();
					sensorBindingData.Project = std::make_optional<QString>(project);

					QByteArray productId = productInstanceInfoPtr->GetProductId();
					sensorBindingData.ProductUuid = std::make_optional<QByteArray>(productId);
				}
			}
		}
	}

	representationPayload.SensorBindingData = std::make_optional<sdl::prolife::SensorBinding::CSensorBindingData::V1_0>(sensorBindingData);

	return true;
}


imtbase::CTreeItemModel* CHardwareProductBindingControllerComp::UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("Unable to update hardware object. Error: GraphQL input parameters is invalid").toUtf8();
		SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	QByteArray itemData = inputParamPtr->GetFieldArgumentValue("Item").toByteArray();
	QString project = inputParamPtr->GetFieldArgumentValue("Project").toString();

	QString name;
	QString description;

	istd::TDelPtr<prolifedata::IHardwareProductBinding> newHardwareBindingObjectPtr;
	newHardwareBindingObjectPtr.SetCastedOrRemove(CreateObjectFromRequest(gqlRequest, objectId, name, description, errorMessage));

	if (!newHardwareBindingObjectPtr.IsValid()){
		errorMessage = QString("Unable to update hardware object. Error: Object from request is invalid").toUtf8();
		SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

		return nullptr;
	}

	prolifedata::IHardwareProductBinding* hardwareBindingObjectPtr = nullptr;

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		hardwareBindingObjectPtr = dynamic_cast<prolifedata::IHardwareProductBinding*>(dataPtr.GetPtr());
	}

	if (hardwareBindingObjectPtr == nullptr){
		istd::TDelPtr<imtbase::CTreeItemModel> resultPtr = InsertObject(gqlRequest, errorMessage);
		if (!resultPtr.IsValid()){
			return nullptr;
		}

		if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			hardwareBindingObjectPtr = dynamic_cast<prolifedata::IHardwareProductBinding*>(dataPtr.GetPtr());
		}
	}

	if (hardwareBindingObjectPtr == nullptr){
		errorMessage = QString("Unable to update hardware object. Error: Object from request is invalid").toUtf8();
		SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

		return nullptr;
	}

	QByteArrayList newHardwareBindingSoftwareIds = newHardwareBindingObjectPtr->GetSoftwareIds();
	QByteArrayList hardwareBindingSoftwareIds = hardwareBindingObjectPtr->GetSoftwareIds();

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

	if (m_deviceCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr deviceDataPtr;
		if (m_deviceCollectionCompPtr->GetObjectData(objectId, deviceDataPtr)){
			prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
			if (deviceInfoPtr != nullptr){
				deviceInfoPtr->SetProject(project.toUtf8());

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
					operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext("BindLicense", objectId, *deviceInfoPtr, &paramsSet);
				}

				if (!m_deviceCollectionCompPtr->SetObjectData(objectId, *deviceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
					errorMessage = QString("Unable to update device object.");
					SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

					return nullptr;
				}
			}
		}
	}

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
						SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

						return nullptr;
					}
				}
			}
		}
	}

	if (m_softwareProductCollectionCompPtr.IsValid()){
		for (const QByteArray& id : addedLicenses){
			imtbase::IObjectCollection::DataPtr softwareDataPtr;
			if (m_softwareProductCollectionCompPtr->GetObjectData(id, softwareDataPtr)){
				imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
				if (productInstanceInfoPtr != nullptr){
					if (!productInstanceInfoPtr->IsInUse()){
						productInstanceInfoPtr->SetProject(project.toUtf8());

						iprm::CTextParam textParam;
						textParam.SetText(objectId);

						iprm::CParamsSet paramsSet;
						paramsSet.SetEditableParameter("AddedHardwareId", &textParam);

						istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
						if (m_softwareOperationContextControllerCompPtr.IsValid()){
							operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Bind", id, *productInstanceInfoPtr, &paramsSet);
						}

						if (!m_softwareProductCollectionCompPtr->SetObjectData(id, *productInstanceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
							errorMessage = QString("Unable to update software instance object.");
							SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

							return nullptr;
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
					textParam.SetText(objectId);

					iprm::CParamsSet paramsSet;
					paramsSet.SetEditableParameter("RemovedHardwareId", &textParam);

					istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
					if (m_softwareOperationContextControllerCompPtr.IsValid()){
						operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext("Bind", id, *productInstanceInfoPtr, &paramsSet);
					}

					if (!m_softwareProductCollectionCompPtr->SetObjectData(id, *productInstanceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
						return nullptr;
					}
				}
			}
		}
	}

	return BaseClass::UpdateObject(gqlRequest, errorMessage);
}


bool CHardwareProductBindingControllerComp::UpdateObjectFromRepresentationRequest(const imtgql::CGqlRequest& rawGqlRequest, const sdl::prolife::SensorBinding::V1_0::CUpdateSensorBindingGqlRequest& updateSensorBindingRequest, istd::IChangeable& object, QString& errorMessage) const
{
	return false;
}


} // namespace prolifegql


