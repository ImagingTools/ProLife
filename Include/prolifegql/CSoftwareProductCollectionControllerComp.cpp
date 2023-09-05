#include <prolifegql/CSoftwareProductCollectionControllerComp.h>


// Acf includes
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtlic/CHardwareInstanceInfo.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>


namespace prolifegql
{


imtbase::CTreeItemModel* CSoftwareProductCollectionControllerComp::ListObjects(const imtgql::CGqlRequest &gqlRequest, QString &errorMessage) const
{
	imtbase::CTreeItemModel* retVal = BaseClass::ListObjects(gqlRequest, errorMessage);

	if (retVal == nullptr){
		return nullptr;
	}

	imtbase::CTreeItemModel* dataModel = retVal->GetTreeItemModel("data");
	if (dataModel == nullptr){
		return nullptr;
	}

	imtbase::CTreeItemModel* itemsModel = dataModel->GetTreeItemModel("items");
	if (itemsModel == nullptr){
		return nullptr;
	}

	if (m_gqlLicenseRequestCompPtr.IsValid()){
		imtgql::CGqlRequest gqlLisaRequest(imtgql::CGqlRequest::RT_QUERY, "LicensesItems");
		imtgql::CGqlObject queryFields("items");
		queryFields.InsertField("Id");
		queryFields.InsertField("Name");
		gqlLisaRequest.AddField(queryFields);

		imtgql::CGqlObject licenseParams("licenses");
		for (int index = 0; index < itemsModel->GetItemsCount(); index++){
			QByteArray licenseId = itemsModel->GetData("LicenseId", index).toByteArray();
			licenseParams.InsertField(licenseId);
		}
		gqlLisaRequest.AddParam(licenseParams);

		QString errorMessage;
		imtbase::CTreeItemModel* licensesModelPtr = m_gqlLicenseRequestCompPtr->CreateResponse(gqlLisaRequest, errorMessage);
		if (licensesModelPtr == nullptr){
			return nullptr;
		}

		if (licensesModelPtr->ContainsKey("data")){
			imtbase::CTreeItemModel* dataModelPtr = licensesModelPtr->GetTreeItemModel("data");
			if (dataModelPtr != nullptr){
				for (int lisaIndex = 0; lisaIndex < dataModelPtr->GetItemsCount(); lisaIndex++){
					QByteArray licenseName = dataModelPtr->GetData("Name", lisaIndex).toByteArray();
					QByteArray id = dataModelPtr->GetData("Id", lisaIndex).toByteArray();
					for (int index = 0; index < itemsModel->GetItemsCount(); index++){
						QByteArray licenseId = itemsModel->GetData("LicenseId", index).toByteArray();
						if (id == licenseId){
							itemsModel->SetData("LicenseName", licenseName, index);
						}
					}
				}
			}
		}
	}

	return retVal;
}


imtbase::CTreeItemModel* CSoftwareProductCollectionControllerComp::DeleteObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = "No collection component was set";

		return nullptr;
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();

	QByteArray objectId = GetObjectIdFromInputParams(inputParams);
	if (objectId.isEmpty()){
		errorMessage = QObject::tr("No object-ID could not be extracted from the request");

		return nullptr;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
		if (productInstanceInfoPtr != nullptr){
			bool isUse = productInstanceInfoPtr->IsInUse();
			if (isUse){
				errorMessage = QString("It is not possible to remove a product that is in use");

				return nullptr;
			}
		}
	}

	imtbase::IOperationContext* operationContextPtr = CreateOperationContext(gqlRequest, QString("Removed the object"));
	if (m_objectCollectionCompPtr->RemoveElement(objectId, operationContextPtr)){
		istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

		imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
		imtbase::CTreeItemModel* notificationModel = dataModelPtr->AddTreeModel("removedNotification");

		notificationModel->SetData("Id", objectId);

		return rootModelPtr.PopPtr();
	}

	errorMessage = QObject::tr("Can't remove object: %1").arg(QString(objectId));

