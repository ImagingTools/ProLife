#include <prolifegql/COrderCollectionControllerComp.h>


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iprm/CTextParam.h>
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtbase/CCollectionFilter.h>
#include <imtauth/ICompanyBaseInfo.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtdb/CSqlDatabaseObjectCollectionComp.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>


namespace prolifegql
{


bool COrderCollectionControllerComp::SetupGqlItem(
			const imtgql::CGqlRequest& gqlRequest,
			imtbase::CTreeItemModel& model,
			int itemIndex,
			const imtbase::IObjectCollectionIterator* objectCollectionIterator,
			QString& errorMessage) const
{
	if (objectCollectionIterator == nullptr){
		errorMessage = QString("Object collection iterator is invalid.");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return false;
	}

	bool retVal = true;

	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		prolifedata::IOrderInfo* orderInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr orderDataPtr;
		if (objectCollectionIterator->GetObjectData(orderDataPtr)){
			orderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(orderDataPtr.GetPtr());
		}

		if (orderInfoPtr != nullptr){
			for (QByteArray informationId : informationIds){
				QVariant elementInformation;
				QByteArray collectionId = objectCollectionIterator->GetObjectId();

				if(informationId == "TypeId"){
					elementInformation = m_objectCollectionCompPtr->GetObjectTypeId(collectionId);
				}
				else if(informationId == "Id"){
					elementInformation = objectCollectionIterator->GetObjectId();
				}
				else if(informationId == "Name"){
					elementInformation = orderInfoPtr->GetOrderId();
				}
				else if(informationId == "Status"){
					int status = orderInfoPtr->GetOrderStatus();
					switch (status){
					case prolifedata::IOrderInfo::OrderStatus::OS_NONE:
					case prolifedata::IOrderInfo::OrderStatus::OS_ON_HOLD:
					case prolifedata::IOrderInfo::OrderStatus::OS_FINISHED:
					case prolifedata::IOrderInfo::OrderStatus::OS_CREATED:
						elementInformation = "Created";
						break;
					case prolifedata::IOrderInfo::OS_IN_PROGRESS:
						elementInformation = "In Progress";
						break;
					case prolifedata::IOrderInfo::OS_CANCELED:
						elementInformation = "Canceled";
						break;
					case prolifedata::IOrderInfo::OS_CLOSED:
						elementInformation = "Closed";
						break;
					}
				}
				else if(informationId == "OrderId"){
					elementInformation = orderInfoPtr->GetOrderId();
				}
				else if(informationId == "PurchaseId"){
					elementInformation = orderInfoPtr->GetPurchaseOrderId();
				}
				else if(informationId == "OrderCustomer"){
					elementInformation = objectCollectionIterator->GetElementInfo("OrderCustomer");
				}
				else if(informationId == "Description"){
					elementInformation = orderInfoPtr->GetDescription();
				}
				else if(informationId == "Added"){
					QDateTime addedTime =  objectCollectionIterator->GetElementInfo("added").toDateTime();
					addedTime.setTimeSpec(Qt::UTC);

					elementInformation = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
				}
				else if(informationId == "LastModified"){
					QDateTime lastTime =  objectCollectionIterator->GetElementInfo("lastmodified").toDateTime();
					lastTime.setTimeSpec(Qt::UTC);

					elementInformation = lastTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
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


void COrderCollectionControllerComp::SetObjectFilter(
			const imtgql::CGqlRequest& gqlRequest,
			const imtbase::CTreeItemModel& objectFilterModel,
			iprm::CParamsSet& filterParams) const
{
	BaseClass::SetObjectFilter(gqlRequest, objectFilterModel, filterParams);

	imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return;
	}

	imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		return;
	}

	QByteArray userId = userInfoPtr->GetId();
	QByteArrayList groupIds = userInfoPtr->GetGroups();
	QByteArrayList userPermissions = userInfoPtr->GetPermissions();

	bool isAdmin = userInfoPtr->IsAdmin();
	bool filterByGroup = true;

	if (m_checkPermissionCompPtr.IsValid()){
		QByteArrayList permissions;
		permissions << *m_permissionIdAttrPtr;

		filterByGroup = !m_checkPermissionCompPtr->CheckPermission(userPermissions, permissions);
	}

	if (isAdmin){
		filterByGroup = false;
	}

	if (filterByGroup){
		iprm::CTextParam* userParamPtr = new iprm::CTextParam();
		userParamPtr->SetText(userId);

		iprm::CTextParam* groupParamPtr = new iprm::CTextParam();
		QByteArray groups;
		if (!groupIds.isEmpty()){
			groups = groupIds.join(';');
		}
		groupParamPtr->SetText(groups);

		iprm::CParamsSet* paramsSetPtr = new iprm::CParamsSet();

		paramsSetPtr->SetEditableParameter("UserParam", userParamPtr, true);
		paramsSetPtr->SetEditableParameter("GroupParam", groupParamPtr, true);

		filterParams.SetEditableParameter("Groups", paramsSetPtr, true);
	}
}


} // namespace prolifegql


