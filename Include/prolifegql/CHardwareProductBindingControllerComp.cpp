#include <prolifegql/CHardwareProductBindingControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/ILicenseDefinition.h>

// ProLife includes
#include <prolifedata/CHardwareProductBinding.h>
#include <prolifedata/IDeviceInfo.h>


namespace prolifegql
{


istd::IChangeable* CHardwareProductBindingControllerComp::CreateObjectFromRequest(
		const imtgql::CGqlRequest& gqlRequest,
		QByteArray& objectId,
		QString& /*name*/,
		QString& /*description*/,
		QString& errorMessage) const
{
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CDeviceControllerComp");

		return nullptr;
	}

	QByteArray itemData;

	objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	itemData = inputParamPtr->GetFieldArgumentValue("Item").toByteArray();

	imtbase::CTreeItemModel itemModel;
	if (!itemModel.CreateFromJson(itemData)){
		errorMessage = QString("Unable to create representation model from json: %1.").arg(qPrintable(itemData));
		SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

		return nullptr;
	}

	istd::TDelPtr<prolifedata::CHardwareProductBinding> hardwareProductBindingPtr;
	hardwareProductBindingPtr.SetPtr(new prolifedata::CHardwareProductBinding());

	if (objectId.isEmpty()){
		errorMessage = QString(QT_TR_NOOP("Unable to create object with empty ID."));
		SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

		return nullptr;
	}

	hardwareProductBindingPtr->SetHardwareId(objectId);

	if (itemModel.ContainsKey("SoftwareIds")){
		QByteArray softwareIds = itemModel.GetData("SoftwareIds").toByteArray();
		if (!softwareIds.isEmpty()){
			hardwareProductBindingPtr->SetSoftwareIds(softwareIds.split(';'));
		}
		else{
			hardwareProductBindingPtr->SetSoftwareIds(QByteArrayList());
		}
	}

	return hardwareProductBindingPtr.PopPtr();
}


