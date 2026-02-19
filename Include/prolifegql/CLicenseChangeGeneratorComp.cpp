// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#include <prolifegql/CLicenseChangeGeneratorComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <iqt/iqt.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/ILicenseDefinition.h>

// ProLife includes
#include <prolifedata/COrderInfo.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>
#include <prolifedata/IDeviceInfo.h>


namespace prolifegql
{


// protected methods

bool CLicenseChangeGeneratorComp::GenerateDocumentChanges(
	const QByteArray& operationTypeId,
	const QByteArray& documentId,
	const istd::IChangeable* documentPtr,
	imtbase::CObjectCollection& documentChangeCollection,
	QString& errorMessage,
	const iprm::IParamsSet* paramsPtr)
{
	if (operationTypeId == "Bind"){
		iprm::TParamsPtr<iprm::ITextParam> addedParamPtr(paramsPtr, "AddedHardwareId");
		if (addedParamPtr.IsValid()){
			QString hardwareId = addedParamPtr->GetText();
			InsertOperationDescription(documentChangeCollection, "BindToSensor", "Hardware", "Hardware", hardwareId.toUtf8(), hardwareId.toUtf8());
		}

		iprm::TParamsPtr<iprm::ITextParam> removedParamPtr(paramsPtr, "RemovedHardwareId");
		if (removedParamPtr.IsValid()){
			QString hardwareId = removedParamPtr->GetText();
			InsertOperationDescription(documentChangeCollection, "UnbindFromSensor", "Hardware", "Hardware", hardwareId.toUtf8(), hardwareId.toUtf8());
		}
	}
	else if (operationTypeId == "TransferToDevice"){
		iprm::TParamsPtr<iprm::ITextParam> toDeviceParamPtr(paramsPtr, "ToDeviceId");
		if (toDeviceParamPtr.IsValid()){
			QString toDeviceId = toDeviceParamPtr->GetText();
			InsertOperationDescription(documentChangeCollection, operationTypeId, "HardwareId", "Hardware-ID", toDeviceId.toUtf8(), toDeviceId.toUtf8());
		}
	}
	else{
		return BaseClass::GenerateDocumentChanges(operationTypeId, documentId, documentPtr, documentChangeCollection, errorMessage, paramsPtr);
	}

	return true;
}


bool CLicenseChangeGeneratorComp::CompareDocuments(
	const istd::IChangeable& oldDocument,
	const istd::IChangeable& newDocument,
	imtbase::CObjectCollection& documentChangeCollection,
	QString& errorMessage)
{
	const prolifedata::COrderedIdentifiableSoftwareInstanceInfo* oldSoftwareInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(&oldDocument);
	if (oldSoftwareInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. Old document is invalid");

		return false;
	}

	const prolifedata::COrderedIdentifiableSoftwareInstanceInfo* newSoftwareInfoPtr = dynamic_cast<const prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(&newDocument);
	if (newSoftwareInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. New document is invalid");

		return false;
	}

	QByteArray oldProject = oldSoftwareInfoPtr->GetProject();
	QByteArray newProject = newSoftwareInfoPtr->GetProject();
	if (oldProject != newProject){
		InsertOperationDescription(documentChangeCollection, "", "Project", QT_TRANSLATE_NOOP("Attribute", "Project"), oldProject, newProject);
	}

	QByteArray oldProductId = oldSoftwareInfoPtr->GetProductId();
	QByteArray newProductId = newSoftwareInfoPtr->GetProductId();
	if (oldProductId != newProductId){
		InsertOperationDescription(documentChangeCollection, "", "ProductId", QT_TRANSLATE_NOOP("Attribute", "Product-ID"), oldProductId, newProductId);
	}

	QByteArray oldSerialNumber = oldSoftwareInfoPtr->GetSerialNumber();
	QByteArray newSerialNumber = newSoftwareInfoPtr->GetSerialNumber();
	if (oldSerialNumber != newSerialNumber){
		InsertOperationDescription(documentChangeCollection, "", "SerialNumber", QT_TRANSLATE_NOOP("Attribute", "Software-ID"), oldSerialNumber, newSerialNumber);
	}

	QByteArray oldOrderId = oldSoftwareInfoPtr->GetOrderId();
	QByteArray newOrderId = newSoftwareInfoPtr->GetOrderId();
	if (oldOrderId != newOrderId){
		InsertOperationDescription(documentChangeCollection, "", "OrderId", QT_TRANSLATE_NOOP("Attribute", "Order-ID"), oldOrderId, newOrderId);
	}

	QByteArray oldLicenseId;
	QByteArray newLicenseId;

	imtbase::ICollectionInfo::Ids oldCollectionIds = oldSoftwareInfoPtr->GetLicenseInstances().GetElementIds();
	if (!oldCollectionIds.isEmpty()){
		oldLicenseId = oldCollectionIds[0];
	}

	imtbase::ICollectionInfo::Ids newCollectionIds = newSoftwareInfoPtr->GetLicenseInstances().GetElementIds();
	if (!newCollectionIds.isEmpty()){
		newLicenseId = newCollectionIds[0];
	}

	if (oldLicenseId != newLicenseId){
		InsertOperationDescription(documentChangeCollection, "", "LicenseId", QT_TRANSLATE_NOOP("Attribute", "License-ID"), oldLicenseId, newLicenseId);
	}
	else{
		const imtlic::ILicenseInstance* oldLicenseInstancePtr = oldSoftwareInfoPtr->GetLicenseInstance(oldLicenseId);
		const imtlic::ILicenseInstance* newLicenseInstancePtr = newSoftwareInfoPtr->GetLicenseInstance(newLicenseId);

		if (oldLicenseInstancePtr != nullptr && newLicenseInstancePtr != nullptr){
			QString oldExpiration = oldLicenseInstancePtr->GetExpiration().toString("yyyy-MM-dd");
			QString newExpiration = newLicenseInstancePtr->GetExpiration().toString("yyyy-MM-dd");
			if (oldExpiration != newExpiration){
				InsertOperationDescription(documentChangeCollection, "", "Expiration", QT_TRANSLATE_NOOP("Attribute", "Expiration"), oldExpiration.toUtf8(), newExpiration.toUtf8());
			}
		}
	}

	return true;
}


QString CLicenseChangeGeneratorComp::CreateCustomOperationDescription(const imtbase::COperationDescription& operationDescription, const QByteArray& languageId) const
{
	QString retVal;

	QByteArray typeId = operationDescription.GetOperationTypeId();
	QByteArray newValue = operationDescription.GetNewValue();
	QByteArray oldValue = operationDescription.GetOldValue();
	
	if (typeId == "TransferToDevice"){
		QString change = iqt::GetTranslation(
			m_translationManagerCompPtr.GetPtr(),
			QString(QT_TR_NOOP("The license has been transferred to the hardware '%1'")).toUtf8(),
			languageId,
			"prolifegql::CLicenseChangeGeneratorComp");
		
		change = change.arg(GetHardwareName(newValue));
		
		retVal += change;
	}
	else if (typeId == "BindToSensor"){
		QString change = iqt::GetTranslation(
			m_translationManagerCompPtr.GetPtr(),
			QString(QT_TR_NOOP("License binded to hardware '%1'")).toUtf8(),
			languageId,
			"prolifegql::CLicenseChangeGeneratorComp");

		change = change.arg(GetHardwareName(oldValue));

		retVal += change;
	}
	else if (typeId == "UnbindFromSensor"){
		QString change = iqt::GetTranslation(
			m_translationManagerCompPtr.GetPtr(),
			QString(QT_TR_NOOP("License unbinded from hardware '%1'")).toUtf8(),
			languageId,
			"prolifegql::CLicenseChangeGeneratorComp");

		change = change.arg(GetHardwareName(oldValue));

		retVal += change;
	}
	else if (typeId == "CreateLicenseFile"){
		QString change = iqt::GetTranslation(
			m_translationManagerCompPtr.GetPtr(),
			QString(QT_TR_NOOP("Created the license file")).toUtf8(),
			languageId,
			"prolifegql::CLicenseChangeGeneratorComp");
		retVal += change;
	}

	return retVal;
}


QString CLicenseChangeGeneratorComp::GetKeyNameForOperation(const QByteArray& key, const QByteArray& value) const
{
	if (key == "ProductId"){
		return GetProductName(value);
	}
	else if (key == "LicenseId"){
		return GetLicenseName(value);
	}
	else if (key == "OrderId"){
		return GetOrderName(value);
	}

	return BaseClass::GetKeyNameForOperation(key, value);
}


// private methods

QString CLicenseChangeGeneratorComp::GetProductName(const QByteArray& productId) const
{
	if (!IsUuid(productId)){
		return productId;
	}

	if (m_productCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_productCollectionCompPtr->GetObjectData(productId, dataPtr)){
			const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(dataPtr.GetPtr());
			if (productInfoPtr != nullptr){
				return productInfoPtr->GetName();
			}
		}
	}

