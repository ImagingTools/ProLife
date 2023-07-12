#include <prolifegql/CSoftwareProductControllerComp.h>


// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>


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
		prolifedata::IOrderInfo* orderPtr = dynamic_cast<prolifedata::IOrderInfo*>(dataPtr.GetPtr());
		if (orderPtr != nullptr){
			imtbase::IObjectCollection* orderProductsPtr = orderPtr->GetProducts();
			if (orderProductsPtr != nullptr){
				imtbase::IObjectCollection::DataPtr productDataPtr;
				if (orderProductsPtr->GetObjectData(objectId, productDataPtr)){
					const imtlic::IProductInstanceInfo* softwareProductPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
					if (softwareProductPtr != nullptr){
						QByteArray serialNumber = softwareProductPtr->GetSerialNumber();

						dataModelPtr->SetData("Id", objectId);
						dataModelPtr->SetData("ProductId", softwareProductPtr->GetProductId());
						dataModelPtr->SetData("CategoryId", softwareProductPtr->GetFactoryId());
						dataModelPtr->SetData("SerialNumber", serialNumber);

						QString name = softwareProductPtr->GetProductId();

						if (!serialNumber.isEmpty()){
							name += " (" + serialNumber + ")";
						}

						dataModelPtr->SetData("Name", name);

						return rootModelPtr.PopPtr();
					}
				}
			}
		}
	}

	return nullptr;
}


imtbase::CTreeItemModel* CSoftwareProductControllerComp::UpdateObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid() || !m_orderCollectionCompPtr.IsValid()){
		return nullptr;
	}

	QByteArray productId;
	QByteArray itemData;
	const imtgql::CGqlObject* inputObjectPtr = gqlRequest.GetParam("input");
	if (inputObjectPtr != nullptr){
		productId = inputObjectPtr->GetFieldArgumentValue("Id").toByteArray();
		itemData = inputObjectPtr->GetFieldArgumentValue("Item").toByteArray();
	}

	if (productId.isEmpty()){
		errorMessage = QObject::tr("Unable to update an object %1").arg(qPrintable(productId)).toUtf8();

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

//	QByteArray orderUuid;
//	if (itemModel.ContainsKey("OrderUuid")){
//		orderUuid = itemModel.GetData("OrderUuid").toByteArray();
//	}

	imtbase::IIdentifiable* productOrderPtr = nullptr;

	imtbase::IObjectCollection::DataPtr orderProductDataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(productId, orderProductDataPtr)){
		productOrderPtr = dynamic_cast<imtbase::IIdentifiable*>(orderProductDataPtr.GetPtr());
	}

	if (productOrderPtr == nullptr){
		return nullptr;
	}

	QByteArray orderUuid = productOrderPtr->GetObjectUuid();

	QString name;

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* notificationModelPtr = dataModelPtr->AddTreeModel("updatedNotification");

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_orderCollectionCompPtr->GetObjectData(orderUuid, dataPtr)){
		prolifedata::IOrderInfo* orderPtr = dynamic_cast<prolifedata::IOrderInfo*>(dataPtr.GetPtr());
		if (orderPtr != nullptr){
			imtbase::IObjectCollection* orderProductsPtr = orderPtr->GetProducts();
			if (orderProductsPtr != nullptr){
				imtbase::IObjectCollection::DataPtr productDataPtr;
				if (orderProductsPtr->GetObjectData(productId, productDataPtr)){
					imtlic::IProductInstanceInfo* softwareProductPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
					if (softwareProductPtr != nullptr){
						softwareProductPtr->SetSerialNumber(serialNumber);

						if (!orderProductsPtr->SetObjectData(productId, *softwareProductPtr)){
							return nullptr;
						}

						if (!m_orderCollectionCompPtr->SetObjectData(orderUuid, *orderPtr)){
							return nullptr;
						}

						name = softwareProductPtr->GetProductId();
					}
				}
			}
		}
	}

	notificationModelPtr->SetData("Id", productId);

	if (!serialNumber.isEmpty()){
		name += " (" + serialNumber + ")";
	}

	notificationModelPtr->SetData("Name", name);

	return rootModelPtr.PopPtr();
}


} // namespace prolifegql


