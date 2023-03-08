#include <prolifegql/COrderCollectionControllerComp.h>


// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>


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
		prolifedata::CIdentifiableOrderInfo* orderInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr orderDataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(collectionId, orderDataPtr)){
			orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(orderDataPtr.GetPtr());
		}

		if (orderInfoPtr != nullptr){
			for (QByteArray informationId : informationIds){
				QVariant elementInformation;

				if(informationId == "TypeId"){
					elementInformation = m_objectCollectionCompPtr->GetObjectTypeId(collectionId);
				}
				else if(informationId == "Id"){
					QByteArray objectUuid = orderInfoPtr->GetObjectUuid();
					if (objectUuid.isEmpty()){
						elementInformation = orderInfoPtr->GetOrderId();
					}
					else{
						elementInformation = objectUuid;
					}
				}
				else if(informationId == "Name"){
					elementInformation = orderInfoPtr->GetOrderId();
				}
				else if(informationId == "Status"){
					int status = orderInfoPtr->GetOrderStatus();
					switch (status){
					case prolifedata::IOrderInfo::OS_NONE:
						elementInformation = "None";
						break;
					case prolifedata::IOrderInfo::OS_CREATED:
						elementInformation = "Created";
						break;
					case prolifedata::IOrderInfo::OS_IN_PROGRESS:
						elementInformation = "In Progress";
						break;
					case prolifedata::IOrderInfo::OS_CANCELED:
						elementInformation = "Canceled";
						break;
					case prolifedata::IOrderInfo::OS_ON_HOLD:
						elementInformation = "Hold";
						break;
					case prolifedata::IOrderInfo::OS_FINISHED:
						elementInformation = "Finished";
						break;
					case prolifedata::IOrderInfo::OS_CLOSED:
						elementInformation = "Closed";
						break;
					}
				}
				else if(informationId == "OrderId"){
					elementInformation = orderInfoPtr->GetOrderId();
				}
				else if(informationId == "OrderCustomer"){
					elementInformation = orderInfoPtr->GetCustomerId();
				}
				else if(informationId == "Description"){
					elementInformation = orderInfoPtr->GetDescription();
				}
				else if(informationId == "Added"){
					idoc::MetaInfoPtr metaInfoPtr = m_objectCollectionCompPtr->GetElementMetaInfo(collectionId);
					if (metaInfoPtr.IsValid()){
						elementInformation = metaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_INSERTION_TIME)
								.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
					}
				}
				else if(informationId == "LastModified"){
					idoc::MetaInfoPtr metaInfoPtr = m_objectCollectionCompPtr->GetElementMetaInfo(collectionId);
					if (metaInfoPtr.IsValid()){
						elementInformation = metaInfoPtr->GetMetaInfo(imtbase::IObjectCollection::MIT_LAST_OPERATION_TIME)
								.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
					}
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


