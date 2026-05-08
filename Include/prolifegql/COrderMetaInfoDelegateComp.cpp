#include <prolifegql/COrderMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>

// ProLife includes
#include <prolifedata/IOrderInfo.h>


namespace prolifegql
{


// protected methods


bool COrderMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo, const QByteArray& /*typeId*/) const
{
	QByteArray customerId = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_CUSTOMER_ID).toByteArray();
	representation["CustomerId"] = QString(customerId);
	
	QString customerName = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_CUSTOMER_NAME).toString();
	representation["CustomerName"] = customerName;
	
	QByteArray orderId = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_ORDER_ID).toByteArray();
	representation["OrderId"] = QString(orderId);

	QByteArray purchaseId = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_PURCHASE_ORDER_ID).toByteArray();
	representation["PurchaseId"] = QString(purchaseId);
	
	int status = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_ORDER_STATUS).toInt();
	representation["Status"] = status;

	QByteArray endCustomerId = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_END_CUSTOMER_ID).toByteArray();
	representation["EndCustomerId"] = QString(endCustomerId);

	QString endCustomerName = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_END_CUSTOMER_NAME).toString();
	representation["EndCustomerName"] = endCustomerName;

	QByteArray invoiceRecipientId = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_INVOICE_RECIPIENT_ID).toByteArray();
	representation["InvoiceRecipientId"] = QString(invoiceRecipientId);

	QString invoiceRecipientName = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_INVOICE_RECIPIENT_NAME).toString();
	representation["InvoiceRecipientName"] = invoiceRecipientName;

	QByteArray deliveryRecipientId = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_DELIVERY_RECIPIENT_ID).toByteArray();
	representation["DeliveryRecipientId"] = QString(deliveryRecipientId);

	QString deliveryRecipientName = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_DELIVERY_RECIPIENT_NAME).toString();
	representation["DeliveryRecipientName"] = deliveryRecipientName;

	QByteArray resellerId = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_RESELLER_ID).toByteArray();
	representation["ResellerId"] = QString(resellerId);

	QString resellerName = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_RESELLER_NAME).toString();
	representation["ResellerName"] = resellerName;

	QByteArray referrerId = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_REFERRER_ID).toByteArray();
	representation["ReferrerId"] = QString(referrerId);

	QString referrerName = metaInfo.GetMetaInfo(prolifedata::IOrderInfo::MIT_REFERRER_NAME).toString();
	representation["ReferrerName"] = referrerName;
	
	return true;
}


bool COrderMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation, const QByteArray& /*typeId*/) const
{
	if (representation.contains("CustomerId")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_CUSTOMER_ID, representation.value("CustomerId"));
	}
	
	if (representation.contains("CustomerName")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_CUSTOMER_NAME, representation.value("CustomerName"));
	}
	
	if (representation.contains("OrderId")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_ORDER_ID, representation.value("OrderId"));
	}
	
	if (representation.contains("PurchaseId")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_PURCHASE_ORDER_ID, representation.value("PurchaseId"));
	}
	
	if (representation.contains("Status")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_ORDER_STATUS, representation.value("Status"));
	}

	if (representation.contains("EndCustomerId")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_END_CUSTOMER_ID, representation.value("EndCustomerId"));
	}

	if (representation.contains("EndCustomerName")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_END_CUSTOMER_NAME, representation.value("EndCustomerName"));
	}

	if (representation.contains("InvoiceRecipientId")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_INVOICE_RECIPIENT_ID, representation.value("InvoiceRecipientId"));
	}

	if (representation.contains("InvoiceRecipientName")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_INVOICE_RECIPIENT_NAME, representation.value("InvoiceRecipientName"));
	}

	if (representation.contains("DeliveryRecipientId")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_DELIVERY_RECIPIENT_ID, representation.value("DeliveryRecipientId"));
	}

	if (representation.contains("DeliveryRecipientName")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_DELIVERY_RECIPIENT_NAME, representation.value("DeliveryRecipientName"));
	}

	if (representation.contains("ResellerId")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_RESELLER_ID, representation.value("ResellerId"));
	}

	if (representation.contains("ResellerName")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_RESELLER_NAME, representation.value("ResellerName"));
	}

	if (representation.contains("ReferrerId")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_REFERRER_ID, representation.value("ReferrerId"));
	}

	if (representation.contains("ReferrerName")){
		metaInfo.SetMetaInfo(prolifedata::IOrderInfo::MIT_REFERRER_NAME, representation.value("ReferrerName"));
	}
	
	return true;
}


} // namespace prolifegql


