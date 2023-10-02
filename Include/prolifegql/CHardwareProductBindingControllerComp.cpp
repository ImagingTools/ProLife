#include <prolifegql/CHardwareProductBindingControllerComp.h>


// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>

// ProLife includes
#include <prolifedata/CHardwareProductBinding.h>


namespace prolifegql
{


istd::IChangeable* CHardwareProductBindingControllerComp::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray& objectId,
			QString& /*name*/,
			QString& /*description*/,
			QString& /*errorMessage*/) const
{
	QByteArray itemData;
	if (!inputParams.empty()){
		objectId = inputParams.at(0).GetFieldArgumentValue("Id").toByteArray();
		itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	}

	imtbase::CTreeItemModel itemModel;
	if (!itemModel.CreateFromJson(itemData)){
		return nullptr;
	}

	istd::TDelPtr<prolifedata::CHardwareProductBinding> hardwareProductBindingPtr;
	hardwareProductBindingPtr.SetPtr(new prolifedata::CHardwareProductBinding());

	if (objectId.isEmpty()){
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
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	QByteArray objectId;
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		prolifedata::IHardwareProductBinding* productBindingPtr = dynamic_cast<prolifedata::IHardwareProductBinding*>(dataPtr.GetPtr());
		if (productBindingPtr != nullptr){
			QByteArray hardwareId = productBindingPtr->GetHardwareId();
			QByteArrayList softwareIds = productBindingPtr->GetSoftwareIds();

			dataModelPtr->SetData("Id", objectId);
			dataModelPtr->SetData("SoftwareIds", softwareIds.join(';'));

			return rootModelPtr.PopPtr();
		}
	}

	return nullptr;
}


imtbase::CTreeItemModel* CHardwareProductBindingControllerComp::UpdateObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray objectId;
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	}

	QString name;
	QString description;
	istd::IChangeable* objectPtr = CreateObject(gqlRequest.GetParams(), objectId, name, description, errorMessage);
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

	if (m_softwareProductCollectionCompPtr.IsValid()){
		for (const QByteArray& id : qAsConst(removedLicenses)){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_softwareProductCollectionCompPtr->GetObjectData(id, dataPtr)){
				imtlic::IProductInstanceInfo* productInstanceInfoPtr =  dynamic_cast<imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
				if (productInstanceInfoPtr != nullptr){
					if (productInstanceInfoPtr->IsInUse()){
						productInstanceInfoPtr->SetInUse(false);

						imtbase::IOperationContext* operationContextPtr = CreateOperationContext(gqlRequest, QString("Updated the object from the hardware binding"));
						if (!m_softwareProductCollectionCompPtr->SetObjectData(id, *productInstanceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
							return nullptr;
						}
					}
				}
			}
		}
	}

	imtbase::IOperationContext* operationContextPtr = CreateOperationContext(gqlRequest, QString("Updated the object"));
	if (!m_objectCollectionCompPtr->SetObjectData(objectId, *newHardwareBindingObjectPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
		errorMessage = QObject::tr("Can not update object: %1").arg(qPrintable(objectId));

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* notificationModelPtr = rootModelPtr->AddTreeModel("updatedNotification");
	Q_ASSERT(notificationModelPtr != nullptr);

	notificationModelPtr->SetData("Id", objectId);
	notificationModelPtr->SetData("Name", name);

	return rootModelPtr.PopPtr();
}


} // namespace prolifegql


