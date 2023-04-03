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


    property bool filterByNewActive: false;

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
                if (container.collectionViewBase.mainDocumentManager){
                    container.collectionViewBase.mainDocumentManager.openDocument("Orders", orderId);
                }
            }
        }
        else if (commandId === "ShowNew"){
            let showNewStr = qsTr("New Sensors");
            let showAllStr = qsTr("All Sensors");

            let filterModel = container.collectionViewBase.modelFilter;
            if (!container.filterByNewActive){
                let objectFilter = filterModel.GetData("ObjectFilter");
                if (!objectFilter){
                    objectFilter = filterModel.AddTreeModel("ObjectFilter")
                }

                objectFilter.SetData("Key", "Status");
                objectFilter.SetData("Value", "none");

                container.commandsProvider.setCommandName("ShowNew", showAllStr);
                container.commandsProvider.setCommandIcon("ShowNew", "HiddenPassword");
            }
            else{
                if (filterModel.ContainsKey("ObjectFilter")){
                    filterModel.RemoveData("ObjectFilter");
                }

                container.commandsProvider.setCommandName("ShowNew", showNewStr);
                container.commandsProvider.setCommandIcon("ShowNew", "ShownPassword");
            }

            container.filterByNewActive = !container.filterByNewActive;

            container.collectionViewBase.updateGui();
        }
    }
}
