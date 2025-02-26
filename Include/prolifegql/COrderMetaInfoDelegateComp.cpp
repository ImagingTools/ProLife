#include <prolifegql/COrderMetaInfoDelegateComp.h>


// ProLife includes
#include <prolifedata/IOrderInfo.h>


namespace prolifegql
{


// protected methods

bool COrderMetaInfoDelegateComp::FillRepresentation(
	sdl::prolife::Orders::COrderData::V1_0& metaInfoRepresentation,
	const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray customerId = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_ORDER_CUSTOMER).toByteArray();
	metaInfoRepresentation.CustomerId = customerId;
	
	QByteArray orderId = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_ORDER_ID).toByteArray();
	metaInfoRepresentation.OrderId = orderId;
	
	QByteArray purchaseId = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_PURCHASE_ORDER_ID).toByteArray();
	metaInfoRepresentation.PurchaseId = purchaseId;

	return true;
}


bool COrderMetaInfoDelegateComp::FillMetaInfo(
	idoc::IDocumentMetaInfo& metaInfo,
	const sdl::prolife::Orders::COrderData::V1_0& metaInfoRepresentation) const
{
	if (metaInfoRepresentation.CustomerId){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_ORDER_CUSTOMER, *metaInfoRepresentation.CustomerId);
	}
	
	if (metaInfoRepresentation.OrderId){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_ORDER_ID, *metaInfoRepresentation.OrderId);
	}
	
	if (metaInfoRepresentation.PurchaseId){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_PURCHASE_ORDER_ID, *metaInfoRepresentation.PurchaseId);
	}
	
	return true;
}


} // namespace prolifegql