imtbase::CTreeItemModel* CHardwareProductBindingControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("'m_objectCollectionCompPtr' is invalid").toUtf8();
		SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

		return nullptr;
	}

	QByteArray objectId;
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	dataModelPtr->SetData("Id", objectId);
	dataModelPtr->SetData("SoftwareIds", "");
	dataModelPtr->SetData("ProductUuid", "");
	dataModelPtr->SetData("Project", "");

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		prolifedata::IHardwareProductBinding* productBindingPtr = dynamic_cast<prolifedata::IHardwareProductBinding*>(dataPtr.GetPtr());
		if (productBindingPtr != nullptr){
			const QByteArray hardwareId = productBindingPtr->GetHardwareId();
			QByteArrayList softwareIds = productBindingPtr->GetSoftwareIds();

			if (!softwareIds.isEmpty()){
				dataModelPtr->SetData("SoftwareIds", softwareIds.join(';'));

				// Get Product-ID from first software ID
				if (m_softwareProductCollectionCompPtr.IsValid()){
					QByteArray softwareId = softwareIds[0];

					imtbase::IObjectCollection::DataPtr softwareDataPtr;
					if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, softwareDataPtr)){
						imtlic::IProductInstanceInfo* productInstanceInfoPtr =  dynamic_cast<imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
						if (productInstanceInfoPtr != nullptr){
							QByteArray project = productInstanceInfoPtr->GetProject();
							dataModelPtr->SetData("Project", project);

							QByteArray productId = productInstanceInfoPtr->GetProductId();
							dataModelPtr->SetData("ProductUuid", productId);
						}
					}
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CHardwareProductBindingControllerComp::InsertObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		errorMessage = QString("GraphQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

		return nullptr;
	}

	QByteArray objectId;
	QString name;
	QString description;

	istd::IChangeable* objectPtr = CreateObjectFromRequest(gqlRequest, objectId, name, description, errorMessage);
	if (objectPtr == nullptr){
		errorMessage = QString("Unable to create object from GQL input params.").toUtf8();
		SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

		return nullptr;
	}

	prolifedata::IHardwareProductBinding* hardwareBindingObjectPtr = dynamic_cast<prolifedata::IHardwareProductBinding*>(objectPtr);
	if (hardwareBindingObjectPtr == nullptr){
		errorMessage = QString("Hardware binding object is nullptr").toUtf8();
		SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

		return nullptr;
	}

	if (m_deviceCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_deviceCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(dataPtr.GetPtr());
			if (deviceInfoPtr != nullptr){
				imtbase::IOperationContext* operationContextPtr = nullptr;

				QByteArrayList softwareIds = hardwareBindingObjectPtr->GetSoftwareIds();

				iprm::CTextParam textParam;
				textParam.SetText(softwareIds.join(';'));

				iprm::CParamsSet paramsSet;
				paramsSet.SetEditableParameter("AddedProductIds", &textParam);

				if (m_deviceOperationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_USER, gqlRequest, objectId, deviceInfoPtr, &paramsSet);
				}

				if (!m_deviceCollectionCompPtr->SetObjectData(objectId, *deviceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
					errorMessage = QString("Unable to update device object.");
					SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

					return nullptr;
				}
			}
		}
	}

	if (m_softwareProductCollectionCompPtr.IsValid()){
		QByteArrayList softwareIds = hardwareBindingObjectPtr->GetSoftwareIds();
		for (const QByteArray& id : std::as_const(softwareIds)){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_softwareProductCollectionCompPtr->GetObjectData(id, dataPtr)){
				imtlic::IProductInstanceInfo* productInstanceInfoPtr =  dynamic_cast<imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
				if (productInstanceInfoPtr != nullptr){
					if (!productInstanceInfoPtr->IsInUse()){
						imtbase::IOperationContext* operationContextPtr = nullptr;

						iprm::CTextParam textParam;
						textParam.SetText(objectId);

						iprm::CParamsSet paramsSet;
						paramsSet.SetEditableParameter("AddedHardwareId", &textParam);

						if (m_softwareOperationContextControllerCompPtr.IsValid()){
							operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_USER, gqlRequest, id, productInstanceInfoPtr, &paramsSet);
						}

						if (!m_softwareProductCollectionCompPtr->SetObjectData(id, *productInstanceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
							errorMessage = QString("Unable to update software instance object.");
							SendErrorMessage(0, errorMessage, "CHardwareProductBindingControllerComp");

							return nullptr;
						}
					}
				}
			}
		}
	}

	imtbase::IOperationContext* operationContextPtr = nullptr;

	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_CREATE, gqlRequest);
	}

	QByteArray newObjectId = m_objectCollectionCompPtr->InsertNewObject("DocumentInfo", name, description, hardwareBindingObjectPtr, objectId, nullptr, nullptr, operationContextPtr);
	if (newObjectId.isEmpty()){
		errorMessage = QT_TR_NOOP(QString("Can not insert object: %1").arg(qPrintable(objectId)));
		SendErrorMessage(0, QString("Can not insert object: %1").arg(qPrintable(objectId)), "CHardwareProductBindingControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	Q_ASSERT(dataModelPtr != nullptr);

	imtbase::CTreeItemModel* notificationModelPtr = dataModelPtr->AddTreeModel("addedNotification");
	Q_ASSERT(notificationModelPtr != nullptr);

	notificationModelPtr->SetData("Id", objectId);
	notificationModelPtr->SetData("Name", name);

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CHardwareProductBindingControllerComp::UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray itemData;
	QByteArray objectId;
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
		itemData = inputParamPtr->GetFieldArgumentValue("Item").toByteArray();
	}

	QByteArray project;
	imtbase::CTreeItemModel itemModel;
	if (itemModel.CreateFromJson(itemData)){
		if (itemModel.ContainsKey("Project")){
			project = itemModel.GetData("Project").toByteArray();
		}
	}

	QString name;
	QString description;
	istd::IChangeable* objectPtr = CreateObjectFromRequest(gqlRequest, objectId, name, description, errorMessage);
	if (objectPtr == nullptr){
		return nullptr;
	}

	prolifedata::IHardwareProductBinding* newHardwareBindingObjectPtr = dynamic_cast<prolifedata::IHardwareProductBinding*>(objectPtr);
	if (newHardwareBindingObjectPtr == nullptr){
		return nullptr;
	}

	prolifedata::IHardwareProductBinding* hardwareBindingObjectPtr = nullptr;

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		hardwareBindingObjectPtr = dynamic_cast<prolifedata::IHardwareProductBinding*>(dataPtr.GetPtr());
	}

	if (hardwareBindingObjectPtr == nullptr){
		imtbase::CTreeItemModel* resultPtr = InsertObject(gqlRequest, errorMessage);
		if (resultPtr == nullptr){
			return nullptr;
		}

		if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
			hardwareBindingObjectPtr = dynamic_cast<prolifedata::IHardwareProductBinding*>(dataPtr.GetPtr());
		}
	}

	if (hardwareBindingObjectPtr == nullptr){
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

				imtbase::IOperationContext* operationContextPtr =  nullptr;

				if (m_deviceOperationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_USER, gqlRequest, objectId, deviceInfoPtr, &paramsSet);
				}

				if (!m_deviceCollectionCompPtr->SetObjectData(objectId, *deviceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
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
				imtlic::IProductInstanceInfo* productInstanceInfoPtr =  dynamic_cast<imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
				if (productInstanceInfoPtr != nullptr){
					productInstanceInfoPtr->SetProject(project);

					imtbase::IOperationContext* operationContextPtr = nullptr;

					if (m_softwareOperationContextControllerCompPtr.IsValid()){
						operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, softwareId, productInstanceInfoPtr);
					}

					if (!m_softwareProductCollectionCompPtr->SetObjectData(softwareId, *productInstanceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
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
				imtlic::IProductInstanceInfo* productInstanceInfoPtr =  dynamic_cast<imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
				if (productInstanceInfoPtr != nullptr){
					if (!productInstanceInfoPtr->IsInUse()){
						productInstanceInfoPtr->SetProject(project);

						imtbase::IOperationContext* operationContextPtr = nullptr;

						iprm::CTextParam textParam;
						textParam.SetText(objectId);

						iprm::CParamsSet paramsSet;
						paramsSet.SetEditableParameter("AddedHardwareId", &textParam);

						if (m_softwareOperationContextControllerCompPtr.IsValid()){
							operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_USER, gqlRequest, id, productInstanceInfoPtr, &paramsSet);
						}

						if (!m_softwareProductCollectionCompPtr->SetObjectData(id, *productInstanceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
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

					imtbase::IOperationContext* operationContextPtr = nullptr;

					iprm::CTextParam textParam;
					textParam.SetText(objectId);

					iprm::CParamsSet paramsSet;
					paramsSet.SetEditableParameter("RemovedHardwareId", &textParam);

					if (m_softwareOperationContextControllerCompPtr.IsValid()){
						operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_USER, gqlRequest, id, productInstanceInfoPtr, &paramsSet);
					}

					if (!m_softwareProductCollectionCompPtr->SetObjectData(id, *productInstanceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
						return nullptr;
					}
				}
			}
		}
	}

	imtbase::IOperationContext* operationContextPtr = nullptr;

	if (m_operationContextControllerCompPtr.IsValid()){
		operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, objectId, newHardwareBindingObjectPtr);
	}

	if (!m_objectCollectionCompPtr->SetObjectData(objectId, *newHardwareBindingObjectPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
		errorMessage = QString("Can not update object: %1").arg(qPrintable(objectId));

		return nullptr;
	}

	// Update project

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* notificationModelPtr = rootModelPtr->AddTreeModel("updatedNotification");
	Q_ASSERT(notificationModelPtr != nullptr);

	notificationModelPtr->SetData("Id", objectId);
	notificationModelPtr->SetData("Name", name);

	return rootModelPtr.PopPtr();
}


QString CHardwareProductBindingControllerComp::GetLicenseName(const QByteArray& productUuid) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_softwareProductCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
		const imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
		if (productInstanceInfoPtr != nullptr){
			imtbase::ICollectionInfo::Ids licenseCollectionIds = productInstanceInfoPtr->GetLicenseInstances().GetElementIds();
			if (!licenseCollectionIds.isEmpty()){
				QByteArray licenseDefinitionUuid = licenseCollectionIds[0];

				imtbase::IObjectCollection::DataPtr licenseDataPtr;
				if (m_licenseCollectionCompPtr->GetObjectData(licenseDefinitionUuid, licenseDataPtr)){
					const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
					if (licenseInfoPtr != nullptr){
						QByteArray licenseId = licenseInfoPtr->GetLicenseId();
						QString licenseName = licenseInfoPtr->GetLicenseName();

						QString name = licenseName + " (" + licenseId + ")";
						return name;
					}
				}
			}
		}
	}

	return QString();
}


} // namespace prolifegql


