#include <prolifegql/CDeviceChangeGeneratorComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <iqt/iqt.h>
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
			const QByteArray& operationTypeId,
			const QByteArray& documentId,
			const istd::IChangeable& document,
			imtbase::CObjectCollection& documentChangeCollection,
			QString& errorMessage,
			const iprm::IParamsSet* paramsPtr)
{
	if (operationTypeId == "BindLicense"){
		iprm::TParamsPtr<iprm::ITextParam> addedIdsParamPtr(paramsPtr, "AddedProductIds");
		if (addedIdsParamPtr.IsValid()){
			QString ids = addedIdsParamPtr->GetText();
			if (!ids.isEmpty()){
				QStringList idList = ids.split(';');

				for (const QString& id : idList){
					InsertOperationDescription(documentChangeCollection, "BindLicense", "License", id.toUtf8(), id.toUtf8(), id.toUtf8());
				}
			}
		}

		iprm::TParamsPtr<iprm::ITextParam> removedIdsParamPtr(paramsPtr, "RemovedProductIds");
		if (removedIdsParamPtr.IsValid()){
			QString ids = removedIdsParamPtr->GetText();
			if (!ids.isEmpty()){
				QStringList idList = ids.split(';');

				for (const QString& id : idList){
					InsertOperationDescription(documentChangeCollection, "UnbindLicense", "License", id.toUtf8(), id.toUtf8(), id.toUtf8());
				}
			}
		}
	}
	else{
		return BaseClass::GenerateDocumentChanges(operationTypeId, documentId, document, documentChangeCollection, errorMessage, paramsPtr);
	}

	return true;
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
		InsertOperationDescription(documentChangeCollection, "", "Project", keyName, oldProject, newProject);
	}

	QByteArray oldSerialNumber = oldDeviceInfoPtr->GetSerialNumber();
	QByteArray newSerialNumber = newDeviceInfoPtr->GetSerialNumber();
	if (oldSerialNumber != newSerialNumber){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Serial Number");
		InsertOperationDescription(documentChangeCollection, "", "SerialNumber", keyName, oldSerialNumber, newSerialNumber);
	}

	QByteArray oldMacAddress = oldDeviceInfoPtr->GetMacAddress();
	QByteArray newMacAddress = newDeviceInfoPtr->GetMacAddress();
	if (oldMacAddress != newMacAddress){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "MAC-Address");
		InsertOperationDescription(documentChangeCollection, "", "MacAddress", keyName, oldMacAddress, newMacAddress);
	}

	QByteArray oldDeviceType = oldDeviceInfoPtr->GetDeviceType();
	QByteArray newDeviceType = newDeviceInfoPtr->GetDeviceType();
	if (oldDeviceType != newDeviceType){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Device Type");
		InsertOperationDescription(documentChangeCollection, "", "DeviceType", keyName, oldDeviceType, newDeviceType);
	}

	QByteArray oldConfigurationType = oldDeviceInfoPtr->GetConfigurationType();
	QByteArray newConfigurationType = newDeviceInfoPtr->GetConfigurationType();
	if (oldConfigurationType != newConfigurationType){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Hardware Configuration");
		InsertOperationDescription(documentChangeCollection, "", "ConfigurationType", keyName, oldConfigurationType, newConfigurationType);
	}

	QByteArray oldOrderId = oldDeviceInfoPtr->GetOrderId();
	QByteArray newOrderId = newDeviceInfoPtr->GetOrderId();
	if (oldOrderId != newOrderId){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Order-ID");
		InsertOperationDescription(documentChangeCollection, "", "OrderId", keyName, oldOrderId, newOrderId);
	}

	QString oldDescription = oldDeviceInfoPtr->GetDescription();
	QString newDescription = newDeviceInfoPtr->GetDescription();
	if (oldDescription != newDescription){
		InsertOperationDescription(documentChangeCollection, "", "Description", QT_TRANSLATE_NOOP("Attribute","Description"), oldDescription.toUtf8(), newDescription.toUtf8());
	}

	prolifedata::IDeviceInfo::DeviceProductionStatus oldStatus = oldDeviceInfoPtr->GetDeviceProductionStatus();
	prolifedata::IDeviceInfo::DeviceProductionStatus newStatus = newDeviceInfoPtr->GetDeviceProductionStatus();
	if (oldStatus != newStatus){
		QStringList statuses = oldDeviceInfoPtr->DeviceProductionStatusGetStrings();
		InsertOperationDescription(documentChangeCollection, "", "ProductionStatus", QT_TRANSLATE_NOOP("Attribute","Production Status"), statuses[oldStatus].toUtf8(), statuses[newStatus].toUtf8());
	}

	return true;
}