	return productId;
}


QString CLicenseChangeGeneratorComp::GetOrderName(const QByteArray& orderId) const
{
	if (!IsUuid(orderId)){
		return orderId;
	}

	if (m_orderCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_orderCollectionCompPtr->GetObjectData(orderId, dataPtr)){
			const prolifedata::IOrderInfo* orderInfoPtr = dynamic_cast<const prolifedata::IOrderInfo*>(dataPtr.GetPtr());
			if (orderInfoPtr != nullptr){
				return orderInfoPtr->GetOrderId();
			}
		}
	}

	return orderId;
}


QString CLicenseChangeGeneratorComp::GetLicenseName(const QByteArray& licenseId) const
{
	if (!IsUuid(licenseId)){
		return licenseId;
	}

	if (m_licenseCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_licenseCollectionCompPtr->GetObjectData(licenseId, dataPtr)){
			const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(dataPtr.GetPtr());
			if (licenseInfoPtr != nullptr){
				return licenseInfoPtr->GetLicenseId();
			}
		}
	}

	return licenseId;
}


QString CLicenseChangeGeneratorComp::GetHardwareName(const QByteArray& hardwareId) const
{
	if (!IsUuid(hardwareId)){
		return hardwareId;
	}

	if (m_deviceCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_deviceCollectionCompPtr->GetObjectData(hardwareId, dataPtr)){
			const prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::IDeviceInfo*>(dataPtr.GetPtr());
			if (deviceInfoPtr != nullptr){
				return deviceInfoPtr->GetMacAddress();
			}
		}
	}

	return hardwareId;
}


} // namespace prolifegql


