#include "imtlic/IProductInstanceInfo.h"
#include <prolifegql/CCustomerCollectionControllerComp.h>
#include <GeneratedFiles/prolifesdl/SDL/1.0/CPP/Accounts.h>


// ACF includes
#include <iqt/iqt.h>
#include <istd/TDelPtr.h>

// ProLife includes
#include <prolifegql/prolifegql.h>
#include <prolifedata/CCustomerInfo.h>
#include <prolifedata/prolifedata.h>
#include <prolifedata/CGroupFilter.h>


namespace prolifegql
{


// protected methods

bool CCustomerCollectionControllerComp::IsVendorCollection() const
{
	return *m_companyRoleAttrPtr == QByteArrayLiteral("Vendor");
}


// reimplemented (imtgql::IGqlRequestHandler)

bool CCustomerCollectionControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	if (!BaseClass::IsRequestSupported(gqlRequest)){
		return false;
	}

	// The customers and the vendors share the commands of the schema (the generated base accepts them for any
	// collection): a request naming its collection goes to that one, a request without it to the first one.
	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr == nullptr){
		return true;
	}

	const QByteArray collectionId = inputParamPtr->GetParamArgumentValue("collectionId").toByteArray();

	return collectionId.isEmpty() || collectionId == *m_collectionIdAttrPtr;
}


// reimplemented (imtservergql::CObjectCollectionControllerCompBase)

void CCustomerCollectionControllerComp::AddFixedFilters(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			imtbase::IComplexCollectionFilter::FilterExpression& fixedExpression) const
{
	if (!IsVendorCollection()){
		fixedExpression.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter(QByteArrayLiteral("IsCustomer"), QVariant(true));

		return;
	}

	imtbase::IComplexCollectionFilter::FilterExpression vendorExpression;
	vendorExpression.logicalOperation = imtbase::IComplexCollectionFilter::LO_OR;
	vendorExpression.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter(QByteArrayLiteral("IsManufacturer"), QVariant(true));
	vendorExpression.fieldFilters << imtbase::IComplexCollectionFilter::FieldFilter(QByteArrayLiteral("IsSupplier"), QVariant(true));

	fixedExpression.filterExpressions << vendorExpression;
}


QByteArray CCustomerCollectionControllerComp::GetObjectTypeIdFromRequest(const imtgql::CGqlRequest& /*gqlRequest*/) const
{
	// Every company is stored as one object type, whatever the page calls its documents
	return m_objectTypeIdAttrPtr.GetCount() > 0 ? m_objectTypeIdAttrPtr[0] : QByteArray();
}


// reimplemented (sdl::V1_0::imtbase::CImtCollectionGqlHandlerCompBase)

sdl::V1_0::imtbase::CVisualStatus CCustomerCollectionControllerComp::OnGetObjectVisualStatus(
			const sdl::V1_0::imtbase::CGetObjectVisualStatusGqlRequest& getObjectVisualStatusRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CVisualStatus retVal = BaseClass::OnGetObjectVisualStatus(getObjectVisualStatusRequest, gqlRequest, errorMessage);
	if (!errorMessage.isEmpty() || !retVal.objectId){
		return retVal;
	}

	// The name of an account is kept in the document, the collection element has none
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(*retVal.objectId, dataPtr)){
		const prolifedata::CCustomerInfo* customerInfoPtr = dynamic_cast<const prolifedata::CCustomerInfo*>(dataPtr.GetPtr());
		if (customerInfoPtr != nullptr){
			retVal.text = customerInfoPtr->GetName();
		}
	}

	return retVal;
}


void CCustomerCollectionControllerComp::OnAfterSetObjectDescription(
			const QByteArray& objectId,
			const QString& description,
			const imtgql::CGqlRequest& gqlRequest) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT(false);
		return;
	}

	// The description is mirrored into the document body, so this update needs its own operation context,
	// otherwise the resulting revision would show up in the history without initiator and description.
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		prolifedata::ICustomerInfo* customerInfoPtr = dynamic_cast<prolifedata::ICustomerInfo*>(dataPtr.GetPtr());
		if (customerInfoPtr != nullptr){
			customerInfoPtr->SetDescription(description);

			istd::TDelPtr<imtbase::IOperationContext> operationContextPtr = nullptr;
			if (m_operationContextControllerCompPtr.IsValid()){
				operationContextPtr = m_operationContextControllerCompPtr->CreateOperationContext("Update", objectId, dataPtr.GetPtr());
			}

			if (!m_objectCollectionCompPtr->SetObjectData(objectId, *customerInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr.GetPtr())){
				SendWarningMessage(0, QString("Unable to set description for object '%1'. Error: Set object data failed").arg(QString::fromUtf8(objectId)));
			}
		}
	}
}


