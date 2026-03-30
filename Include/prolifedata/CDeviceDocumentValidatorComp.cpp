// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

#include <prolifedata/CDeviceDocumentValidatorComp.h>


// Qt includes
#include <QtCore/QString>

// ProLife includes
#include <prolifedata/prolifedata.h>
#include <prolifedata/CDeviceInfo.h>


namespace prolifedata
{


// reimplemented (imtdoc::IDocumentValidator)

bool CDeviceDocumentValidatorComp::ValidateDocumentData(const QByteArray& objectId, const istd::IChangeable& document, QString& errorMessage) const
{
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

	return true;
}


} // namespace prolifedata


