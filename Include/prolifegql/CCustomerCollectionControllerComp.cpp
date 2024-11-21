#include <prolifegql/CCustomerCollectionControllerComp.h>


// ACF includes
#include <iprm/CTextParam.h>

// ImtCore includes
#include <iqt/iqt.h>

// ProLife includes
#include <prolifedata/CCustomerInfo.h>


namespace prolifegql
{


// protected methods

// reimplemented (sdl::prolife::Accounts::V1_0::CAccountCollectionControllerCompBase)

bool CCustomerCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::prolife::Accounts::V1_0::CAccountsListGqlRequest& accountsListRequest,
			sdl::prolife::Accounts::V1_0::CAccountItem& representationObject,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to create representation from object. Error: Attribute 'm_objectCollectionCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return false;
	}

	sdl::prolife::Accounts::V1_0::AccountsListRequestInfo requestInfo = accountsListRequest.GetRequestInfo();

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

	if (requestInfo.items.isIdRequested) {
		representationObject.SetId(objectId);
	}

	if (requestInfo.items.isCustomerIdRequested) {
		representationObject.SetCustomerId(customerInfoPtr->GetCustomerId());
	}

	if (requestInfo.items.isTypeIdRequested) {
		representationObject.SetTypeId(m_objectCollectionCompPtr->GetObjectTypeId(objectId));
	}

	if (requestInfo.items.isNameRequested) {
		representationObject.SetName(customerInfoPtr->GetName());
	}

	if (requestInfo.items.isDescriptionRequested) {
		representationObject.SetDescription(customerInfoPtr->GetDescription());
	}

	if (requestInfo.items.isEmailRequested) {
		representationObject.SetEmail(customerInfoPtr->GetEmail());
	}

	if (requestInfo.items.isAddedRequested) {
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.SetAdded(added);
	}

	if (requestInfo.items.isLastModifiedRequested) {
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("LastModified").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.SetLastModified(lastModified);
	}

	return true;
}


istd::IChangeable* CCustomerCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::prolife::Accounts::V1_0::CAccountData& accountDataRepresentation,
			QByteArray& newObjectId,
			QString& name,
			QString& description,
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

	QByteArray accountId = accountDataRepresentation.GetId();
	if (accountId.isEmpty()){
		accountId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	customerInfoPtr->SetObjectUuid(accountId);
	newObjectId = accountId;

	QString accountName = accountDataRepresentation.GetName();
	if (accountName.isEmpty()){
		errorMessage = QString("Account name cannnot be empty");
		return nullptr;
	}

	customerInfoPtr->SetName(accountName);
	name = accountName;

	QString accountDescription = accountDataRepresentation.GetDescription();
	customerInfoPtr->SetDescription(accountDescription);
	description = accountDescription;

	QString accountEmail = accountDataRepresentation.GetEmail();
	customerInfoPtr->SetEmail(accountEmail);

	QString accountGroups = accountDataRepresentation.GetGroups();
	if (!accountGroups.isEmpty()){
		QByteArrayList groupIds = accountGroups.toUtf8().split(';');
		for (const QByteArray& groupId : groupIds){
			customerInfoPtr->AddGroup(groupId);
		}
	}

	QByteArray accountCustomerId = accountDataRepresentation.GetCustomerId();
	customerInfoPtr->SetCustomerId(accountCustomerId);

	imtauth::CAddress address;

	QString accountCity = accountDataRepresentation.GetCity();
	address.SetCity(accountCity);

	QString accountStreet = accountDataRepresentation.GetStreet();
	address.SetStreet(accountStreet);

	QString accountPostalCode = accountDataRepresentation.GetPostalCode();
	address.SetPostalCode(accountPostalCode.toInt());

	QString accountCountry = accountDataRepresentation.GetCountry();
	address.SetCountry(accountCountry);

	customerInfoPtr->AddAddress(address);

	return companyInstancePtr.PopPtr();
}


bool CCustomerCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::prolife::Accounts::V1_0::CAccountItemGqlRequest& accountItemRequest,
			sdl::prolife::Accounts::V1_0::CAccountDataPayload& representationPayload,
			QString& errorMessage) const
{
	const prolifedata::CCustomerInfo* customerInfoPtr = dynamic_cast<const prolifedata::CCustomerInfo*>(&data);
	if (customerInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return false;
	}

	sdl::prolife::Accounts::V1_0::AccountItemRequestArguments arguments = accountItemRequest.GetRequestedArguments();

	sdl::prolife::Accounts::V1_0::CAccountData accountData;

	QByteArray id = arguments.input.GetId();
	accountData.SetId(id);

	QString name = customerInfoPtr->GetName();
	accountData.SetName(name);

	QString description = customerInfoPtr->GetDescription();
	accountData.SetDescription(description);

	QString email = customerInfoPtr->GetEmail();
	accountData.SetEmail(email);

	QByteArray customerId = customerInfoPtr->GetCustomerId();
	accountData.SetCustomerId(customerId);

	QByteArrayList groups = customerInfoPtr->GetGroups();
	std::sort(groups.begin(), groups.end());
	accountData.SetGroups(groups.join(';'));

	const imtauth::IAddressProvider* addressProviderPtr = customerInfoPtr->GetAddresses();
	if (addressProviderPtr != nullptr){
		imtbase::ICollectionInfo::Ids addressesIds = addressProviderPtr->GetAddressList().GetElementIds();
		if (!addressesIds.isEmpty()){
			const imtauth::IAddress* addressPtr = addressProviderPtr->GetAddress(addressesIds[0]);
			if (addressPtr != nullptr){
				QString city = addressPtr->GetCity();
				accountData.SetCity(city);

				QString country = addressPtr->GetCountry();
				accountData.SetCountry(country);

				QString street = addressPtr->GetStreet();
				accountData.SetStreet(street);

				QString postalCodeStr;
				int postalCode = addressPtr->GetPostalCode();
				if (postalCode > 0){
					postalCodeStr = QString::number(postalCode);
				}
				accountData.SetPostalCode(postalCodeStr);
			}
		}
	}

	representationPayload.SetAccountData(accountData);

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


} // namespace prolifegql


