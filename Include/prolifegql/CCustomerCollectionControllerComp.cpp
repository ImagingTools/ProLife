#include <prolifegql/CCustomerCollectionControllerComp.h>


// ACF includes
#include <iqt/iqt.h>
#include <iprm/CTextParam.h>

// ProLife includes
#include <prolifedata/CCustomerInfo.h>


namespace prolifegql
{


// protected methods

// reimplemented (sdl::prolife::Accounts::CAccountCollectionControllerCompBase)

bool CCustomerCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::prolife::Accounts::CAccountsListGqlRequest& accountsListRequest,
			sdl::prolife::Accounts::CAccountItem::V1_0& representationObject,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to create representation from object. Error: Attribute 'm_objectCollectionCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return false;
	}

	sdl::prolife::Accounts::AccountsListRequestInfo requestInfo = accountsListRequest.GetRequestInfo();

	QByteArray objectId = objectCollectionIterator.GetObjectId();

	prolifedata::CCustomerInfo* customerInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (objectCollectionIterator.GetObjectData(dataPtr)){
		customerInfoPtr = dynamic_cast<prolifedata::CCustomerInfo*>(dataPtr.GetPtr());
	}

	if (customerInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object '%1'").arg(qPrintable(objectId));
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return false;
	}

	if (requestInfo.items.isIdRequested){
		representationObject.Id = (objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		representationObject.TypeId = (collectionObjectId);
	}

	if (requestInfo.items.isCustomerIdRequested){
		representationObject.CustomerId = (customerInfoPtr->GetCustomerId());
	}

	if (requestInfo.items.isNameRequested){
		representationObject.Name = (customerInfoPtr->GetName());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.Description = (customerInfoPtr->GetDescription());
	}

	if (requestInfo.items.isEmailRequested){
		representationObject.Email = (customerInfoPtr->GetEmail());
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.Added = (added);
	}

	if (requestInfo.items.isTimeStampRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("Timestamp").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.TimeStamp = (lastModified);
	}

	return true;
}


istd::IChangeable* CCustomerCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::prolife::Accounts::CAccountData::V1_0& accountDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_accountInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_accountInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtauth::ICompanyInfo> companyInstancePtr = m_accountInfoFactCompPtr.CreateInstance();
	if (!companyInstancePtr.IsValid()){
		errorMessage = QString("Unable to create company instance. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return nullptr;
	}

	prolifedata::CCustomerInfo* customerInfoPtr = dynamic_cast<prolifedata::CCustomerInfo*>(companyInstancePtr.GetPtr());
	if (customerInfoPtr == nullptr){
		errorMessage = QString("Unable to cast company info to customer info. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return nullptr;
	}

	if (!FillObjectFromRepresentation(accountDataRepresentation, *customerInfoPtr, newObjectId, errorMessage)){
		errorMessage = QString("Unable to create customer from representation. Error: '%1'");
		return nullptr;
	}

	return companyInstancePtr.PopPtr();
}


bool CCustomerCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::prolife::Accounts::CAccountItemGqlRequest& accountItemRequest,
			sdl::prolife::Accounts::CAccountDataPayload::V1_0& representationPayload,
			QString& errorMessage) const
{
	const prolifedata::CCustomerInfo* customerInfoPtr = dynamic_cast<const prolifedata::CCustomerInfo*>(&data);
	if (customerInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return false;
	}

	sdl::prolife::Accounts::AccountItemRequestArguments arguments = accountItemRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0){
		I_CRITICAL();

		return false;
	}

	sdl::prolife::Accounts::CAccountData::V1_0 accountData;

	QByteArray id;
	if (arguments.input.Version_1_0->Id){
		id = *arguments.input.Version_1_0->Id;
	}

	accountData.Id = (id);

	QString name = customerInfoPtr->GetName();
	accountData.Name = (name);

	QString description = customerInfoPtr->GetDescription();
	accountData.Description = (description);

	QString email = customerInfoPtr->GetEmail();
	accountData.Email = (email);

	QByteArray customerId = customerInfoPtr->GetCustomerId();
	accountData.CustomerId = (customerId);

	QByteArrayList groups = customerInfoPtr->GetGroups();
	std::sort(groups.begin(), groups.end());
	accountData.Groups = (groups.join(';'));

	const imtauth::IAddressProvider* addressProviderPtr = customerInfoPtr->GetAddresses();
	if (addressProviderPtr != nullptr){
		imtbase::ICollectionInfo::Ids addressesIds = addressProviderPtr->GetAddressList().GetElementIds();
		if (!addressesIds.isEmpty()){
			const imtauth::IAddress* addressPtr = addressProviderPtr->GetAddress(addressesIds[0]);
			if (addressPtr != nullptr){
				QString city = addressPtr->GetCity();
				accountData.City = (city);

				QString country = addressPtr->GetCountry();
				accountData.Country = (country);

				QString street = addressPtr->GetStreet();
				accountData.Street = (street);

				QString postalCodeStr;
				int postalCode = addressPtr->GetPostalCode();
				if (postalCode > 0){
					postalCodeStr = QString::number(postalCode);
				}
				accountData.PostalCode = (postalCodeStr);
			}
		}
	}

	representationPayload.AccountData = accountData;

	return true;
}


