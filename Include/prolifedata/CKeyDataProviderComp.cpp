#include <prolifedata/CKeyDataProviderComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtCore/QTemporaryDir>

// ACF includes
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtlic/CHardwareInstanceInfo.h>
#include <imtlic/ILicenseInstance.h>
#include <imtlic/CProductInfo.h>
#include <imtlic/CFeatureInfo.h>

// ProLife includes
#include <prolifedata/IOrderInfo.h>
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/IHardwareProductBinding.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>


namespace prolifedata
{


// public methods

// reimplemented (imtbase::IBinaryDataProvider)

bool CKeyDataProviderComp::GetData(QByteArray& data, const QByteArray& dataId) const
{
	if (!m_bindingCollectionCompPtr.IsValid()){
		SendCriticalMessage(0, "m_bindingCollectionCompPtr is not valid", "CKeyDataProviderComp");

		return false;
	}

	if (!m_deviceCollectionCompPtr.IsValid()){
		SendCriticalMessage(0, "m_deviceCollectionCompPtr is not valid", "CKeyDataProviderComp");

		return false;
	}

	if (!m_softwareProductCollectionCompPtr.IsValid()){
		SendCriticalMessage(0, "m_softwareProductCollectionCompPtr is not valid", "CKeyDataProviderComp");

		return false;
	}

	imtbase::IObjectCollection::DataPtr bindingDataPtr;
	if (!m_bindingCollectionCompPtr->GetObjectData(dataId, bindingDataPtr)){
		SendCriticalMessage(0, QString("Unable to get binding object with ID: %1").arg(qPrintable(dataId)), "CKeyDataProviderComp");

		return false;
	}

	prolifedata::IHardwareProductBinding* bindingInfoPtr = dynamic_cast<prolifedata::IHardwareProductBinding*>(bindingDataPtr.GetPtr());
	if (bindingInfoPtr == nullptr){
		SendCriticalMessage(0, QString("Unable to cast an object."), "CKeyDataProviderComp");

		return false;
	}

	QByteArray hardwareObjectId = bindingInfoPtr->GetHardwareId();
	imtbase::IObjectCollection::DataPtr deviceDataPtr;
	if (!m_deviceCollectionCompPtr->GetObjectData(hardwareObjectId, deviceDataPtr)){
		SendCriticalMessage(0, QString("Unable to get device with ID: %1").arg(qPrintable(hardwareObjectId)), "CKeyDataProviderComp");

		return false;
	}

	prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
	if (deviceInfoPtr == nullptr){
		SendCriticalMessage(0, "Device instance error: " + hardwareObjectId, "CKeyDataProviderComp");

		return false;
	}

	if (!m_gqlLicenseRequestCompPtr.IsValid()){
		SendCriticalMessage(0, "Internal error. m_gqlLicenseRequestCompPtr is invalid.", "CKeyDataProviderComp");

		return false;
	}

	QByteArray productId;
	QByteArray customerId;
	QByteArray instanceId = deviceInfoPtr->GetMacAddress();

	QByteArrayList softwareIds = bindingInfoPtr->GetSoftwareIds();

	if (softwareIds.isEmpty()){
		SendErrorMessage(0, "Unable to create license file with empty software products.", "CKeyDataProviderComp");

		return false;
	}

	// Get Product-ID from first software product
	imtbase::IObjectCollection::DataPtr firstSoftwareProductDataPtr;
	if (m_softwareProductCollectionCompPtr->GetObjectData(softwareIds[0], firstSoftwareProductDataPtr)){
		prolifedata::COrderedIdentifiableSoftwareInstanceInfo* orderProductInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(firstSoftwareProductDataPtr.GetPtr());
		if (orderProductInfoPtr != nullptr){
			productId = orderProductInfoPtr->GetProductId();
		}
	}

	imtlic::IProductInfo* productInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr productInfoDataPtr;
	if (m_productCollectionCompPtr->GetObjectData(productId, productInfoDataPtr)){
		productInfoPtr = dynamic_cast<imtlic::IProductInfo*>(productInfoDataPtr.GetPtr());
	}

	if (productInfoPtr == nullptr){
		SendErrorMessage(0, QString("Unable to get a product info with ID: %1").arg(qPrintable(productId)), "CKeyDataProviderComp");

		return false;
	}

	imtlic::CProductInstanceInfo* productInstancePtr = new imtlic::CProductInstanceInfo();

	productInstancePtr->SetupProductInstance(productId, instanceId, customerId);

	for (const QByteArray& softwareId : softwareIds){
		imtbase::IObjectCollection::DataPtr softwareProductDataPtr;
		if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, softwareProductDataPtr)){
			imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(softwareProductDataPtr.GetPtr());
			if (productInstanceInfoPtr != nullptr){
				const imtbase::ICollectionInfo& licenseList = productInstanceInfoPtr->GetLicenseInstances();
				for (const QByteArray& licenseId : licenseList.GetElementIds()){
					imtlic::ILicenseInstance* licenseInstancePtr = dynamic_cast<imtlic::ILicenseInstance*>(const_cast<imtlic::ILicenseInstance*>(productInstanceInfoPtr->GetLicenseInstance(licenseId)));
					if (licenseInstancePtr != nullptr){
						QDateTime expiration = licenseInstancePtr->GetExpiration();
						productInstancePtr->AddLicense(licenseId, expiration);

						imtlic::ILicenseInstance* productLicenseInstancePtr = const_cast<imtlic::ILicenseInstance*>(productInstancePtr->GetLicenseInstance(licenseId));
						if (productLicenseInstancePtr != nullptr){

							// Set name to license
							imtbase::ICollectionInfo::Ids licenseCollectionIds = m_licenseCollectionCompPtr->GetElementIds();
							for (const imtbase::ICollectionInfo::Id& licenseCollectionId: licenseCollectionIds){
								imtbase::IObjectCollection::DataPtr licenseDataPtr;
								if (m_licenseCollectionCompPtr->GetObjectData(licenseCollectionId, licenseDataPtr)){
									imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
									if (licenseInfoPtr != nullptr){
										if (licenseCollectionId == licenseId){
											productLicenseInstancePtr->SetLicenseName(licenseInfoPtr->GetLicenseName());
										}
									}
								}
							}

							imtlic::ILicenseInstance::FeatureInfos featureInfos;

							// License together with all dependent licenses
							QByteArrayList licenses;
							licenses << licenseId;

							licenses += GetAllLicenseDependencies(licenseId);

							for (const QByteArray& dependencyId : licenses){
								const imtlic::CLicenseDefinition* licenseInfoPtr = GetLicenseInfo(dependencyId);
//								if (licenseInfoPtr != nullptr){
//									QByteArrayList features = licenseInfoPtr->GetFeatures();
//									for (const QByteArray& featureUuid : features){
//										imtbase::IObjectCollection::DataPtr featureDataPtr;
//										if (m_featureCollectionCompPtr->GetObjectData(featureUuid, featureDataPtr)){
//											imtlic::IFeatureInfo* featureInfoPtr = dynamic_cast<imtlic::IFeatureInfo*>(featureDataPtr.GetPtr());
//											if (featureInfoPtr != nullptr){
//												imtlic::ILicenseInstance::FeatureInfo featureInfo;

//												featureInfo.id = featureInfoPtr->GetFeatureId();
//												featureInfo.name = featureInfoPtr->GetFeatureName();

//												if (!featureInfos.contains(featureInfo)){
//													featureInfos.append(featureInfo);
//												}
//											}
//										}
//									}
//								}
							}

							productLicenseInstancePtr->SetFeatureInfos(featureInfos);
						}
					}
				}
			}
		}
	}

	m_productInstanceId = instanceId;

	QTemporaryDir tempDir;
	QString filePathTmp = tempDir.path() + "/" + QUuid::createUuid().toString() + ".xml";

	int state = m_licensePersistenceCompPtr->SaveToFile(*productInstancePtr, filePathTmp);
	if (state != ifile::IFilePersistence::OS_OK){
		SendErrorMessage(0, "License file could not be saved", "CKeyDataProviderComp");

		return false;
	}

	QFile file(filePathTmp);

	if (!file.open(QIODevice::ReadOnly)){
		SendErrorMessage(0, "License file could not be opened", "CKeyDataProviderComp");

		return false;
	}

	data = file.readAll();

	file.close();

	for (const QByteArray& softwareId : softwareIds){
		imtbase::IObjectCollection::DataPtr softwareProductDataPtr;
		if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, softwareProductDataPtr)){
			prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productInstanceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(softwareProductDataPtr.GetPtr());
			if (productInstanceInfoPtr != nullptr){
				productInstanceInfoPtr->SetInUse(true);

				if (!m_softwareProductCollectionCompPtr->SetObjectData(softwareId, *productInstanceInfoPtr)){
					return false;
				}
			}
		}
	}

	SendInfoMessage(0, QString("License file for product %1 successfully created").arg(QString(productId)), "CKeyDataProviderComp");

	return true;
}


