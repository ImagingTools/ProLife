#include <prolifegql/CDeviceChangeGeneratorComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtbase/COperationDescription.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/ILicenseDefinition.h>

// ProLife includes
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/TOrderedWrap.h>
#include <prolifedata/IOrderInfo.h>


namespace prolifegql
{


// protected methods


bool CDeviceChangeGeneratorComp::GenerateDocumentChanges(
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
			iprm::TParamsPtr<iprm::ITextParam> addedIdsParamPtr(paramsPtr, "AddedProductIds");
			if (addedIdsParamPtr.IsValid()){
				QString ids = addedIdsParamPtr->GetText();
				if (!ids.isEmpty()){
					QStringList idList = ids.split(';');

					for (const QString& id : idList){
						QString licenseName = GetLicenseName(id.toUtf8());

						documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(operationType, "License", licenseName, "", id.toUtf8()));
					}
				}
			}

			imtbase::IOperationDescription::OperationType removedOperation = (imtbase::IOperationDescription::OperationType) (imtbase::IOperationDescription::OperationType::OT_USER + 2);
			iprm::TParamsPtr<iprm::ITextParam> removedIdsParamPtr(paramsPtr, "RemovedProductIds");
			if (removedIdsParamPtr.IsValid()){
				QString ids = removedIdsParamPtr->GetText();
				if (!ids.isEmpty()){
					QStringList idList = ids.split(';');

					for (const QString& id : idList){
						QString licenseName = GetLicenseName(id.toUtf8());

						documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(removedOperation, "License", licenseName, id.toUtf8(), ""));
					}
				}
			}

			retVal = true;
		}
		else if (operationType == imtbase::IOperationDescription::OperationType::OT_USER + 1){
			documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(operationType, "Hardware", "", "", ""));
		}
	}

	return retVal;
}


bool CDeviceChangeGeneratorComp::CompareDocuments(const istd::IChangeable& oldDocument, const istd::IChangeable& newDocument, imtbase::CObjectCollection& documentChangeCollection, QString& errorMessage)
{
	const prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* oldDeviceInfoPtr = dynamic_cast<const prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(&oldDocument);
	if (oldDeviceInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. Old document is invalid");

		return false;
	}

	const prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* newDeviceInfoPtr = dynamic_cast<const prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(&newDocument);
	if (newDeviceInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. New document is invalid");

		return false;
	}

	QByteArray oldProject = oldDeviceInfoPtr->GetProject();
	QByteArray newProject = newDeviceInfoPtr->GetProject();
	if (oldProject != newProject){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Project");
		documentChangeCollection.InsertNewObject(
			"OperationInfo",
			"",
			"",
			CreateOperationDescription(imtbase::IOperationDescription::OperationType::OT_UNKNOWN, "Project", keyName, oldProject, newProject), "Project");
	}

	QByteArray oldSerialNumber = oldDeviceInfoPtr->GetSerialNumber();
	QByteArray newSerialNumber = newDeviceInfoPtr->GetSerialNumber();
	if (oldSerialNumber != newSerialNumber){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Serial Number");
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(imtbase::IOperationDescription::OperationType::OT_UNKNOWN, "SerialNumber", keyName, oldSerialNumber, newSerialNumber), "SerialNumber");
	}

	QByteArray oldMacAddress = oldDeviceInfoPtr->GetMacAddress();
	QByteArray newMacAddress = newDeviceInfoPtr->GetMacAddress();
	if (oldMacAddress != newMacAddress){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "MAC-Address");

		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(imtbase::IOperationDescription::OperationType::OT_UNKNOWN, "MacAddress", keyName, oldMacAddress, newMacAddress), "MacAddress");
	}

	QByteArray oldDeviceType = oldDeviceInfoPtr->GetDeviceType();
	QByteArray newDeviceType = newDeviceInfoPtr->GetDeviceType();
	if (oldDeviceType != newDeviceType){
		if (m_productCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr oldDataPtr;
			if (m_productCollectionCompPtr->GetObjectData(oldDeviceType, oldDataPtr)){
				const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(oldDataPtr.GetPtr());
				if (productInfoPtr != nullptr){
					oldDeviceType = productInfoPtr->GetName().toUtf8();
				}
			}

			imtbase::IObjectCollection::DataPtr newDataPtr;
			if (m_productCollectionCompPtr->GetObjectData(newDeviceType, newDataPtr)){
				const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(newDataPtr.GetPtr());
				if (productInfoPtr != nullptr){
					newDeviceType = productInfoPtr->GetName().toUtf8();
				}
			}
		}

		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Device Type");

		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(imtbase::IOperationDescription::OperationType::OT_UNKNOWN, "DeviceType", keyName, oldDeviceType, newDeviceType), "DeviceType");
	}

	QByteArray oldConfigurationType = oldDeviceInfoPtr->GetConfigurationType();
	QByteArray newConfigurationType = newDeviceInfoPtr->GetConfigurationType();
	if (oldConfigurationType != newConfigurationType){
		if (m_licenseCollectionCompPtr.IsValid()){
			imtbase::IObjectCollection::DataPtr oldDataPtr;
			if (m_licenseCollectionCompPtr->GetObjectData(oldConfigurationType, oldDataPtr)){
				const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(oldDataPtr.GetPtr());
				if (licenseInfoPtr != nullptr){
					oldConfigurationType = licenseInfoPtr->GetLicenseName().toUtf8();
				}
			}

			imtbase::IObjectCollection::DataPtr newDataPtr;
			if (m_licenseCollectionCompPtr->GetObjectData(newConfigurationType, newDataPtr)){
				const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(newDataPtr.GetPtr());
				if (licenseInfoPtr != nullptr){
					newConfigurationType = licenseInfoPtr->GetLicenseName().toUtf8();
				}
			}
		}

		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Hardware Configuration");

		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(imtbase::IOperationDescription::OperationType::OT_UNKNOWN, "ConfigurationType", keyName, oldConfigurationType, newConfigurationType), "ConfigurationType");
	}

	QByteArray oldOrderId = oldDeviceInfoPtr->GetOrderId();
	QByteArray newOrderId = newDeviceInfoPtr->GetOrderId();
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

		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(imtbase::IOperationDescription::OperationType::OT_UNKNOWN, "OrderId", keyName, oldOrderId, newOrderId), "OrderId");
	}

	QString oldDescription = oldDeviceInfoPtr->GetDescription();
	QString newDescription = newDeviceInfoPtr->GetDescription();
	if (oldDescription != newDescription){
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(imtbase::IOperationDescription::OperationType::OT_UNKNOWN, "Description", QT_TRANSLATE_NOOP("Attribute","Description"), oldDescription.toUtf8(), newDescription.toUtf8()), "Description");
	}

	prolifedata::IDeviceInfo::DeviceProductionStatus oldStatus = oldDeviceInfoPtr->GetDeviceProductionStatus();
	prolifedata::IDeviceInfo::DeviceProductionStatus newStatus = newDeviceInfoPtr->GetDeviceProductionStatus();
	if (oldStatus != newStatus){
		QStringList statuses = oldDeviceInfoPtr->DeviceProductionStatusGetStrings();
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription(imtbase::IOperationDescription::OperationType::OT_UNKNOWN, "ProductionStatus", QT_TRANSLATE_NOOP("Attribute","Production Status"), statuses[oldStatus].toUtf8(), statuses[newStatus].toUtf8()), "ProductionStatus");
	}

	return true;
}


