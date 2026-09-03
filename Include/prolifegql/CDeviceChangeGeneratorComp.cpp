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
			const istd::IChangeable* documentPtr,
			imtbase::CObjectCollection& documentChangeCollection,
			QString& errorMessage,
			const iprm::IParamsSet* paramsPtr)
{
	static const QString licenseKeyName = QT_TRANSLATE_NOOP("Attribute", "License");
	static const QString deviceKeyName = QT_TRANSLATE_NOOP("Attribute", "Device-ID");

	if (operationTypeId == "BindLicense"){
		const QStringList addedIds = GetTextParamValue(paramsPtr, "AddedProductIds").split(';', Qt::SkipEmptyParts);
		for (const QString& id : addedIds){
			InsertOperationDescription(documentChangeCollection, "BindLicense", "License", licenseKeyName, QByteArray(), id.toUtf8());
		}

		const QStringList removedIds = GetTextParamValue(paramsPtr, "RemovedProductIds").split(';', Qt::SkipEmptyParts);
		for (const QString& id : removedIds){
			InsertOperationDescription(documentChangeCollection, "UnbindLicense", "License", licenseKeyName, id.toUtf8(), QByteArray());
		}
	}
	else if (operationTypeId == "TransferFromDevice"){
		const QByteArray fromDeviceId = GetTextParamValue(paramsPtr, "FromDeviceId").toUtf8();
		if (!fromDeviceId.isEmpty()){
			InsertOperationDescription(documentChangeCollection, operationTypeId, "DeviceId", deviceKeyName, fromDeviceId, QByteArray());
		}
	}
	else if (operationTypeId == "TransferToDevice"){
		const QByteArray toDeviceId = GetTextParamValue(paramsPtr, "ToDeviceId").toUtf8();
		if (!toDeviceId.isEmpty()){
			InsertOperationDescription(documentChangeCollection, operationTypeId, "DeviceId", deviceKeyName, QByteArray(), toDeviceId);
		}
	}
	else{
		return BaseClass::GenerateDocumentChanges(operationTypeId, documentId, documentPtr, documentChangeCollection, errorMessage, paramsPtr);
	}

	// Binding and transfer also write attributes of the device itself (project, ...), they belong to the same revision.
	return AppendDocumentComparison(documentId, documentPtr, documentChangeCollection, errorMessage);
}


bool CDeviceChangeGeneratorComp::CompareDocuments(const istd::IChangeable& oldDocument, const istd::IChangeable& newDocument, imtbase::CObjectCollection& documentChangeCollection, QString& errorMessage)
{
	const prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* oldDeviceInfoPtr = dynamic_cast<const prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(&oldDocument);
	const prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* newDeviceInfoPtr = dynamic_cast<const prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(&newDocument);

	// The device collection holds sensors as well as IoT devices, both need their own comparison.
	if (oldDeviceInfoPtr == nullptr || newDeviceInfoPtr == nullptr){
		const prolifedata::IIotDeviceInfo* oldIotDeviceInfoPtr = dynamic_cast<const prolifedata::IIotDeviceInfo*>(&oldDocument);
		const prolifedata::IIotDeviceInfo* newIotDeviceInfoPtr = dynamic_cast<const prolifedata::IIotDeviceInfo*>(&newDocument);
		if (oldIotDeviceInfoPtr != nullptr && newIotDeviceInfoPtr != nullptr){
			return CompareIotDevices(*oldIotDeviceInfoPtr, *newIotDeviceInfoPtr, documentChangeCollection);
		}

		errorMessage = (oldDeviceInfoPtr == nullptr)
					? QString("Unable to compare documents. Old document is invalid")
					: QString("Unable to compare documents. New document is invalid");

		return false;
	}

	InsertChange(documentChangeCollection, "Project", QT_TRANSLATE_NOOP("Attribute", "Project"), oldDeviceInfoPtr->GetProject(), newDeviceInfoPtr->GetProject());
	InsertChange(documentChangeCollection, "SerialNumber", QT_TRANSLATE_NOOP("Attribute", "Serial Number"), oldDeviceInfoPtr->GetSerialNumber(), newDeviceInfoPtr->GetSerialNumber());
	InsertChange(documentChangeCollection, "MacAddress", QT_TRANSLATE_NOOP("Attribute", "MAC-Address"), oldDeviceInfoPtr->GetMacAddress(), newDeviceInfoPtr->GetMacAddress());
	InsertChange(documentChangeCollection, "DeviceType", QT_TRANSLATE_NOOP("Attribute", "Device Type"), oldDeviceInfoPtr->GetDeviceType(), newDeviceInfoPtr->GetDeviceType());
	InsertChange(documentChangeCollection, "ConfigurationType", QT_TRANSLATE_NOOP("Attribute", "Hardware Configuration"), oldDeviceInfoPtr->GetConfigurationType(), newDeviceInfoPtr->GetConfigurationType());
	InsertChange(documentChangeCollection, "OrderId", QT_TRANSLATE_NOOP("Attribute", "Order-ID"), oldDeviceInfoPtr->GetOrderId(), newDeviceInfoPtr->GetOrderId());
	InsertTextChange(documentChangeCollection, "Description", QT_TRANSLATE_NOOP("Attribute", "Description"), oldDeviceInfoPtr->GetDescription(), newDeviceInfoPtr->GetDescription());
	InsertFlagChange(documentChangeCollection, "InternalUse", QT_TRANSLATE_NOOP("Attribute", "Internal Use"), oldDeviceInfoPtr->IsInternalUse(), newDeviceInfoPtr->IsInternalUse());
	InsertEnumChange(
				documentChangeCollection,
				"ProductionStatus",
				QT_TRANSLATE_NOOP("Attribute", "Production Status"),
				oldDeviceInfoPtr->GetDeviceProductionStatus(),
				newDeviceInfoPtr->GetDeviceProductionStatus(),
				oldDeviceInfoPtr->DeviceProductionStatusGetStrings());

	return true;
}


