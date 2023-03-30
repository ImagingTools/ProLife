import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Dialog {
    id: root;

    width: 500;

    property TreeItemModel productModel: TreeItemModel{}

    property TreeItemModel documentModel: TreeItemModel{}
    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}
    property TreeItemModel orderProductsModel: TreeItemModel{}
    property int activeProductIndex: -1;

    property string orderId;
    property string orderUuid;

    property bool isPairEditing: false;

    UuidGenerator {
        id: uuidGenerator;
    }

    Component.onCompleted: {
        root.buttons.addButton({"Id": "Save", "Name": "Ok", "Enabled": false});
        root.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        root.title = qsTr("Product editor");
    }

    function unpairProducts(pairId){
        console.log("\nunpairProducts");
            let orderProductsModel = root.orderProductsModel;

            let linkIndex = -1;
            for (let i = 0; i < orderProductsModel.GetItemsCount(); i++){
                let id = orderProductsModel.GetData("Id", i);
                if (id === pairId){
                    linkIndex = i;
                    break;
                }
            }

            if (linkIndex >= 0){
                let categoryId = orderProductsModel.GetData("CategoryId", linkIndex);
                if (categoryId === "Pair"){
                    let softwareProductModel = orderProductsModel.GetData("SoftwareProduct", linkIndex);
                    console.log("\nsoftwareProductModel", softwareProductModel.toJSON());
                    let index = orderProductsModel.InsertNewItem();
//                    orderProductsModel.CopyItemDataFromModel(index, softwareProductModel);

                    let softwareKeys = softwareProductModel.GetKeys();
                    for (let i = 0; i < softwareKeys.length; i++){
                        orderProductsModel.SetData(softwareKeys[i], softwareProductModel.GetData(softwareKeys[i]), index);
                    }

                    console.log("\norderProductsModel insert softwareProductModel", orderProductsModel.toJSON());

                    let hardwareProductModel = orderProductsModel.GetData("HardwareProduct", linkIndex);
                    console.log("\nhardwareProductModel", hardwareProductModel.toJSON());
                    hardwareProductModel.SetData("PairId", "");
                    index = orderProductsModel.InsertNewItem();

                    let hardwareKeys = hardwareProductModel.GetKeys();
                    for (let i = 0; i < hardwareKeys.length; i++){
                        orderProductsModel.SetData(hardwareKeys[i], hardwareProductModel.GetData(hardwareKeys[i]), index);
                    }

//                    orderProductsModel.CopyItemDataFromModel(index, hardwareProductModel);

                    console.log("\norderProductsModel insert hardwareProductModel", orderProductsModel.toJSON());

                    orderProductsModel.RemoveItem(linkIndex);

                    console.log("\norderProductsModel RemoveItem", orderProductsModel.toJSON());

                    return true;
                }
            }

        return false;
    }

    function pairIsValid(pairId){
        console.log("pairIsValid", pairId);
        for (let i = 0; i < root.orderProductsModel.GetItemsCount(); i++){
            let id = root.orderProductsModel.GetData("Id", i);
            if (id === pairId){
                let hardwareProductModel = root.orderProductsModel.GetData("HardwareProduct", i);
                let softwareProductModel = root.orderProductsModel.GetData("SoftwareProduct", i);

                let hardwarePairId = hardwareProductModel.GetData("PairId");
                let softwareId = softwareProductModel.GetData("Id");
                console.log("hardwarePairId", hardwarePairId);
                console.log("softwareId", softwareId);
                if (hardwarePairId === softwareId){
                    return true;
                }
            }
        }

        return false;
    }

    function createProductsPair(softwareId, hardwareId){
         console.log("createProductsPair", softwareId, hardwareId);
        console.log("start root.orderProductsModel", root.orderProductsModel.toJSON());
        let softwareIndex = -1;
        let softwareModel = null;

        for (let i = 0; i < root.orderProductsModel.GetItemsCount(); i++){
            let categoryId = root.orderProductsModel.GetData("CategoryId", i);
            if (categoryId && categoryId !== "Pair"){
                let productId = root.orderProductsModel.GetData("Id", i);
                if (productId === softwareId){
                    softwareIndex = i;
                    softwareModel = root.orderProductsModel.GetModelFromItem(i);

                    console.log("softwareModel 1 ", softwareModel.toJSON());

                    break;
                }
            }
        }

        if (softwareIndex > -1){
            root.orderProductsModel.RemoveItem(softwareIndex);
        }

        console.log("softwareModel 2", softwareModel.toJSON());

        let hardwareIndex = -1;
        let hardwareModel = null;

        for (let i = 0; i < root.orderProductsModel.GetItemsCount(); i++){
            let categoryId = root.orderProductsModel.GetData("CategoryId", i);
            if (categoryId && categoryId !== "Pair"){
                let productId = root.orderProductsModel.GetData("Id", i);
                if (productId === hardwareId){
                    hardwareIndex = i;
                    root.orderProductsModel.SetData("PairId", softwareId, i);
                    hardwareModel = root.orderProductsModel.GetModelFromItem(i);
                    break;
                }
            }
        }

        if (hardwareIndex > -1){
            root.orderProductsModel.RemoveItem(hardwareIndex);
        }

        if (softwareModel != null && hardwareModel != null){
            console.log("softwareModel", softwareModel.toJSON());
            console.log("hardwareModel", hardwareModel.toJSON());

            console.log("root.orderProductsModel 1 ", root.orderProductsModel.toJSON());

            let index = root.orderProductsModel.InsertNewItem();

            root.orderProductsModel.SetData("Id", uuidGenerator.generateUUID(), index);
            root.orderProductsModel.SetData("CategoryId", "Pair", index);

            let emptySoftwareModel = root.orderProductsModel.AddTreeModel("SoftwareProduct", index);
            let softwareKeys = softwareModel.GetKeys();
            for (let i = 0; i < softwareKeys.length; i++){
                emptySoftwareModel.SetData(softwareKeys[i], softwareModel.GetData(softwareKeys[i]));
            }

            console.log("root.orderProductsModel 2", root.orderProductsModel.toJSON());

            let emptyHardwareModel = root.orderProductsModel.AddTreeModel("HardwareProduct", index);

            let hardwareKeys = hardwareModel.GetKeys();
            for (let i = 0; i < hardwareKeys.length; i++){
                emptyHardwareModel.SetData(hardwareKeys[i], hardwareModel.GetData(hardwareKeys[i]));
            }
            //emptyHardwareModel.Copy(hardwareModel);

            console.log("root.orderProductsModel END ", root.orderProductsModel.toJSON());
        }
    }

    contentComp: Component {
        id: installationEditor;

        ProductEditor {
            licensesModel: root.licensesModel;
            productsModel: root.productsModel;
            orderProductsModel: root.orderProductsModel;
            orderId: root.orderId;
            orderUuid: root.orderUuid;
            width: root.width - 100;
            height: 350;

            rootItem: root;

            productModel: root.productModel;
        }
    }
}//Container


