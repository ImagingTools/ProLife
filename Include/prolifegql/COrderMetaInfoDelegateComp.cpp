// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

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
	
	return true;
}


} // namespace prolifegql


