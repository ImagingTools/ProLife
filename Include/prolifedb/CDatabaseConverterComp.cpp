#include <prolifedb/CDatabaseConverterComp.h>


// Qt includes
#include <QtCore/QDebug>

// ImtCore includes
#include <imtbase/CObjectLink.h>
#include <imtlic/CHardwareInstanceInfo.h>
#include <imtlic/IProductInfo.h>
#include <imtbase/CObjectCollection.h>

// ProLife includes
#include <prolifedata/CHardwareProductBinding.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>
#include <prolifedata/COrderInfo.h>
#include <prolifedata/CDeviceInfo.h>


namespace prolifedb
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CDatabaseConverterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	qDebug() << "ProLife convertation started!";

	if (m_deviceCollectionCompPtr.IsValid() && m_productCollectionCompPtr.IsValid() && m_licenseCollectionCompPtr.IsValid()){
		qDebug() << "Device collection converting ...";

		imtbase::ICollectionInfo::Ids deviceCollectionIds = m_deviceCollectionCompPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& deviceCollectionId: deviceCollectionIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_deviceCollectionCompPtr->GetObjectData(deviceCollectionId, dataPtr)){
				prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>* deviceInfoPtr = dynamic_cast<prolifedata::TOrderedWrap<prolifedata::CIdentifiableDeviceInfo>*>(dataPtr.GetPtr());
				if (deviceInfoPtr != nullptr){
					QByteArray productId = deviceInfoPtr->GetDeviceType();
					QByteArray productUuid = GetProductUuidByProductId(productId);
					if (!productUuid.isEmpty()){
						deviceInfoPtr->SetDeviceType(productUuid);
					}
					else{
						qDebug() << QString("Product-ID: %1 not founded.").arg(qPrintable(productId));
					}

					QByteArray licenseId = deviceInfoPtr->GetConfigurationType();
					QByteArray licenseUuid = GetLicenseUuidByLicenseId(licenseId);
					if (!licenseUuid.isEmpty()){
						deviceInfoPtr->SetConfigurationType(licenseUuid);
					}
					else{
						qDebug() << QString("License-ID: %1 not founded.").arg(qPrintable(licenseId));
					}

					if (m_deviceCollectionCompPtr->SetObjectData(deviceCollectionId, *deviceInfoPtr)){
						qDebug() << QString("Device with ID: %1 successfully changed.").arg(qPrintable(deviceCollectionId));
					}
					else{
						qDebug() << QString("Device with ID: %1 change failed.").arg(qPrintable(deviceCollectionId));
					}
				}
			}
		}

		qDebug() << "Device collection converting finished!";
	}

	if (m_softwareInstanceCollectionCompPtr.IsValid() && m_productCollectionCompPtr.IsValid() && m_licenseCollectionCompPtr.IsValid()){
		qDebug() << "Software Instance collection converting ...";

		imtbase::ICollectionInfo::Ids softwareInstanceCollectionIds = m_softwareInstanceCollectionCompPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& softwareInstanceCollectionId: softwareInstanceCollectionIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_softwareInstanceCollectionCompPtr->GetObjectData(softwareInstanceCollectionId, dataPtr)){
				prolifedata::COrderedIdentifiableSoftwareInstanceInfo* softwareInstanceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(dataPtr.GetPtr());
				if (softwareInstanceInfoPtr != nullptr){
					QByteArray productId = softwareInstanceInfoPtr->GetProductId();

					QByteArray productUuid = GetProductUuidByProductId(productId);
					if (!productUuid.isEmpty()){
						softwareInstanceInfoPtr->SetupProductInstance(productUuid, softwareInstanceInfoPtr->GetProductInstanceId(), softwareInstanceInfoPtr->GetCustomerId());
					}
					else{
						qDebug() << QString("Product-ID: %1 not founded.").arg(qPrintable(productId));
					}

					imtbase::ICollectionInfo::Ids licenseIds = softwareInstanceInfoPtr->GetLicenseInstances().GetElementIds();
					if (!licenseIds.isEmpty()){
						QByteArray licenseId = licenseIds[0];

						imtlic::ILicenseInstance* licenseInstancePtr = const_cast<imtlic::ILicenseInstance*>(softwareInstanceInfoPtr->GetLicenseInstance(licenseId));
						if (licenseInstancePtr != nullptr){
							QDateTime expiration = licenseInstancePtr->GetExpiration();
							QByteArray licenseUuid = GetLicenseUuidByLicenseId(licenseId);
							if (!licenseUuid.isEmpty()){
								licenseInstancePtr->SetLicenseId(licenseUuid);
							}
							else{
								qDebug() << QString("License-ID: %1 not founded.").arg(qPrintable(licenseId));
							}
						}
					}

					if (m_softwareInstanceCollectionCompPtr->SetObjectData(softwareInstanceCollectionId, *softwareInstanceInfoPtr)){
						qDebug() << QString("Software instance with ID: %1 successfully changed.").arg(qPrintable(softwareInstanceCollectionId));
					}
					else{
						qDebug() << QString("Software instance with ID: %1 change failed.").arg(qPrintable(softwareInstanceCollectionId));
					}
				}
			}
		}

		qDebug() << "Software Instance collection converting finished!";
	}

	qDebug() << "ProLife convertation finished!";

	QCoreApplication::exit();
}


void CDatabaseConverterComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


QByteArray CDatabaseConverterComp::GetLicenseUuidByLicenseId(const QByteArray& licenseId) const
{
	if (!m_licenseCollectionCompPtr.IsValid()){
		return QByteArray();
	}

	imtbase::ICollectionInfo::Ids licenseCollectionIds = m_licenseCollectionCompPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& licenseCollectionId: licenseCollectionIds){
		imtbase::IObjectCollection::DataPtr licenseDataPtr;
		if (m_licenseCollectionCompPtr->GetObjectData(licenseCollectionId, licenseDataPtr)){
			imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
			if (licenseInfoPtr != nullptr){
				QByteArray currentLicenseId = licenseInfoPtr->GetLicenseId();
				if (currentLicenseId == licenseId){
					return licenseCollectionId;
				}
			}
		}
	} // for

	return QByteArray();
}


QByteArray CDatabaseConverterComp::GetProductUuidByProductId(const QByteArray& productId) const
{
	if (!m_productCollectionCompPtr.IsValid()){
		return QByteArray();
	}

	imtbase::ICollectionInfo::Ids productCollectionIds = m_productCollectionCompPtr->GetElementIds();
	for (const imtbase::ICollectionInfo::Id& productCollectionId: productCollectionIds){
		imtbase::IObjectCollection::DataPtr productDataPtr;
		if (m_productCollectionCompPtr->GetObjectData(productCollectionId, productDataPtr)){
			const imtlic::IProductInfo* productInfoPtr = dynamic_cast<const imtlic::IProductInfo*>(productDataPtr.GetPtr());
			if (productInfoPtr != nullptr){
				QByteArray currentProductId = productInfoPtr->GetProductId();
				if (currentProductId == productId){
					return productCollectionId;
				}
			}
		}
	} // for

	return QByteArray();
}


} // namespace imtdb


