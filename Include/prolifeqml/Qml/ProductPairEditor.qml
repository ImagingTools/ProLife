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

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", productEditor.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", productEditor.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        productEditor.updateHeaders();

        errorType.updateMessage();
    }

    QtObject {
        id: errorType;
        property int type: -1;

        onTypeChanged: {
            errorType.updateMessage();
        }

        function updateMessage(){
            if (errorType.type == 0){
                message.text = qsTr("There are no suitable products in this order");
            }
            else if (errorType.type == 1){
                message.text = qsTr("Unable to select this product, empty Mac Address");
            }
            else if (errorType.type < 0){
                message.text = "";
            }
        }
    }

    CollectionDataProvider {
        id: hardwareProductCollection;

        commandId: "HardwareProducts";

        fields: ["Id", "ProductId", "MacAddress", "SerialNumber"]

        onModelUpdated: {
            hardwareProductsTable.elements = hardwareProductCollection.collectionModel;

            let elementsCount = hardwareProductCollection.collectionModel.GetItemsCount();
            if (elementsCount !== 0){
                message.visible = false;
//                message.text = "";
                errorType.type = -1;
            }
            else{
                message.visible = true;
                errorType.type = 0;
//                message.text = qsTr("There are no suitable products in this order");
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
//                        message.text = qsTr("Unable to select this product, empty Mac Address");
                        errorType.type = 1;
                    }
                    else{
                        productEditor.selectedProductId = hardwareId;

                        message.visible = false;
                        errorType.type = -1;
                    }
                }
                else{
                    productEditor.selectedProductId = "";

                    message.visible = false;
                    errorType.type = -1;
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
            productEditor.updateHeaders();
        }
    }

    function updateHeaders(){
        headersModel.Clear();

        let index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "ProductId", index);
        headersModel.SetData("Name", qsTr("Product"), index);

        index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "MacAddress", index);
        headersModel.SetData("Name", qsTr("MAC Address"), index);

        index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "SerialNumber", index);
        headersModel.SetData("Name", qsTr("Serial Number"), index);

        hardwareProductsTable.headers = headersModel;
    }
}//Container


