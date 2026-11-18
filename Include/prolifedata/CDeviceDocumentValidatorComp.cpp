#include <prolifedata/CDeviceDocumentValidatorComp.h>


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtdoc/imtdoc.h>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/CDeviceInfo.h>
#include <prolifedata/IHardwareConfigurationInfo.h>


namespace prolifedata
{


// reimplemented (imtdoc::IDocumentValidator)

bool CDeviceDocumentValidatorComp::ValidateDocumentData(
			const istd::IChangeable& document,
			QString& errorMessage,
			const iprm::IParamsSet* paramsPtr,
			const imtbase::IOperationContext* /*operationContextPtr*/) const
{
	// Empty for a document never saved: the device itself is not taken for a duplicate
	const QByteArray objectId = imtdoc::GetObjectIdFromParams(paramsPtr);

	const prolifedata::COrderedIdentifiableDeviceInfo* deviceInfoPtr =
		dynamic_cast<const prolifedata::COrderedIdentifiableDeviceInfo*>(&document);
	if (deviceInfoPtr == nullptr){
		errorMessage = QString("Unable to validate document. Error: Object is invalid");
		return false;
	}

	QByteArray macAddress = deviceInfoPtr->GetMacAddress();
	if (!macAddress.isEmpty()){
		bool ok = prolifedata::CheckDeviceMacAddressExists(objectId, macAddress, *m_objectCollectionCompPtr);
		if (!ok){
			errorMessage = QString("MAC-Address '%1' already exists").arg(QString::fromUtf8(macAddress));
			return false;
		}
	}

	QByteArray serialNumber = deviceInfoPtr->GetSerialNumber();
	if (!serialNumber.isEmpty()){
		bool ok = prolifedata::CheckDeviceSerialNumberExists(objectId, serialNumber, *m_objectCollectionCompPtr);
		if (!ok){
			errorMessage = QString("Serial Number '%1' already exists").arg(QString::fromUtf8(serialNumber));
			return false;
		}
	}

	QByteArray configurationType = deviceInfoPtr->GetConfigurationType();
	if (configurationType.isEmpty()){
		errorMessage = QString("Configuration cannot be empty");
		return false;
	}

	QByteArray deviceType = deviceInfoPtr->GetDeviceType();
	if (deviceType.isEmpty()){
		errorMessage = QString("Device type cannot be empty");
		return false;
	}

	if (!m_configurationCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to check the configuration of the device");
		return false;
	}

	imtbase::IObjectCollection::DataPtr configurationDataPtr;
	const IHardwareConfigurationInfo* configurationInfoPtr = nullptr;
	if (m_configurationCollectionCompPtr->GetObjectData(configurationType, configurationDataPtr)){
		configurationInfoPtr = dynamic_cast<const IHardwareConfigurationInfo*>(configurationDataPtr.GetPtr());
	}

	if (configurationInfoPtr == nullptr){
		errorMessage = QString("The configuration '%1' cannot be read").arg(QString::fromUtf8(configurationType));
		return false;
	}

	if (configurationInfoPtr->GetProductId() != deviceType){
		errorMessage = QString("The configuration '%1' does not belong to the device type").arg(configurationInfoPtr->GetName());
		return false;
	}

	return true;
}


} // namespace prolifedata


