import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

CollectionViewCommandsDelegateBase {
    id: container;

//    onSelectedIndexChanged: {
//        console.log("DeviceCollectionViewCommands onSelectedIndexChanged", container.selectedIndex);

//        let isEnabled = container.selectedIndex > -1;
//        if (isEnabled){
//            let elementsModel = container.tableData.elements;
//            let orderId = elementsModel.GetData("OrderId", container.selectedIndex);
//            isEnabled = isEnabled && orderId !== "";
//        }

//        if (container.commandsProvider){
//            commandsProvider.setCommandIsEnabled("OpenOrder", isEnabled);
//        }
//    }

    onSelectionChanged: {
        let indexes = container.tableData.getSelectedIndexes();
        let isEnabled = indexes.length === 1;
        if (isEnabled){
            let elementsModel = container.tableData.elements;
            let orderId = elementsModel.GetData("OrderId", indexes[0]);
            isEnabled = isEnabled && orderId !== "";
        }

        if (container.commandsProvider){
            commandsProvider.setCommandIsEnabled("OpenOrder", isEnabled);
        }
    }

    onCommandActivated: {
        if (commandId === "OpenOrder"){
            let indexes = container.tableData.getSelectedIndexes();
            let elementsModel = container.tableData.elements;
            let orderId = elementsModel.GetData("OrderUuid", indexes[0]);
            if (orderId !== ""){
                mainDocumentManager.openDocument("Orders", orderId);
            }
        }
    }
}