bool CCustomerCollectionControllerComp::IsDescriptionStoredInDocument() const
{
	return true;
}


bool CCustomerCollectionControllerComp::OnBeforeRemoveElements(
			const QByteArrayList& elementIds,
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (m_softwareCollectionCompPtr.IsValid()){
		if (!elementIds.isEmpty()){
			for (const QByteArray& elementId : elementIds){
				istd::TDelPtr<const iprm::IParamsSet> customerIdFilterParamPtr = CreateComplexFilter("CustomerId", elementId);
				if (customerIdFilterParamPtr.IsValid()){
					imtbase::ICollectionInfo::Ids softwareIds = m_softwareCollectionCompPtr->GetElementIds(0, -1, customerIdFilterParamPtr.GetPtr());
					for (const QByteArray& softwareId : softwareIds){
						imtbase::IObjectCollection::DataPtr dataPtr;
						if (m_softwareCollectionCompPtr->GetObjectData(softwareId, dataPtr)){
							imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
							if (productInstanceInfoPtr != nullptr){
								QByteArray productId = productInstanceInfoPtr->GetProductId();
								productInstanceInfoPtr->SetupProductInstance(productId, "", "");
								m_softwareCollectionCompPtr->SetObjectData(softwareId, *productInstanceInfoPtr);
							}
						}
					}
				}
			}
		}
	}

	return true;
}


// reimplemented (sdl::V1_0::prolife::CAccountCollectionControllerCompBase)

bool CCustomerCollectionControllerComp::CreateRepresentationFromObject(
			const imtbase::IObjectCollectionIterator& objectCollectionIterator,
			const sdl::V1_0::prolife::CAccountsListGqlRequest& accountsListRequest,
			sdl::V1_0::prolife::CAccountItem& representationObject,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to create representation from object. Error: Attribute 'm_objectCollectionCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return false;
	}

	sdl::V1_0::prolife::AccountsListRequestInfo requestInfo = accountsListRequest.GetRequestInfo();

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
		representationObject.id = (objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		// The vendors page opens a company as its own document type; stored it is an account like any other
		QByteArray collectionObjectId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		representationObject.typeId = IsVendorCollection() ? *m_companyRoleAttrPtr : collectionObjectId;
	}

	if (requestInfo.items.isCustomerIdRequested){
		representationObject.customerId = (customerInfoPtr->GetCustomerId());
	}

	if (requestInfo.items.isNameRequested){
		representationObject.name = (customerInfoPtr->GetName());
	}

	if (requestInfo.items.isDescriptionRequested){
		representationObject.description = (customerInfoPtr->GetDescription());
	}

	if (requestInfo.items.isEmailRequested){
		representationObject.email = (customerInfoPtr->GetEmail());
	}

	if (requestInfo.items.isCountryRequested){
		QString country;
		const imtaccount::IAddressProvider* addressProviderPtr = customerInfoPtr->GetAddresses();
		if (addressProviderPtr != nullptr){
			const imtbase::ICollectionInfo::Ids addressIds = addressProviderPtr->GetAddressList().GetElementIds();
			const imtaccount::IAddress* addressPtr = addressIds.isEmpty() ? nullptr : addressProviderPtr->GetAddress(addressIds[0]);
			if (addressPtr != nullptr){
				country = addressPtr->GetCountry();
			}
		}

		representationObject.country = country;
	}

	if (requestInfo.items.isWebsiteRequested){
		representationObject.website = customerInfoPtr->GetWebsite();
	}

	if (requestInfo.items.isIsCustomerRequested){
		representationObject.isCustomer = customerInfoPtr->IsCustomer();
	}

	if (requestInfo.items.isIsManufacturerRequested){
		representationObject.isManufacturer = customerInfoPtr->IsManufacturer();
	}

	if (requestInfo.items.isIsSupplierRequested){
		representationObject.isSupplier = customerInfoPtr->IsSupplier();
	}

	if (requestInfo.items.isRolesRequested){
		representationObject.roles = prolifedata::GetCompanyRolesText(*customerInfoPtr);
	}

	if (requestInfo.items.isAddedRequested){
		QDateTime addedTime = objectCollectionIterator.GetElementInfo("Added").toDateTime();
		addedTime.setTimeSpec(Qt::UTC);

		QString added = addedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.added = (added);
	}

	if (requestInfo.items.isTimeStampRequested){
		QDateTime lastModifiedTime = objectCollectionIterator.GetElementInfo("Timestamp").toDateTime();
		lastModifiedTime.setTimeSpec(Qt::UTC);

		QString lastModified = lastModifiedTime.toLocalTime().toString("dd.MM.yyyy hh:mm:ss");
		representationObject.timeStamp = (lastModified);
	}

	return true;
}


