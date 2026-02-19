// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#include <prolifegql/CCustomerChangeGeneratorComp.h>


// ACF includes
#include <iqt/iqt.h>

// ImtCore includes
#include <imtauth/IUserGroupInfo.h>

// ProLife includes
#include <prolifedata/CCustomerInfo.h>


namespace prolifegql
{


// protected methods

bool CCustomerChangeGeneratorComp::CompareDocuments(
	const istd::IChangeable& oldDocument,
	const istd::IChangeable& newDocument,
	imtbase::CObjectCollection& documentChangeCollection,
	QString& errorMessage)
{
	prolifedata::CCustomerInfo* oldCustomerInfoPtr = dynamic_cast<prolifedata::CCustomerInfo*>(const_cast<istd::IChangeable*>(&oldDocument));
	if (oldCustomerInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. Old document is invalid");
		return false;
	}

	prolifedata::CCustomerInfo* newCustomerInfoPtr = dynamic_cast<prolifedata::CCustomerInfo*>(const_cast<istd::IChangeable*>(&newDocument));
	if (newCustomerInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. New document is invalid");
		return false;
	}

	QByteArray oldCustomerId = oldCustomerInfoPtr->GetCustomerId();
	QByteArray newCustomerId = newCustomerInfoPtr->GetCustomerId();
	if (oldCustomerId != newCustomerId){
		InsertOperationDescription(documentChangeCollection, "", "CustomerId", QT_TRANSLATE_NOOP("Attribute", "Customer-ID"), oldCustomerId, newCustomerId);
	}

	QString oldName = oldCustomerInfoPtr->GetName();
	QString newName = newCustomerInfoPtr->GetName();
	if (oldName != newName){
		InsertOperationDescription(documentChangeCollection, "", "Name", QT_TRANSLATE_NOOP("Attribute", "Customer Name"), oldName.toUtf8(), newName.toUtf8());
	}

	QString oldDescription = oldCustomerInfoPtr->GetDescription();
	QString newDescription = newCustomerInfoPtr->GetDescription();
	if (oldDescription != newDescription){
		InsertOperationDescription(documentChangeCollection, "", "Description", QT_TRANSLATE_NOOP("Attribute", "Description"), oldDescription.toUtf8(), newDescription.toUtf8());
	}

	QString oldEmail = oldCustomerInfoPtr->GetEmail();
	QString newEmail = newCustomerInfoPtr->GetEmail();
	if (oldEmail != newEmail){
		InsertOperationDescription(documentChangeCollection, "", "Email", QT_TRANSLATE_NOOP("Attribute", "Email"), oldEmail.toUtf8(), newEmail.toUtf8());
	}

	QByteArrayList oldGroups = oldCustomerInfoPtr->GetGroups();
	QByteArrayList newGroups = newCustomerInfoPtr->GetGroups();

	QByteArrayList addedGroups;
	QByteArrayList removedGroups;

	GenerateChanges(oldGroups, newGroups, addedGroups, removedGroups);

	for (const QByteArray& groupId : addedGroups){
		QString name = GetGroupName(groupId);
		InsertOperationDescription(documentChangeCollection, "AddGroup", "GroupId", name, groupId, groupId);
	}

	for (const QByteArray& groupId : removedGroups){
		QString name = GetGroupName(groupId);
		InsertOperationDescription(documentChangeCollection, "RemoveGroup", "GroupId", name, groupId, groupId);
	}

	const imtauth::IAddress* oldAddressPtr = nullptr;
	const imtauth::IAddressProvider* oldAddressProviderPtr = oldCustomerInfoPtr->GetAddresses();
	if (oldAddressProviderPtr != nullptr){
		imtbase::ICollectionInfo::Ids addressesIds = oldAddressProviderPtr->GetAddressList().GetElementIds();
		if (!addressesIds.isEmpty()){
			oldAddressPtr = oldAddressProviderPtr->GetAddress(addressesIds[0]);
		}
	}

	const imtauth::IAddress* newAddressPtr = nullptr;
	const imtauth::IAddressProvider* newAddressProviderPtr = newCustomerInfoPtr->GetAddresses();
	if (newAddressProviderPtr != nullptr){
		imtbase::ICollectionInfo::Ids addressesIds = newAddressProviderPtr->GetAddressList().GetElementIds();
		if (!addressesIds.isEmpty()){
			newAddressPtr = newAddressProviderPtr->GetAddress(addressesIds[0]);
		}
	}

	if (oldAddressPtr != nullptr && newAddressPtr != nullptr){
		QString oldCity = oldAddressPtr->GetCity();
		QString newCity = newAddressPtr->GetCity();
		if (oldCity != newCity){
			InsertOperationDescription(documentChangeCollection, "", "City", QT_TRANSLATE_NOOP("Attribute", "City"), oldCity.toUtf8(), newCity.toUtf8());
		}

		QString oldCountry = oldAddressPtr->GetCountry();
		QString newCountry = newAddressPtr->GetCountry();
		if (oldCountry != newCountry){
			InsertOperationDescription(documentChangeCollection, "", "Country", QT_TRANSLATE_NOOP("Attribute", "Country"), oldCountry.toUtf8(), newCountry.toUtf8());
		}

		QString oldStreet = oldAddressPtr->GetStreet();
		QString newStreet = newAddressPtr->GetStreet();
		if (oldStreet != newStreet){
			InsertOperationDescription(documentChangeCollection, "", "Street", QT_TRANSLATE_NOOP("Attribute", "Street"), oldStreet.toUtf8(), newStreet.toUtf8());
		}

		int oldPostalCode = oldAddressPtr->GetPostalCode();
		int newPostalCode = newAddressPtr->GetPostalCode();
		if (oldPostalCode != newPostalCode){
			InsertOperationDescription(documentChangeCollection, "", "PostalCode", QT_TRANSLATE_NOOP("Attribute", "PostalCode"), QString::number(oldPostalCode).toUtf8(), QString::number(newPostalCode).toUtf8());
		}
	}

	return true;
}


QString CCustomerChangeGeneratorComp::CreateCustomOperationDescription(const imtbase::COperationDescription& operationDescription, const QByteArray& languageId) const
{
	QString retVal;

	QByteArray typeId = operationDescription.GetOperationTypeId();
	QByteArray newValue = operationDescription.GetNewValue();

	if (typeId == "AddGroup"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Added group '%1'"), languageId, "prolifegql::CCustomerChangeGeneratorComp");
		change = change.arg(GetGroupName(newValue));
		retVal += change;
	}
	else if (typeId == "RemoveGroup"){
		QString change = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), QT_TR_NOOP("Removed group '%1'"), languageId, "prolifegql::CCustomerChangeGeneratorComp");
		change = change.arg(GetGroupName(newValue));
		retVal += change;
	}

	return retVal;
}


// private methods

QString CCustomerChangeGeneratorComp::GetGroupName(const QByteArray& groupId) const
{
	if (!IsUuid(groupId)){
		return groupId;
	}

	if (m_groupCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_groupCollectionCompPtr->GetObjectData(groupId, dataPtr)){
			const imtauth::IUserGroupInfo* groupInfoPtr = dynamic_cast<const imtauth::IUserGroupInfo*>(dataPtr.GetPtr());
			if (groupInfoPtr != nullptr){
				return groupInfoPtr->GetName();
			}
		}
	}

	return QString();
}


} // namespace prolifegql


