#include <prolifegql/CSoftwareProductControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>
#include <iprm/CEnableableParam.h>

// ImtCore includes
#include <imtbase/CObjectLink.h>
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/IHardwareInstanceInfo.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>


namespace prolifegql
{


imtbase::CTreeItemModel* CSoftwareProductControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();

		return nullptr;
	}

	QByteArray objectId;
	const imtgql::CGqlObject* inputObjectPtr = gqlRequest.GetParam("input");
	if (inputObjectPtr != nullptr){
		objectId = inputObjectPtr->GetFieldArgumentValue("Id").toByteArray();
	}

	if (objectId.isEmpty()){
		errorMessage = QObject::tr("Unable to get an object").toUtf8();

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productOrderInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
		if (productOrderInfoPtr != nullptr){
			QByteArray serialNumber = productOrderInfoPtr->GetSerialNumber();

			dataModelPtr->SetData("Id", objectId);
			dataModelPtr->SetData("ProductId", productOrderInfoPtr->GetProductId());
			dataModelPtr->SetData("CategoryId", productOrderInfoPtr->GetFactoryId());
			dataModelPtr->SetData("SerialNumber", serialNumber);
			dataModelPtr->SetData("Project", productOrderInfoPtr->GetProject());
			dataModelPtr->SetData("InUse", productOrderInfoPtr->IsInUse());
			dataModelPtr->SetData("OrderUuid", productOrderInfoPtr->GetOrderId());

			imtbase::ICollectionInfo::Ids licenseIds = productOrderInfoPtr->GetLicenseInstances().GetElementIds();
			if (!licenseIds.isEmpty()){
				QByteArray licenseId = licenseIds[0];

				const imtlic::ILicenseInstance* licenseInstancePtr = productOrderInfoPtr->GetLicenseInstance(licenseId);
				if (licenseInstancePtr != nullptr){
					dataModelPtr->SetData("LicenseId", licenseInstancePtr->GetLicenseId());
					dataModelPtr->SetData("Expiration", licenseInstancePtr->GetExpiration().toString("yyyy-MM-dd"));
				}
			}

			QString name = productOrderInfoPtr->GetProductId();

			if (!serialNumber.isEmpty()){
				name += " (" + serialNumber + ")";
			}

			dataModelPtr->SetData("Name", name);

			return rootModelPtr.PopPtr();
		}
	}

	return nullptr;
}