	return nullptr;
}


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
		prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productOrderInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr orderDataPtr;
		if (objectCollectionIterator->GetObjectData(orderDataPtr)){
			productOrderInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(orderDataPtr.GetPtr());
		}

		if (productOrderInfoPtr != nullptr){
			QByteArray serialNumber = productOrderInfoPtr->GetSerialNumber();
			QByteArray productId = productOrderInfoPtr->GetProductId();
			QByteArray orderUuid = productOrderInfoPtr->GetOrderId();

			QByteArray hardwareMacAddress = objectCollectionIterator->GetElementInfo("DeviceId").toByteArray();
			QByteArray hardwareUuid = objectCollectionIterator->GetElementInfo("DeviceUuid").toByteArray();;

			for (const QByteArray& informationId : informationIds){
				QVariant elementInformation;
				if (informationId == "Id"){
					elementInformation = collectionId;
				}
				else if (informationId == "OrderId"){
					elementInformation = objectCollectionIterator->GetElementInfo("OrderId").toByteArray();
				}
				else if (informationId == "Customer"){
					elementInformation = objectCollectionIterator->GetElementInfo("Customer").toByteArray();
				}
				else if (informationId == "OrderUuid"){
					elementInformation = orderUuid;
//					if (m_orderCollectionCompPtr.IsValid() && !orderUuid.isEmpty()){
//						imtbase::IObjectCollection::DataPtr dataPtr;
//						if (m_orderCollectionCompPtr->GetObjectData(orderUuid, dataPtr)){
//							elementInformation = orderUuid;
//						}
//						else{
//							elementInformation = QByteArray("undefined");
//						}
//					}
				}
				else if (informationId == "HardwareUuid"){
					elementInformation = hardwareUuid;
				}
				else if (informationId == "ProductId"){
					elementInformation = productId;
				}
				else if (informationId == "SerialNumber"){
					elementInformation = serialNumber;
				}
				else if (informationId == "IsPaired"){
					elementInformation = !hardwareMacAddress.isEmpty();
				}
				else if (informationId == "InUse"){
					bool isUse = objectCollectionIterator->GetElementInfo("InUse").toBool();
					if (isUse){
						elementInformation = "InUse";
					}
				}
				else if (informationId == "Status"){
					bool isPaired = !hardwareMacAddress.isEmpty();
					if (isPaired){
						elementInformation = "IsPaired";
					}
					else{
						elementInformation = "NotPaired";
					}

					if (isPaired){
						bool isUse = objectCollectionIterator->GetElementInfo("InUse").toBool();
						if (isUse){
							elementInformation = "InUse";
						}
					}
				}
				else if (informationId == "DeviceId"){
					if (!hardwareMacAddress.isEmpty()){
						elementInformation = hardwareMacAddress;
					}
					else{
						elementInformation = "";
					}
				}
				else if (informationId == "LicenseName"){
					elementInformation = objectCollectionIterator->GetElementInfo("LicenseName").toByteArray();
				}
				else if (informationId == "LicenseId"){
					elementInformation = objectCollectionIterator->GetElementInfo("LicenseId").toByteArray();
				}
				else if (informationId == "CustomerUuid"){
					elementInformation = objectCollectionIterator->GetElementInfo("CustomerUuid").toByteArray();;
				}
				else if (informationId == "Project"){
					elementInformation = objectCollectionIterator->GetElementInfo("Project").toByteArray();;
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


void CSoftwareProductCollectionControllerComp::SetObjectFilter(
			const imtgql::CGqlRequest& gqlRequest,
			const imtbase::CTreeItemModel& objectFilterModel,
			iprm::CParamsSet& filterParams) const
{
	BaseClass::SetObjectFilter(gqlRequest, objectFilterModel, filterParams);

	if (!m_accountCollectionCompPtr.IsValid() || !m_orderCollectionCompPtr.IsValid()){
		return;
	}

	imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return;
	}

	bool filterByGroup = true;

	QByteArrayList userGroupIds;
	imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr != nullptr){
		userGroupIds = userInfoPtr->GetGroups();

		if (userInfoPtr->IsAdmin()){
			filterByGroup = false;
		}
	}

	if (filterByGroup){
		iprm::CParamsSet accountFilter;

		iprm::CParamsSet groupsObjectFilter;
		iprm::COptionsManager groupsOptionsManager;

		for (const QByteArray& groupId : userGroupIds){
			groupsOptionsManager.InsertOption("", groupId);
		}

		groupsObjectFilter.SetEditableParameter("Groups", &groupsOptionsManager);
		accountFilter.SetEditableParameter("ObjectFilter", &groupsObjectFilter);

		iprm::COptionsManager accountsOptionsManager;
		imtbase::ICollectionInfo::Ids accountIds = m_accountCollectionCompPtr->GetElementIds(0, -1, &accountFilter);
		for (const QByteArray& accountId : accountIds){
			accountsOptionsManager.InsertOption("", accountId);
		}

		istd::TDelPtr<iprm::COptionsManager> ordersOptionsManagerPtr;
		ordersOptionsManagerPtr.SetPtr(new iprm::COptionsManager());

		iprm::CParamsSet orderFilter;

		iprm::CParamsSet accountParams;
		accountParams.SetEditableParameter("OrderCustomers", &accountsOptionsManager);
		orderFilter.SetEditableParameter("ObjectFilter", &accountParams);

		imtbase::ICollectionInfo::Ids ordersIds = m_orderCollectionCompPtr->GetElementIds(0, -1, &orderFilter);
		for (const QByteArray& orderId : ordersIds){
			ordersOptionsManagerPtr->InsertOption("", orderId);
		}

		filterParams.SetEditableParameter("Orders", ordersOptionsManagerPtr.PopPtr());
	}

	imtbase::CTreeItemModel* licenseFilterPtr = objectFilterModel.GetTreeItemModel("LicenseFilter");
	if (licenseFilterPtr != nullptr){
		QByteArray key;
		if (licenseFilterPtr->ContainsKey("Key")){
			key = licenseFilterPtr->GetData("Key").toByteArray();
		}

		istd::TDelPtr<iprm::CTextParam> textParamPtr(new iprm::CTextParam());
		if (licenseFilterPtr->ContainsKey("Value")){
			QString value = licenseFilterPtr->GetData("Value").toString();
			textParamPtr->SetText(value);
		}
		filterParams.SetEditableParameter("LicenseStatus", textParamPtr.PopPtr());
	}

	QByteArrayList keys;

	if (objectFilterModel.ContainsKey("FilterIds")){
		imtbase::CTreeItemModel* filterIdsModelPtr = objectFilterModel.GetTreeItemModel("FilterIds");
		if (filterIdsModelPtr != nullptr){
			istd::TDelPtr<iprm::CParamsSet> filterIdsParamsSetPtr(new iprm::CParamsSet);

			if (filterIdsModelPtr->ContainsKey("ExcludeIds")){
				QByteArray filterValue = filterIdsModelPtr->GetData("ExcludeIds").toByteArray();
				if (!filterValue.isEmpty()){
					istd::TDelPtr<iprm::CTextParam> textParamPtr(new iprm::CTextParam());
					textParamPtr->SetText(filterValue);

					filterIdsParamsSetPtr->SetEditableParameter("ExcludeIds", textParamPtr.PopPtr());
				}
			}

			if (filterIdsModelPtr->ContainsKey("IncludeIds")){
				QByteArray filterValue = filterIdsModelPtr->GetData("IncludeIds").toByteArray();
				if (!filterValue.isEmpty()){
					istd::TDelPtr<iprm::CTextParam> textParamPtr(new iprm::CTextParam());
					textParamPtr->SetText(filterValue);

					filterIdsParamsSetPtr->SetEditableParameter("IncludeIds", textParamPtr.PopPtr());
				}
			}

			filterParams.SetEditableParameter("FilterIds", filterIdsParamsSetPtr.PopPtr());
		}
	}

	keys << "HardwareUuid" << "CustomerUuid" << "ProductId";

	for (QByteArray key: keys){
		if (objectFilterModel.ContainsKey(key)){
			QByteArray filterValue = objectFilterModel.GetData(key).toByteArray();
			if (!filterValue.isEmpty()){
				istd::TDelPtr<iprm::CTextParam> textParamPtr(new iprm::CTextParam());
				textParamPtr->SetText(filterValue);
				filterParams.SetEditableParameter(key, textParamPtr.PopPtr());
			}
		}
	}
}


} // namespace prolifegql


