import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    property bool filterByNewActive: false;
    property string filterLicense: "";

    Component.onCompleted: {
        console.log("DeviceCollectionViewCommandsDelegate onCompleted");
        Events.subscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    function onLocalizationChanged(languageId){
        let filterModel = container.collectionViewBase.modelFilter;
        if (filterModel.ContainsKey("ObjectFilter")){
            filterModel.RemoveData("ObjectFilter");
        }
    }

    onSelectionChanged: {
        console.log("DeviceCollection onSelectionChanged");
        let elementsModel = container.tableData.elements;
        if (!elementsModel){
            return;
        }

        console.log("elementsModel", elementsModel);

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
            let count = elementsModel.GetData("SoftwareLinksCount", indexes[0]);
            createLicenseFileEnabled = createLicenseFileEnabled && macAddress !== "" && count > 0;
        }

        console.log("container.commandsProvider", container.commandsProvider);

        if (container.commandsProvider){
            commandsProvider.setCommandIsEnabled("OpenOrder", isOpenOrderEnabled);
            commandsProvider.setCommandIsEnabled("Bind", isBindEnabled);
            commandsProvider.setCommandIsEnabled("CreateLicenseFile", createLicenseFileEnabled);
        }
    }

    onCommandActivated: {
        let filterModel = container.collectionViewBase.modelFilter;
        let licenseFilter = filterModel.GetData("LicenseFilter");
        if (!licenseFilter){
            licenseFilter = filterModel.AddTreeModel("LicenseFilter")
        }

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

        else if (commandId === "WithLicense"){
            let index = commandsProvider.getCommandIndex("WithLicense");
            let withoutLicenseindex = commandsProvider.getCommandIndex("WithoutLicense");
            let isToggled = commandsProvider.commandsModel.GetData("IsToggled", index);

            isToggled = !isToggled

            commandsProvider.commandsModel.SetData("IsToggled", isToggled, index);

            if (isToggled){
                licenseFilter.SetData("Key", "Status");
                licenseFilter.SetData("Value", "WithLicense");
                commandsProvider.commandsModel.SetData("IsToggled", false, withoutLicenseindex);
                container.filterLicense = commandsProvider.commandsModel.GetData("Name", index);
            }
            else{
                licenseFilter.SetData("Key", "Status");
                licenseFilter.SetData("Value", "None");
                container.filterLicense = ""
            }

            container.collectionViewBase.updateGui();
        }

        else if (commandId === "WithoutLicense"){
            let index = commandsProvider.getCommandIndex("WithoutLicense");
            let withLicenseindex = commandsProvider.getCommandIndex("WithLicense");
            let isToggled = commandsProvider.commandsModel.GetData("IsToggled", index);

            isToggled = !isToggled
            commandsProvider.commandsModel.SetData("IsToggled", isToggled, index);

            if (isToggled){
                licenseFilter.SetData("Key", "Status");
                licenseFilter.SetData("Value", "WithoutLicense");
                commandsProvider.commandsModel.SetData("IsToggled", false, withLicenseindex);
                container.filterLicense = commandsProvider.commandsModel.GetData("Name", index);
            }
            else{
                licenseFilter.SetData("Key", "Status");
                licenseFilter.SetData("Value", "None");
                container.filterLicense = ""
            }

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

            onSaved: {
                container.collectionViewBase.updateGui();
            }
        }
    }
}
