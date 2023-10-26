#include <prolifegql/CSoftwareProductControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CTextParam.h>
#include <iprm/CEnableableParam.h>

// ImtCore includes
#include <imtbase/CObjectLink.h>
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/IHardwareInstanceInfo.h>
#include <imtlic/IProductInfo.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>
#include <prolifegql/COrderControllerComp.h>


namespace prolifegql
{


imtbase::CTreeItemModel* CSoftwareProductControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage, "CSoftwareProductControllerComp");

		return nullptr;
	}

	QByteArray objectId;
	const imtgql::CGqlObject* inputObjectPtr = gqlRequest.GetParam("input");
	if (inputObjectPtr != nullptr){
		objectId = inputObjectPtr->GetFieldArgumentValue("Id").toByteArray();
	}

	if (objectId.isEmpty()){
		errorMessage = QObject::tr("Unable to get an object").toUtf8();
		SendErrorMessage(0, errorMessage, "CSoftwareProductControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productOrderInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
		if (productOrderInfoPtr != nullptr){
			QByteArray serialNumber = productOrderInfoPtr->GetSerialNumber();
			QByteArray productUuid = productOrderInfoPtr->GetProductId();

			dataModelPtr->SetData("Id", objectId);
			dataModelPtr->SetData("ProductId", productUuid);
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

			QString name = productUuid;

			if (m_productCollectionCompPtr.IsValid()){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_productCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
					imtlic::IProductInfo* remoteProductInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
					if (remoteProductInfoPtr != nullptr){
						name = remoteProductInfoPtr->GetName();
					}
				}
			}

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
		SendErrorMessage(0, errorMessage, "CSoftwareProductControllerComp");

		return nullptr;
	}

	prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productOrderInfoPtr = nullptr;

	imtbase::IObjectCollection::DataPtr orderProductDataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectUuid, orderProductDataPtr)){
		productOrderInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(orderProductDataPtr.GetPtr());
	}

	if (productOrderInfoPtr == nullptr){
		errorMessage = QObject::tr("Unable to get an software product with ID: %1").arg(qPrintable(objectUuid)).toUtf8();
		SendErrorMessage(0, errorMessage, "CSoftwareProductControllerComp");

		return nullptr;
	}

	if (productOrderInfoPtr->IsInUse()){
		errorMessage = QString("It is not possible to update an product in use");
		SendErrorMessage(0, errorMessage, "CSoftwareProductControllerComp");

		return nullptr;
	}

	imtbase::CTreeItemModel itemModel;
	if (!itemModel.CreateFromJson(itemData)){
		errorMessage = QObject::tr("Unable to create an item model from json: %1").arg(qPrintable(itemData)).toUtf8();
		SendErrorMessage(0, errorMessage, "CSoftwareProductControllerComp");

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

		iprm::CParamsSet paramsSet;
		paramsSet.SetEditableParameter("SerialNumber", &valueParam);

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

		imtbase::IOperationContext* operationContextPtr = nullptr;

		if (m_operationContextControllerCompPtr.IsValid()){
			operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, objectUuid, productOrderInfoPtr);
		}

		if (!m_objectCollectionCompPtr->SetObjectData(objectUuid, *productOrderInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
			errorMessage = QString("Unable to set object by ID: %1.").arg(qPrintable(objectUuid));
			SendErrorMessage(0, errorMessage, "CSoftwareProductControllerComp");

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
								imtbase::IOperationContext* operationContextPtr = nullptr;

								if (m_orderOperationContextControllerCompPtr.IsValid()){
									operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, oldOrderUuid, productOrderInfoPtr);
								}

								m_orderCollectionCompPtr->SetObjectData(oldOrderUuid, *productOrderInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr);
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

							imtbase::IOperationContext* operationContextPtr = nullptr;

							if (m_orderOperationContextControllerCompPtr.IsValid()){
								operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, orderUuid, productOrderInfoPtr);
							}

							m_orderCollectionCompPtr->SetObjectData(orderUuid, *productOrderInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr);
						}
					}
				}
			}
		}
	}

	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* notificationModelPtr = dataModelPtr->AddTreeModel("updatedNotification");

	notificationModelPtr->SetData("Id", objectUuid);

	QString name = productId;

	if (m_productCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_productCollectionCompPtr->GetObjectData(productId, dataPtr)){
			imtlic::IProductInfo* remoteProductInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
			if (remoteProductInfoPtr != nullptr){
				name = remoteProductInfoPtr->GetName();
			}
		}
	}

	if (!serialNumber.isEmpty()){
		name += " (" + serialNumber + ")";
	}

	notificationModelPtr->SetData("Name", name);

	return rootModelPtr.PopPtr();
}


