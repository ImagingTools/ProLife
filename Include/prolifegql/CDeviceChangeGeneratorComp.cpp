#include <prolifegql/CDeviceChangeGeneratorComp.h>


// ImtCore includes
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

bool CDeviceChangeGeneratorComp::CompareDocuments(
			const istd::IChangeable* oldDocumentPtr,
			const istd::IChangeable* newDocumentPtr,
			imtbase::CObjectCollection& documentChangeCollection,
			QString& errorMessage)
{
	const prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* oldDeviceInfoPtr = dynamic_cast<const prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(oldDocumentPtr);
	if (oldDeviceInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. Old document is invalid");

		return false;
	}

	const prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* newDeviceInfoPtr = dynamic_cast<const prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(newDocumentPtr);
	if (newDeviceInfoPtr == nullptr){
		errorMessage = QString("Unable to compare documents. New document is invalid");

		return false;
	}

	QByteArray oldSerialNumber = oldDeviceInfoPtr->GetSerialNumber();
	QByteArray newSerialNumber = newDeviceInfoPtr->GetSerialNumber();
	if (oldSerialNumber != newSerialNumber){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Serial Number");
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("Change", "SerialNumber", keyName, oldSerialNumber, newSerialNumber), "SerialNumber");
	}

	QByteArray oldMacAddress = oldDeviceInfoPtr->GetMacAddress();
	QByteArray newMacAddress = newDeviceInfoPtr->GetMacAddress();
	if (oldMacAddress != newMacAddress){
		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Mac Address");

		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("Change", "MacAddress", keyName, oldMacAddress, newMacAddress), "MacAddress");
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

		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("Change", "DeviceType", keyName, oldDeviceType, newDeviceType), "DeviceType");
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

		QString keyName = QT_TRANSLATE_NOOP("Attribute", "Configuration Type");

		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("Change", "ConfigurationType", keyName, oldConfigurationType, newConfigurationType), "ConfigurationType");
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

		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("Change", "OrderId", keyName, oldOrderId, newOrderId), "OrderId");
	}

	QString oldDescription = oldDeviceInfoPtr->GetDescription();
	QString newDescription = newDeviceInfoPtr->GetDescription();
	if (oldDescription != newDescription){
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("Change", "Description", QT_TRANSLATE_NOOP("Attribute","Description"), oldDescription.toUtf8(), newDescription.toUtf8()), "Description");
	}

	prolifedata::IDeviceInfo::DeviceProductionStatus oldStatus = oldDeviceInfoPtr->GetDeviceProductionStatus();
	prolifedata::IDeviceInfo::DeviceProductionStatus newStatus = newDeviceInfoPtr->GetDeviceProductionStatus();
	if (oldStatus != newStatus){
		QStringList statuses = oldDeviceInfoPtr->DeviceProductionStatusGetStrings();
		documentChangeCollection.InsertNewObject("OperationInfo", "", "", CreateOperationDescription("Change", "ProductionStatus", QT_TRANSLATE_NOOP("Attribute","Production Status"), statuses[oldStatus].toUtf8(), statuses[newStatus].toUtf8()), "ProductionStatus");
	}

	return true;
}


} // namespace prolifegql


