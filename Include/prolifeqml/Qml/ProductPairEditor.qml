import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Item {
    id: productEditor;

    property string orderId;

    property string selectedProductId;
    property int margin: 10;

    property int contentHeight: bodyColumn.height;

    property alias collectionModel: hardwareProductCollection.collectionModel;
    property alias table: hardwareProductsTable;

    signal selectionChanged();

    onOrderIdChanged: {
        hardwareProductCollection.updateModel({"OrderId":productEditor.orderId});
    }

    CollectionDataProvider {
        id: hardwareProductCollection;

        commandId: "HardwareProducts";

        fields: ["Id", "ProductId", "MacAddress", "SerialNumber"]

        onModelUpdated: {
            console.log("hardwareProductCollection onModelUpdated", hardwareProductCollection.collectionModel.toJSON());

            hardwareProductsTable.elements = hardwareProductCollection.collectionModel;

            let elementsCount = hardwareProductCollection.collectionModel.GetItemsCount();
            if (elementsCount !== 0){
                message.visible = false;
                message.text = "";
            }
            else{
                message.visible = true;
                message.text = qsTr("There are no suitable products in this order");
            }
        }

        onStateModelChanged: {
            if (hardwareProductCollection.stateModel === "Loading"){
                loading.start();
            }
            else{
                loading.stop();
            }
        }
    }

    Column {
        id: bodyColumn;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        anchors.left: parent.left;
        anchors.rightMargin: productEditor.margin;
        anchors.leftMargin: productEditor.margin;

        spacing: 10;

        AuxTable {
            id: hardwareProductsTable;

            width: parent.width;
            height: 300;

            radius: 0;

            isMultiSelect: false;

            onSelectionChanged: {
                let indexes = hardwareProductsTable.getSelectedIndexes();
                if (indexes.length > 0){
                    let index = indexes[0];
                    let hardwareId = productEditor.collectionModel.GetData("Id", index);
                    let macAddress = productEditor.collectionModel.GetData("MacAddress", index);

                    if (macAddress === ""){
                        productEditor.selectedProductId = "";

                        message.visible = true;
                        message.text = qsTr("Unable to select this product, empty Mac Address");
                    }
                    else{
                        productEditor.selectedProductId = hardwareId;

                        message.visible = false;
                        message.text = "";
                    }
                }
                else{
                    productEditor.selectedProductId = "";

                    message.visible = false;
                    message.text = "";
                }

                productEditor.selectionChanged();
            }
        }

        BaseText {
            id: message;
            color: Style.errorTextColor;

            visible: false;
        }
    }

    Loading {
        id: loading;

        anchors.fill: parent;
    }

    TreeItemModel {
        id: headersModel;

        Component.onCompleted: {
            let index = headersModel.InsertNewItem();
            headersModel.SetData("Id", "ProductId", index);
            headersModel.SetData("Name", "Product", index);

            index = headersModel.InsertNewItem();
            headersModel.SetData("Id", "MacAddress", index);
            headersModel.SetData("Name", "MAC Address", index);

            index = headersModel.InsertNewItem();
            headersModel.SetData("Id", "SerialNumber", index);
            headersModel.SetData("Name", "Serial Number", index);

            hardwareProductsTable.headers = headersModel;
        }
    }
}//Container