// reimplemented (imtcrypt::IEncryptionKeysProvider)

QByteArray CKeyDataProviderComp::GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KeyType type) const
{
	QByteArray retVal;

	if (type == KT_PASSWORD){
		return m_productInstanceId;
	}
	else if (type == KT_INIT_VECTOR){
		if (m_vectorKeyCompPtr.IsValid()){
			retVal = m_vectorKeyCompPtr->GetId();
		}
	}

	return retVal;
}


QByteArrayList CKeyDataProviderComp::GetAllLicenseDependencies(const QByteArray& licenseId, const imtbase::CTreeItemModel& dependenciesModel) const
{
	QByteArrayList retVal;

	if (dependenciesModel.ContainsKey(licenseId)){
		QByteArray dependencies = dependenciesModel.GetData(licenseId).toByteArray();
		QByteArrayList dependenciesIDs = dependencies.split(';');

		retVal += dependenciesIDs;

		for (const QByteArray& dependencyId : dependenciesIDs){
			retVal += GetAllLicenseDependencies(dependencyId, dependenciesModel);
		}
	}

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
	QSet<QByteArray> set(retVal.cbegin(), retVal.cend());
	retVal = set.values();
#else
	QSet<QByteArray> set(retVal.toSet());
	retVal = set.values();
#endif
	return retVal;
}


