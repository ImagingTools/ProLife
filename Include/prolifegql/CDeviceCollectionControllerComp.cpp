#include <prolifegql/CDeviceCollectionControllerComp.h>


// ACF includes
#include <idoc/IDocumentMetaInfo.h>
#include <iprm/CTextParam.h>

// ImtCore includes
#include <imtbase/CCollectionFilter.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtdb/CSqlDatabaseObjectCollectionComp.h>

// ProLife includes
#include <prolifedata/IOrderInfo.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/TOrderedWrap.h>
#include <prolifedata/CHardwareProductBinding.h>


namespace prolifegql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

imtbase::CTreeItemModel* CDeviceCollectionControllerComp::ListObjects(
		const imtgql::CGqlRequest& gqlRequest,
		QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid() || !m_accountCollectionCompPtr.IsValid() || !m_orderCollectionCompPtr.IsValid()){
		return nullptr;
	}

	imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		return nullptr;
	}

	bool isAdmin = false;
	QByteArrayList userPermissions;
	imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr != nullptr){
		userPermissions = userInfoPtr->GetPermissions();

		isAdmin = userInfoPtr->IsAdmin();
	}

	bool filterByGroup = false;
	if (m_checkPermissionCompPtr.IsValid()){
		QByteArrayList permissions;
		permissions << *m_permissionIdAttrPtr;
		filterByGroup = !m_checkPermissionCompPtr->CheckPermission(userPermissions, permissions);
	}

	if (isAdmin){
		filterByGroup = false;
	}

	iprm::CParamsSet objectFilter;
	iprm::CParamsSet licenseFilter;

	iprm::CParamsSet accountFilter;
	iprm::CParamsSet groups;
	iprm::CParamsSet accountParams;
	iprm::CParamsSet orderFilter;

	iprm::COptionsManager optionsManager;
	iprm::COptionsManager accountsOptionsManager;
	iprm::COptionsManager ordersOptionsManager;

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

		if (optionsManager.GetOptionsCount() > 0){
			groups.SetEditableParameter("Groups", &optionsManager);
			accountFilter.SetEditableParameter("ObjectFilter", &groups);

			imtbase::ICollectionInfo::Ids accountIds = m_accountCollectionCompPtr->GetElementIds(0, -1, &accountFilter);
			for (const QByteArray& accountId : accountIds){
				accountsOptionsManager.InsertOption("", accountId);
			}

			accountParams.SetEditableParameter("OrderCustomers", &accountsOptionsManager);
			orderFilter.SetEditableParameter("ObjectFilter", &accountParams);

			imtbase::ICollectionInfo::Ids ordersIds = m_orderCollectionCompPtr->GetElementIds(0, -1, &orderFilter);
			for (const QByteArray& orderId : ordersIds){
				ordersOptionsManager.InsertOption("", orderId);
			}

			if (ordersOptionsManager.GetOptionsCount() == 0){
				ordersOptionsManager.InsertOption("", "");
			}

			if (ordersOptionsManager.GetOptionsCount() > 0){
				objectFilter.SetEditableParameter("Orders", &ordersOptionsManager);
			}
		}
		else{
			return rootModelPtr.PopPtr();
		}
	}

	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();
	imtbase::CTreeItemModel* notificationModel = nullptr;

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		notificationModel = new imtbase::CTreeItemModel();

		const imtgql::CGqlObject* viewParamsGql = nullptr;
		if (inputParams.size() > 0){
			viewParamsGql = inputParams.at(0).GetFieldArgumentObjectPtr("viewParams");
		}

		iprm::CParamsSet filterParams;

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
					QByteArray key;
					if (objectFilterPtr->ContainsKey("Key")){
						key = objectFilterPtr->GetData("Key").toByteArray();
					}

					istd::TDelPtr<iprm::CTextParam> textParamPtr(new iprm::CTextParam());
					if (objectFilterPtr->ContainsKey("Value")){
						QString value = objectFilterPtr->GetData("Value").toString();
						textParamPtr->SetText(value);
					}
					objectFilter.SetEditableParameter(key, textParamPtr.PopPtr());
				}

				imtbase::CTreeItemModel* licenseFilterPtr = generalModel.GetTreeItemModel("LicenseFilter");
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
					objectFilter.SetEditableParameter("LicenseStatus", textParamPtr.PopPtr());
				}

			}

			filterParams.SetEditableParameter("Filter", &m_filter);
			filterParams.SetEditableParameter("ObjectFilter", &objectFilter);
			filterParams.SetEditableParameter("LicenseFilter", &licenseFilter);

			this->SetAdditionalFilters(*viewParamsGql, &filterParams);
		}

		int elementsCount = m_objectCollectionCompPtr->GetElementsCount(&filterParams);
		int pagesCount = std::ceil(elementsCount / (double)count);
		if (pagesCount <= 0){
			pagesCount = 1;
		}

		notificationModel->SetData("PagesCount", pagesCount);

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
							return nullptr;
						}
					}
				}
				else{
					return nullptr;
				}
			}
		}

		// Get the number of new devices
		iprm::CTextParam textParam;
		textParam.SetText("none");

		objectFilter.SetEditableParameter("Status", &textParam);

		iprm::CParamsSet newObjectsFilterParams;
		newObjectsFilterParams.SetEditableParameter("ObjectFilter", &objectFilter);

		int newElementsCount = m_objectCollectionCompPtr->GetElementsCount(&newObjectsFilterParams);
		notificationModel->SetData("NewCount", newElementsCount);

		dataModel->SetExternTreeModel("notification", notificationModel);
	}

	return rootModelPtr.PopPtr();
}


