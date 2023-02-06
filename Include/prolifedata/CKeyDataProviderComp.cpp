#include <prolifedata/CKeyDataProviderComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtCore/QTemporaryDir>

// ImtCore includes
#include <imtlic/CProductInstanceInfo.h>
#include <imtlic/CLicensedHardwareInstanceInfo.h>
#include <imtlic/ILicenseInstance.h>

// ProLife includes
#include <prolifedata/IOrderInfo.h>


namespace prolifedata
{


// public methods

// reimplemented (imtbase::IBinaryDataProvider)

bool CKeyDataProviderComp::GetData(QByteArray& data, const QByteArray& dataId) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return false;
	}

	QByteArrayList ids = dataId.split('/');

	if (ids.size() != 2){
		return false;
	}

	QByteArray orderId = ids[0];
	QByteArray productObjectId = ids[1];

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(orderId, dataPtr)){
		prolifedata::IOrderInfo* orderPtr = dynamic_cast<prolifedata::IOrderInfo*>(dataPtr.GetPtr());
		if (orderPtr == nullptr){
			return false;
		}

		imtbase::IObjectCollection* productCollectionPtr = orderPtr->GetProducts();
		if (productCollectionPtr == nullptr){
			return false;
		}

		imtbase::IObjectCollection::DataPtr hardwareDataPtr;
		if (productCollectionPtr->GetObjectData(productObjectId, hardwareDataPtr)){
			imtlic::CLicensedHardwareInstanceInfo* hardwareInstancePtr = dynamic_cast<imtlic::CLicensedHardwareInstanceInfo*>(hardwareDataPtr.GetPtr());


			if (hardwareInstancePtr != nullptr){
				if (m_licensePersistenceCompPtr.IsValid()){
					QByteArray softwareInstanceId = hardwareInstancePtr->GetSoftwareId();
					imtbase::IObjectCollection::DataPtr softwareDataPtr;
					if (productCollectionPtr->GetObjectData(softwareInstanceId, softwareDataPtr)){
						imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
						if (productInstancePtr == nullptr){
							SendCriticalMessage(0, "Software instance error: " + softwareInstanceId, "Server data provider");

							return false;
						}
						QByteArray instanceId = hardwareInstancePtr->GetProductInstanceId();
						QByteArray productId = productInstancePtr->GetProductId();
						QByteArray customerId = orderPtr->GetCustomerId();
						productInstancePtr->SetupProductInstance(productId, instanceId, customerId);

						if (m_gqlLicenseRequestCompPtr.IsValid()){
							imtgql::CGqlRequest gqlRequest(imtgql::CGqlRequest::RT_QUERY, "ProductItem");
							imtgql::CGqlObject queryFields("item");
							queryFields.InsertField("Id");
							gqlRequest.AddField(queryFields);

							imtgql::CGqlObject inputParams("input");
							inputParams.InsertField(QByteArray("Id"), QVariant(productId));
							gqlRequest.AddParam(inputParams);

							QString errorMessage;
							imtbase::CTreeItemModel* productModelPtr = m_gqlLicenseRequestCompPtr->CreateResponse(gqlRequest, errorMessage);
							if (productModelPtr != nullptr){
								imtbase::CTreeItemModel* dataModelPtr = productModelPtr->GetTreeItemModel("data");
								if (dataModelPtr != nullptr){
									imtbase::CTreeItemModel* licensesModelPtr = dataModelPtr->GetTreeItemModel("Features");
									if (licensesModelPtr != nullptr){
										qDebug() << productModelPtr->toJSON();
										productInstancePtr->GetLicenseInstances();
										const imtbase::ICollectionInfo& licenseList = productInstancePtr->GetLicenseInstances();

										imtbase::ICollectionInfo::Ids licenseIds = licenseList.GetElementIds();
										for (const QByteArray& licenseId : licenseIds){
											imtlic::ILicenseInstance* licenseInstancePtr = dynamic_cast<imtlic::ILicenseInstance*>( const_cast<imtlic::ILicenseInstance*>( productInstancePtr->GetLicenseInstance(licenseId)));
											if (licenseInstancePtr != nullptr){
												if (!licensesModelPtr->ContainsKey(licenseId)){
													SendCriticalMessage(0, "The product does not contain a license " + licenseId, "Server data provider");

													return false;
												}
												imtbase::CTreeItemModel* featuresModelPtr = licensesModelPtr->GetTreeItemModel(licenseId);
												if (featuresModelPtr != nullptr){
													imtlic::ILicenseInfo::FeatureInfos featureInfos;
													for (int featureIndex = 0; featureIndex < featuresModelPtr->GetItemsCount(); featureIndex++){
														imtlic::ILicenseInfo::FeatureInfo featureInfo;
														featureInfo.name = featuresModelPtr->GetData("Name", featureIndex).toString();
														featureInfo.id = featuresModelPtr->GetData("Id", featureIndex).toByteArray();
														featureInfos.append(featureInfo);
													}
													licenseInstancePtr->SetFeatureInfos(featureInfos);
												}

												imtbase::CTreeItemModel* licensesItemsModelPtr = dataModelPtr->GetTreeItemModel("Items");
												if (licensesItemsModelPtr != nullptr){
													for (int itemIndex = 0; itemIndex < licensesItemsModelPtr->GetItemsCount(); itemIndex++){
														if (licensesItemsModelPtr->GetData("Id", itemIndex).toByteArray() == licenseId){
															licenseInstancePtr->SetLicenseName(licensesItemsModelPtr->GetData("Name", itemIndex).toByteArray());
														}
													}
												}
											}
										}
									}
									else{
										SendCriticalMessage(0, "No features in product: " + productId, "Server data provider");

										return false;
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

						if (!file.open(QIODevice::ReadOnly )){
							SendErrorMessage(0, "License file could not be opened", "Server data provider");

							return false;
						}

						data = file.readAll();

						file.close();

						SendInfoMessage(0, QString("License file for product %1 successfully created").arg(QString(productId)), "Server data provider");


						return true;
					}
				}
			}
		}
	}

	return false;
}


// reimplemented (imtcrypt::IEncryptionKeysProvider)

QByteArray CKeyDataProviderComp::GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KeyType type) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return QByteArray();
	}

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


} // namespace prolifedata


