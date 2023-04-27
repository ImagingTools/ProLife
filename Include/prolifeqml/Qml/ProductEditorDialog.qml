import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Dialog {
    id: productEditorDialog;

    width: 500;

    property TreeItemModel productModel: TreeItemModel {}

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
        productEditorDialog.buttons.addButton({"Id": "Save", "Name": "Ok", "Enabled": false});
        productEditorDialog.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        productEditorDialog.title = qsTr("Product editor");
    }

    onProductModelChanged: {
        console.log("ProductEditorDialog onProductModelChanged", productEditorDialog.productModel);
    }

    function unpairProducts(pairId){
        console.log("\nunpairProducts");
            let orderProductsModel = productEditorDialog.orderProductsModel;

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
        for (let i = 0; i < productEditorDialog.orderProductsModel.GetItemsCount(); i++){
            let id = productEditorDialog.orderProductsModel.GetData("Id", i);
            if (id === pairId){
                let hardwareProductModel = productEditorDialog.orderProductsModel.GetData("HardwareProduct", i);
                let softwareProductModel = productEditorDialog.orderProductsModel.GetData("SoftwareProduct", i);

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
        console.log("start productEditorDialog.orderProductsModel", productEditorDialog.orderProductsModel.toJSON());
        let softwareIndex = -1;
        let softwareModel = null;

        for (let i = 0; i < productEditorDialog.orderProductsModel.GetItemsCount(); i++){
            let categoryId = productEditorDialog.orderProductsModel.GetData("CategoryId", i);
            if (categoryId && categoryId !== "Pair"){
                let productId = productEditorDialog.orderProductsModel.GetData("Id", i);
                if (productId === softwareId){
                    softwareIndex = i;
                    softwareModel = productEditorDialog.orderProductsModel.GetModelFromItem(i);

                    console.log("softwareModel 1 ", softwareModel.toJSON());

                    break;
                }
            }
        }

        if (softwareIndex > -1){
            productEditorDialog.orderProductsModel.RemoveItem(softwareIndex);
        }

        console.log("softwareModel 2", softwareModel.toJSON());

        let hardwareIndex = -1;
        let hardwareModel = null;

        for (let i = 0; i < productEditorDialog.orderProductsModel.GetItemsCount(); i++){
            let categoryId = productEditorDialog.orderProductsModel.GetData("CategoryId", i);
            if (categoryId && categoryId !== "Pair"){
                let productId = productEditorDialog.orderProductsModel.GetData("Id", i);
                if (productId === hardwareId){
                    hardwareIndex = i;
                    productEditorDialog.orderProductsModel.SetData("PairId", softwareId, i);
                    hardwareModel = productEditorDialog.orderProductsModel.GetModelFromItem(i);
                    break;
                }
            }
        }

        if (hardwareIndex > -1){
            productEditorDialog.orderProductsModel.RemoveItem(hardwareIndex);
        }

        if (softwareModel != null && hardwareModel != null){
            console.log("softwareModel", softwareModel.toJSON());
            console.log("hardwareModel", hardwareModel.toJSON());

            console.log("productEditorDialog.orderProductsModel 1 ", productEditorDialog.orderProductsModel.toJSON());

            let index = productEditorDialog.orderProductsModel.InsertNewItem();

            productEditorDialog.orderProductsModel.SetData("Id", uuidGenerator.generateUUID(), index);
            productEditorDialog.orderProductsModel.SetData("CategoryId", "Pair", index);

            let emptySoftwareModel = productEditorDialog.orderProductsModel.AddTreeModel("SoftwareProduct", index);
            let softwareKeys = softwareModel.GetKeys();
            for (let i = 0; i < softwareKeys.length; i++){
                emptySoftwareModel.SetData(softwareKeys[i], softwareModel.GetData(softwareKeys[i]));
            }

            console.log("productEditorDialog.orderProductsModel 2", productEditorDialog.orderProductsModel.toJSON());

            let emptyHardwareModel = productEditorDialog.orderProductsModel.AddTreeModel("HardwareProduct", index);

            let hardwareKeys = hardwareModel.GetKeys();
            for (let i = 0; i < hardwareKeys.length; i++){
                emptyHardwareModel.SetData(hardwareKeys[i], hardwareModel.GetData(hardwareKeys[i]));
            }
            //emptyHardwareModel.Copy(hardwareModel);

            console.log("productEditorDialog.orderProductsModel END ", productEditorDialog.orderProductsModel.toJSON());
        }
    }

    contentComp: Component {
        id: installationEditor;

        ProductEditor {
//            licensesModel: productEditorDialog.licensesModel;
//            productsModel: productEditorDialog.productsModel;
//            orderProductsModel: productEditorDialog.orderProductsModel;
//            orderId: productEditorDialog.orderId;
//            orderUuid: productEditorDialog.orderUuid;
            width: productEditorDialog.width - 100;
            height: 350;

            rootItem: productEditorDialog;

//            productModel: productEditorDialog.productModel;
        }
    }
}//Container


