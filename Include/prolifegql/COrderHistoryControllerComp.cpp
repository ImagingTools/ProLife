#include <prolifegql/COrderHistoryControllerComp.h>


// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>
#include <iprm/CEnableableParam.h>

// ImtCore includes
#include <imtbase/IObjectCollectionIterator.h>
#include <imtbase/IIdentifiable.h>
#include <imtlic/IProductInstanceInfo.h>
#include <imtlic/CHardwareInstanceInfo.h>

// ProLife includes
#include <prolifedata/IOrderInfo.h>


namespace prolifegql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* COrderHistoryControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_orderCollectionCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	QByteArray objectId;
	const imtgql::CGqlObject* gqlObjectPtr = gqlRequest.GetParam("input");
	if (gqlObjectPtr != nullptr){
		objectId = gqlObjectPtr->GetFieldArgumentValue("Id").toByteArray();
	}

	int offset = 0;
	int count = -1;

	iprm::CParamsSet filterParams;

	iprm::CIdParam idParam;
	idParam.SetId(objectId);

	filterParams.SetEditableParameter("Id", &idParam);

	iprm::CEnableableParam enableableParam;
	enableableParam.SetEnabled(true);

	filterParams.SetEditableParameter("IsHistory", &enableableParam);

	istd::TDelPtr<prolifedata::IOrderInfo> prevOrderInfoPtr;

	istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(m_orderCollectionCompPtr->CreateObjectCollectionIterator(offset, count, &filterParams));
	if (objectCollectionIterator != nullptr){
		while (objectCollectionIterator->Next()){
			imtbase::IObjectCollection::DataPtr objectDataPtr;
			if (objectCollectionIterator->GetObjectData(objectDataPtr)){
				prolifedata::IOrderInfo* currentOrderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(objectDataPtr.GetPtr());
				if (currentOrderInfoPtr != nullptr){
					if (prevOrderInfoPtr.IsValid()){
						QStringList changesList;

						QByteArray prevOrderId = prevOrderInfoPtr->GetOrderId();
						QByteArray currentOrderId = currentOrderInfoPtr->GetOrderId();
						if (prevOrderId != currentOrderId){
							changesList << QString("Order-ID changed from %1 to %2").arg(qPrintable(prevOrderId)).arg(qPrintable(currentOrderId));
						}

						QByteArray prevPurchaseOrderId = prevOrderInfoPtr->GetPurchaseOrderId();
						QByteArray currentPurchaseOrderId = currentOrderInfoPtr->GetPurchaseOrderId();
						if (prevPurchaseOrderId != currentPurchaseOrderId){
							changesList << QString("Purchase Order-ID changed from %1 to %2").arg(qPrintable(prevPurchaseOrderId)).arg(qPrintable(currentPurchaseOrderId));
						}

						QByteArray prevCustomerId = prevOrderInfoPtr->GetCustomerId();
						QByteArray currentCustomerId = currentOrderInfoPtr->GetCustomerId();
						if (prevCustomerId != currentCustomerId){
							changesList << QString("Customer changed from %1 to %2").arg(qPrintable(prevCustomerId)).arg(qPrintable(currentCustomerId));
						}

						prolifedata::IOrderInfo::OrderStatus prevStatus = prevOrderInfoPtr->GetOrderStatus();
						prolifedata::IOrderInfo::OrderStatus currentStatus = currentOrderInfoPtr->GetOrderStatus();
						if (prevStatus != currentStatus){
							QStringList statuses = prevOrderInfoPtr->OrderStatusGetStrings();
							changesList << QString("Order Status changed from %1 to %2").arg(statuses[prevStatus]).arg(statuses[currentStatus]);
						}

						QString prevDescription = prevOrderInfoPtr->GetDescription();
						QString currentDescription = currentOrderInfoPtr->GetDescription();
						if (prevDescription != currentDescription){
							changesList << QString("Description changed from %1 to %2").arg(prevDescription).arg(currentDescription);
						}

						QByteArrayList addedProducts;
						QByteArrayList removedProducts;
						QByteArrayList updatedProducts;

						imtbase::IObjectCollection* prevProductCollectionPtr = prevOrderInfoPtr->GetProducts();
						imtbase::IObjectCollection* currentProductCollectionPtr = currentOrderInfoPtr->GetProducts();

						if (prevProductCollectionPtr != nullptr && currentProductCollectionPtr != nullptr){
							GenerateDifferences(*prevProductCollectionPtr, *currentProductCollectionPtr, addedProducts, removedProducts, updatedProducts);
						}

						for (const QByteArray& productObjectId : addedProducts){
							const imtbase::IIdentifiable* productIdentifiablePtr = nullptr;
							imtbase::IObjectCollection::DataPtr dataPtr;
							if (currentProductCollectionPtr->GetObjectData(productObjectId, dataPtr)){
								productIdentifiablePtr = dynamic_cast<const imtbase::IIdentifiable*>(dataPtr.GetPtr());
							}

							if (productIdentifiablePtr != nullptr){
								QByteArray productId = GetProductId(*productIdentifiablePtr);

								changesList << QString("Added product %1").arg(qPrintable(productId));
							}
						}

						for (const QByteArray& productObjectId : removedProducts){
							const imtbase::IIdentifiable* productIdentifiablePtr = nullptr;
							imtbase::IObjectCollection::DataPtr dataPtr;
							if (prevProductCollectionPtr->GetObjectData(productObjectId, dataPtr)){
								productIdentifiablePtr = dynamic_cast<const imtbase::IIdentifiable*>(dataPtr.GetPtr());
							}

							if (productIdentifiablePtr != nullptr){
								QByteArray productId = GetProductId(*productIdentifiablePtr);

								changesList << QString("Removed product %1").arg(qPrintable(productId));
							}
						}

						for (const QByteArray& productObjectId : updatedProducts){
							const imtbase::IIdentifiable* productIdentifiablePtr = nullptr;
							imtbase::IObjectCollection::DataPtr dataPtr;
							if (currentProductCollectionPtr->GetObjectData(productObjectId, dataPtr)){
								productIdentifiablePtr = dynamic_cast<const imtbase::IIdentifiable*>(dataPtr.GetPtr());
							}

							if (productIdentifiablePtr != nullptr){
								QByteArray productId = GetProductId(*productIdentifiablePtr);

								changesList << QString("Updated product %1").arg(qPrintable(productId));
							}
						}

						if (!changesList.isEmpty()){
							int i = dataModelPtr->InsertNewItem(0);
							QByteArray userName = objectCollectionIterator->GetElementInfo("OwnerName").toByteArray();
							QDateTime dateTime = objectCollectionIterator->GetElementInfo("LastModified").toDateTime();

							dataModelPtr->SetData("User", userName, i);
							dataModelPtr->SetData("DateTime", dateTime.toString("dd.MM.yyyy hh:mm:ss"), i);

							imtbase::CTreeItemModel* changesModelPtr = dataModelPtr->AddTreeModel("ChangesModel", i);
							for (const QString& change : changesList){
								int index = changesModelPtr->InsertNewItem();

								changesModelPtr->SetData("Change", change, index);
							}
						}
					}

					prevOrderInfoPtr.SetCastedOrRemove(currentOrderInfoPtr->CloneMe());
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


void COrderHistoryControllerComp::GenerateDifferences(
			imtbase::IObjectCollection& prevOrderProducts,
			imtbase::IObjectCollection& currentOrderProducts,
			QByteArrayList& addProducts,
			QByteArrayList& removedProducts,
			QByteArrayList& updatedProducts) const
{
	imtbase::ICollectionInfo::Ids prevProductIds = prevOrderProducts.GetElementIds();
	imtbase::ICollectionInfo::Ids currentProductIds = currentOrderProducts.GetElementIds();

	for (const imtbase::ICollectionInfo::Id& productId : currentProductIds){
		if (!prevProductIds.contains(productId)){
			addProducts << productId;
		}
	}

	for (const imtbase::ICollectionInfo::Id& productId : prevProductIds){
		if (!currentProductIds.contains(productId)){
			removedProducts << productId;
		}
	}

	for (const imtbase::ICollectionInfo::Id& productId : prevProductIds){
		if (currentProductIds.contains(productId)){
			bool isUpdated = false;
			const imtbase::IIdentifiable* prevProductIdentifiablePtr = nullptr;
			imtbase::IObjectCollection::DataPtr prevDataPtr;
			if (prevOrderProducts.GetObjectData(productId, prevDataPtr)){
				prevProductIdentifiablePtr = dynamic_cast<const imtbase::IIdentifiable*>(prevDataPtr.GetPtr());
			}

			const imtbase::IIdentifiable* currentProductIdentifiablePtr = nullptr;
			imtbase::IObjectCollection::DataPtr currentDataPtr;
			if (currentOrderProducts.GetObjectData(productId, currentDataPtr)){
				currentProductIdentifiablePtr = dynamic_cast<const imtbase::IIdentifiable*>(currentDataPtr.GetPtr());
			}

			if (prevProductIdentifiablePtr != nullptr && currentProductIdentifiablePtr != nullptr){
				const imtlic::IProductInstanceInfo* prevSoftwareProductPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(prevProductIdentifiablePtr);
				if (prevSoftwareProductPtr != nullptr){
					const imtlic::IProductInstanceInfo* currentSoftwareProductPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(currentProductIdentifiablePtr);
					if (currentSoftwareProductPtr != nullptr){
						QByteArray prevProductId = prevSoftwareProductPtr->GetProductId();
						QByteArray currentProductId = currentSoftwareProductPtr->GetProductId();
						if (prevProductId != currentProductId){
							isUpdated = true;
						}

						QByteArray prevProductInstanceId = prevSoftwareProductPtr->GetProductInstanceId();
						QByteArray currentProductInstanceId = currentSoftwareProductPtr->GetProductInstanceId();
						if (prevProductInstanceId != currentProductInstanceId){
							isUpdated = true;
						}

						QByteArray prevCustomerId = prevSoftwareProductPtr->GetCustomerId();
						QByteArray currentCustomerId = currentSoftwareProductPtr->GetCustomerId();
						if (prevCustomerId != currentCustomerId){
							isUpdated = true;
						}

						QByteArray prevSerialNumber = prevSoftwareProductPtr->GetSerialNumber();
						QByteArray currentSerialNumber = currentSoftwareProductPtr->GetSerialNumber();
						if (prevSerialNumber != currentSerialNumber){
							isUpdated = true;
						}
					}
				}

				const imtlic::CIdentifiableHardwareInstanceInfo* prevHardwareProductPtr = dynamic_cast<const imtlic::CIdentifiableHardwareInstanceInfo*>(prevProductIdentifiablePtr);
				if (prevHardwareProductPtr != nullptr){
					const imtlic::CIdentifiableHardwareInstanceInfo* currentHardwareProductPtr = dynamic_cast<const imtlic::CIdentifiableHardwareInstanceInfo*>(currentProductIdentifiablePtr);
					if (currentHardwareProductPtr != nullptr){
						QByteArray prevProductId = prevHardwareProductPtr->GetProductId();
						QByteArray currentProductId = currentHardwareProductPtr->GetProductId();
						if (prevProductId != currentProductId){
							isUpdated = true;
						}

						QByteArray prevDeviceId = prevHardwareProductPtr->GetObjectUuid();
						QByteArray currentDeviceId = currentHardwareProductPtr->GetObjectUuid();
						if (prevDeviceId != currentDeviceId){
							isUpdated = true;
						}

						QByteArray prevModelTypeId = prevHardwareProductPtr->GetModelTypeId();
						QByteArray currentModelTypeId = currentHardwareProductPtr->GetModelTypeId();
						if (prevModelTypeId != currentModelTypeId){
							isUpdated = true;
						}

						QByteArray prevSoftwareId = prevHardwareProductPtr->GetSoftwareId();
						QByteArray currentSoftwareId = currentHardwareProductPtr->GetSoftwareId();
						if (prevSoftwareId != currentSoftwareId){
							isUpdated = true;
						}
					}
				}
			}

			if (isUpdated){
				updatedProducts << productId;
			}
		}
	}
}

QByteArray COrderHistoryControllerComp::GetProductId(const imtbase::IIdentifiable& identifiablePtr) const
{
	QByteArray productId;

	const imtlic::IProductInstanceInfo* softwareProductPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(&identifiablePtr);
	if (softwareProductPtr != nullptr){
		productId = softwareProductPtr->GetProductId();
	}

	const imtlic::IHardwareInstanceInfo* hardwareProductPtr = dynamic_cast<const imtlic::IHardwareInstanceInfo*>(&identifiablePtr);
	if (hardwareProductPtr != nullptr){
		productId = hardwareProductPtr->GetProductId();
	}

	return productId;
}


} // namespace prolifegql


