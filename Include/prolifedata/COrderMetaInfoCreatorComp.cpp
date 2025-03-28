#include <prolifedata/COrderMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ProLife includes
#include <prolifedata/IOrderInfo.h>
#include <prolifedata/ICustomerInfo.h>


namespace prolifedata
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool COrderMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (typeId != *m_objectTypeIdAttrPtr){
		return false;
	}

	metaInfoPtr.SetPtr(new imod::TModelWrap<MetaInfo>);

	if (dataPtr == nullptr){
		return true;
	}

	const IOrderInfo* orderInfoPtr = dynamic_cast<const IOrderInfo*>(dataPtr);
	if (orderInfoPtr == nullptr){
		return false;
	}
	
	QByteArray customerId = orderInfoPtr->GetCustomerId();
	if (m_accountCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr customerDataPtr;
		if (m_accountCollectionCompPtr->GetObjectData(customerId, customerDataPtr)){
			const ICustomerInfo* customerInfoPtr = dynamic_cast<const ICustomerInfo*>(customerDataPtr.GetPtr());
			if (customerInfoPtr != nullptr){
				metaInfoPtr->SetMetaInfo(IOrderInfo::MIT_CUSTOMER_ID, customerId);
				
				QString customerName = customerInfoPtr->GetName();
				metaInfoPtr->SetMetaInfo(IOrderInfo::MIT_CUSTOMER_NAME, customerName);
			}
		}
	}
	
	metaInfoPtr->SetMetaInfo(IOrderInfo::MIT_ORDER_STATUS, orderInfoPtr->GetOrderStatus());
	metaInfoPtr->SetMetaInfo(IOrderInfo::MIT_ORDER_ID, orderInfoPtr->GetOrderId());
	metaInfoPtr->SetMetaInfo(IOrderInfo::MIT_PURCHASE_ORDER_ID, orderInfoPtr->GetPurchaseOrderId());

	return true;
}


// public methods of embedded class MetaInfo

QString COrderMetaInfoCreatorComp::MetaInfo::GetMetaInfoName(int /*metaInfoType*/) const
{
	return QString();
}


} // namespace prolifedata