istd::IChangeable* CSoftwareProductControllerComp::CreateObject(
			const imtgql::CGqlRequest& gqlRequest,
			QByteArray& objectId,
			QString& name,
			QString& /*description*/,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error.");
		SendErrorMessage(0, errorMessage, "CSoftwareProductControllerComp");

		return nullptr;
	}

	if (!m_orderCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error.");
		SendErrorMessage(0, errorMessage, "CSoftwareProductControllerComp");

		return nullptr;
	}

	const imtgql::CGqlObject* gqlParamsPtr = gqlRequest.GetParam("input");
	if (gqlParamsPtr == nullptr){
		errorMessage = QString("GQL input params is invalid.").toUtf8();
		SendErrorMessage(0, errorMessage, "CSoftwareProductControllerComp");

		return nullptr;
	}

	objectId = gqlParamsPtr->GetFieldArgumentValue("Id").toByteArray();
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}

	QByteArray itemData = gqlParamsPtr->GetFieldArgumentValue("Item").toByteArray();

	imtbase::CTreeItemModel itemModel;
	if (!itemModel.CreateFromJson(itemData)){
		errorMessage = QString("Unable to create representation model from JSON: %1.").arg(qPrintable(itemData)).toUtf8();
		SendErrorMessage(0, errorMessage, "CSoftwareProductControllerComp");

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
		SendErrorMessage(0, errorMessage, "CSoftwareProductControllerComp");

		return nullptr;
	}

	QByteArray serialNumber;
	if (itemModel.ContainsKey("SerialNumber")){
		serialNumber = itemModel.GetData("SerialNumber").toByteArray();
	}

	if (!serialNumber.isEmpty()){
		iprm::CTextParam valueParam;
		valueParam.SetText(serialNumber);

		iprm::CParamsSet paramsSet;
		paramsSet.SetEditableParameter("SerialNumber", &valueParam);

		iprm::CParamsSet filterParam;
		filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

		imtbase::IObjectCollection::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
		if (!collectionIds.isEmpty()){
			QByteArray id = collectionIds[0];
			if (objectId != id){
				errorMessage = QT_TR_NOOP("Serial Number already exists");
				SendErrorMessage(0, errorMessage, "CSoftwareProductControllerComp");

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

				imtbase::IOperationContext* operationContextPtr = nullptr;

				if (m_orderOperationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_orderOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_UPDATE, gqlRequest, orderUuid, productOrderInfoPtr);
				}

				m_orderCollectionCompPtr->SetObjectData(orderUuid, *productOrderInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr);
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

	if (m_productCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_productCollectionCompPtr->GetObjectData(productId, dataPtr)){
			imtlic::IProductInfo* remoteProductInfoPtr = dynamic_cast<imtlic::IProductInfo*>(dataPtr.GetPtr());
			if (remoteProductInfoPtr != nullptr){
				name = remoteProductInfoPtr->GetName();
			}
		}
	}

	if (!serialNumber.isEmpty()){
		name += " (" + serialNumber + ")";
	}

	return productOrderInfoPtr.PopPtr();
}


} // namespace prolifegql


