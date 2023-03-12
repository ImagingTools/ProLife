import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

Item {
    id: root;

    property CommandsProvider orderCommandsProvider: null;
    property TreeItemModel productCommandsModel: null;

    property Item addProductButton: null;
    property Item listView: null;

    Component.onDestruction: {
        if (root.orderCommandsProvider != null){
            root.orderCommandsProvider.modelLoaded.disconnect(root.orderCommandsModelLoaded);
        }
    }

    onOrderCommandsProviderChanged: {
        if (root.orderCommandsProvider != null){
            root.orderCommandsProvider.modelLoaded.connect(root.orderCommandsModelLoaded);
        }
    }

    function orderCommandsModelLoaded(){
        let saveExists = root.orderCommandsProvider.commandExists("Save");
        let createLicenseExists = root.orderCommandsProvider.commandExists("CreateLicenseFile");

        if (root.listView != null){
            root.listView.readOnly = !saveExists;
            root.listView.createLicenseOnly = !createLicenseExists;
        }

        if (root.addProductButton != null){
            root.addProductButton.enabled = saveExists;
            root.addProductButton.visible = saveExists;
        }
    }

    function getProductCommandIndex(commandId){
        if (root.productCommandsModel == null){
            return -1;
        }

        for (let i = 0; i < root.productCommandsModel.GetItemsCount(); i++){
            let id = root.productCommandsModel.GetData("Id", i);
            if (id === commandId){
                return i;
            }
        }

        return -1;
    }
}
