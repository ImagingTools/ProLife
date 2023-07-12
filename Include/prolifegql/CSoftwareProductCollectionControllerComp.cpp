#include <prolifegql/CSoftwareProductCollectionControllerComp.h>


// ProLife includes
#include <prolifedata/COrderInfo.h>


namespace prolifegql
{


bool CSoftwareProductCollectionControllerComp::SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& /*errorMessage*/) const
{
	if (objectCollectionIterator == nullptr){
		return false;
	}

	bool retVal = true;

	QByteArray collectionId = objectCollectionIterator->GetObjectId();
	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		prolifedata::CIdentifiableOrderInfo* orderInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr orderDataPtr;
		if (objectCollectionIterator->GetObjectData(orderDataPtr)){
			orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(orderDataPtr.GetPtr());
		}

		if (orderInfoPtr != nullptr){
			QByteArray serialNumber;
			QByteArray productId;

			imtbase::IObjectCollection* productCollectionPtr = orderInfoPtr->GetProducts();
			if (productCollectionPtr != nullptr){
				imtbase::IObjectCollection::DataPtr productDataPtr;
				if (productCollectionPtr->GetObjectData(collectionId, productDataPtr)){
					imtlic::IProductInstanceInfo* softwareProductPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
					if (softwareProductPtr != nullptr){
						serialNumber = softwareProductPtr->GetSerialNumber();
						productId = softwareProductPtr->GetProductId();
					}
				}
			}

			for (const QByteArray& informationId : informationIds){
				QVariant elementInformation;
				if (informationId == "Id"){
					elementInformation = collectionId;
				}
				else if (informationId == "OrderId"){
					elementInformation = orderInfoPtr->GetOrderId();
				}
				else if (informationId == "ProductId"){
					elementInformation = productId;
				}
				else if (informationId == "SerialNumber"){
					elementInformation = serialNumber;
				}

				if (elementInformation.isNull()){
					elementInformation = "";
				}

				retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
			}
		}
	}

	return retVal;
}


} // namespace prolifegql


