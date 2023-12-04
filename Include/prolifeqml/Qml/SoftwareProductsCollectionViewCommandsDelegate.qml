import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0

CollectionViewCommandsDelegateBase {
    id: container;

    property bool onlyUnpaired: false;
    property bool onlyPaired: false;

    removeDialogTitle: qsTr("Removing the software instance");
    removeMessage: qsTr("Do you really want to remove this product? In case of deletion, it will disappear in all orders in which it is present.");

    onOnlyPairedChanged: {
        console.log("onOnlyPairedChanged", onlyPaired);
        if (container.commandsProvider){
            let unpairedIndex = commandsProvider.getCommandIndex("OnlyUnpaired");
            let pairedIndex = commandsProvider.getCommandIndex("OnlyPaired");

            commandsProvider.commandsModel.SetData("IsToggled", onlyPaired, pairedIndex);

            if (onlyPaired){
                commandsProvider.commandsModel.SetData("IsToggled", false,  unpairedIndex);
            }
        }
    }

    onOnlyUnpairedChanged: {
        console.log("onOnlyUnpairedChanged", onlyUnpaired);
        if (container.commandsProvider){
            let unpairedIndex = commandsProvider.getCommandIndex("OnlyUnpaired");
            let pairedIndex = commandsProvider.getCommandIndex("OnlyPaired");

            commandsProvider.commandsModel.SetData("IsToggled", onlyUnpaired, unpairedIndex);

            if (onlyUnpaired){
                commandsProvider.commandsModel.SetData("IsToggled", false,  pairedIndex);
            }
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

        let openOrderEnabled = isEnabled;
        if (openOrderEnabled){
            let orderUuid = elementsModel.GetData("OrderUuid", indexes[0]);
            let orderId = elementsModel.GetData("OrderId", indexes[0]);

            if (orderUuid == "undefined" || orderUuid == ""){
                openOrderEnabled = false;
            }
        }

        if (container.commandsProvider){
            commandsProvider.setCommandIsEnabled("Pair", pairIsEnabled);
            commandsProvider.setCommandIsEnabled("OpenOrder", openOrderEnabled);
            commandsProvider.setCommandIsEnabled("CreateLicenseFile", createLicenseFileIsEnabled);
        }
    }

    onCommandActivated: {
        let commandsProvider = container.commandsProvider;
        let unpairedIndex = commandsProvider.getCommandIndex("OnlyUnpaired");
        let pairedIndex = commandsProvider.getCommandIndex("OnlyPaired");

        if (commandId === "Pair"){
//            console.log("Pair");
//            let indexes = container.tableData.getSelectedIndexes();
//            if (indexes.length === 1){
//                let elementsModel = container.tableData.elements;
//                let orderUuid = elementsModel.GetData("OrderUuid", indexes[0]);
//                let id = elementsModel.GetData("Id", indexes[0]);

//                modalDialogManager.openDialog(productPairEditorDialog, {"orderId" : orderUuid, "softwareId" : id});
//            }
        }
        else if (commandId === "OnlyPaired"){
            let filterModel = container.collectionViewBase.modelFilter;

            let isToggled = commandsProvider.commandsModel.GetData("IsToggled", pairedIndex);
            if (isToggled){
                if (filterModel.ContainsKey("ObjectFilter")){
                    filterModel.RemoveData("ObjectFilter");
                }

                commandsProvider.commandsModel.SetData("IsToggled", false, pairedIndex);
            }
            else{
                let objectFilter = filterModel.GetData("ObjectFilter");
                if (!objectFilter){
                    objectFilter = filterModel.AddTreeModel("ObjectFilter")
                }

                objectFilter.SetData("Key", "DeviceId");
                objectFilter.SetData("Value", "");
                objectFilter.SetData("IsEqual", false);

                commandsProvider.commandsModel.SetData("IsToggled", true, pairedIndex);
                commandsProvider.commandsModel.SetData("IsToggled", false, unpairedIndex);
            }

            container.collectionViewBase.updateGui();
        }
        else if (commandId === "OnlyUnpaired"){
            let filterModel = container.collectionViewBase.modelFilter;

            let isToggled = commandsProvider.commandsModel.GetData("IsToggled", unpairedIndex);
            if (isToggled){
                if (filterModel.ContainsKey("ObjectFilter")){
                    filterModel.RemoveData("ObjectFilter");
                }

                commandsProvider.commandsModel.SetData("IsToggled", false, unpairedIndex);
            }
            else{
                let objectFilter = filterModel.GetData("ObjectFilter");
                if (!objectFilter){
                    objectFilter = filterModel.AddTreeModel("ObjectFilter")
                }

                objectFilter.SetData("Key", "DeviceId");
                objectFilter.SetData("Value", "");
                objectFilter.SetData("IsEqual", true);

                commandsProvider.commandsModel.SetData("IsToggled", true, unpairedIndex);
                commandsProvider.commandsModel.SetData("IsToggled", false, pairedIndex);
            }

            container.collectionViewBase.updateGui();
        }
        else if (commandId === "OpenOrder"){
            let indexes = container.tableData.getSelectedIndexes();
            let elementsModel = container.tableData.elements;
            let orderUuid = elementsModel.GetData("OrderUuid", indexes[0]);
            if (orderUuid !== ""){
                let parameters = {}
                parameters["TypeId"] = "Orders";
                parameters["DocumentId"] = orderUuid;
                parameters["DocumentTypeId"] = "Order";

                Events.sendEvent("OpenDocument", parameters);
            }
        }
    }

//    LicenseFileController {
//        id: licenseFileController;
//    }

//    Component {
//        id: productPairEditorDialog;

//        ProductPairEditorDialog {
//            id: dialog;
//            onFinished: {
//                if (buttonId === "Link"){
//                    if (dialog.bodyItem){
//                        let productId = dialog.bodyItem.selectedProductId;
//                        if (productId !== ""){
//                            console.log("selectedProductId", productId);
//                            productModel.Clear();

//                            let indexes = container.tableData.getSelectedIndexes();
//                            if (indexes.length === 1){
//                                let elementsModel = container.tableData.elements;

//                                let id = elementsModel.GetData("Id", indexes[0]);
//                                let serialNumber = elementsModel.GetData("SerialNumber", indexes[0]);

//                                productModel.SetData("Id", id)
//                                productModel.SetData("SerialNumber", serialNumber)
//                                productModel.SetData("PairId", productId)

//                                documentController.updateData(dialog.softwareId, productModel)
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }

//    TreeItemModel {
//        id: productModel;
//    }

//    GqlDocumentDataController {
//        id: documentController;

//        documentTypeId: "SoftwareProduct";

//        onDocumentUpdated: {
//            container.collectionViewBase.updateGui();
//        }

//        onError: {

//        }
//    }
}
