#include <prolifegql/COrderCollectionControllerComp.h>


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iprm/CTextParam.h>

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
		prolifedata::CIdentifiableOrderInfo* orderInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr orderDataPtr;
		if (objectCollectionIterator->GetObjectData(orderDataPtr)){
			orderInfoPtr = dynamic_cast<prolifedata::CIdentifiableOrderInfo*>(orderDataPtr.GetPtr());
		}

		if (orderInfoPtr != nullptr){
			for (QByteArray informationId : informationIds){
				QVariant elementInformation;
				QByteArray collectionId = objectCollectionIterator->GetObjectId();

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


imtbase::CTreeItemModel* COrderCollectionControllerComp::ListObjects(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid() || !m_accountCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error.");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();
	const imtgql::CGqlObject* viewParamsGql = nullptr;
	if (inputParams.size() > 0){
		viewParamsGql = inputParams.at(0).GetFieldArgumentObjectPtr("viewParams");
	}

	iprm::CParamsSet filterParams;
	QByteArray customerUuid;

	imtbase::CCollectionFilter m_filter;
	int offset = 0, count = -1;

	if (viewParamsGql != nullptr){
		offset = viewParamsGql->GetFieldArgumentValue("Offset").toInt();
		count = viewParamsGql->GetFieldArgumentValue("Count").toInt();

		QByteArray filterBA = viewParamsGql->GetFieldArgumentValue("FilterModel").toByteArray();
		if (!filterBA.isEmpty()){
			imtbase::CTreeItemModel generalModel;
			generalModel.CreateFromJson(filterBA);

			imtbase::CTreeItemModel* filterModel = generalModel.GetTreeItemModel("FilterIds");
			if (filterModel != nullptr){
				QByteArrayList filteringInfoIds;
				for (int i = 0; i < filterModel->GetItemsCount(); i++){
					QByteArray headerId = filterModel->GetData("Id", i).toByteArray();
					if (!headerId.isEmpty()){
						filteringInfoIds << headerId;
					}
				}
				m_filter.SetFilteringInfoIds(filteringInfoIds);
			}

			QString filterText = generalModel.GetData("TextFilter").toString();
			if (!filterText.isEmpty()){
				m_filter.SetTextFilter(filterText);
			}

			imtbase::CTreeItemModel* sortModel = generalModel.GetTreeItemModel("Sort");
			if (sortModel != nullptr){
				QByteArray headerId = sortModel->GetData("HeaderId").toByteArray();
				QByteArray sortOrder = sortModel->GetData("SortOrder").toByteArray();
				if (!headerId.isEmpty() && !sortOrder.isEmpty()){
					m_filter.SetSortingOrder(sortOrder == "ASC" ? imtbase::ICollectionFilter::SO_ASC : imtbase::ICollectionFilter::SO_DESC);
					m_filter.SetSortingInfoIds(QByteArrayList() << headerId);
				}
			}

			imtbase::CTreeItemModel* objectFilterPtr = generalModel.GetTreeItemModel("ObjectFilter");
			if (objectFilterPtr != nullptr){
				if (objectFilterPtr->ContainsKey("AccountFilter")){
					customerUuid = objectFilterPtr->GetData("AccountFilter").toByteArray();
				}
			}
		}

		filterParams.SetEditableParameter("Filter", &m_filter);

		this->SetAdditionalFilters(*viewParamsGql, &filterParams);
	}

	imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		errorMessage = QString("GraphQL context is invalid.");
		SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

		return nullptr;
	}

	bool filterByGroup = true;
	if (m_checkPermissionCompPtr.IsValid()){
		QByteArrayList userPermissions;

		imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			userPermissions = userInfoPtr->GetPermissions();
		}

		QByteArrayList permissions;
		permissions << *m_permissionIdAttrPtr;
		filterByGroup = !m_checkPermissionCompPtr->CheckPermission(userPermissions, permissions);
	}

	iprm::COptionsManager optionsManager;
	iprm::COptionsManager accountsOptionsManager;

	iprm::CParamsSet accountFilter;
	iprm::CParamsSet objectFilter;

	iprm::CParamsSet groups;
	iprm::CParamsSet orderParams;

	iprm::CTextParam customerParam;
	if (!customerUuid.isEmpty()){
		customerParam.SetText(customerUuid);
		objectFilter.SetEditableParameter("CustomerUuid", &customerParam);
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");
	imtbase::CTreeItemModel* itemsModel = dataModel->AddTreeModel("items");

	if (filterByGroup){
		// User group ID-s from GQL context user
		QByteArrayList userGroupIds;
		imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			userGroupIds = userInfoPtr->GetGroups();
		}

		for (const QByteArray& groupId : userGroupIds){
			optionsManager.InsertOption("", groupId);
		}

		groups.SetEditableParameter("Groups", &optionsManager);
		accountFilter.SetEditableParameter("ObjectFilter", &groups);

		imtbase::ICollectionInfo::Ids accountIds = m_accountCollectionCompPtr->GetElementIds(0, -1, &accountFilter);
		for (const QByteArray& accountId : accountIds){
			accountsOptionsManager.InsertOption("", accountId);
		}

		if (accountsOptionsManager.GetOptionsCount() == 0){
			return rootModelPtr.PopPtr();
		}

		objectFilter.SetEditableParameter("OrderCustomers", &accountsOptionsManager);
	}

	filterParams.SetEditableParameter("ObjectFilter", &objectFilter);

	imtbase::CTreeItemModel* notificationModel = dataModel->AddTreeModel("notification");

	int elementsCount = m_objectCollectionCompPtr->GetElementsCount(&filterParams);

	int pagesCount = std::ceil(elementsCount / (double)count);
	if (pagesCount <= 0){
		pagesCount = 1;
	}

	notificationModel->SetData("PagesCount", pagesCount);
	notificationModel->SetData("TotalCount", elementsCount);

	if (offset >= elementsCount){
		offset -= count;
	}

	imtdb::CSqlDatabaseObjectCollectionComp* objectCollectionCompPtr = dynamic_cast<imtdb::CSqlDatabaseObjectCollectionComp*>(m_objectCollectionCompPtr.GetPtr());
	istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(objectCollectionCompPtr->CreateObjectCollectionIterator(offset, count, &filterParams));

	if (objectCollectionIterator != nullptr){
		while (objectCollectionIterator->Next()){
			imtbase::IObjectCollection::DataPtr objectDataPtr;
			if (objectCollectionIterator->GetObjectData(objectDataPtr)){
				int itemIndex = itemsModel->InsertNewItem();
				if (itemIndex >= 0){
					if (!SetupGqlItem(gqlRequest, *itemsModel, itemIndex, objectCollectionIterator.GetPtr(), errorMessage)){
						errorMessage = QString("Error when trying setup GQL item.");
						SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

						return nullptr;
					}
				}
			}
			else{
				errorMessage = QString("Unable to get an object from object iterator.");
				SendErrorMessage(0, errorMessage, "COrderCollectionControllerComp");

				return nullptr;
			}
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace prolifegql


