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
#include <imtgql/IGqlRequestProvider.h>
#include <imtgql/IGqlRequest.h>

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
	QByteArray productUuid;
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
			productUuid = orderProductInfoPtr->GetProductId();
		}
	}

	QByteArrayList productFields;

	productFields << "Id";
	productFields << "ProductId";
	productFields << "ProductName";

	imtbase::CTreeItemModel* productItemsModelPtr = GetRemoteCollectionData("ProductsList", productFields);
	if (productItemsModelPtr == nullptr){
		SendErrorMessage(0, "Unable to get product list from Lisa.", "CKeyDataProviderComp");

		return false;
	}

	QByteArrayList licenseFields;

	licenseFields << "Id";
	licenseFields << "LicenseId";
	licenseFields << "ProductId";
	licenseFields << "LicenseName";
	licenseFields << "ParentLicenses";
	licenseFields << "Features";

	imtbase::CTreeItemModel* licenseItemsModelPtr = GetRemoteCollectionData("LicensesList", licenseFields);
	if (licenseItemsModelPtr == nullptr){
		SendErrorMessage(0, "Unable to get license list from Lisa.", "CKeyDataProviderComp");

		return false;
	}

	QByteArrayList featureFields;

	featureFields << "Id";
	featureFields << "FeatureId";
	featureFields << "FeatureName";

	imtbase::CTreeItemModel* featureItemsModelPtr = GetRemoteCollectionData("FeaturesList", featureFields);
	if (featureItemsModelPtr == nullptr){
		SendErrorMessage(0, "Unable to get feature list from Lisa.", "CKeyDataProviderComp");

		return false;
	}

	for (int i = 0; i < productItemsModelPtr->GetItemsCount(); i++){
		QByteArray currentProductUuid = productItemsModelPtr->GetData("Id", i).toByteArray();
		if (currentProductUuid == productUuid){
			productId = productItemsModelPtr->GetData("ProductId", i).toByteArray();
			break;
		}
	}

	if (productId.isEmpty()){
		SendErrorMessage(0, "Unable to create license file with empty product-ID", "CKeyDataProviderComp");

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
				for (const QByteArray& licenseCollectionId : licenseList.GetElementIds()){
					imtlic::ILicenseInstance* licenseInstancePtr = dynamic_cast<imtlic::ILicenseInstance*>(const_cast<imtlic::ILicenseInstance*>(productInstanceInfoPtr->GetLicenseInstance(licenseCollectionId)));
					if (licenseInstancePtr != nullptr){
						QByteArray licenseId;
						QString licenseName;

						licenseId = GetLicenseId(licenseCollectionId, *licenseItemsModelPtr);
						licenseName = GetLicenseName(licenseCollectionId, *licenseItemsModelPtr);

						QDateTime expiration = licenseInstancePtr->GetExpiration();
						productInstancePtr->AddLicense(licenseId, expiration);

						imtlic::ILicenseInstance* productLicenseInstancePtr = const_cast<imtlic::ILicenseInstance*>(productInstancePtr->GetLicenseInstance(licenseId));
						if (productLicenseInstancePtr == nullptr){
							SendErrorMessage(0, QString("License instance pointer in invalid."), "CKeyDataProviderComp");

							return false;
						}

						productLicenseInstancePtr->SetLicenseName(licenseName);

						imtlic::ILicenseInstance::FeatureInfos featureInfos;

						// License together with all dependent licenses
						QByteArrayList licenses;
						licenses << licenseCollectionId;

						licenses += GetAllLicenseDependencies(licenseCollectionId, *licenseItemsModelPtr);

						for (const QByteArray& dependencyId : licenses){
							for (int i = 0; i < licenseItemsModelPtr->GetItemsCount(); i++){
								QByteArray licenseUuid = licenseItemsModelPtr->GetData("Id", i).toByteArray();
								if (licenseUuid == dependencyId){
									QByteArray features = licenseItemsModelPtr->GetData("Features", i).toByteArray();
									if (!features.isEmpty()){
										QByteArrayList featureUuids = features.split(';');

										for (const QByteArray& featureUuid : featureUuids){

											QByteArray featureId = GetFeatureId(featureUuid, *featureItemsModelPtr);
											QString featureName = GetFeatureName(featureUuid, *featureItemsModelPtr);

											imtlic::ILicenseInstance::FeatureInfo resultFeatureInfo;

											resultFeatureInfo.id = featureId;
											resultFeatureInfo.name = featureName;

											if (!featureInfos.contains(resultFeatureInfo)){
												featureInfos.append(resultFeatureInfo);
											}
										}
									}
								}
							}
						}

						productLicenseInstancePtr->SetFeatureInfos(featureInfos);
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

	imtgql::CGqlRequest gqlRequest;
	imtgql::IGqlRequestProvider* gqlRequestProviderPtr = QueryInterface<imtgql::IGqlRequestProvider>(dynamic_cast<istd::IPolymorphic*>(const_cast<CKeyDataProviderComp*>(this)));
	if (gqlRequestProviderPtr != nullptr){
		const imtgql::IGqlRequest* gqlRequestPtr = gqlRequestProviderPtr->GetGqlRequest();
		if (gqlRequestPtr != nullptr){
			if (!gqlRequest.CopyFrom(*gqlRequestPtr)){
			}
		}
	}

	for (const QByteArray& softwareId : softwareIds){
		imtbase::IObjectCollection::DataPtr softwareProductDataPtr;
		if (m_softwareProductCollectionCompPtr->GetObjectData(softwareId, softwareProductDataPtr)){
			prolifedata::COrderedIdentifiableSoftwareInstanceInfo* productInstanceInfoPtr = dynamic_cast<prolifedata::COrderedIdentifiableSoftwareInstanceInfo*>(softwareProductDataPtr.GetPtr());
			if (productInstanceInfoPtr != nullptr){
				productInstanceInfoPtr->SetInUse(true);

				imtbase::IOperationContext* operationContextPtr =  nullptr;

				if (m_softwareOperationContextControllerCompPtr.IsValid()){
					operationContextPtr = m_softwareOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_USER + 1, gqlRequest);
				}

				if (!m_softwareProductCollectionCompPtr->SetObjectData(softwareId, *productInstanceInfoPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
					SendWarningMessage(0, "Error when trying update software instance", "CKeyDataProviderComp");
				}
			}
		}
	}

	{
		imtbase::IOperationContext* operationContextPtr =  nullptr;

		if (m_deviceOperationContextControllerCompPtr.IsValid()){
			operationContextPtr = m_deviceOperationContextControllerCompPtr->CreateOperationContext(imtbase::IDocumentChangeGenerator::OT_USER + 1, gqlRequest);
		}

		if (!m_deviceCollectionCompPtr->SetObjectData(hardwareObjectId, *deviceDataPtr, istd::IChangeable::CM_WITHOUT_REFS, operationContextPtr)){
			SendWarningMessage(0, "Error when trying update hardware product", "CKeyDataProviderComp");
		}
	}

	SendInfoMessage(0, QString("License file for product %1 successfully created").arg(QString(productUuid)), "CKeyDataProviderComp");

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


QByteArrayList CKeyDataProviderComp::GetAllLicenseDependencies(const QByteArray& licenseId, const imtbase::CTreeItemModel& licensesModel) const
{
	QByteArrayList retVal;

	for (int i = 0; i < licensesModel.GetItemsCount(); i++){
		QByteArray licenseUuid = licensesModel.GetData("Id", i).toByteArray();
		if (licenseUuid == licenseId){
			QByteArray parentLicenses = licensesModel.GetData("ParentLicenses", i).toByteArray();

			if (!parentLicenses.isEmpty()){
				QByteArrayList dependencies = parentLicenses.split(';');

				retVal += dependencies;

				for (const QByteArray& dependencyId : dependencies){
					retVal += GetAllLicenseDependencies(dependencyId, licensesModel);
				}
			}

			break;
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


QByteArray CKeyDataProviderComp::GetLicenseId(const QByteArray& licenseUuid, const imtbase::CTreeItemModel& licensesModel) const
{
	for (int i = 0; i < licensesModel.GetItemsCount(); i++){
		QByteArray currentLicenseId = licensesModel.GetData("Id", i).toByteArray();
		if (currentLicenseId == licenseUuid){
			QByteArray licenseId = licensesModel.GetData("LicenseId", i).toByteArray();
			return licenseId;
		}
	}

	return QByteArray();
}


QString CKeyDataProviderComp::GetLicenseName(const QByteArray& licenseId, const imtbase::CTreeItemModel& licensesModel) const
{
	for (int i = 0; i < licensesModel.GetItemsCount(); i++){
		QByteArray currentLicenseId = licensesModel.GetData("Id", i).toByteArray();
		if (currentLicenseId == licenseId){
			QString currentLicenseName = licensesModel.GetData("LicenseName", i).toString();
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


QByteArray CKeyDataProviderComp::GetFeatureId(const QByteArray& featureUuid, const imtbase::CTreeItemModel& featuresModel) const
{
	for (int i = 0; i < featuresModel.GetItemsCount(); i++){
		QByteArray uuid = featuresModel.GetData("Id", i).toByteArray();
		if (uuid == featureUuid){
			return featuresModel.GetData("FeatureId", i).toByteArray();
		}
	}

	return QByteArray();
}


QString CKeyDataProviderComp::GetFeatureName(const QByteArray& featureUuid, const imtbase::CTreeItemModel& featuresModel) const
{
	for (int i = 0; i < featuresModel.GetItemsCount(); i++){
		QByteArray uuid = featuresModel.GetData("Id", i).toByteArray();
		if (uuid == featureUuid){
			return featuresModel.GetData("FeatureName", i).toString();
		}
	}

	return QString();
}


imtbase::CTreeItemModel* CKeyDataProviderComp::GetRemoteCollectionData(const QByteArray& collectionCommandId, QByteArrayList fields) const
{
	if (!m_gqlLicenseRequestCompPtr.IsValid()){
		return nullptr;
	}

	imtgql::CGqlRequest request(imtgql::CGqlRequest::RT_QUERY, collectionCommandId);

	imtgql::CGqlObject itemsObject("items");

	for (const QByteArray& fieldId : fields){
		itemsObject.InsertField(fieldId);
	}

	request.AddField(itemsObject);

	QString errorMessage;
	imtbase::CTreeItemModel* modelPtr = m_gqlLicenseRequestCompPtr->CreateResponse(request, errorMessage);
	if (modelPtr == nullptr){
		return nullptr;
	}

	imtbase::CTreeItemModel* dataModelPtr = modelPtr->GetTreeItemModel("data");
	if (dataModelPtr == nullptr){
		return nullptr;
	}

	imtbase::CTreeItemModel* itemsModelPtr = dataModelPtr->GetTreeItemModel("items");
	if (itemsModelPtr == nullptr){
		return nullptr;
	}

	return itemsModelPtr;
}


} // namespace prolifedata


