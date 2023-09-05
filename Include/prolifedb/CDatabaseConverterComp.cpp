#include <prolifedb/CDatabaseConverterComp.h>


// Qt includes
#include <QtCore/QDebug>

// ImtCore includes
#include <imtbase/CObjectLink.h>
#include <imtlic/CHardwareInstanceInfo.h>
#include <imtbase/CObjectCollection.h>

// ProLife includes
#include <prolifedata/CHardwareProductBinding.h>
#include <prolifedata/COrderedIdentifiableSoftwareInstanceInfo.h>
#include <prolifedata/IOrderInfo.h>
#include <prolifedata/IDeviceInfo.h>


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
			if (orderObjectId == "03b38efe-1ec1-44a8-8bf3-4e17c4a50f05"){
				qDebug() << "Find";
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

							const imtbase::ICollectionInfo& licenseInstances = productInstancePtr->GetLicenseInstances();
							const imtbase::ICollectionInfo::Ids licenseIds = licenseInstances.GetElementIds();
							for (int index = 0; index < licenseIds.count(); index++){
								QByteArray licenseId = licenseIds[index];
								const imtlic::ILicenseInstance* licenseInstancePtr = productInstancePtr->GetLicenseInstance(licenseId);
								if (licenseInstancePtr != nullptr && m_productInstanceCollectionCompPtr.IsValid()){
									istd::TDelPtr<prolifedata::COrderedIdentifiableSoftwareInstanceInfo> orderedProductInstancePtr;
									orderedProductInstancePtr.SetPtr(new prolifedata::COrderedIdentifiableSoftwareInstanceInfo);

									QByteArray newProductUuid = productId;
									if (licenseIds.count() > 1){
										newProductUuid = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
									}

									orderedProductInstancePtr->SetupProductInstance(productInstancePtr->GetProductId(),
																					productInstancePtr->GetProductInstanceId(),
																					productInstancePtr->GetCustomerId());
									orderedProductInstancePtr->SetOrderId(orderObjectId);
									orderedProductInstancePtr->AddLicense(licenseInstancePtr->GetLicenseId(), licenseInstancePtr->GetExpiration());
									orderedProductInstancePtr->SetObjectUuid(newProductUuid);

									if (!deviceId.isEmpty()){
										orderedProductInstancePtr->SetInUse(true);
									}

									QByteArray serialNumber = productInstancePtr->GetSerialNumber();
									if (!serialNumber.isEmpty() && index > 0){
										serialNumber += QString("-%1").arg(index).toUtf8();
									}
									orderedProductInstancePtr->SetSerialNumber(serialNumber);
									m_productInstanceCollectionCompPtr->InsertNewObject("", "", "", orderedProductInstancePtr.GetPtr(), newProductUuid);
									mapNewSoftware.insert(newProductUuid, orderedProductInstancePtr.PopPtr());

									qDebug() << QString("Insert object into productInstance collection: ") << newProductUuid;

									if (m_bindingCollectionCompPtr.IsValid()){
										imtbase::IObjectCollection::DataPtr bindingDataPtr;
										if (m_bindingCollectionCompPtr->GetObjectData(deviceId, bindingDataPtr)){
											prolifedata::CHardwareProductBinding* bindingDataInfoPtr = dynamic_cast<prolifedata::CHardwareProductBinding*>(bindingDataPtr.GetPtr());
											if (bindingDataInfoPtr != nullptr){
												bindingDataInfoPtr->Bind(newProductUuid);

												m_bindingCollectionCompPtr->SetObjectData(deviceId, *bindingDataInfoPtr);
											}
										}
										else{
											istd::TDelPtr<prolifedata::CHardwareProductBinding> productBindingPtr;
											productBindingPtr.SetPtr(new prolifedata::CHardwareProductBinding);

											productBindingPtr->SetHardwareId(deviceId);
											productBindingPtr->Bind(newProductUuid);

											m_bindingCollectionCompPtr->InsertNewObject("", "", "", productBindingPtr.GetPtr(), deviceId);
										}

										qDebug() << QString("Insert object into binding collection: ") << deviceId;
									}
								}
							}
						}

						for (const QByteArray& productId : hardwareProductsIds){
							imtlic::CIdentifiableHardwareInstanceInfo* hardwareProductPtr = mapHardware.value(productId);
							QByteArray deviceId = hardwareProductPtr->GetDeviceId();

							QByteArray modelTypeId = hardwareProductPtr->GetModelTypeId();

							imtbase::IObjectCollection::DataPtr deviceDataPtr;
							if (m_deviceCollectionCompPtr->GetObjectData(deviceId, deviceDataPtr)){
								prolifedata::IDeviceInfo* deviceInfoPtr = dynamic_cast<prolifedata::IDeviceInfo*>(deviceDataPtr.GetPtr());
								if (deviceInfoPtr != nullptr){
									deviceInfoPtr->SetConfigurationType(modelTypeId);

									m_deviceCollectionCompPtr->SetObjectData(deviceId, *deviceInfoPtr);
								}
							}

							productCollectionPtr->RemoveElement(productId);

							istd::TDelPtr<imtbase::CObjectLink> deviceLinkPtr;
							deviceLinkPtr.SetPtr(new imtbase::CObjectLink);

							deviceLinkPtr->SetFactoryId("HardwareInfo");
							deviceLinkPtr->SetObjectUuid(hardwareProductPtr->GetDeviceId());

							productCollectionPtr->InsertNewObject(deviceLinkPtr->GetFactoryId(), "", "", deviceLinkPtr.GetPtr(), hardwareProductPtr->GetDeviceId());
						}

						for (const QByteArray& productId : softwareProductsIds){
							productCollectionPtr->RemoveElement(productId);

							istd::TDelPtr<imtbase::CObjectLink> softwareLinkPtr;
							softwareLinkPtr.SetPtr(new imtbase::CObjectLink);

							softwareLinkPtr->SetFactoryId("SoftwareInfo");
							softwareLinkPtr->SetObjectUuid(productId);

							productCollectionPtr->InsertNewObject(softwareLinkPtr->GetFactoryId(), "", "", softwareLinkPtr.GetPtr(), productId);
						}

						m_orderCollectionCompPtr->SetObjectData(orderObjectId, *orderInfoPtr);
						qDebug() << QString("Insert object into order collection: ") << orderObjectId;

						qDeleteAll(mapHardware);
						qDeleteAll(mapNewSoftware);
						qDeleteAll(mapSoftware);
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

	QCoreApplication::exit();
}


void CDatabaseConverterComp::OnComponentDestroyed()
{
	BaseClass::OnComponentDestroyed();
}


} // namespace imtdb


