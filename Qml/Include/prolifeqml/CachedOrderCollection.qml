pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0

CollectionDataProvider {
    id: container;

    commandId: "OrdersList";
    subscriptionCommandId: "OnOrdersCollectionChanged"

    sortByField: "OrderId";

    fields: ["Id", "OrderId", "Description", "OrderCustomer"];
}


