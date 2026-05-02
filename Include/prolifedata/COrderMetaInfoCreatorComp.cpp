#include <prolifedata/COrderMetaInfoCreatorComp.h>


// ACF includes
#include <imod/TModelWrap.h>

// ProLife includes
#include <prolifedata/IOrderInfo.h>
#include <prolifedata/ICustomerInfo.h>
#include <prolifedata/IOrderCustomerRole.h>
#include <prolifedata/COrderCustomerRole.h>


namespace prolifedata
{


// protected methods

// reimplemented (imtbase::IMetaInfoCreator)

bool COrderMetaInfoCreatorComp::CreateMetaInfo(
			const istd::IChangeable* dataPtr,
			const QByteArray& typeId,
			idoc::MetaInfoPtr& metaInfoPtr) const
{
	if (m_objectTypeIdsAttrPtr.FindValue(typeId) == -1){
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
	metaInfoPtr->SetMetaInfo(IOrderInfo::MIT_CUSTOMER_ID, customerId);
	
	if (m_accountCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr customerDataPtr;
		if (m_accountCollectionCompPtr->GetObjectData(customerId, customerDataPtr)){
			const ICustomerInfo* customerInfoPtr = dynamic_cast<const ICustomerInfo*>(customerDataPtr.GetPtr());
			if (customerInfoPtr != nullptr){
				
				QString customerName = customerInfoPtr->GetName();
				metaInfoPtr->SetMetaInfo(IOrderInfo::MIT_CUSTOMER_NAME, customerName);
			}
		}

		// Populate end-customer meta info from the customer roles collection
		const imtbase::IObjectCollection* rolesCollectionPtr = orderInfoPtr->GetCustomerRoles();
		if (rolesCollectionPtr != nullptr){
			imtbase::ICollectionInfo::Ids roleIds = rolesCollectionPtr->GetElementIds();
			for (const imtbase::ICollectionInfo::Id& roleId : roleIds){
				imtbase::IObjectCollection::DataPtr roleDataPtr;
				if (rolesCollectionPtr->GetObjectData(roleId, roleDataPtr)){
					const COrderCustomerRole* rolePtr = dynamic_cast<const COrderCustomerRole*>(roleDataPtr.GetPtr());
					if (rolePtr != nullptr && rolePtr->GetRoleType() == IOrderCustomerRole::RT_END_CUSTOMER){
						QByteArray endCustomerId = rolePtr->GetCustomerId();
						metaInfoPtr->SetMetaInfo(IOrderInfo::MIT_END_CUSTOMER_ID, endCustomerId);

						imtbase::IObjectCollection::DataPtr endCustomerDataPtr;
						if (m_accountCollectionCompPtr->GetObjectData(endCustomerId, endCustomerDataPtr)){
							const ICustomerInfo* endCustomerInfoPtr = dynamic_cast<const ICustomerInfo*>(endCustomerDataPtr.GetPtr());
							if (endCustomerInfoPtr != nullptr){
								metaInfoPtr->SetMetaInfo(IOrderInfo::MIT_END_CUSTOMER_NAME, endCustomerInfoPtr->GetName());
							}
						}
						break;
					}
				}
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