QString CDeviceChangeGeneratorComp::CreateCustomOperationDescription(
	const imtbase::COperationDescription& operationDescription,
	const QByteArray& languageId) const
{
	QString retVal;

	QByteArray typeId = operationDescription.GetOperationTypeId();

	QByteArray key = operationDescription.GetKey();
	QByteArray oldValue = operationDescription.GetOldValue();
	QByteArray newValue = operationDescription.GetNewValue();
	QString keyName = operationDescription.GetKeyName();
	keyName = iqt::GetTranslation(m_translationManagerCompPtr.GetPtr(), keyName.toUtf8(), languageId, "Attribute");

	if (typeId == "CreateLicenseFile"){
		QString change = iqt::GetTranslation(
			m_translationManagerCompPtr.GetPtr(),
			QString(QT_TR_NOOP("Created the license file")).toUtf8(),
			languageId,
			"prolifegql::CDeviceChangeGeneratorComp");


		retVal += change + "\n";
	}
	else if (typeId == "UnbindLicense"){
		QString change = iqt::GetTranslation(
			m_translationManagerCompPtr.GetPtr(),
			QString(QT_TR_NOOP("Removed the license '%1'")).toUtf8(),
			languageId,
			"prolifegql::CDeviceChangeGeneratorComp");

		change = change.arg(GetSoftwareName(oldValue));

		retVal += change + "\n";
	}
	else if (typeId == "BindLicense"){
		QString change = iqt::GetTranslation(
			m_translationManagerCompPtr.GetPtr(),
			QString(QT_TR_NOOP("Added the license '%1'")).toUtf8(),
			languageId,
			"prolifegql::CDeviceChangeGeneratorComp");
		change = change.arg(GetSoftwareName(newValue));

		retVal += change + "\n";
	}

	return retVal;
}


QString CDeviceChangeGeneratorComp::GetKeyNameForOperation(const QByteArray& key, const QByteArray& value) const
{
	if (key == "DeviceType"){
		return GetProductName(value);
	}
	else if (key == "ConfigurationType"){
		return GetLicenseName(value);
	}
	else if (key == "OrderId"){
		return GetOrderName(value);
	}
	else{
		return BaseClass::GetKeyNameForOperation(key, value);
	}
}


QString CDeviceChangeGeneratorComp::GetLicenseName(const QByteArray& productUuid) const
{
	if (m_licenseCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_licenseCollectionCompPtr->GetObjectData(productUuid, dataPtr)){
			const imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<const imtlic::ILicenseDefinition*>(dataPtr.GetPtr());
			if (licenseInfoPtr != nullptr){
				return licenseInfoPtr->GetLicenseId();
			}
		}
	}

	return productUuid;
}


QString CDeviceChangeGeneratorComp::GetSoftwareName(const QByteArray& softwareId) const
{
	if (m_softwareInstanceCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_softwareInstanceCollectionCompPtr->GetObjectData(softwareId, dataPtr)){
			const imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
			if (productInstanceInfoPtr != nullptr){
				QByteArray productId = productInstanceInfoPtr->GetProductId();
				QByteArray serialNumber = productInstanceInfoPtr->GetSerialNumber();
				QString productName = GetProductName(productId);

				if (!productName.isEmpty() && !serialNumber.isEmpty()){
					return productName + " (" + serialNumber + ")";
				}
				else if (!productName.isEmpty() && serialNumber.isEmpty()){
					return productName;
				}

				else if (productName.isEmpty() && !serialNumber.isEmpty()){
					return serialNumber;
				}

				return softwareId;
			}
		}
	}
	return softwareId;
}


QString CDeviceChangeGeneratorComp::GetProductName(const QByteArray& productId) const
{
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


QString CDeviceChangeGeneratorComp::GetOrderName(const QByteArray& orderId) const
{
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


} // namespace prolifegql


