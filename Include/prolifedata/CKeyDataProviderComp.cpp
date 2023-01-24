#include <prolifedata/CKeyDataProviderComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QUuid>
#include <QtCore/QTemporaryDir>

// ImtCore includes
#include <imtlic/CProductInstanceInfo.h>

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

		m_productInstanceId = orderId;

		imtbase::IObjectCollection::DataPtr productDataPtr;
		if (productCollectionPtr->GetObjectData(productObjectId, productDataPtr)){
			imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
			if (productInstancePtr != nullptr){
				if (m_licensePersistenceCompPtr.IsValid()){
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

					return true;
				}
			}
		}

//		for (const imtbase::IObjectCollection::Id& objectId : productsIds){
//			imtbase::IObjectCollection::DataPtr productDataPtr;
//			if (productCollectionPtr->GetObjectData(objectId, productDataPtr)){
//				imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<imtlic::IProductInstanceInfo*>(productDataPtr.GetPtr());
//				if (productInstancePtr != nullptr){
//					QByteArray id = productInstancePtr->GetProductId();
//					if (id == productId){
//						if (m_licensePersistenceCompPtr.IsValid()){
//							QTemporaryDir tempDir;
//							QString filePathTmp = tempDir.path() + "/" + QUuid::createUuid().toString() + ".xml";

//							int state = m_licensePersistenceCompPtr->SaveToFile(*productInstancePtr, filePathTmp);
//							if (state != ifile::IFilePersistence::OS_OK){
//								SendErrorMessage(0, "License file could not be saved", "Server data provider");

//								return false;
//							}

//							QFile file(filePathTmp);

//							if (!file.open(QIODevice::ReadOnly )){
//								SendErrorMessage(0, "License file could not be opened", "Server data provider");

//								return false;
//							}

//							data = file.readAll();

//							file.close();

//							return true;
//						}
//					}
//				}
//			}
//		}
	}

	return false;
}


// reimplemented (imtcrypt::IEncryptionKeysProvider)

QByteArray CKeyDataProviderComp::GetEncryptionKey(imtcrypt::IEncryptionKeysProvider::KeyType type) const
{
	QByteArray retVal;

	if (type == KT_PASSWORD){
		QByteArray id = m_productInstanceId;
		imtbase::IObjectCollection::DataPtr dataPtr;
		m_objectCollectionCompPtr->GetObjectData(id, dataPtr);
		if (dataPtr.IsValid()){
			imtlic::IProductInstanceInfo* productInstanceInfoPtr = dynamic_cast<imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
			Q_ASSERT(productInstanceInfoPtr != nullptr);

			if (productInstanceInfoPtr != nullptr){
				retVal = productInstanceInfoPtr->GetProductInstanceId();
			}
		}
	}
	else if (type == KT_INIT_VECTOR){
		if (m_vectorKeyCompPtr.IsValid()){
			retVal = m_vectorKeyCompPtr->GetId();
		}
	}

	return retVal;
}


} // namespace prolifedata