istd::IChangeableUniquePtr CCustomerCollectionControllerComp::CreateObjectFromRepresentation(
			const sdl::V1_0::prolife::CAccountData& accountDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_accountInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_accountInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtaccount::ICompanyInfo> companyInstancePtr = m_accountInfoFactCompPtr.CreateInstance();
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
		errorMessage = QString("Unable to create customer. Error: '%1'").arg(errorMessage);
		return nullptr;
	}

	istd::IChangeableUniquePtr retVal;
	retVal.MoveCastedPtr<imtaccount::ICompanyInfo>(std::move(companyInstancePtr));

	return retVal;
}


bool CCustomerCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::V1_0::prolife::CAccountItemGqlRequest& accountItemRequest,
			sdl::V1_0::prolife::CAccountData& representationPayload,
			QString& errorMessage) const
{
	const prolifedata::CCustomerInfo* customerInfoPtr = dynamic_cast<const prolifedata::CCustomerInfo*>(&data);
	if (customerInfoPtr == nullptr){
		errorMessage = QString("Unable to create representation from object. Error: Object is invalid");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return false;
	}

	sdl::V1_0::prolife::AccountItemRequestArguments arguments = accountItemRequest.GetRequestedArguments();
	if (!arguments.input){
		I_CRITICAL();

		return false;
	}

	QByteArray id;
	if (arguments.input->id){
		id = *arguments.input->id;
	}

	representationPayload.id = (id);

	QString name = customerInfoPtr->GetName();
	representationPayload.name = (name);

	QString description = customerInfoPtr->GetDescription();
	representationPayload.description = (description);

	QString email = customerInfoPtr->GetEmail();
	representationPayload.email = (email);

	QByteArray customerId = customerInfoPtr->GetCustomerId();
	representationPayload.customerId = (customerId);

	representationPayload.website = customerInfoPtr->GetWebsite();
	representationPayload.isCustomer = customerInfoPtr->IsCustomer();
	representationPayload.isManufacturer = customerInfoPtr->IsManufacturer();
	representationPayload.isSupplier = customerInfoPtr->IsSupplier();

	QByteArrayList groups = customerInfoPtr->GetGroups();
	std::sort(groups.begin(), groups.end());
	representationPayload.groups = (groups.join(';'));

	const imtaccount::IAddressProvider* addressProviderPtr = customerInfoPtr->GetAddresses();
	if (addressProviderPtr != nullptr){
		imtbase::ICollectionInfo::Ids addressesIds = addressProviderPtr->GetAddressList().GetElementIds();
		if (!addressesIds.isEmpty()){
			const imtaccount::IAddress* addressPtr = addressProviderPtr->GetAddress(addressesIds[0]);
			if (addressPtr != nullptr){
				QString city = addressPtr->GetCity();
				representationPayload.city = (city);

				QString country = addressPtr->GetCountry();
				representationPayload.country = (country);

				QString street = addressPtr->GetStreet();
				representationPayload.street = (street);

				QString postalCodeStr;
				int postalCode = addressPtr->GetPostalCode();
				if (postalCode > 0){
					postalCodeStr = QString::number(postalCode);
				}
				representationPayload.postalCode = (postalCodeStr);
			}
		}
	}

	return true;
}


void CCustomerCollectionControllerComp::SetAdditionalFilters(
	const imtgql::CGqlRequest& gqlRequest,
	const imtgql::CGqlParamObject& /*viewParamsGql*/,
	iprm::CParamsSet* filterParams) const
{
	if (m_groupFilterParamJoinerCompPtr.IsValid()){
		if (!m_groupFilterParamJoinerCompPtr->JoinGroupFilterParam(gqlRequest, *filterParams)){
			SendWarningMessage(0, QString("Unable to join group filter param"), "COrderCollectionControllerComp");
		}
	}
}


