#include <prolifedata/CKeyDataProviderComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtCore/QTemporaryDir>

// ImtCore includes
#include <imtlic/CHardwareInstanceInfo.h>
#include <imtlic/ILicenseInstance.h>

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
		SendCriticalMessage(0, "m_bindingCollectionCompPtr is not valid", "Server data provider");

		return false;
	}

	if (!m_deviceCollectionCompPtr.IsValid()){
		SendCriticalMessage(0, "m_deviceCollectionCompPtr is not valid", "Server data provider");

		return false;
	}

	if (!m_softwareProductCollectionCompPtr.IsValid()){
		SendCriticalMessage(0, "m_softwareProductCollectionCompPtr is not valid", "Server data provider");

		return false;
	}

	imtbase::IObjectCollection::DataPtr bindingDataPtr;
	if (!m_bindingCollectionCompPtr->GetObjectData(dataId, bindingDataPtr)){
		return false;
	}

	prolifedata::IHardwareProductBinding* bindingInfoPtr = dynamic_cast<prolifedata::IHardwareProductBinding*>(bindingDataPtr.GetPtr());
	if (bindingInfoPtr == nullptr){
		return false;
	}

	QByteArray hardwareObjectId = bindingInfoPtr->GetHardwareId();
	imtbase::IObjectCollection::DataPtr deviceDataPtr;
	if (!m_deviceCollectionCompPtr->GetObjectData(hardwareObjectId, deviceDataPtr)){
		SendCriticalMessage(0, "Don't get data object pointer for id: " + hardwareObjectId, "Server data provider");

		return false;
	}

	prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
	if (deviceInfoPtr == nullptr){
		SendCriticalMessage(0, "Device instance error: " + hardwareObjectId, "Server data provider");

		return false;
	}

	if (!m_gqlLicenseRequestCompPtr.IsValid()){
		SendCriticalMessage(0, "gqlLicenseRequestCompPtr is not valid", "Server data provider");

		return false;
	}

	QByteArray productId;
	QByteArray customerId;
	QByteArray instanceId = deviceInfoPtr->GetMacAddress();

	QByteArrayList softwareIds = bindingInfoPtr->GetSoftwareIds();

	if (softwareIds.isEmpty()){
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

	imtgql::CGqlRequest gqlRequest(imtgql::CGqlRequest::RT_QUERY, "ProductItem");
	imtgql::CGqlObject queryFields("item");
	queryFields.InsertField("Id");
	gqlRequest.AddField(queryFields);

	imtgql::CGqlObject inputParams("input");
	inputParams.InsertField(QByteArray("Id"), QVariant(productId));
	gqlRequest.AddParam(inputParams);

	QString errorMessage;
	imtbase::CTreeItemModel* productModelPtr = m_gqlLicenseRequestCompPtr->CreateResponse(gqlRequest, errorMessage);
	if (productModelPtr == nullptr){
		SendCriticalMessage(0, "Don't create response: " + errorMessage, "Server data provider");

		return false;
	}

	imtbase::CTreeItemModel* dataModelPtr = productModelPtr->GetTreeItemModel("data");
	if (dataModelPtr == nullptr){
		SendCriticalMessage(0, "No date in product: " + productId, "Server data provider");

		return false;
	}

	imtlic::CProductInstanceInfo* productInstancePtr = new imtlic::CProductInstanceInfo();

	imtbase::CTreeItemModel* licensesModelPtr = dataModelPtr->GetTreeItemModel("Features");
	imtbase::CTreeItemModel* dependenciesModelPtr = dataModelPtr->GetTreeItemModel("Dependencies");

	imtbase::CTreeItemModel* licensesItemsModelPtr = dataModelPtr->GetTreeItemModel("Items");
	if (licensesItemsModelPtr == nullptr){
		return false;
	}

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
						if (licensesModelPtr != nullptr){
							QDateTime expiration = licenseInstancePtr->GetExpiration();
							productInstancePtr->AddLicense(licenseId, expiration);

							imtlic::ILicenseInstance* productLicenseInstancePtr = dynamic_cast<imtlic::ILicenseInstance*>(const_cast<imtlic::ILicenseInstance*>(productInstancePtr->GetLicenseInstance(licenseId)));
							if (productLicenseInstancePtr != nullptr){
								if (licensesItemsModelPtr != nullptr){
									QString name = GetLicenseName(licenseId, *licensesItemsModelPtr);

									productLicenseInstancePtr->SetLicenseName(name);
								}

//								imtlic::ILicenseInfo::FeatureInfos featureInfos;

//								// License together with all dependent licenses
//								QByteArrayList licenses;
//								licenses << licenseId;

//								if (dependenciesModelPtr != nullptr){
//									licenses += GetAllLicenseDependencies(licenseId, *dependenciesModelPtr);
//								}

//								for (const QByteArray& dependencyId : licenses){
//									imtbase::CTreeItemModel* featuresModelPtr = licensesModelPtr->GetTreeItemModel(dependencyId);
//									if (featuresModelPtr != nullptr){
//										for (int featureIndex = 0; featureIndex < featuresModelPtr->GetItemsCount(); featureIndex++){
//											imtlic::ILicenseInfo::FeatureInfo featureInfo;
//											featureInfo.id = featuresModelPtr->GetData("Id", featureIndex).toByteArray();
//											featureInfo.name = featuresModelPtr->GetData("Name", featureIndex).toString();

//											if (!featureInfos.contains(featureInfo)){
//												featureInfos.append(featureInfo);
//											}
//										}
//									}
//								}

//								productLicenseInstancePtr->SetFeatureInfos(featureInfos);
							}
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
		SendErrorMessage(0, "License file could not be saved", "Server data provider");

		return false;
	}

	QFile file(filePathTmp);

	if (!file.open(QIODevice::ReadOnly)){
		SendErrorMessage(0, "License file could not be opened", "Server data provider");

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

	SendInfoMessage(0, QString("License file for product %1 successfully created").arg(QString(productId)), "Server data provider");

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


} // namespace prolifedata


