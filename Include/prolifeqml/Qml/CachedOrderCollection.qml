pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0

CollectionDataProvider {
    id: container;

    commandId: "Orders";

    sortByField: "OrderId";

    fields: ["Id", "OrderId", "Description"];

    function updateModel(){
        if (container.collectionModel.GetItemsCount() === 0){
            container.itemsInfoModel.updateModel({}, container.fields);
        }
    }
}


