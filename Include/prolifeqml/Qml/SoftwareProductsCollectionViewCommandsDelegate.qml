import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    property bool onlyUnpaired: false;
    property bool onlyPaired: false;

    onOnlyPairedChanged: {
        if (container.commandsProvider){
            commandsProvider.setCommandIsEnabled("OnlyUnpaired", !container.onlyPaired);
        }
    }

    onOnlyUnpairedChanged: {
        if (container.commandsProvider){
            commandsProvider.setCommandIsEnabled("OnlyPaired", !container.onlyUnpaired);
        }
    }

    onSelectionChanged: {
        let indexes = container.tableData.getSelectedIndexes();
        let isEnabled = indexes.length === 1;

        let elementsModel = container.tableData.elements;
        let pairIsEnabled = isEnabled;
        if (pairIsEnabled){

            let deviceId = elementsModel.GetData("DeviceId", indexes[0]);
            let licenseNumber = elementsModel.GetData("SerialNumber", indexes[0]);

            pairIsEnabled = deviceId === "" && licenseNumber !== "";
        }

        let createLicenseFileIsEnabled = isEnabled;
        if (createLicenseFileIsEnabled){
            let deviceId = elementsModel.GetData("DeviceId", indexes[0]);
            let licenseNumber = elementsModel.GetData("SerialNumber", indexes[0]);

            createLicenseFileIsEnabled = deviceId !== "" && licenseNumber !== "";
        }

        if (container.commandsProvider){
            commandsProvider.setCommandIsEnabled("Pair", pairIsEnabled);
            commandsProvider.setCommandIsEnabled("OpenOrder", isEnabled);
            commandsProvider.setCommandIsEnabled("CreateLicenseFile", createLicenseFileIsEnabled);
        }
    }

    onCommandActivated: {
        if (commandId === "Pair"){
            console.log("Pair");
            let indexes = container.tableData.getSelectedIndexes();
            if (indexes.length === 1){
                let elementsModel = container.tableData.elements;
                let orderUuid = elementsModel.GetData("OrderUuid", indexes[0]);
                let id = elementsModel.GetData("Id", indexes[0]);

                modalDialogManager.openDialog(productPairEditorDialog, {"orderId" : orderUuid, "softwareId" : id});
            }
        }
        else if (commandId === "OnlyPaired"){
            let commandsProvider = container.commandsProvider;
            let filterModel = container.collectionViewBase.modelFilter;

            if (container.onlyPaired){
                if (filterModel.ContainsKey("ObjectFilter")){
                    filterModel.RemoveData("ObjectFilter");
                }

                container.onlyPaired = false;
                commandsProvider.setCommandIcon("OnlyPaired", "ShownPassword");
            }
            else{
                let objectFilter = filterModel.GetData("ObjectFilter");
                if (!objectFilter){
                    objectFilter = filterModel.AddTreeModel("ObjectFilter")
                }

                objectFilter.SetData("Key", "DeviceId");
                objectFilter.SetData("Value", "");
                objectFilter.SetData("IsEqual", false);

                container.onlyPaired = true;
                commandsProvider.setCommandIcon("OnlyPaired", "HiddenPassword");
            }

            container.collectionViewBase.updateGui();
        }
        else if (commandId === "OnlyUnpaired"){
            let commandsProvider = container.commandsProvider;
            let filterModel = container.collectionViewBase.modelFilter;

            if (container.onlyUnpaired){
                if (filterModel.ContainsKey("ObjectFilter")){
                    filterModel.RemoveData("ObjectFilter");
                }

                container.onlyUnpaired = false;
                commandsProvider.setCommandIcon("OnlyUnpaired", "ShownPassword");
            }
            else{
                let objectFilter = filterModel.GetData("ObjectFilter");
                if (!objectFilter){
                    objectFilter = filterModel.AddTreeModel("ObjectFilter")
                }

                objectFilter.SetData("Key", "DeviceId");
                objectFilter.SetData("Value", "");
                objectFilter.SetData("IsEqual", true);

                container.onlyUnpaired = true;
                commandsProvider.setCommandIcon("OnlyUnpaired", "HiddenPassword");
            }

            container.collectionViewBase.updateGui();
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
        else if (commandId === "CreateLicenseFile"){
            let indexes = container.tableData.getSelectedIndexes();
            let elementsModel = container.tableData.elements;
            let orderId = elementsModel.GetData("OrderUuid", indexes[0]);
            let hardwareId = elementsModel.GetData("HardwareUuid", indexes[0]);
            let macAddress = elementsModel.GetData("DeviceId", indexes[0]);

            if (macAddress !== null && macAddress !== ""){
                let splitData = macAddress.split(':');
                licenseFileController.fileName = splitData.join('_') + '_' + licenseFileController.defaultName;
            }

            licenseFileController.createLicenseFile(orderId + "/" + hardwareId);
        }
    }

    LicenseFileController {
        id: licenseFileController;
    }

    Component {
        id: productPairEditorDialog;

        ProductPairEditorDialog {
            id: dialog;
            onFinished: {
                if (buttonId === "Link"){
                    if (dialog.bodyItem){
                        let productId = dialog.bodyItem.selectedProductId;
                        if (productId !== ""){
                            console.log("selectedProductId", productId);
                            productModel.Clear();

                            let indexes = container.tableData.getSelectedIndexes();
                            if (indexes.length === 1){
                                let elementsModel = container.tableData.elements;

                                let id = elementsModel.GetData("Id", indexes[0]);
                                let serialNumber = elementsModel.GetData("SerialNumber", indexes[0]);

                                productModel.SetData("Id", id)
                                productModel.SetData("SerialNumber", serialNumber)
                                productModel.SetData("PairId", productId)

                                documentController.updateData(dialog.softwareId, productModel)
                            }
                        }
                    }
                }
            }
        }
    }

    TreeItemModel {
        id: productModel;
    }

    GqlDocumentDataController {
        id: documentController;

        documentTypeId: "SoftwareProduct";

        onDocumentUpdated: {
            container.collectionViewBase.updateGui();
        }

        onError: {

        }
    }
}
