import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Dialog {
    id: root;

    width: 500;

    property TreeItemModel documentModel: TreeItemModel{}
    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}
    property TreeItemModel orderProductsModel: TreeItemModel{}
    property int activeProductIndex: -1;

    property string orderId;
    property string orderUuid;

    UuidGenerator {
        id: uuidGenerator;
    }

    Component.onCompleted: {
        root.buttons.addButton({"Id": "Save", "Name": "Ok", "Enabled": false});
        root.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        root.title = qsTr("Product editor");
    }

    function createProductsPair(softwareId, hardwareId){
        let softwareIndex = -1;
        let softwareModel = null;

        for (let i = 0; i < root.orderProductsModel.GetItemsCount(); i++){
            let categoryId = root.orderProductsModel.GetData("CategoryId", i);
            if (categoryId && categoryId !== "Pair"){
                let productId = root.orderProductsModel.GetData("Id", i);
                if (productId === softwareId){
                    softwareIndex = i;
                    softwareModel = root.orderProductsModel.GetModelFromItem(i);
                    break;
                }
            }
        }

        if (softwareIndex > -1){
            root.orderProductsModel.RemoveItem(softwareIndex);
        }

        let hardwareIndex = -1;
        let hardwareModel = null;

        for (let i = 0; i < root.orderProductsModel.GetItemsCount(); i++){
            let categoryId = root.orderProductsModel.GetData("CategoryId", i);
            if (categoryId && categoryId !== "Pair"){
                let productId = root.orderProductsModel.GetData("Id", i);
                if (productId === hardwareId){
                    hardwareIndex = i;
                    hardwareModel = root.orderProductsModel.GetModelFromItem(i);
                    break;
                }
            }
        }

        if (hardwareIndex > -1){
            root.orderProductsModel.RemoveItem(hardwareIndex);
        }

        if (softwareModel != null && hardwareModel != null){
            let index = root.orderProductsModel.InsertNewItem();

            root.orderProductsModel.SetData("Id", uuidGenerator.generateUUID(), index);
            root.orderProductsModel.SetData("CategoryId", "Pair", index);

            let emptySoftwareModel = root.orderProductsModel.AddTreeModel("SoftwareProduct", index);
            emptySoftwareModel.Copy(softwareModel);

            let emptyHardwareModel = root.orderProductsModel.AddTreeModel("HardwareProduct", index);
            emptyHardwareModel.Copy(hardwareModel);
        }
    }

    contentComp: Component {
        id: installationEditor;
        InstallationEditor {

            licensesModel: root.licensesModel;
            productsModel: root.productsModel;
            orderProductsModel: root.orderProductsModel;
            activeProductIndex: root.activeProductIndex;
            orderId: root.orderId;
            orderUuid: root.orderUuid;
            width: root.width - 100;
            height: 350;

            rootItem: root;

            onActiveProductIndexChanged: {
                root.activeProductIndex = activeProductIndex;
            }
        }
    }
}//Container