QString CDeviceChangeGeneratorComp::GetOperationDescription(imtbase::CObjectCollection& documentChangeCollection, const QByteArray& languageId)
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
									QString(QT_TR_NOOP("Added the license '%1'")).toUtf8(),
									languageId,
									"prolifegql::CDeviceChangeGeneratorComp");

						change = change.arg(keyName);

						retVal += change + "\n";
					}
					else if (type == imtbase::IOperationDescription::OperationType::OT_USER + 2){
						QString change = imtbase::GetTranslation(
									m_translationManagerCompPtr.GetPtr(),
									QString(QT_TR_NOOP("Removed the license '%1'")).toUtf8(),
									languageId,
									"prolifegql::CDeviceChangeGeneratorComp");

						change = change.arg(keyName);

						retVal += change + "\n";
					}
					else if (type == imtbase::IOperationDescription::OperationType::OT_USER + 1){
						QString change = imtbase::GetTranslation(
									m_translationManagerCompPtr.GetPtr(),
									QString(QT_TR_NOOP("Created the license file")).toUtf8(),
									languageId,
									"prolifegql::CDeviceChangeGeneratorComp");
						retVal += change + "\n";
					}
				}
			}
		}
	}

	return retVal;
}


QString CDeviceChangeGeneratorComp::GetLicenseName(const QByteArray& productUuid) const
{
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_softwareInstanceCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
		const imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
		if (productInstanceInfoPtr != nullptr){
			imtbase::ICollectionInfo::Ids licenseCollectionIds = productInstanceInfoPtr->GetLicenseInstances().GetElementIds();
			if (!licenseCollectionIds.isEmpty()){
				QByteArray licenseDefinitionUuid = licenseCollectionIds[0];

				imtbase::IObjectCollection::DataPtr licenseDataPtr;
				if (m_licenseCollectionCompPtr->GetObjectData(licenseDefinitionUuid, licenseDataPtr)){
					const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
					if (licenseInfoPtr != nullptr){
						QByteArray licenseId = licenseInfoPtr->GetLicenseId();
						QString licenseName = licenseInfoPtr->GetLicenseName();

						QString name = licenseName + " (" + licenseId + ")";
						return name;
					}
				}
			}
		}
	}

	return QString();
}


} // namespace prolifegql