bool CDeviceChangeGeneratorComp::CompareIotDevices(
			const prolifedata::IIotDeviceInfo& oldDeviceInfo,
			const prolifedata::IIotDeviceInfo& newDeviceInfo,
			imtbase::CObjectCollection& documentChangeCollection)
{
	InsertChange(documentChangeCollection, "SerialNumber", QT_TRANSLATE_NOOP("Attribute", "Serial Number"), oldDeviceInfo.GetSerialNumber(), newDeviceInfo.GetSerialNumber());
	InsertChange(documentChangeCollection, "Manufacturer", QT_TRANSLATE_NOOP("Attribute", "Manufacturer"), oldDeviceInfo.GetManufacturer(), newDeviceInfo.GetManufacturer());
	InsertChange(documentChangeCollection, "ModelId", QT_TRANSLATE_NOOP("Attribute", "Model-ID"), oldDeviceInfo.GetModelId(), newDeviceInfo.GetModelId());

	return true;
}


QString CDeviceChangeGeneratorComp::CreateCustomOperationDescription(
			const imtbase::COperationDescription& operationDescription,
			const QByteArray& languageId) const
{
	static const QByteArray translationContext = QByteArrayLiteral("prolifegql::CDeviceChangeGeneratorComp");

	const QByteArray typeId = operationDescription.GetOperationTypeId();
	const QByteArray oldValue = operationDescription.GetOldValue();
	const QByteArray newValue = operationDescription.GetNewValue();

	if (typeId == "TransferFromDevice"){
		return Translate(QT_TR_NOOP("Licenses were transferred from the sensor '%1'"), languageId, translationContext).arg(GetDeviceName(oldValue).toHtmlEscaped());
	}

	if (typeId == "TransferToDevice"){
		return Translate(QT_TR_NOOP("Licenses have been transferred to the sensor '%1'"), languageId, translationContext).arg(GetDeviceName(newValue).toHtmlEscaped());
	}

	if (typeId == "CreateLicenseFile"){
		return Translate(QT_TR_NOOP("Created the license file"), languageId, translationContext);
	}

	if (typeId == "UnbindLicense"){
		return Translate(QT_TR_NOOP("Removed the license '%1'"), languageId, translationContext).arg(GetSoftwareName(oldValue).toHtmlEscaped());
	}

	if (typeId == "BindLicense"){
		return Translate(QT_TR_NOOP("Added the license '%1'"), languageId, translationContext).arg(GetSoftwareName(newValue).toHtmlEscaped());
	}

	return QString();
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
	if (!IsUuid(productUuid)){
		return productUuid;
	}

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
	if (!IsUuid(softwareId)){
		return softwareId;
	}

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


QString CDeviceChangeGeneratorComp::GetOrderName(const QByteArray& orderId) const
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


QString CDeviceChangeGeneratorComp::GetDeviceName(const QByteArray& deviceId) const
{
	if (!IsUuid(deviceId)){
		return deviceId;
	}

	if (m_objectCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_objectCollectionCompPtr->GetObjectData(deviceId, dataPtr)){
			const prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<const prolifedata::IDeviceInfo* >(dataPtr.GetPtr());
			if (deviceInfoPtr != nullptr){
				return deviceInfoPtr->GetMacAddress();
			}
		}
	}

	return deviceId;
}


} // namespace prolifegql


