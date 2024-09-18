#include <prolifegql/CLicenseChangeGeneratorComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtbase/imtbase.h>
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
			imtbase::IOperationDescription::OperationType operationType,
			const QByteArray& documentId,
			const istd::IChangeable& document,
			imtbase::CObjectCollection& documentChangeCollection,
			QString& errorMessage,
			const iprm::IParamsSet* paramsPtr)
{
	bool retVal = BaseClass::GenerateDocumentChanges(operationType, documentId, document, documentChangeCollection, errorMessage, paramsPtr);

	if (!retVal){
		if (operationType == imtbase::IOperationDescription::OperationType::OT_USER){
			iprm::TParamsPtr<iprm::ITextParam> addedParamPtr(paramsPtr, "AddedHardwareId");
			if (addedParamPtr.IsValid()){
				QString hardwareId = addedParamPtr->GetText();

				QString name = hardwareId;

				if (m_deviceCollectionCompPtr.IsValid()){
					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_deviceCollectionCompPtr->GetObjectData(hardwareId.toUtf8(), dataPtr)){
						const prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::IDeviceInfo*>(dataPtr.GetPtr());
						if (deviceInfoPtr != nullptr){
							name = deviceInfoPtr->GetMacAddress();
						}
					}
				}

				documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(operationType, "Hardware", name, hardwareId.toUtf8(), ""));
			}

			iprm::TParamsPtr<iprm::ITextParam> removedParamPtr(paramsPtr, "RemovedHardwareId");
			if (removedParamPtr.IsValid()){
				QString hardwareId = removedParamPtr->GetText();

				QString name = hardwareId;

				if (m_deviceCollectionCompPtr.IsValid()){
					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_deviceCollectionCompPtr->GetObjectData(hardwareId.toUtf8(), dataPtr)){
						const prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::IDeviceInfo*>(dataPtr.GetPtr());
						if (deviceInfoPtr != nullptr){
							name = deviceInfoPtr->GetMacAddress();
						}
					}
				}

				documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(imtbase::IOperationDescription::OperationType (operationType + 2), "Hardware", name, hardwareId.toUtf8(), ""));
			}

			retVal = true;
		}
		else if (operationType == imtbase::IOperationDescription::OperationType::OT_USER + 1){
			documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(operationType, "Hardware", "", "", ""));
		}
	}

	return retVal;
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
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(imtbase::IOperationDescription::OT_UNKNOWN, "Project", QT_TRANSLATE_NOOP("Attribute", "Project"), oldProject, newProject), "Project");
	}

	QByteArray oldProductId = oldSoftwareInfoPtr->GetProductId();
	QByteArray newProductId = newSoftwareInfoPtr->GetProductId();
	if (oldProductId != newProductId){
		if (m_productCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr oldDataPtr;
			if (m_productCollectionCompPtr->GetObjectData(oldProductId, oldDataPtr)){
				const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(oldDataPtr.GetPtr());
				if (productInfoPtr != nullptr){
					oldProductId = productInfoPtr->GetName().toUtf8();
				}
			}

			imtbase::IObjectCollection::DataPtr newDataPtr;
			if (m_productCollectionCompPtr->GetObjectData(newProductId, newDataPtr)){
				const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(newDataPtr.GetPtr());
				if (productInfoPtr != nullptr){
					newProductId = productInfoPtr->GetName().toUtf8();
				}
			}
		}

		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(imtbase::IOperationDescription::OT_UNKNOWN, "ProductId", QT_TRANSLATE_NOOP("Attribute", "Product-ID"), oldProductId, newProductId), "ProductId");
	}

	QByteArray oldSerialNumber = oldSoftwareInfoPtr->GetSerialNumber();
	QByteArray newSerialNumber = newSoftwareInfoPtr->GetSerialNumber();
	if (oldSerialNumber != newSerialNumber){
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(imtbase::IOperationDescription::OT_UNKNOWN, "SerialNumber", QT_TRANSLATE_NOOP("Attribute", "Software-ID"), oldSerialNumber, newSerialNumber), "SerialNumber");
	}

	QByteArray oldOrderId = oldSoftwareInfoPtr->GetOrderId();
	QByteArray newOrderId = newSoftwareInfoPtr->GetOrderId();
	if (oldOrderId != newOrderId){
		if (m_orderCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr oldDataPtr;
			if (m_orderCollectionCompPtr->GetObjectData(oldOrderId, oldDataPtr)){
				const prolifedata::IOrderInfo* orderInfoPtr = dynamic_cast<const prolifedata::IOrderInfo*>(oldDataPtr.GetPtr());
				if (orderInfoPtr != nullptr){
					oldOrderId = orderInfoPtr->GetOrderId();
				}
			}

			imtbase::IObjectCollection::DataPtr newDataPtr;
			if (m_orderCollectionCompPtr->GetObjectData(newOrderId, newDataPtr)){
				const prolifedata::IOrderInfo* orderInfoPtr = dynamic_cast<const prolifedata::IOrderInfo*>(newDataPtr.GetPtr());
				if (orderInfoPtr != nullptr){
					newOrderId = orderInfoPtr->GetOrderId();
				}
			}
		}

		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Order-ID");
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(imtbase::IOperationDescription::OT_UNKNOWN, "OrderId", keyName, oldOrderId, newOrderId), "OrderId");
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
		if (m_licenseCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr oldDataPtr;
			if (m_licenseCollectionCompPtr->GetObjectData(oldLicenseId, oldDataPtr)){
				const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(oldDataPtr.GetPtr());
				if (licenseInfoPtr != nullptr){
					oldLicenseId = licenseInfoPtr->GetLicenseId();
				}
			}

			imtbase::IObjectCollection::DataPtr newDataPtr;
			if (m_licenseCollectionCompPtr->GetObjectData(newLicenseId, newDataPtr)){
				const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(newDataPtr.GetPtr());
				if (licenseInfoPtr != nullptr){
					newLicenseId = licenseInfoPtr->GetLicenseId();
				}
			}
		}

		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(imtbase::IOperationDescription::OT_UNKNOWN, "LicenseId", QT_TRANSLATE_NOOP("Attribute", "License-ID"), oldLicenseId, newLicenseId), "LicenseId");
	}
	else{
		const imtlic::ILicenseInstance* oldLicenseInstancePtr = oldSoftwareInfoPtr->GetLicenseInstance(oldLicenseId);
		const imtlic::ILicenseInstance* newLicenseInstancePtr = newSoftwareInfoPtr->GetLicenseInstance(newLicenseId);

		if (oldLicenseInstancePtr != nullptr && newLicenseInstancePtr != nullptr){
			QString oldExpiration = oldLicenseInstancePtr->GetExpiration().toString("yyyy-MM-dd");
			QString newExpiration = newLicenseInstancePtr->GetExpiration().toString("yyyy-MM-dd");
			if (oldExpiration != newExpiration){
				documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(imtbase::IOperationDescription::OT_UNKNOWN, "Expiration", QT_TRANSLATE_NOOP("Attribute", "Expiration"), oldExpiration.toUtf8(), newExpiration.toUtf8()), "Expiration");
			}
		}
	}

	return true;
}