bool CDeviceCollectionControllerComp::SetupGqlItem(
		const imtgql::CGqlRequest& gqlRequest,
		imtbase::CTreeItemModel& model,
		int itemIndex,
		const imtbase::IObjectCollectionIterator* objectCollectionIterator,
		QString& errorMessage) const
{
	bool retVal = true;

	QByteArrayList informationIds = GetInformationIds(gqlRequest, "items");

	if (!informationIds.isEmpty() && m_objectCollectionCompPtr.IsValid()){
		prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr = nullptr;
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (objectCollectionIterator->GetObjectData(dataPtr)){
			deviceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableDeviceInfo*>(dataPtr.GetPtr());
		}

		if (deviceInfoPtr != nullptr){
			QByteArray collectionId = objectCollectionIterator->GetObjectId();
			int licenseCount = 0;

			for (QByteArray informationId : informationIds){
				QVariant elementInformation;

				if (informationId == "TypeId"){
					elementInformation = m_objectCollectionCompPtr->GetObjectTypeId(collectionId);
				}
				else if(informationId == "Id"){
					elementInformation = collectionId;
				}
				else if(informationId == "Name"){
					QByteArray deviceType = deviceInfoPtr->GetDeviceType();
					QByteArray macAddress = deviceInfoPtr->GetMacAddress();

					elementInformation = deviceType;
					if (!macAddress.isEmpty()){
						elementInformation = deviceType + " (" + macAddress + ")";
					}
				}
				else if(informationId == "MacAddress"){
					elementInformation = deviceInfoPtr->GetMacAddress();
				}
				else if(informationId == "SerialNumber"){
					elementInformation = deviceInfoPtr->GetSerialNumber();
				}
				else if(informationId == "Description"){
					elementInformation = deviceInfoPtr->GetDescription();
				}
				else if(informationId == "DeviceType"){
					elementInformation = deviceInfoPtr->GetDeviceType();
				}
				else if(informationId == "OrderId"){
					if (m_orderCollectionCompPtr.IsValid()){
						QByteArray orderUuid = deviceInfoPtr->GetOrderId();
						imtbase::IObjectCollection::DataPtr dataPtr;
						if (m_orderCollectionCompPtr->GetObjectData(orderUuid, dataPtr)){
							prolifedata::IOrderInfo* orderPtr = dynamic_cast<prolifedata::IOrderInfo*>(dataPtr.GetPtr());
							if (orderPtr != nullptr){
								QByteArray orderId = orderPtr->GetOrderId();

								elementInformation = orderId;
							}
						}
					}
				}
				else if(informationId == "OrderUuid"){
					elementInformation = deviceInfoPtr->GetOrderId();
				}
				else if(informationId == "Status"){
					int status = deviceInfoPtr->GetDeviceProductionStatus();
					switch (status){
					case prolifedata::IDeviceInfo::DPS_NONE:
						elementInformation = "None";
						break;
					case prolifedata::IDeviceInfo::DPS_ACCEPTED:
						elementInformation = "Accepted";
						break;
					case prolifedata::IDeviceInfo::DPS_IN_PROGRESS:
						elementInformation = "In Progress";
						break;
					case prolifedata::IDeviceInfo::DPS_CANCELED:
						elementInformation = "Canceled";
						break;
					case prolifedata::IDeviceInfo::DPS_ON_HOLD:
						elementInformation = "Hold";
						break;
					case prolifedata::IDeviceInfo::DPS_FINISHED:
						elementInformation = "Finished";
						break;
					}
				}
				else if(informationId == "Added"){
					QDateTime addedTime =  objectCollectionIterator->GetElementInfo("added").toDateTime();
					elementInformation = addedTime.toString("dd.MM.yyyy hh:mm:ss");
				}
				else if(informationId == "LastModified"){
					QDateTime lastTime =  objectCollectionIterator->GetElementInfo("lastmodified").toDateTime();
					elementInformation = lastTime.toString("dd.MM.yyyy hh:mm:ss");
				}
				else if(informationId == "Licenses"){
					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_bindingCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
						const prolifedata::IHardwareProductBinding* bindingInfoPtr = dynamic_cast<const prolifedata::IHardwareProductBinding*>(dataPtr.GetPtr());
						if (bindingInfoPtr != nullptr){
							QByteArrayList softwareIds = bindingInfoPtr->GetSoftwareIds();
							elementInformation = softwareIds.join(';');
						}
					}
				}

				if (elementInformation.isNull()){
					elementInformation = "";
				}

				retVal = retVal && model.SetData(informationId, elementInformation, itemIndex);
			}

			QVariant  softwareLinksCount = objectCollectionIterator->GetElementInfo("SoftwareLinksCount").toInt();
			retVal = retVal && model.SetData("SoftwareLinksCount", softwareLinksCount, itemIndex);


			return true;
		}
	}

	return false;
}


