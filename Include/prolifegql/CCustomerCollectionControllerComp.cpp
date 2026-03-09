// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#include "imtlic/IProductInstanceInfo.h"
#include <prolifegql/CCustomerCollectionControllerComp.h>


// ACF includes
#include <iqt/iqt.h>
#include <istd/TDelPtr.h>

// ProLife includes
#include <prolifedata/CCustomerInfo.h>
#include <prolifedata/CGroupFilter.h>


namespace prolifegql
{


// protected methods

void CCustomerCollectionControllerComp::OnAfterSetObjectDescription(
			const QByteArray& objectId,
			const QString& description,
			const imtgql::CGqlRequest& gqlRequest) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		Q_ASSERT(false);
		return;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		prolifedata::ICustomerInfo* customerInfoPtr = dynamic_cast<prolifedata::ICustomerInfo*>(dataPtr.GetPtr());
		if (customerInfoPtr != nullptr){
			customerInfoPtr->SetDescription(description);

			if (!m_objectCollectionCompPtr->SetObjectData(objectId, *customerInfoPtr)){
				SendWarningMessage(0, QString("Unable to set description for object '%1'. Error: Set object data failed").arg(QString::fromUtf8(objectId)));
			}
		}
	}
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
		representationObject.id = (objectId);
	}

	if (requestInfo.items.isTypeIdRequested){
		QByteArray collectionObjectId = m_objectCollectionCompPtr->GetObjectTypeId(objectId);
		representationObject.typeId = (collectionObjectId);
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
			const sdl::prolife::Accounts::CAccountData::V1_0& accountDataRepresentation,
			QByteArray& newObjectId,
			QString& errorMessage) const
{
	if (!m_accountInfoFactCompPtr.IsValid()){
		errorMessage = QString("Unable to create object from representation. Error: Attribute 'm_accountInfoFactCompPtr' was not set");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return nullptr;
	}

	istd::TUniqueInterfacePtr<imtauth::ICompanyInfo> companyInstancePtr = m_accountInfoFactCompPtr.CreateInstance();
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
	retVal.MoveCastedPtr<imtauth::ICompanyInfo>(companyInstancePtr);

	return retVal;
}


bool CCustomerCollectionControllerComp::CreateRepresentationFromObject(
			const istd::IChangeable& data,
			const sdl::prolife::Accounts::CAccountItemGqlRequest& accountItemRequest,
			sdl::prolife::Accounts::CAccountData::V1_0& representationPayload,
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

	QByteArray id;
	if (arguments.input.Version_1_0->id){
		id = *arguments.input.Version_1_0->id;
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

	QByteArrayList groups = customerInfoPtr->GetGroups();
	std::sort(groups.begin(), groups.end());
	representationPayload.groups = (groups.join(';'));

	const imtauth::IAddressProvider* addressProviderPtr = customerInfoPtr->GetAddresses();
	if (addressProviderPtr != nullptr){
		imtbase::ICollectionInfo::Ids addressesIds = addressProviderPtr->GetAddressList().GetElementIds();
		if (!addressesIds.isEmpty()){
			const imtauth::IAddress* addressPtr = addressProviderPtr->GetAddress(addressesIds[0]);
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
	
	if (!requestArguments.input.Version_1_0->item.has_value()){
		I_CRITICAL();
		
		return false;
	}

	const sdl::prolife::Accounts::CAccountData::V1_0& accountData = *requestArguments.input.Version_1_0->item;
	QByteArray objectId = *accountData.id;

	prolifedata::CCustomerInfo *customerInfoPtr =
		dynamic_cast<prolifedata::CCustomerInfo *>(&object);
	if (customerInfoPtr == nullptr){
		errorMessage = QString(
			"Unable to cast company info to customer info. Error: Invalid object");
		SendErrorMessage(0, errorMessage, "CCustomerCollectionControllerComp");

		return false;
	}

	customerInfoPtr->ResetData();

	if (!FillObjectFromRepresentation(accountData, object, objectId, errorMessage)){
		errorMessage = QString("Unable to update customer. Error: '%1'").arg(errorMessage);
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

	imtauth::CAddress address;

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