QString CLicenseChangeGeneratorComp::GetOperationDescription(
			imtbase::CObjectCollection& documentChangeCollection,
			const QByteArray& languageId)
{
	QString retVal = BaseClass::GetOperationDescription(documentChangeCollection, languageId);

	if (retVal.isEmpty()){
		imtbase::ICollectionInfo::Ids elementIds = documentChangeCollection.GetElementIds();
		for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (documentChangeCollection.GetObjectData(elementId, dataPtr)){
				const imtbase::COperationDescription* operationDescriptionPtr = dynamic_cast<const imtbase::COperationDescription*>(dataPtr.GetPtr());
				if (operationDescriptionPtr != nullptr){
					imtbase::IOperationDescription::OperationType type = operationDescriptionPtr->GetOperationType();
					QString keyName = operationDescriptionPtr->GetKeyName();
					keyName = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), keyName.toUtf8(), languageId, "Attribute");

					if (type == imtbase::IOperationDescription::OperationType::OT_USER){
						QString change = imtbase::GetTranslation(
									m_translationManagerCompPtr.GetPtr(),
									QString(QT_TR_NOOP("License binded to hardware '%1'")).toUtf8(),
									languageId,
									"prolifegql::CLicenseChangeGeneratorComp");

						change = change.arg(keyName);

						retVal += change + "\n";
					}
					else if (type == imtbase::IOperationDescription::OperationType::OT_USER + 2){
						QString change = imtbase::GetTranslation(
									m_translationManagerCompPtr.GetPtr(),
									QString(QT_TR_NOOP("License unbinded from hardware '%1'")).toUtf8(),
									languageId,
									"prolifegql::CLicenseChangeGeneratorComp");

						change = change.arg(keyName);

						retVal += change + "\n";
					}
					else if (type == imtbase::IOperationDescription::OperationType::OT_USER + 1){
						QString change = imtbase::GetTranslation(
									m_translationManagerCompPtr.GetPtr(),
									QString(QT_TR_NOOP("Created the license file")).toUtf8(),
									languageId,
									"prolifegql::CLicenseChangeGeneratorComp");
						retVal += change + "\n";
					}
				}
			}
		}
	}

	return retVal;
}


} // namespace prolifegql


