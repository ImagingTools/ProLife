import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0

DocumentCollectionViewDelegate {
    id: container;

    documentTypeId: "SoftwareProduct";
    viewTypeId: "SoftwareProductEditor";

    removeDialogTitle: qsTr("Removing the software instance");
    removeMessage: qsTr("Do you really want to remove this product? In case of deletion, it will disappear in all orders in which it is present.");

    function updateItemSelection(selectedItems){
        if (container.collectionView && container.collectionView.commandsController){
            let indexes = container.collectionView.table.getSelectedIndexes();
            let isEnabled = indexes.length === 1;

            let elementsModel = container.collectionView.table.elements;
            let createLicenseFileIsEnabled = isEnabled;
            if (createLicenseFileIsEnabled){
                let deviceId = elementsModel.getData("DeviceId", indexes[0]);
                let licenseNumber = elementsModel.getData("SerialNumber", indexes[0]);

                createLicenseFileIsEnabled = deviceId !== "" && licenseNumber !== "";
            }

            let openOrderEnabled = isEnabled;
            if (openOrderEnabled){
                let orderUuid = elementsModel.getData("OrderUuid", indexes[0]);
                let orderId = elementsModel.getData("OrderId", indexes[0]);

                if (orderUuid == "undefined" || orderUuid == ""){
                    openOrderEnabled = false;
                }
            }

            let commandsController = container.collectionView.commandsController;
            if(commandsController){
                commandsController.setCommandIsEnabled("Remove", isEnabled);
                commandsController.setCommandIsEnabled("Edit", isEnabled);
                commandsController.setCommandIsEnabled("OpenOrder", openOrderEnabled);
                commandsController.setCommandIsEnabled("CreateLicenseFile", createLicenseFileIsEnabled);
            }
        }
    }

    function setupContextMenu(){
        let commandsController = collectionView.commandsController;
        if (commandsController){
            container.contextMenuModel.clear();

            let canEdit = commandsController.commandExists("Edit");
            let canRemove = commandsController.commandExists("Remove");

            if (canEdit){
                let index = container.contextMenuModel.insertNewItem();

                container.contextMenuModel.setData("Id", "Edit", index);
                container.contextMenuModel.setData("Name", qsTr("Edit"), index);
                container.contextMenuModel.setData("Icon", "Icons/Edit", index);
            }

            if (canRemove){
                let index = container.contextMenuModel.insertNewItem();

                container.contextMenuModel.setData("Id", "Remove", index);
                container.contextMenuModel.setData("Name", qsTr("Remove"), index);
                container.contextMenuModel.setData("Icon", "Icons/Delete", index);
            }

            container.contextMenuModel.refresh();
        }
    }

    onCommandActivated: {
        if (commandId === "OpenOrder"){
            let indexes = container.collectionView.table.getSelectedIndexes();
            let elementsModel = container.collectionView.table.elements;
            let orderUuid = elementsModel.getData("OrderUuid", indexes[0]);
            if (orderUuid !== ""){
                MainDocumentManager.openDocument("Orders", orderUuid, "Order", "OrderEditor")
            }
        }
    }
}
