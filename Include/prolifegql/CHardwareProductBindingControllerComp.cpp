#include <prolifegql/CHardwareProductBindingControllerComp.h>


// ProLife includes
#include <prolifedata/CHardwareProductBinding.h>


namespace prolifegql
{


istd::IChangeable* CHardwareProductBindingControllerComp::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray& objectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
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

	if (itemModel.ContainsKey("SoftwareLinks")){
		imtbase::CTreeItemModel *linksModel = itemModel.GetTreeItemModel("SoftwareLinks");

		for (int linkIndex = 0; linkIndex < linksModel->GetItemsCount(); linksModel++){
			QByteArray softwareId = linksModel->GetData("softwareId", linkIndex).toByteArray();
			QByteArray hardwareId = linksModel->GetData("hardwareId", linkIndex).toByteArray();
			hardwareProductBindingPtr->Bind(softwareId);
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


} // namespace prolifegql


