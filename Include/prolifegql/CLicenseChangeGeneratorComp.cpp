#include <prolifegql/CLicenseChangeGeneratorComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <iqt/iqt.h>
#include <imtlic/IProductInfo.h>
#include <imtlic/ILicenseDefinition.h>
#include <imtauth/CCompanyInfo.h>

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
	static const QString hardwareKeyName = QT_TRANSLATE_NOOP("Attribute", "Hardware");
	static const QString hardwareIdKeyName = QT_TRANSLATE_NOOP("Attribute", "Hardware-ID");

	if (operationTypeId == "Bind"){
		const QByteArray addedHardwareId = GetTextParamValue(paramsPtr, "AddedHardwareId").toUtf8();
		if (!addedHardwareId.isEmpty()){
			InsertOperationDescription(documentChangeCollection, "BindToSensor", "Hardware", hardwareKeyName, QByteArray(), addedHardwareId);
		}

		const QByteArray removedHardwareId = GetTextParamValue(paramsPtr, "RemovedHardwareId").toUtf8();
		if (!removedHardwareId.isEmpty()){
			InsertOperationDescription(documentChangeCollection, "UnbindFromSensor", "Hardware", hardwareKeyName, removedHardwareId, QByteArray());
		}
	}
	else if (operationTypeId == "TransferToDevice"){
		const QByteArray toDeviceId = GetTextParamValue(paramsPtr, "ToDeviceId").toUtf8();
		if (!toDeviceId.isEmpty()){
			InsertOperationDescription(documentChangeCollection, operationTypeId, "HardwareId", hardwareIdKeyName, QByteArray(), toDeviceId);
		}
	}
	else{
		return BaseClass::GenerateDocumentChanges(operationTypeId, documentId, documentPtr, documentChangeCollection, errorMessage, paramsPtr);
	}

	// Binding and transfer also write attributes of the instance itself (project, usage state, ...),
	// they belong to the same revision.
	return AppendDocumentComparison(documentId, documentPtr, documentChangeCollection, errorMessage);
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

	InsertChange(documentChangeCollection, "Project", QT_TRANSLATE_NOOP("Attribute", "Project"), oldSoftwareInfoPtr->GetProject(), newSoftwareInfoPtr->GetProject());
	InsertChange(documentChangeCollection, "ProductId", QT_TRANSLATE_NOOP("Attribute", "Product-ID"), oldSoftwareInfoPtr->GetProductId(), newSoftwareInfoPtr->GetProductId());
	InsertChange(documentChangeCollection, "SerialNumber", QT_TRANSLATE_NOOP("Attribute", "Software-ID"), oldSoftwareInfoPtr->GetSerialNumber(), newSoftwareInfoPtr->GetSerialNumber());
	InsertChange(documentChangeCollection, "OrderId", QT_TRANSLATE_NOOP("Attribute", "Order-ID"), oldSoftwareInfoPtr->GetOrderId(), newSoftwareInfoPtr->GetOrderId());
	InsertChange(documentChangeCollection, "CustomerId", QT_TRANSLATE_NOOP("Attribute", "Customer-ID"), oldSoftwareInfoPtr->GetCustomerId(), newSoftwareInfoPtr->GetCustomerId());
	InsertChange(documentChangeCollection, "ProductInstanceId", QT_TRANSLATE_NOOP("Attribute", "Instance-ID"), oldSoftwareInfoPtr->GetProductInstanceId(), newSoftwareInfoPtr->GetProductInstanceId());
	InsertChange(documentChangeCollection, "ParentInstanceId", QT_TRANSLATE_NOOP("Attribute", "Parent Instance-ID"), oldSoftwareInfoPtr->GetParentInstanceId(), newSoftwareInfoPtr->GetParentInstanceId());
	InsertFlagChange(documentChangeCollection, "InUse", QT_TRANSLATE_NOOP("Attribute", "In Use"), oldSoftwareInfoPtr->IsInUse(), newSoftwareInfoPtr->IsInUse());
	InsertFlagChange(documentChangeCollection, "InternalUse", QT_TRANSLATE_NOOP("Attribute", "Internal Use"), oldSoftwareInfoPtr->IsInternalUse(), newSoftwareInfoPtr->IsInternalUse());
	InsertFlagChange(documentChangeCollection, "MultiProduct", QT_TRANSLATE_NOOP("Attribute", "Multi Product"), oldSoftwareInfoPtr->IsMultiProduct(), newSoftwareInfoPtr->IsMultiProduct());
	InsertNumberChange(documentChangeCollection, "ProductCount", QT_TRANSLATE_NOOP("Attribute", "Product Count"), oldSoftwareInfoPtr->GetProductCount(), newSoftwareInfoPtr->GetProductCount());

	CompareLicenseInstances(*oldSoftwareInfoPtr, *newSoftwareInfoPtr, documentChangeCollection);

	return true;
}


