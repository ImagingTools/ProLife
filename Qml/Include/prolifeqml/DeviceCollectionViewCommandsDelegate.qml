import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0

DocumentCollectionViewDelegate {
    id: container;

    property bool filterByNewActive: false;
    property string filterLicense: "";

    documentTypeId: "Device";
    viewTypeId: "DeviceEditor";

    removeDialogTitle: qsTr("Removing the sensor");
    removeMessage: qsTr("Do you really want to remove this sensor? In case of deletion, it will disappear in all orders in which it is present.");

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
    }

    function onLocalizationChanged(languageId){
    }

    function updateItemSelection(selectedItems){
        if (container.collectionView && container.collectionView.commandsController){
            let elementsModel = container.collectionView.table.elements;
            if (!elementsModel){
                return;
            }

            let macAddress = elementsModel.getData("MacAddress", selectedItems[0]);

            let isEnabled = selectedItems.length === 1;

            let isOpenOrderEnabled = isEnabled;
            if (isOpenOrderEnabled){
                let orderId = elementsModel.getData("OrderId", selectedItems[0]);
                isOpenOrderEnabled = isOpenOrderEnabled && orderId !== "";
            }

            let isBindEnabled = isEnabled;
            if (isBindEnabled){
                isBindEnabled = isBindEnabled && macAddress !== "";
            }

            let commandsController = container.collectionView.commandsController;
            if(commandsController){
				commandsController.setCommandIsEnabled("Remove", selectedItems.length > 0);
				commandsController.setCommandIsEnabled("Edit", selectedItems.length > 0);
                commandsController.setCommandIsEnabled("OpenOrder", isOpenOrderEnabled);
                commandsController.setCommandIsEnabled("Bind", isBindEnabled);
                commandsController.setCommandIsEnabled("CreateLicenseFile", isEnabled);
				commandsController.setCommandIsEnabled("Revision", isEnabled);
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
        let indexes = container.collectionView.table.getSelectedIndexes();
        let elementsModel = container.collectionView.table.elements;

        if (commandId === "Bind"){
            let hardwareId = elementsModel.getData("Id", indexes[0]);
            let macAddress = elementsModel.getData("MacAddress", indexes[0]);

            let title = qsTr("Add license to sensor '%1'");
            title = title.replace("%1", macAddress);

            ModalDialogManager.openDialog(productPairEditorDialog, {"hardwareId": hardwareId, "title": title});
        }
        else if (commandId === "OpenOrder"){
            let orderId = elementsModel.getData("OrderUuid", indexes[0]);
            if (orderId !== ""){
                MainDocumentManager.openDocument("Orders", orderId, "Order", "OrderEditor")
            }
        }
        else if (commandId === "CreateLicenseFile"){
            let count = elementsModel.getData("SoftwareLinksCount", indexes[0])
            if (count <= 0){
                ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("No license is linked")})
                return;
            }

            let macAddress = elementsModel.getData("MacAddress", indexes[0])
            if (macAddress === ""){
                ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("The MAC-Address is not set")})
                return;
            }

            let serialNumber = elementsModel.getData("SerialNumber", indexes[0])
            if (serialNumber === ""){
                ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("The Serial Number is not set")})
                return;
            }

            let status = elementsModel.getData("Status", indexes[0])
            if (status !== "Finished"){
                ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("The production status must be 'Finished'")})
                return;
            }

            let data = macAddress.split(':');
            let fileName = data.join('_') + "_" + licenseFileController.defaultName;

            licenseFileController.fileName = fileName;

            let hardwareId = elementsModel.getData("Id", indexes[0]);
            licenseFileController.createLicenseFile(hardwareId);
        }
    }

    Component {
        id: errorDialogComp;

        ErrorDialog {
            width: 450;
            title: qsTr("The license file could not be created");
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
                container.collectionView.doUpdateGui();
            }
        }
    }
}
