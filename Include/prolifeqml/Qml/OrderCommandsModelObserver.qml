import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

Item {
    id: root;

    property CommandsProvider orderCommandsProvider: null;
    property TreeItemModel productCommandsModel: null;

    property Item addProductButton: null;

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
        if (root.productCommandsModel != null){
            let saveExists = root.orderCommandsProvider.commandExists("Save");

            let editIndex = root.getProductCommandIndex("Edit");
            if (editIndex >= 0){
                root.productCommandsModel.SetData("IsEnabled", saveExists, editIndex);
            }

            let removeIndex = root.getProductCommandIndex("Remove");
            if (removeIndex >= 0){
                root.productCommandsModel.SetData("IsEnabled", saveExists, removeIndex);
            }

            if (root.addProductButton != null){
                root.addProductButton.enabled = saveExists;
                root.addProductButton.visible = saveExists;
            }

            let createLicenseExists = root.orderCommandsProvider.commandExists("CreateLicenseFile");

            let createLicenseIndex = root.getProductCommandIndex("CreateLicenseFile");
            if (createLicenseIndex >= 0){
                root.productCommandsModel.SetData("IsEnabled", createLicenseExists, createLicenseIndex);
            }
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