void CLicenseChangeGeneratorComp::CompareLicenseInstances(
	const imtlic::ILicenseInstanceProvider& oldProvider,
	const imtlic::ILicenseInstanceProvider& newProvider,
	imtbase::CObjectCollection& documentChangeCollection)
{
	const QByteArrayList oldLicenseIds = oldProvider.GetLicenseInstances().GetElementIds();
	const QByteArrayList newLicenseIds = newProvider.GetLicenseInstances().GetElementIds();

	InsertListChanges(
				documentChangeCollection,
				"AddLicenseInstance",
				"RemoveLicenseInstance",
				"LicenseId",
				QT_TRANSLATE_NOOP("Attribute", "License-ID"),
				oldLicenseIds,
				newLicenseIds);

	// License instances present in both documents are compared attribute by attribute.
	for (const QByteArray& licenseId : newLicenseIds){
		if (!oldLicenseIds.contains(licenseId)){
			continue;
		}

		const imtlic::ILicenseInstance* oldLicenseInstancePtr = oldProvider.GetLicenseInstance(licenseId);
		const imtlic::ILicenseInstance* newLicenseInstancePtr = newProvider.GetLicenseInstance(licenseId);
		if (oldLicenseInstancePtr == nullptr || newLicenseInstancePtr == nullptr){
			continue;
		}

		InsertDateTimeChange(
					documentChangeCollection,
					"Expiration",
					QT_TRANSLATE_NOOP("Attribute", "Expiration"),
					oldLicenseInstancePtr->GetExpiration(),
					newLicenseInstancePtr->GetExpiration());
		InsertTextChange(
					documentChangeCollection,
					"LicenseName",
					QT_TRANSLATE_NOOP("Attribute", "License Name"),
					oldLicenseInstancePtr->GetLicenseName(),
					newLicenseInstancePtr->GetLicenseName());
	}
}


QString CLicenseChangeGeneratorComp::CreateCustomOperationDescription(const imtbase::COperationDescription& operationDescription, const QByteArray& languageId) const
{
	static const QByteArray translationContext = QByteArrayLiteral("prolifegql::CLicenseChangeGeneratorComp");

	const QByteArray typeId = operationDescription.GetOperationTypeId();
	const QByteArray oldValue = operationDescription.GetOldValue();
	const QByteArray newValue = operationDescription.GetNewValue();

	if (typeId == "TransferToDevice"){
		return Translate(QT_TR_NOOP("The license has been transferred to the hardware '%1'"), languageId, translationContext).arg(GetHardwareName(newValue).toHtmlEscaped());
	}

	if (typeId == "BindToSensor"){
		return Translate(QT_TR_NOOP("License bound to hardware '%1'"), languageId, translationContext).arg(GetHardwareName(newValue).toHtmlEscaped());
	}

	if (typeId == "UnbindFromSensor"){
		return Translate(QT_TR_NOOP("License unbound from hardware '%1'"), languageId, translationContext).arg(GetHardwareName(oldValue).toHtmlEscaped());
	}

	if (typeId == "CreateLicenseFile"){
		return Translate(QT_TR_NOOP("Created the license file"), languageId, translationContext);
	}

	if (typeId == "AddLicenseInstance"){
		return Translate(QT_TR_NOOP("Activated the license '%1'"), languageId, translationContext).arg(GetLicenseName(newValue).toHtmlEscaped());
	}

	if (typeId == "RemoveLicenseInstance"){
		return Translate(QT_TR_NOOP("Deactivated the license '%1'"), languageId, translationContext).arg(GetLicenseName(oldValue).toHtmlEscaped());
	}

	return QString();
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
	else if (key == "CustomerId"){
		return GetAccountName(value);
	}
	else if (key == "Hardware" || key == "HardwareId"){
		return GetHardwareName(value);
	}
	else if (key == "ParentInstanceId" || key == "ProductInstanceId"){
		return GetSoftwareInstanceName(value);
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


QString CLicenseChangeGeneratorComp::GetAccountName(const QByteArray& accountId) const
{
	if (!IsUuid(accountId) || !m_accountCollectionCompPtr.IsValid()){
		return accountId;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_accountCollectionCompPtr->GetObjectData(accountId, dataPtr)){
		const imtauth::CIdentifiableCompanyInfo* companyInfoPtr = dynamic_cast<const imtauth::CIdentifiableCompanyInfo*>(dataPtr.GetPtr());
		if (companyInfoPtr != nullptr){
			return companyInfoPtr->GetName();
		}
	}

	// Falling back to the raw ID keeps a deleted customer traceable in the history.
	return accountId;
}


QString CLicenseChangeGeneratorComp::GetSoftwareInstanceName(const QByteArray& instanceId) const
{
	if (!IsUuid(instanceId) || !m_objectCollectionCompPtr.IsValid()){
		return instanceId;
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(instanceId, dataPtr)){
		const imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<const imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
		if (productInstanceInfoPtr != nullptr){
			const QByteArray serialNumber = productInstanceInfoPtr->GetSerialNumber();
			if (!serialNumber.isEmpty()){
				return serialNumber;
			}
		}
	}

	return instanceId;
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


