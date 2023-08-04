import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    property bool filterByNewActive: false;

    onSelectionChanged: {
        let elementsModel = container.tableData.elements;
        if (!elementsModel){
            return;
        }

        let indexes = container.tableData.getSelectedIndexes();

        let macAddress = elementsModel.GetData("MacAddress", indexes[0]);

        let isEnabled = indexes.length === 1;

        let isOpenOrderEnabled = isEnabled;
        if (isOpenOrderEnabled){
            let orderId = elementsModel.GetData("OrderId", indexes[0]);
            isOpenOrderEnabled = isOpenOrderEnabled && orderId !== "";
        }

        let isBindEnabled = isEnabled;
        if (isBindEnabled){
            isBindEnabled = isBindEnabled && macAddress !== "";
        }

        let createLicenseFileEnabled = indexes.length === 1;
        if (createLicenseFileEnabled){
            let licenses = elementsModel.GetData("Licenses", indexes[0]);
            if (licenses === ""){
                createLicenseFileEnabled = false;
            }
            else{
                createLicenseFileEnabled = createLicenseFileEnabled && macAddress !== "";
            }
        }

        if (container.commandsProvider){
            commandsProvider.setCommandIsEnabled("OpenOrder", isOpenOrderEnabled);
            commandsProvider.setCommandIsEnabled("Bind", isBindEnabled);
            commandsProvider.setCommandIsEnabled("CreateLicenseFile", createLicenseFileEnabled);
        }
    }

    onCommandActivated: {
        if (commandId === "Bind"){
            let indexes = container.tableData.getSelectedIndexes();
            let elementsModel = container.tableData.elements;
            let hardwareId = elementsModel.GetData("Id", indexes[0]);

            modalDialogManager.openDialog(productPairEditorDialog, {"hardwareId": hardwareId});
        }
        else if (commandId === "OpenOrder"){
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

        else if (commandId === "CreateLicenseFile"){
            let indexes = container.tableData.getSelectedIndexes();
            let elementsModel = container.tableData.elements;

            let hardwareId = elementsModel.GetData("Id", indexes[0]);
            licenseFileController.createLicenseFile(hardwareId);
        }
    }

    LicenseFileController {
        id: licenseFileController;
    }

    Component {
        id: productPairEditorDialog;

        HardwareProductBindingDialog {
            id: dialog;

            onFinished: {
                if (buttonId == "Save"){
                    container.collectionViewBase.updateGui();
                }
            }
        }
    }
}
