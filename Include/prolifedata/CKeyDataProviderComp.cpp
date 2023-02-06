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
        SendCriticalMessage(0, "objectCollectionCompPtr is not valid", "Server data provider");

		return false;
	}

	QByteArrayList ids = dataId.split('/');

	if (ids.size() != 2){
        SendCriticalMessage(0, "Number of data Id not equal 2", "Server data provider");

		return false;
	}

	QByteArray orderId = ids[0];
	QByteArray productObjectId = ids[1];

	imtbase::IObjectCollection::DataPtr dataPtr;
    if (!m_objectCollectionCompPtr->GetObjectData(orderId, dataPtr)){
        SendCriticalMessage(0, "Don't get data pointer for order id: " + orderId, "Server data provider");

        return false;
    }

    prolifedata::IOrderInfo* orderPtr = dynamic_cast<prolifedata::IOrderInfo*>(dataPtr.GetPtr());
    if (orderPtr == nullptr){
        SendCriticalMessage(0, "OrderInfo pointer is null: " + orderId, "Server data provider");

        return false;
    }

    imtbase::IObjectCollection* productCollectionPtr = orderPtr->GetProducts();
    if (productCollectionPtr == nullptr){
        SendCriticalMessage(0, "Order don't contain prodicts: " + orderId, "Server data provider");

        return false;
    }

    imtbase::IObjectCollection::DataPtr hardwareDataPtr;
    if (!productCollectionPtr->GetObjectData(productObjectId, hardwareDataPtr)){
        SendCriticalMessage(0, "Don't get hardware data pointer for id: " + productObjectId, "Server data provider");

        return false;
    }

    imtlic::CLicensedHardwareInstanceInfo* hardwareInstancePtr = dynamic_cast<imtlic::CLicensedHardwareInstanceInfo*>(hardwareDataPtr.GetPtr());
    if (hardwareInstancePtr == nullptr){
        SendCriticalMessage(0, "Hardware instance error", "Server data provider");

        return false;
    }


    QByteArray softwareInstanceId = hardwareInstancePtr->GetSoftwareId();
    imtbase::IObjectCollection::DataPtr softwareDataPtr;
    if (!productCollectionPtr->GetObjectData(softwareInstanceId, softwareDataPtr)){
        SendCriticalMessage(0, "Don't get data object pointer for id: " + softwareInstanceId, "Server data provider");

        return false;
    }

    imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<imtlic::IProductInstanceInfo*>(softwareDataPtr.GetPtr());
    if (productInstancePtr == nullptr){
        SendCriticalMessage(0, "Software instance error: " + softwareInstanceId, "Server data provider");

        return false;
    }

    QByteArray instanceId = hardwareInstancePtr->GetProductInstanceId();
    QByteArray productId = productInstancePtr->GetProductId();
    QByteArray customerId = orderPtr->GetCustomerId();
    productInstancePtr->SetupProductInstance(productId, instanceId, customerId);

    if (!m_gqlLicenseRequestCompPtr.IsValid()){
        SendCriticalMessage(0, "gqlLicenseRequestCompPtr is not valid", "Server data provider");

        return false;
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

    imtbase::CTreeItemModel* licensesModelPtr = dataModelPtr->GetTreeItemModel("Features");
    if (licensesModelPtr == nullptr){
        SendCriticalMessage(0, "No features in product: " + productId, "Server data provider");

        return false;
    }

    qDebug() << productModelPtr->toJSON();
    productInstancePtr->GetLicenseInstances();
    const imtbase::ICollectionInfo& licenseList = productInstancePtr->GetLicenseInstances();

    imtbase::ICollectionInfo::Ids licenseIds = licenseList.GetElementIds();
    for (const QByteArray& licenseId : licenseIds){
        imtlic::ILicenseInstance* licenseInstancePtr = dynamic_cast<imtlic::ILicenseInstance*>( const_cast<imtlic::ILicenseInstance*>( productInstancePtr->GetLicenseInstance(licenseId)));
        if (licenseInstancePtr == nullptr){
            SendCriticalMessage(0, "License instance error: " + licenseId, "Server data provider");

            return false;
        }

        if (!licensesModelPtr->ContainsKey(licenseId)){
            SendCriticalMessage(0, "The product does not contain a license " + licenseId, "Server data provider");

            return false;
        }

        imtbase::CTreeItemModel* featuresModelPtr = licensesModelPtr->GetTreeItemModel(licenseId);
        if (featuresModelPtr == nullptr){
            SendCriticalMessage(0, "Future model pointer is null " + licenseId, "Server data provider");

            return false;
        }

        imtlic::ILicenseInfo::FeatureInfos featureInfos;
        for (int featureIndex = 0; featureIndex < featuresModelPtr->GetItemsCount(); featureIndex++){
            imtlic::ILicenseInfo::FeatureInfo featureInfo;
            featureInfo.name = featuresModelPtr->GetData("Name", featureIndex).toString();
            featureInfo.id = featuresModelPtr->GetData("Id", featureIndex).toByteArray();
            featureInfos.append(featureInfo);
        }

        licenseInstancePtr->SetFeatureInfos(featureInfos);
        imtbase::CTreeItemModel* licensesItemsModelPtr = dataModelPtr->GetTreeItemModel("Items");
        if (licensesItemsModelPtr == nullptr){
            SendCriticalMessage(0, "Items model pointer is null ", "Server data provider");

            return false;
        }

        for (int itemIndex = 0; itemIndex < licensesItemsModelPtr->GetItemsCount(); itemIndex++){
            if (licensesItemsModelPtr->GetData("Id", itemIndex).toByteArray() == licenseId){
                licenseInstancePtr->SetLicenseName(licensesItemsModelPtr->GetData("Name", itemIndex).toByteArray());
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