void CCustomerCollectionControllerComp::SetObjectFilter(
			const imtgql::CGqlRequest& gqlRequest,
			const imtbase::CTreeItemModel& objectFilterModel,
			iprm::CParamsSet& filterParams) const
{
	BaseClass::SetObjectFilter(gqlRequest, objectFilterModel, filterParams);

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		SendErrorMessage(0, QString("Unable to create an object filter. GraphQL context is nullptr."), "CSoftwareProductCollectionControllerComp");

		return;
	}

	imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		SendErrorMessage(0, QString("Unable to create an object filter. Error: user from GraphQL is invalid."), "CSoftwareProductCollectionControllerComp");

		return;
	}

	bool filterByGroup = true;

	QByteArray userId = userInfoPtr->GetId();
	QByteArrayList userGroupIds = userInfoPtr->GetGroups();
	QByteArrayList userPermissions = userInfoPtr->GetPermissions();

	if (m_checkPermissionCompPtr.IsValid()){
		QByteArrayList permissions;
		permissions << *m_permissionIdAttrPtr;

		filterByGroup = !m_checkPermissionCompPtr->CheckPermission(userPermissions, permissions);
	}

	if (userInfoPtr->IsAdmin()){
		filterByGroup = false;
	}

	if (filterByGroup){
		iprm::CTextParam* userParamPtr = new iprm::CTextParam();
		userParamPtr->SetText(userId);

		iprm::CTextParam* groupParamPtr = new iprm::CTextParam();
		QByteArray groups;
		if (!userGroupIds.isEmpty()){
			groups = userGroupIds.join(';');
		}
		groupParamPtr->SetText(groups);

		iprm::CParamsSet* paramsSetPtr = new iprm::CParamsSet();

		paramsSetPtr->SetEditableParameter("UserParam", userParamPtr, true);
		paramsSetPtr->SetEditableParameter("GroupParam", groupParamPtr, true);

		filterParams.SetEditableParameter("Groups", paramsSetPtr, true);
	}
}


bool CCustomerCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& /*rawGqlRequest*/,
			const sdl::prolife::Accounts::CAccountUpdateGqlRequest& accountUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	sdl::prolife::Accounts::AccountUpdateRequestArguments requestArguments = accountUpdateRequest.GetRequestedArguments();
	if (!requestArguments.input.Version_1_0){
		I_CRITICAL();

		return false;
	}

	const sdl::prolife::Accounts::CAccountData::V1_0& accountData = *accountUpdateRequest.GetRequestedArguments().input.Version_1_0->Item;
	QByteArray objectId = *accountData.Id;

	prolifedata::CCustomerInfo *customerInfoPtr =
		dynamic_cast<prolifedata::CCustomerInfo *>(&object);
	if (customerInfoPtr == nullptr) {
		errorMessage = QString(
			"Unable to cast company info to customer info. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return false;
	}

	customerInfoPtr->ResetData();

	if (!FillObjectFromRepresentation(accountData, object, objectId, errorMessage)){
		errorMessage = QString("Unable to create customer from representation. Error: '%1'");
		return false;
	}

	return true;
}


// private methods

bool CCustomerCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::prolife::Accounts::CAccountData::V1_0& accountDataRepresentation,
			istd::IChangeable& object,
			QByteArray& objectId,
			QString& errorMessage) const
{
	prolifedata::CCustomerInfo* customerInfoPtr = dynamic_cast<prolifedata::CCustomerInfo*>(&object);
	if (customerInfoPtr == nullptr){
		errorMessage = QString("Unable to cast company info to customer info. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return false;
	}

	if (accountDataRepresentation.Id){
		objectId = *accountDataRepresentation.Id;
	}
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	customerInfoPtr->SetObjectUuid(objectId);

	QString accountName;
	if (accountDataRepresentation.Name){
		accountName = *accountDataRepresentation.Name;
	}

	if (accountName.isEmpty()){
		errorMessage = QString("Account name cannnot be empty");
		return false;
	}

	customerInfoPtr->SetName(accountName);

	if (accountDataRepresentation.Description){
		QString description = *accountDataRepresentation.Description;
		customerInfoPtr->SetDescription(description);
	}

	if (accountDataRepresentation.Email){
		QString accountEmail = *accountDataRepresentation.Email;
		customerInfoPtr->SetEmail(accountEmail);
	}

	if (accountDataRepresentation.Groups){
		QString accountGroups = *accountDataRepresentation.Groups;
		if (!accountGroups.isEmpty()){
			QByteArrayList groupIds = accountGroups.toUtf8().split(';');
			for (const QByteArray& groupId : groupIds){
				customerInfoPtr->AddGroup(groupId);
			}
		}
	}

	if (accountDataRepresentation.CustomerId){
		QByteArray accountCustomerId = *accountDataRepresentation.CustomerId;
		customerInfoPtr->SetCustomerId(accountCustomerId);
	}

	imtauth::CAddress address;

	if (accountDataRepresentation.City){
		QString accountCity = *accountDataRepresentation.City;
		address.SetCity(accountCity);
	}

	if (accountDataRepresentation.Street){
		QString accountStreet = *accountDataRepresentation.Street;
		address.SetStreet(accountStreet);
	}

	if (accountDataRepresentation.PostalCode){
		QString accountPostalCode = *accountDataRepresentation.PostalCode;
		address.SetPostalCode(accountPostalCode.toInt());
	}

	if (accountDataRepresentation.Country){
		QString accountCountry = *accountDataRepresentation.Country;
		address.SetCountry(accountCountry);
	}

	customerInfoPtr->AddAddress(address);

	return true;
}


} // namespace prolifegql


