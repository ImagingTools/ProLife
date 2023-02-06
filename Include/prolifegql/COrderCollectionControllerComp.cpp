#include <prolifegql/COrderCollectionControllerComp.h>


// ProLife includes
#include <prolifedata/IOrderInfo.h>


namespace prolifegql
{


bool COrderCollectionControllerComp::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		imtbase::CTreeItemModel& model,
		int itemIndex,
		const QByteArray& collectionId,
		QString& errorMessage) const
{
	bool retVal = true;

	gqlRequest.GetFields();
	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		prolifedata::IOrderInfo* orderInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr orderDataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, orderDataPtr)){
			orderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(orderDataPtr.GetPtr());
		}

		if (orderInfoPtr != nullptr){
			for (QByteArray informationId : informationIds){
				QVariant elementInformation;

				if(informationId == "TypeId"){
					elementInformation = m_objectCollectionCompPtr->GetObjectTypeId(collectionId);
				}
				else if(informationId == "Id" || informationId == "Name"){
					elementInformation = QString(collectionId);
				}
				else if(informationId == "OrderId"){
					elementInformation = orderInfoPtr->GetOrderId();
				}
				else if(informationId == "CustomerId"){
					elementInformation = orderInfoPtr->GetCustomerId();
				}
                else if(informationId == "Description"){
                    elementInformation = orderInfoPtr->GetDescription();
                }
				else{
				}

				if (elementInformation.isNull()){
					elementInformation = "";
				}

				retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
			}
		}

		return true;
	}

	return false;
}


} // namespace prolifegql


