#include <prolifedb/CDatabaseConverterComp.h>
#include "imtlic/IProductInstanceInfo.h"


namespace prolifedb
{

// protected methods

// reimplemented (icomp::CComponentBase)

void CDatabaseConverterComp::OnComponentCreated()
{
    BaseClass::OnComponentCreated();

    imtbase::IObjectCollection::DataPtr dataPtr;
    imtbase::ICollectionInfo::Ids elementIds = m_productInstanceCollectionCompPtr->GetElementIds();

    for(const imtbase::ICollectionInfo::Id& elementId : elementIds){
        if (m_productInstanceCollectionCompPtr->GetObjectData(elementId, dataPtr)){
            imtlic::IProductInstanceInfo* productInstancePtr = dynamic_cast<imtlic::IProductInstanceInfo*>(dataPtr.GetPtr());
            istd::TDelPtr<prolifedata::IOrderInfo> orderPtr = m_orderPtr.CreateInstance();
            //

            orderPtr->SetOrderId(productInstancePtr->GetProductId());
            orderPtr->SetCustomerId(productInstancePtr->GetCustomerId());
            //
            QString errorMessage;
            orderPtr.PopPtr();

        }
    }
}


void CDatabaseConverterComp::OnComponentDestroyed()
{
    BaseClass::OnComponentDestroyed();
}


} // namespace imtdb