bool CCustomerCollectionControllerComp::UpdateObjectFromRepresentationRequest(
			const imtgql::CGqlRequest& rawGqlRequest,
			const sdl::V1_0::prolife::CAccountUpdateGqlRequest& accountUpdateRequest,
			istd::IChangeable& object,
			QString& errorMessage) const
{
	sdl::V1_0::prolife::AccountUpdateRequestArguments requestArguments = accountUpdateRequest.GetRequestedArguments();
	if (!requestArguments.input){
		I_CRITICAL();

		return false;
	}
	
	if (!requestArguments.input->item.has_value()){
		I_CRITICAL();
		
		return false;
	}

	const sdl::V1_0::prolife::CAccountData& accountData = *requestArguments.input->item;
	QByteArray objectId;
	if (accountData.id){
		objectId = *accountData.id;
	}
	else if (requestArguments.input->id){
		objectId = *requestArguments.input->id;
	}

	prolifedata::CCustomerInfo *customerInfoPtr =
		dynamic_cast<prolifedata::CCustomerInfo *>(&object);
	if (customerInfoPtr == nullptr){
		errorMessage = QString(
			"Unable to cast company info to customer info. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return false;
	}

	istd::IChangeableUniquePtr oldObjectPtr = object.CloneMe();
	const prolifedata::ICustomerInfo* oldCustomerInfoPtr = dynamic_cast<const prolifedata::ICustomerInfo*>(oldObjectPtr.GetPtr());
	if (oldCustomerInfoPtr == nullptr){
		errorMessage = QString("Unable to update customer. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return false;
	}

	customerInfoPtr->ResetData();

	if (!FillObjectFromRepresentation(accountData, object, objectId, errorMessage)){
		errorMessage = QString("Unable to update customer. Error: '%1'").arg(errorMessage);
		return false;
	}

	// The vendor page is permitted as a whole by the hardware management
	if (IsVendorCollection()){
		return true;
	}

	return CheckAccountChangePermissions(*oldCustomerInfoPtr, *customerInfoPtr, rawGqlRequest, errorMessage);
}


bool CCustomerCollectionControllerComp::CheckObjectData(
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& objectId,
			const istd::IChangeable& object,
			QString& errorMessage) const
{
	// A new company is checked by the permission to add companies, a vendor by the hardware management
	if (objectId.isEmpty() || IsVendorCollection() || !m_objectCollectionCompPtr.IsValid()){
		return true;
	}

	const prolifedata::ICustomerInfo* newCustomerInfoPtr = dynamic_cast<const prolifedata::ICustomerInfo*>(&object);
	imtbase::IObjectCollection::DataPtr storedDataPtr;
	if (newCustomerInfoPtr == nullptr || !m_objectCollectionCompPtr->GetObjectData(objectId, storedDataPtr)){
		errorMessage = QString("Object is invalid");

		return false;
	}

	const prolifedata::ICustomerInfo* storedCustomerInfoPtr = dynamic_cast<const prolifedata::ICustomerInfo*>(storedDataPtr.GetPtr());
	if (storedCustomerInfoPtr == nullptr){
		errorMessage = QString("Object is invalid");

		return false;
	}

	return CheckAccountChangePermissions(*storedCustomerInfoPtr, *newCustomerInfoPtr, gqlRequest, errorMessage);
}


// private methods

bool CCustomerCollectionControllerComp::FillObjectFromRepresentation(
			const sdl::V1_0::prolife::CAccountData& accountDataRepresentation,
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

	if (accountDataRepresentation.id){
		objectId = *accountDataRepresentation.id;
	}
	if (objectId.isEmpty()){
		objectId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
	}
	customerInfoPtr->SetObjectUuid(objectId);

	QString accountName;
	if (accountDataRepresentation.name){
		accountName = *accountDataRepresentation.name;
	}

	if (accountName.isEmpty()){
		errorMessage = QString("Account name cannot be empty");
		return false;
	}
	
	istd::TDelPtr<const iprm::IParamsSet> filterParamPtr = CreateComplexFilter("Name", accountName.toUtf8());
	if (filterParamPtr.IsValid()){
		imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, filterParamPtr.GetPtr());
		if (!collectionIds.isEmpty()){
			QByteArray collectionId = collectionIds[0];
			if (objectId != collectionId){
				imtbase::IObjectCollection::DataPtr dataPtr;
				if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
					const prolifedata::CCustomerInfo* foundCustomerInfoPtr = dynamic_cast<const prolifedata::CCustomerInfo*>(dataPtr.GetPtr());
					if (foundCustomerInfoPtr != nullptr){
						QString foundCustomerName = foundCustomerInfoPtr->GetName();
						if (foundCustomerName.toLower() == accountName.toLower()){
							errorMessage = QString("Account name '%1' already exists").arg(accountName);
							SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");
							return false;
						}
					}
				}
			}
		}
	}

	customerInfoPtr->SetName(accountName);

	if (accountDataRepresentation.description){
		QString description = *accountDataRepresentation.description;
		customerInfoPtr->SetDescription(description);
	}

	if (accountDataRepresentation.email){
		QString accountEmail = *accountDataRepresentation.email;
		customerInfoPtr->SetEmail(accountEmail);
	}

	if (accountDataRepresentation.groups){
		QString accountGroups = *accountDataRepresentation.groups;
		if (!accountGroups.isEmpty()){
			QByteArrayList groupIds = accountGroups.toUtf8().split(';');
			for (const QByteArray& groupId : groupIds){
				customerInfoPtr->AddGroup(groupId);
			}
		}
	}

	if (accountDataRepresentation.customerId){
		QByteArray accountCustomerId = *accountDataRepresentation.customerId;
		if (!accountCustomerId.isEmpty()){
			istd::TDelPtr<const iprm::IParamsSet> customerIdFilterParamPtr = CreateComplexFilter("CustomerId", accountCustomerId);
			if (customerIdFilterParamPtr.IsValid()){
				imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, customerIdFilterParamPtr.GetPtr());
				if (!collectionIds.isEmpty()){
					QByteArray collectionId = collectionIds[0];
					if (objectId != collectionId){
						imtbase::IObjectCollection::DataPtr dataPtr;
						if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
							const prolifedata::CCustomerInfo* foundCustomerInfoPtr = dynamic_cast<const prolifedata::CCustomerInfo*>(dataPtr.GetPtr());
							if (foundCustomerInfoPtr != nullptr){
								QByteArray foundCustomerId = foundCustomerInfoPtr->GetCustomerId();
								if (foundCustomerId.toLower() == accountCustomerId.toLower()){
									errorMessage = QString("Customer-ID '%1' already exists").arg(QString::fromUtf8(foundCustomerId));
									SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");
									return false;
								}
							}
						}
					}
				}
			}
		}

		customerInfoPtr->SetCustomerId(accountCustomerId);
	}

	imtaccount::CAddress address;

	if (accountDataRepresentation.city){
		QString accountCity = *accountDataRepresentation.city;
		address.SetCity(accountCity);
	}

	if (accountDataRepresentation.street){
		QString accountStreet = *accountDataRepresentation.street;
		address.SetStreet(accountStreet);
	}

	if (accountDataRepresentation.postalCode){
		QString accountPostalCode = *accountDataRepresentation.postalCode;
		address.SetPostalCode(accountPostalCode.toInt());
	}

	if (accountDataRepresentation.country){
		QString accountCountry = *accountDataRepresentation.country;
		address.SetCountry(accountCountry);
	}

	customerInfoPtr->AddAddress(address);

	if (accountDataRepresentation.website){
		customerInfoPtr->SetWebsite(*accountDataRepresentation.website);
	}
	if (accountDataRepresentation.isCustomer){
		customerInfoPtr->SetCustomer(*accountDataRepresentation.isCustomer);
	}
	if (accountDataRepresentation.isManufacturer){
		customerInfoPtr->SetManufacturer(*accountDataRepresentation.isManufacturer);
	}
	if (accountDataRepresentation.isSupplier){
		customerInfoPtr->SetSupplier(*accountDataRepresentation.isSupplier);
	}

	// A company saved from a page has the role of that page.
	if (!IsVendorCollection()){
		customerInfoPtr->SetCustomer(true);
	}
	else if (!customerInfoPtr->IsManufacturer() && !customerInfoPtr->IsSupplier()){
		errorMessage = QStringLiteral("The company is neither a manufacturer nor a supplier");

		return false;
	}

	return true;
}


const iprm::IParamsSet* CCustomerCollectionControllerComp::CreateComplexFilter(const QByteArray& fieldId, const QByteArray& fieldValue) const
{
	imtbase::IComplexCollectionFilter::FieldFilter fieldFilter;
	fieldFilter.fieldId = fieldId;
	fieldFilter.filterValue = fieldValue;
	
	imtbase::IComplexCollectionFilter::FilterExpression groupFilter;
	groupFilter.fieldFilters << fieldFilter;

	imtbase::CComplexCollectionFilter* complexFilterPtr = new imtbase::CComplexCollectionFilter();
	complexFilterPtr->SetFilterExpression(groupFilter);
	
	iprm::CParamsSet* filterParamPtr = new iprm::CParamsSet();
	filterParamPtr->SetEditableParameter("ComplexFilter", complexFilterPtr, true);
	
	return filterParamPtr;
}


} // namespace prolifegql


