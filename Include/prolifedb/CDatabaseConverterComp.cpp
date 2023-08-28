#include <prolifedb/CDatabaseConverterComp.h>


// Qt includes
#include <QtCore/QDebug>>

// ImtCore includes
#include <imtlic/CHardwareInstanceInfo.h>

// ProLife includes
#include <prolifedata/CHardwareProductBinding.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>
#include <prolifedata/IOrderInfo.h>


namespace prolifedb
{


// protected methods

// reimplemented (icomp::CComponentBase)

void CDatabaseConverterComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	qDebug() << "ProLife convertation has started";

	if (m_orderCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::Ids orderObjectIds = m_orderCollectionCompPtr->GetElementIds();
		for (const imtbase::IObjectCollection::Id& orderObjectId : orderObjectIds){
			if (orderObjectId == "395e4ac9-1481-4305-9cce-0042903f4999"){
				qDebug() << "find";
			}
			imtbase::IObjectCollection::DataPtr orderDataPtr;
			if (m_orderCollectionCompPtr->GetObjectData(orderObjectId, orderDataPtr)){
				prolifedata::IOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(orderDataPtr.GetPtr());
				if (orderInfoPtr != nullptr){
					imtbase::IObjectCollection* productCollectionPtr = orderInfoPtr->GetProducts();
					if (productCollectionPtr != nullptr){
						QMap<QByteArray, imtlic::CIdentifiableSoftwareInstanceInfo*> mapSoftware;
						QMap<QByteArray, imtlic::CIdentifiableHardwareInstanceInfo*> mapHardware;
						QMap<QByteArray, imtlic::CIdentifiableSoftwareInstanceInfo*> mapNewSoftware;
						imtbase::ICollectionInfo::Ids orderedProductsIds = productCollectionPtr->GetElementIds();
						for(const QByteArray& productId : orderedProductsIds){
							imtbase::IObjectCollection::DataPtr productDataPtr;
							if (productCollectionPtr->GetObjectData(productId, productDataPtr)){
								imtlic::CIdentifiableSoftwareInstanceInfo* productInstancePtr = dynamic_cast<imtlic::CIdentifiableSoftwareInstanceInfo*>(productDataPtr.GetPtr());
								if (productInstancePtr != nullptr){
									mapSoftware.insert(productId, dynamic_cast<imtlic::CIdentifiableSoftwareInstanceInfo*>(productInstancePtr->CloneMe()));
								}
								imtlic::CIdentifiableHardwareInstanceInfo* hardwareProductPtr = dynamic_cast<imtlic::CIdentifiableHardwareInstanceInfo*>(productDataPtr.GetPtr());
								if (hardwareProductPtr != nullptr){
									mapHardware.insert(productId,  dynamic_cast<imtlic::CIdentifiableHardwareInstanceInfo*>(hardwareProductPtr->CloneMe()));
								}
							}
						}
						QList<QByteArray> hardwareProductsIds = mapHardware.keys();
						QList<QByteArray> softwareProductsIds = mapSoftware.keys();
						QByteArray deviceId;
						for (const QByteArray& productId : softwareProductsIds){
							imtlic::CIdentifiableHardwareInstanceInfo* hardwareProductPtr = nullptr;
							imtlic::CIdentifiableSoftwareInstanceInfo* productInstancePtr = mapSoftware.value(productId);
							for (const QByteArray& hardwareProductId : hardwareProductsIds){
								hardwareProductPtr = mapHardware.value(hardwareProductId);
								if (hardwareProductPtr->GetSoftwareId() == productId){
									deviceId = hardwareProductPtr->GetDeviceId();
								}
							}
//							if (hardwareProductPtr != nullptr){
//								hardwareProductPtr->SetSoftwareId("");
//								productCollectionPtr->SetObjectData(productId, *hardwareProductPtr);
//							}
							const imtbase::ICollectionInfo& licenseInstances = productInstancePtr->GetLicenseInstances();
							const imtbase::ICollectionInfo::Ids licenseIds = licenseInstances.GetElementIds();
							for (int index = 0; index < licenseIds.count(); index++){
								QByteArray licenseId = licenseIds[index];
								const imtlic::ILicenseInstance* licenseInstancePtr = productInstancePtr->GetLicenseInstance(licenseId);
								if (licenseInstancePtr != nullptr && m_productInstanceCollectionCompPtr.IsValid()){
									istd::TDelPtr<prolifedata::COrderedIdentifiableSoftwareInstanceInfo> orderedProductInstancePtr;
									orderedProductInstancePtr.SetPtr(new prolifedata::COrderedIdentifiableSoftwareInstanceInfo);
//											dynamic_cast<imtlic::CIdentifiableSoftwareInstanceInfo*>(orderedProductInstancePtr.GetPtr())->CopyFrom(*productInstancePtr);
									QByteArray newProductUuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
									orderedProductInstancePtr->SetupProductInstance(productInstancePtr->GetProductId(),
																					productInstancePtr->GetProductInstanceId(),
																					productInstancePtr->GetCustomerId());
									orderedProductInstancePtr->SetOrderId(orderObjectId);
									orderedProductInstancePtr->AddLicense(licenseInstancePtr->GetLicenseId(), licenseInstancePtr->GetExpiration());
									orderedProductInstancePtr->SetObjectUuid(newProductUuid);
									QByteArray serialNumber = productInstancePtr->GetSerialNumber();
									if (!serialNumber.isEmpty() && index > 0){
										serialNumber += QString("-%1").arg(index).toUtf8();
									}
									orderedProductInstancePtr->SetSerialNumber(serialNumber);
									m_productInstanceCollectionCompPtr->InsertNewObject("", "", "", orderedProductInstancePtr.GetPtr(), newProductUuid);
									mapNewSoftware.insert(newProductUuid, orderedProductInstancePtr.PopPtr());

									qDebug() << QString("Insert object into productInstance collection: ") << newProductUuid;

									if (m_bindingCollectionCompPtr.IsValid()){
										istd::TDelPtr<prolifedata::CHardwareProductBinding> productBindingPtr;
										productBindingPtr.SetPtr(new prolifedata::CHardwareProductBinding);


										productBindingPtr->SetHardwareId(deviceId);
//										productBindingPtr->SetOrderId(orderObjectId);

										productBindingPtr->Bind(newProductUuid);

										m_bindingCollectionCompPtr->InsertNewObject("", "", "", productBindingPtr.GetPtr(), deviceId);

										qDebug() << QString("Insert object into binding collection: ") << deviceId;
									}

								}

							}
						}

						productCollectionPtr->ResetData();

						for (const QByteArray& productId : hardwareProductsIds){
							imtlic::CIdentifiableHardwareInstanceInfo* hardwareProductPtr = mapHardware.value(productId);
							QByteArray deviceId = hardwareProductPtr->GetDeviceId();
							QUuid deviceUuid = QUuid::fromString(QString(deviceId));
							if (deviceId != deviceUuid.toString(QUuid::WithoutBraces).toUtf8()){
								deviceId = deviceUuid.toString(QUuid::WithoutBraces).toUtf8();
							}
							productCollectionPtr->InsertNewObject("Hardware","","",nullptr,hardwareProductPtr->GetDeviceId());
						}

						for (const QByteArray& productId : softwareProductsIds){
							productCollectionPtr->InsertNewObject("Software","","",nullptr,productId);
						}

						m_orderCollectionCompPtr->SetObjectData(orderObjectId, *orderInfoPtr);
						qDebug() << QString("Insert object into order collection: ") << orderObjectId;

						qDeleteAll(mapHardware);
						qDeleteAll(mapNewSoftware);
						qDeleteAll(mapSoftware);
//						for(const QByteArray& productId : orderedProductsIds){
//							imtbase::IObjectCollection::DataPtr productDataPtr;
//							if (productCollectionPtr->GetObjectData(productId, productDataPtr)){


//								if (hardwareProductPtr != nullptr && productInstancePtr != nullptr){
//									QByteArray productInstanceId = productInstancePtr->GetProductInstanceId();
//									QByteArray deviceId = hardwareProductPtr->GetDeviceId();
//									hardwareProductPtr->SetSoftwareId("");
//									productCollectionPtr->SetObjectData(productId, *hardwareProductPtr);
//									m_orderCollectionCompPtr->SetObjectData(orderObjectId, *orderInfoPtr);


//								}
//							}
//						}
					}
					else{
						qDebug() << QString("OrderInfo is NULL!!!: ") << orderObjectId;
					}
				}
			}
			else{
				qDebug() << QString("Error for read order: ") << orderObjectId;
			}

		}
	}

	qDebug() << "ProLife convertation finished!";
}


void CDatabaseConverterComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


} // namespace imtdb