imtbase::CTreeItemModel* CDeviceCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_bindingCollectionCompPtr.IsValid()){
		return nullptr;
	}

	if (!m_softwareProductCollectionCompPtr.IsValid()){
		return nullptr;
	}

	QByteArray objectId;
	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel);
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	int index = dataModelPtr->InsertNewItem();

	dataModelPtr->SetData("Name", QT_TR_NOOP("Licenses"), index);
	imtbase::CTreeItemModel* childrenModelPtr = dataModelPtr->AddTreeModel("Children", index);
//	childrenModelPtr->SetData("Value", QObject::tr("No related licenses"));

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_bindingCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const prolifedata::IHardwareProductBinding* bindingInfoPtr = dynamic_cast<const prolifedata::IHardwareProductBinding*>(dataPtr.GetPtr());
		if (bindingInfoPtr != nullptr){
			QByteArrayList softwareIds = bindingInfoPtr->GetSoftwareIds();
			for (const QByteArray& softwareId : softwareIds){
				imtbase::IObjectCollection::DataPtr productDataPtr;
				if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, productDataPtr)){
					imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
					if (productInstanceInfoPtr != nullptr){
						QByteArray productId = productInstanceInfoPtr->GetProductId();
						QByteArray serialNumber = productInstanceInfoPtr->GetSerialNumber();

						const imtbase::ICollectionInfo& licenseList = productInstanceInfoPtr->GetLicenseInstances();
						for (const QByteArray& licenseId : licenseList.GetElementIds()){
							const imtlic::ILicenseInstance* licenseInstancePtr = productInstanceInfoPtr->GetLicenseInstance(licenseId);
							if (licenseInstancePtr != nullptr){
								int childrenIndex = childrenModelPtr->InsertNewItem();

								childrenModelPtr->SetData("Value", productId + " (" + licenseId + ")", childrenIndex);
							}
						}
					}
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace prolifegql