imtbase::CTreeItemModel* CSoftwareProductControllerComp::UpdateObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	QByteArray objectUuid;
	QByteArray itemData;
	const imtgql::CGqlObject* inputObjectPtr = gqlRequest.GetParam("input");
	if (inputObjectPtr != nullptr){
		objectUuid = inputObjectPtr->GetFieldArgumentValue("Id").toByteArray();
		itemData = inputObjectPtr->GetFieldArgumentValue("Item").toByteArray();
	}

	if (objectUuid.isEmpty()){
		errorMessage = QObject::tr("Unable to update an object %1").arg(qPrintable(objectUuid)).toUtf8();

		return nullptr;
	}

	prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productOrderInfoPtr = nullptr;

	imtbase::IObjectCollection::DataPtr orderProductDataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectUuid, orderProductDataPtr)){
		productOrderInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(orderProductDataPtr.GetPtr());
	}

	if (productOrderInfoPtr == nullptr){
		return nullptr;
	}

	if (productOrderInfoPtr->IsInUse()){
		errorMessage = QString("It is not possible to update an product in use");

		return nullptr;
	}

	imtbase::CTreeItemModel itemModel;
	if (!itemModel.CreateFromJson(itemData)){
		errorMessage = QObject::tr("Unable to create an item model from json: %1").arg(qPrintable(itemData)).toUtf8();

		return nullptr;
	}

	QByteArray serialNumber;
	if (itemModel.ContainsKey("SerialNumber")){
		serialNumber = itemModel.GetData("SerialNumber").toByteArray();
	}

	QByteArray project;
	if (itemModel.ContainsKey("Project")){
		project = itemModel.GetData("Project").toByteArray();
	}

	QByteArray productId;
	if (itemModel.ContainsKey("ProductId")){
		productId = itemModel.GetData("ProductId").toByteArray();
	}

	QByteArray orderUuid;
	if (itemModel.ContainsKey("OrderUuid")){
		orderUuid = itemModel.GetData("OrderUuid").toByteArray();
	}

	QByteArray licenseId;
	if (itemModel.ContainsKey("LicenseId")){
		licenseId = itemModel.GetData("LicenseId").toByteArray();
	}

	QByteArray expiration;
	if (itemModel.ContainsKey("Expiration")){
		expiration = itemModel.GetData("Expiration").toByteArray();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	if (!serialNumber.isEmpty()){
		iprm::CTextParam valueParam;
		valueParam.SetText(serialNumber);

		iprm::CEnableableParam isEqualParam;
		isEqualParam.SetEnabled(true);

		iprm::CParamsSet valueParamsSet;
		valueParamsSet.SetEditableParameter("Value", &valueParam);
		valueParamsSet.SetEditableParameter("IsEqual", &isEqualParam);

		iprm::CParamsSet paramsSet;
		paramsSet.SetEditableParameter("SerialNumber", &valueParamsSet);

		iprm::CParamsSet filterParam;
		filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

		imtbase::IObjectCollection::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
		if (!collectionIds.isEmpty()){
			QByteArray objectId = collectionIds[0];
			if (objectId != objectUuid){
				errorMessage = QT_TR_NOOP("Serial Number already exists");

				imtbase::CTreeItemModel* errorsModelPtr = rootModelPtr->AddTreeModel("errors");
				errorsModelPtr->SetData("message", errorMessage);

				return rootModelPtr.PopPtr();
			}
		}
	}

	QByteArray oldOrderUuid = productOrderInfoPtr->GetOrderId();
	QByteArray oldProject = productOrderInfoPtr->GetProject();
	QByteArray oldProductId = productOrderInfoPtr->GetProductId();

	QByteArray oldExpiration;
	QByteArray oldLicenseId;
	imtbase::ICollectionInfo::Ids licenseIds = productOrderInfoPtr->GetLicenseInstances().GetElementIds();
	if (!licenseIds.isEmpty()){
		QByteArray licenseId = licenseIds[0];

		const imtlic::ILicenseInstance* licenseInstancePtr = productOrderInfoPtr->GetLicenseInstance(licenseId);
		if (licenseInstancePtr != nullptr){
			oldLicenseId = licenseInstancePtr->GetLicenseId();
			oldExpiration = licenseInstancePtr->GetExpiration().toString("yyyy-MM-dd").toUtf8();
		}
	}

	QByteArray oldSerialNumber = productOrderInfoPtr->GetSerialNumber();
	if (oldSerialNumber != serialNumber || oldExpiration != expiration || oldLicenseId != licenseId ||
			oldProject != project || oldOrderUuid != orderUuid || oldProductId != productId){
		productOrderInfoPtr->SetSerialNumber(serialNumber);
		productOrderInfoPtr->SetProject(project);
		productOrderInfoPtr->SetOrderId(orderUuid);

		productOrderInfoPtr->SetupProductInstance(productId, "", "");

		productOrderInfoPtr->ClearLicenses();
		productOrderInfoPtr->AddLicense(licenseId, QDateTime::fromString(expiration, "yyyy-MM-dd"));

		if (!m_objectCollectionCompPtr->SetObjectData(objectUuid, *productOrderInfoPtr)){
			return nullptr;
		}

		if (oldOrderUuid != orderUuid){
			if (!oldOrderUuid.isEmpty()){
				imtbase::IObjectCollection::DataPtr oldOrderDataPtr;
				if (m_orderCollectionCompPtr->GetObjectData(oldOrderUuid, oldOrderDataPtr)){
					prolifedata::IOrderInfo* productOrderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(oldOrderDataPtr.GetPtr());
					if (productOrderInfoPtr != nullptr){
						imtbase::IObjectCollection* productCollectionPtr = productOrderInfoPtr->GetProducts();
						if (productCollectionPtr != nullptr){
							if (productCollectionPtr->RemoveElement(objectUuid)){
								m_orderCollectionCompPtr->SetObjectData(oldOrderUuid, *productOrderInfoPtr);
							}
						}
					}
				}
			}

			if (!orderUuid.isEmpty()){
				imtbase::IObjectCollection::DataPtr orderDataPtr;
				if (m_orderCollectionCompPtr->GetObjectData(orderUuid, orderDataPtr)){
					prolifedata::IOrderInfo* productOrderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(orderDataPtr.GetPtr());
					if (productOrderInfoPtr != nullptr){
						istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
						objectLinkPtr.SetPtr(new imtbase::CObjectLink());

						objectLinkPtr->SetObjectUuid(objectUuid);
						objectLinkPtr->SetFactoryId("SoftwareInfo");

						imtbase::IObjectCollection* productCollectionPtr = productOrderInfoPtr->GetProducts();
						if (productCollectionPtr != nullptr){
							productCollectionPtr->InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), objectUuid);

							m_orderCollectionCompPtr->SetObjectData(orderUuid, *productOrderInfoPtr);
						}
					}
				}
			}
		}
	}

	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* notificationModelPtr = dataModelPtr->AddTreeModel("updatedNotification");

	notificationModelPtr->SetData("Id", objectUuid);

	QString name = productOrderInfoPtr->GetProductId();
	if (!serialNumber.isEmpty()){
		name += " (" + serialNumber + ")";
	}

	notificationModelPtr->SetData("Name", name);

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CSoftwareProductControllerComp::CreateObject(
			const QList<imtgql::CGqlObject>& inputParams,
			QByteArray& objectId,
			QString& name,
			QString& /*description*/,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_orderCollectionCompPtr.IsValid()){
		return nullptr;
	}

	if (inputParams.isEmpty()){
		return nullptr;
	}

	objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	if (!inputParams.isEmpty()){
		QByteArray itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();

		imtbase::CTreeItemModel itemModel;
		if (!itemModel.CreateFromJson(itemData)){
			return nullptr;
		}

		istd::TDelPtr<prolifedata::COrderedIdentifiableSoftwareInstanceInfo> productOrderInfoPtr;
		productOrderInfoPtr.SetPtr(new prolifedata::COrderedIdentifiableSoftwareInstanceInfo);

		productOrderInfoPtr->SetObjectUuid(objectId);

		QByteArray productId;
		if (itemModel.ContainsKey("ProductId")){
			productId = itemModel.GetData("ProductId").toByteArray();
		}

		if (productId.isEmpty()){
			errorMessage = QT_TR_NOOP("Product cannot be empty!");

			return nullptr;
		}

		QByteArray serialNumber;
		if (itemModel.ContainsKey("SerialNumber")){
			serialNumber = itemModel.GetData("SerialNumber").toByteArray();
		}

		if (!serialNumber.isEmpty()){
			iprm::CTextParam valueParam;
			valueParam.SetText(serialNumber);

			iprm::CEnableableParam isEqualParam;
			isEqualParam.SetEnabled(true);

			iprm::CParamsSet valueParamsSet;
			valueParamsSet.SetEditableParameter("Value", &valueParam);
			valueParamsSet.SetEditableParameter("IsEqual", &isEqualParam);

			iprm::CParamsSet paramsSet;
			paramsSet.SetEditableParameter("SerialNumber", &valueParamsSet);

			iprm::CParamsSet filterParam;
			filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

			imtbase::IObjectCollection::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
			if (!collectionIds.isEmpty()){
				QByteArray id = collectionIds[0];
				if (objectId != id){
					errorMessage = QT_TR_NOOP("Serial Number already exists");

					return nullptr;
				}
			}
		}

		productOrderInfoPtr->SetSerialNumber(serialNumber);

		QByteArray project;
		if (itemModel.ContainsKey("Project")){
			project = itemModel.GetData("Project").toByteArray();

			productOrderInfoPtr->SetProject(project);
		}

		QByteArray orderUuid;
		if (itemModel.ContainsKey("OrderUuid")){
			orderUuid = itemModel.GetData("OrderUuid").toByteArray();

			productOrderInfoPtr->SetOrderId(orderUuid);
		}

		QByteArray customerUuid;
		imtbase::IObjectCollection::DataPtr orderDataPtr;
		if (m_orderCollectionCompPtr->GetObjectData(orderUuid, orderDataPtr)){
			prolifedata::IOrderInfo* productOrderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(orderDataPtr.GetPtr());
			if (productOrderInfoPtr != nullptr){
				customerUuid = productOrderInfoPtr->GetCustomerId();

				istd::TDelPtr<imtbase::CObjectLink> objectLinkPtr;
				objectLinkPtr.SetPtr(new imtbase::CObjectLink());

				objectLinkPtr->SetObjectUuid(objectId);
				objectLinkPtr->SetFactoryId("SoftwareInfo");

				imtbase::IObjectCollection* productCollectionPtr = productOrderInfoPtr->GetProducts();
				if (productCollectionPtr != nullptr){
					productCollectionPtr->InsertNewObject(objectLinkPtr->GetFactoryId(), "", "", objectLinkPtr.GetPtr(), objectId);

					m_orderCollectionCompPtr->SetObjectData(orderUuid, *productOrderInfoPtr);
				}
			}
		}

		if (itemModel.ContainsKey("Project")){
			QByteArray project = itemModel.GetData("Project").toByteArray();

			productOrderInfoPtr->SetProject(project);
		}

		productOrderInfoPtr->SetupProductInstance(productId, "", customerUuid);

		QByteArray licenseId;
		if (itemModel.ContainsKey("LicenseId")){
			licenseId = itemModel.GetData("LicenseId").toByteArray();
		}

		QByteArray expiration;
		if (itemModel.ContainsKey("Expiration")){
			expiration = itemModel.GetData("Expiration").toByteArray();
		}

		productOrderInfoPtr->AddLicense(licenseId, QDateTime::fromString(expiration, "yyyy-MM-dd"));

		name = productId;

		if (!serialNumber.isEmpty()){
			name += " (" + serialNumber + ")";
		}

		return productOrderInfoPtr.PopPtr();
	}

	errorMessage = QObject::tr("Can not create product: %1").arg(QString(objectId));

	return nullptr;
}


} // namespace prolifegql


