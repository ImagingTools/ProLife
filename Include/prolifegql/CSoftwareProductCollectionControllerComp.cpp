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

	keys << "HardwareUuid" << "ExcludeIds" << "CustomerUuid" << "ProductId";

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