QByteArrayList CKeyDataProviderComp::GetAllLicenseDependencies(const QByteArray& licenseId) const
{
	QByteArrayList retVal;

	if (m_licenseCollectionCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids licenseCollectionIds = m_licenseCollectionCompPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& licenseCollectionId: licenseCollectionIds){
			imtbase::IObjectCollection::DataPtr licenseDataPtr;
			if (m_licenseCollectionCompPtr->GetObjectData(licenseCollectionId, licenseDataPtr)){
				imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
				if (licenseInfoPtr != nullptr){
					if (licenseInfoPtr->GetLicenseId() == licenseId){
						QByteArrayList dependencies = licenseInfoPtr->GetDependencies();

						retVal += dependencies;

						for (const QByteArray& dependencyId : dependencies){
							retVal += GetAllLicenseDependencies(dependencyId);
						}

						break;
					}
				}
			}
		}
	}

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
	QSet<QByteArray> set(retVal.cbegin(), retVal.cend());
	retVal = set.values();
#else
	QSet<QByteArray> set(retVal.toSet());
	retVal = set.values();
#endif

	return retVal;
}


QString CKeyDataProviderComp::GetLicenseName(const QByteArray& licenseId, const imtbase::CTreeItemModel& licensesModel) const
{
	for (int i = 0; i < licensesModel.GetItemsCount(); i++){
		QByteArray currentLicenseId = licensesModel.GetData("Id", i).toByteArray();
		if (currentLicenseId == licenseId){
			QString currentLicenseName = licensesModel.GetData("Name", i).toString();
			return 	currentLicenseName;
		}
	}

	return QString();
}


const imtlic::CLicenseDefinition* CKeyDataProviderComp::GetLicenseInfo(const QByteArray& licenseId) const
{
	if (m_licenseCollectionCompPtr.IsValid()){
		imtbase::ICollectionInfo::Ids licenseCollectionIds = m_licenseCollectionCompPtr->GetElementIds();
		for (const imtbase::ICollectionInfo::Id& licenseCollectionId: licenseCollectionIds){
			imtbase::IObjectCollection::DataPtr licenseDataPtr;
			if (m_licenseCollectionCompPtr->GetObjectData(licenseCollectionId, licenseDataPtr)){
				imtlic::ILicenseDefinition* licenseInfoPtr = dynamic_cast<imtlic::ILicenseDefinition*>(licenseDataPtr.GetPtr());
				if (licenseInfoPtr != nullptr){
					if (licenseCollectionId == licenseId){
						istd::IChangeable* clonedObjectPtr = licenseInfoPtr->CloneMe();
						if (clonedObjectPtr != nullptr){
							return dynamic_cast<imtlic::CLicenseDefinition*>(clonedObjectPtr);
						}

						break;
					}
				}
			}
		}
	}

	return nullptr;
}


} // namespace prolifedata


