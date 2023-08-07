#include <prolifedb/CDatabaseConverterComp.h>


// Qt includes
#include <QtCore/QDebug>>

// ImtCore includes
#include <imtlic/IHardwareInstanceInfo.h>

// ProLife includes
#include <prolifedata/CHardwareProductBinding.h>
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
			imtbase::IObjectCollection::DataPtr orderDataPtr;
			if (m_orderCollectionCompPtr->GetObjectData(orderObjectId, orderDataPtr)){
				prolifedata::IOrderInfo* orderInfoPtr = dynamic_cast<prolifedata::IOrderInfo*>(orderDataPtr.GetPtr());
				if (orderInfoPtr != nullptr){
					imtbase::IObjectCollection* productCollectionPtr = orderInfoPtr->GetProducts();
					if (productCollectionPtr != nullptr){
						imtbase::ICollectionInfo::Ids orderedProductsIds = productCollectionPtr->GetElementIds();
						for(const QByteArray& productId : orderedProductsIds){
							imtbase::IObjectCollection::DataPtr productDataPtr;
							if (productCollectionPtr->GetObjectData(productId, productDataPtr)){
								imtlic::IHardwareInstanceInfo* hardwareProductPtr = dynamic_cast<imtlic::IHardwareInstanceInfo*>(productDataPtr.GetPtr());
								if (hardwareProductPtr != nullptr){
									QByteArray softwareId = hardwareProductPtr->GetSoftwareId();
									QByteArray deviceId = hardwareProductPtr->GetDeviceId();
									if (!softwareId.isEmpty()){
										if (m_bindingCollectionCompPtr.IsValid()){
											istd::TDelPtr<prolifedata::CHardwareProductBinding> productBindingPtr;
											productBindingPtr.SetPtr(new prolifedata::CHardwareProductBinding);

											productBindingPtr->SetHardwareId(deviceId);

											QByteArrayList softwareIds;
											softwareIds << softwareId;

											productBindingPtr->SetSoftwareIds(softwareIds);

											m_bindingCollectionCompPtr->InsertNewObject("", "", "", productBindingPtr.GetPtr(), deviceId);

											qDebug() << QString("Insert object into binding collection: ") << deviceId;
										}

										hardwareProductPtr->SetSoftwareId("");

										productCollectionPtr->SetObjectData(productId, *hardwareProductPtr);

										m_orderCollectionCompPtr->SetObjectData(orderObjectId, *orderInfoPtr);
									}
								}
							}
						}
					}
				}
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


