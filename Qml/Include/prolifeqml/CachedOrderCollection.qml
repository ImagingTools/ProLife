pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0
import prolifeOrdersSdl 1.0

CollectionDataProvider {
    id: container;
    commandId: ProlifeOrdersSdlCommandIds.s_ordersList;
    subscriptionCommandId: "OnOrdersCollectionChanged"
    sortByField: OrderDataTypeMetaInfo.s_orderId;
    fields: [
        OrderItemTypeMetaInfo.s_id,
        OrderItemTypeMetaInfo.s_orderId,
        OrderItemTypeMetaInfo.s_description,
        OrderItemTypeMetaInfo.s_customerId,
		OrderItemTypeMetaInfo.s_customerName
    ];
}


