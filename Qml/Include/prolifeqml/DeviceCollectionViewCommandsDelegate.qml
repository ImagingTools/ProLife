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

            let macAddress = elementsModel.GetData("MacAddress", selectedItems[0]);

            let isEnabled = selectedItems.length === 1;

            let isOpenOrderEnabled = isEnabled;
            if (isOpenOrderEnabled){
                let orderId = elementsModel.GetData("OrderId", selectedItems[0]);
                isOpenOrderEnabled = isOpenOrderEnabled && orderId !== "";
            }

            let isBindEnabled = isEnabled;
            if (isBindEnabled){
                isBindEnabled = isBindEnabled && macAddress !== "";
            }

            let createLicenseFileEnabled = selectedItems.length === 1;
            if (createLicenseFileEnabled){
                let count = elementsModel.GetData("SoftwareLinksCount", selectedItems[0]);
                createLicenseFileEnabled = createLicenseFileEnabled && macAddress !== "" && count > 0;
            }

            let commandsController = container.collectionView.commandsController;
            if(commandsController){
                commandsController.setCommandIsEnabled("Remove", isEnabled);
                commandsController.setCommandIsEnabled("Edit", isEnabled);
                commandsController.setCommandIsEnabled("OpenOrder", isOpenOrderEnabled);
                commandsController.setCommandIsEnabled("Bind", isBindEnabled);
                commandsController.setCommandIsEnabled("CreateLicenseFile", createLicenseFileEnabled);
            }
        }
    }

    function setupContextMenu(){
        let commandsController = collectionView.commandsController;
        if (commandsController){
            container.contextMenuModel.Clear();

            let canEdit = commandsController.commandExists("Edit");
            let canRemove = commandsController.commandExists("Remove");

            if (canEdit){
                let index = container.contextMenuModel.InsertNewItem();

                container.contextMenuModel.SetData("Id", "Edit", index);
                container.contextMenuModel.SetData("Name", qsTr("Edit"), index);
                container.contextMenuModel.SetData("Icon", "Icons/Edit", index);
            }

            if (canRemove){
                let index = container.contextMenuModel.InsertNewItem();

                container.contextMenuModel.SetData("Id", "Remove", index);
                container.contextMenuModel.SetData("Name", qsTr("Remove"), index);
                container.contextMenuModel.SetData("Icon", "Icons/Delete", index);
            }

            container.contextMenuModel.Refresh();
        }
    }

    onCommandActivated: {
        let indexes = container.collectionView.table.getSelectedIndexes();
        let elementsModel = container.collectionView.table.elements;

        if (commandId === "Bind"){
            let hardwareId = elementsModel.GetData("Id", indexes[0]);
            let macAddress = elementsModel.GetData("MacAddress", indexes[0]);

            let title = qsTr("Add license to sensor '%1'");
            title = title.replace("%1", macAddress);

            modalDialogManager.openDialog(productPairEditorDialog, {"hardwareId": hardwareId, "title": title});
        }
        else if (commandId === "OpenOrder"){
            let orderId = elementsModel.GetData("OrderUuid", indexes[0]);
            if (orderId !== ""){
                let parameters = {}
                parameters["TypeId"] = "Orders";
                parameters["DocumentId"] = orderId;
                parameters["DocumentTypeId"] = "Order";
                parameters["ViewTypeId"] = "OrderEditor";

                Events.sendEvent("OpenDocument", parameters);
            }
        }
        else if (commandId === "CreateLicenseFile"){
            let macAddress = elementsModel.GetData("MacAddress", indexes[0])

            let data = macAddress.split(':');
            let fileName = data.join('_') + "_" + licenseFileController.defaultName;

            licenseFileController.fileName = fileName;

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
                container.collectionView.doUpdateGui();
            }
        }
    }
}
