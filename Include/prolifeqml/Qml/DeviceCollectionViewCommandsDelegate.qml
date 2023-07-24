import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

CollectionViewCommandsDelegateBase {
    id: container;

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

            let index = commandsProvider.getCommandIndex("ShowNew");
            let isToggled = commandsProvider.commandsModel.GetData("IsToggled", index);

            commandsProvider.commandsModel.SetData("IsToggled", !isToggled, index);

            let filterModel = container.collectionViewBase.modelFilter;
            if (isToggled){
                if (filterModel.ContainsKey("ObjectFilter")){
                    filterModel.RemoveData("ObjectFilter");
                }
            }
            else{
                let objectFilter = filterModel.GetData("ObjectFilter");
                if (!objectFilter){
                    objectFilter = filterModel.AddTreeModel("ObjectFilter")
                }

                objectFilter.SetData("Key", "Status");
                objectFilter.SetData("Value", "none");
            }

            container.filterByNewActive = !container.filterByNewActive;
            container.collectionViewBase.updateGui();
        }
    }
}
