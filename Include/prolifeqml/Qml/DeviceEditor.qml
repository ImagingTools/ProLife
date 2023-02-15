import QtQuick 2.12
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import Acf 1.0

DocumentBase {
    id: deviceEditorContainer;

    property TreeItemModel accountsModel: TreeItemModel {}
    property TreeItemModel productsModel: TreeItemModel {}

    property bool blockUpdatingModel: false;

    onDocumentModelChanged: {
        productsList.updateModel({}, ["Id", "Name", "Description"]);
    }

    CollectionDataProvider {
        id: productsList;

        commandId: "Products";

        onCollectionModelChanged: {
            if (productsList.collectionModel != null){
                productCB.model = productsList.collectionModel;
                ordersList.updateModel({}, ["OrderId", "Description"]);
            }
        }
    }

    CollectionDataProvider {
        id: ordersList;

        commandId: "Orders";

        onCollectionModelChanged: {
            if (ordersList.collectionModel != null){
                orderCB.model = ordersList.collectionModel;

                deviceEditorContainer.updateGui();

                undoRedoManager.registerModel(documentModel)
            }
        }
    }

    TreeItemModel {
        id: statusModel;

        Component.onCompleted: {
            let index = statusModel.InsertNewItem();

            statusModel.SetData("Id", "None", index);
            statusModel.SetData("Name", qsTr("None"), index);

            index = statusModel.InsertNewItem();

            statusModel.SetData("Id", "Accepted", index);
            statusModel.SetData("Name", qsTr("Accepted"), index);

            index = statusModel.InsertNewItem();

            statusModel.SetData("Id", "InProgress", index);
            statusModel.SetData("Name", qsTr("In Progress"), index);

            index = statusModel.InsertNewItem();

            statusModel.SetData("Id", "Canceled", index);
            statusModel.SetData("Name", qsTr("Canceled"), index);

            index = statusModel.InsertNewItem();

            statusModel.SetData("Id", "OnHold", index);
            statusModel.SetData("Name", qsTr("On Hold"), index);


            index = statusModel.InsertNewItem();

            statusModel.SetData("Id", "Finished", index);
            statusModel.SetData("Name", qsTr("Finished"), index);

            index = statusModel.InsertNewItem();

            statusModel.SetData("Id", "Closed", index);
            statusModel.SetData("Name", qsTr("Closed"), index);
            statusCB.model = statusModel;
        }
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: deviceEditorContainer.commandsId;
        documentBase: deviceEditorContainer;

        onModelStateChanged: {
            deviceEditorContainer.updateGui();
        }
    }

    MouseArea {
        anchors.fill: parent;

        onClicked: {
            deviceEditorContainer.forceActiveFocus();
        }
    }

    function updateGui(){
        console.log("DeviceEditor begin updateGui");
        deviceEditorContainer.blockUpdatingModel = true;

        if (deviceEditorContainer.documentModel.ContainsKey("Description")){
            descriptionInput.text = deviceEditorContainer.documentModel.GetData("Description");
        }

        if (deviceEditorContainer.documentModel.ContainsKey("OrderId")){
            orderInput.text = deviceEditorContainer.documentModel.GetData("OrderId");
        }

        if (deviceEditorContainer.documentModel.ContainsKey("SerialNumber")){
            serialNumberInput.text = deviceEditorContainer.documentModel.GetData("SerialNumber");
        }

        if (deviceEditorContainer.documentModel.ContainsKey("MacAddress")){
            macAddressInput.text = deviceEditorContainer.documentModel.GetData("MacAddress");
        }

        statusCB.currentIndex = -1;
        if (deviceEditorContainer.documentModel.ContainsKey("ProductionStatus")){
            let status = deviceEditorContainer.documentModel.GetData("ProductionStatus");
            statusCB.currentIndex = status;
        }

        productCB.currentIndex = -1;
        if (deviceEditorContainer.documentModel.ContainsKey("DeviceType")){
            let productId = deviceEditorContainer.documentModel.GetData("DeviceType");
            let productModel = productCB.model;
            for (let i = 0; i < productModel.GetItemsCount(); i++){
                let id = productModel.GetData("Id", i);
                if (id === productId){
                    productCB.currentIndex = i;
                    break;
                }
            }
        }

        orderCB.currentIndex = -1;
        if (deviceEditorContainer.documentModel.ContainsKey("OrderId")){
            let orderId = deviceEditorContainer.documentModel.GetData("OrderId");
            let ordersModel = orderCB.model;
            for (let i = 0; i < ordersModel.GetItemsCount(); i++){
                let id = ordersModel.GetData("Id", i);
                if (id === orderId){
                    orderCB.currentIndex = i;
                    break;
                }
            }
        }

        deviceEditorContainer.blockUpdatingModel = false;
        console.log("DeviceEditor end updateGui");
    }

    function updateModel(){
        console.log("DeviceEditor begin updateModel");
        if (deviceEditorContainer.blockUpdatingModel){
            return;
        }

        undoRedoManager.beginChanges();

        if (productCB.currentIndex >= 0){
            let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
            deviceEditorContainer.documentModel.SetData("DeviceType", selectedProductId);
        }
        else{
            deviceEditorContainer.documentModel.SetData("DeviceType", "");
        }

        if (orderCB.currentIndex >= 0){
            let selectedOrderId = orderCB.model.GetData("OrderId", orderCB.currentIndex);
            deviceEditorContainer.documentModel.SetData("OrderId", selectedOrderId);
        }
        else{
            deviceEditorContainer.documentModel.SetData("OrderId", "");
        }

        let description = descriptionInput.text;
        deviceEditorContainer.documentModel.SetData("Description", description);

        let serialNumber = serialNumberInput.text;
        deviceEditorContainer.documentModel.SetData("SerialNumber", serialNumber);

        let macAddress = macAddressInput.text;
        deviceEditorContainer.documentModel.SetData("MacAddress", macAddress);
        deviceEditorContainer.documentModel.SetData("Id", macAddress);
        deviceEditorContainer.documentModel.SetData("Name", macAddress);

        documentModel.SetData("ProductionStatus", statusCB.currentIndex);

        undoRedoManager.endChanges();
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Column {
        id: bodyColumn;

        width: 500;
        height: childrenRect.height;

        spacing: 7;

        Text {
            id: titleDeviceInformationBlock;
            text: qsTr("Device information");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle { ////////////////////
            id: deviceInformationBlockBorders;

            width: parent.width;
            height: deviceInformationBlock.height + 25;

            color: "transparent";

            border.width: 1;
            border.color: Style.borderColor;

            Column {
                id: deviceInformationBlock;

                anchors.horizontalCenter: deviceInformationBlockBorders.horizontalCenter;
                anchors.verticalCenter: deviceInformationBlockBorders.verticalCenter;

                width: parent.width - 20;

                spacing: 7;

                Text {
                    id: titleDeviceName;
                    text: qsTr("Device Type");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }

                ComboBox {
                    id: productCB;

                    width: parent.width;
                    height: 23;

                    radius: 3;

                    model: deviceEditorContainer.productsModel;

                    onCurrentIndexChanged: {
                        deviceEditorContainer.updateModel();
                    }
                }

                Text {
                    id: titleDescriptionId;
                    text: qsTr("Description");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }

                CustomTextField {
                    id: descriptionInput;

                    width: parent.width;
                    height: 60;

                    placeHolderText: qsTr("Enter description");

                    borderColor: Style.iconColorOnSelected;

                    onEditingFinished: {
                        let oldText = deviceEditorContainer.documentModel.GetData("Description");
                        if (oldText != descriptionInput.text){
                            updateModel();
                        }
                    }

                    KeyNavigation.tab: serialNumberInput;
                }

                Text {
                    id: titleSerialNumberId;
                    text: qsTr("Serial number");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }

                CustomTextField {
                    id: serialNumberInput;

                    width: parent.width;
                    height: 30;

                    placeHolderText: qsTr("Enter serial number");

                    borderColor: Style.iconColorOnSelected;

                    onEditingFinished: {
                        let oldText = deviceEditorContainer.documentModel.GetData("SerialNumber");
                        if (oldText != serialNumberInput.text){
                            updateModel();
                        }
                    }

                    KeyNavigation.tab: macAddressInput;
                }

                Text {
                    id: titleMacAddressId;
                    text: qsTr("MAC-Address");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }

                CustomTextField {
                    id: macAddressInput;

                    width: parent.width;
                    height: 30;

                    placeHolderText: qsTr("Enter MAC-Address");

                    borderColor: Style.iconColorOnSelected;

                    onEditingFinished: {
                        let oldText = documentModel.GetData("MacAddress");
                        if (oldText != macAddressInput.text){
                            updateModel();
                        }
                    }

                    KeyNavigation.tab: deviceNameInput;
                }
            }
        }

        Text {
            id: titleAdditonalInfoId;
            text: qsTr("Additional information");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: additionalInfoBorders;

            width: parent.width;
            height: additionalInfoBlock.height + 25;

            color: "transparent";

            border.width: 1;
            border.color: Style.borderColor;

            Column {
                id: additionalInfoBlock;

                anchors.horizontalCenter: additionalInfoBorders.horizontalCenter;
                anchors.verticalCenter: additionalInfoBorders.verticalCenter;

                width: parent.width - 20;

                spacing: 7;

                Text {
                    id: titleStatusId;
                    text: qsTr("Production Status");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }

                ComboBox {
                    id: statusCB;

                    width: parent.width;
                    height: 23;

                    radius: 3;

                    onCurrentIndexChanged: {
                        deviceEditorContainer.updateModel();
                    }
                }

                Text {
                    id: titleOrderId;
                    text: qsTr("Order ID");
                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }

                ComboBox {
                    id: orderCB;

                    width: parent.width;
                    height: 23;

                    radius: 3;

                    nameId: "OrderId";

                    onCurrentIndexChanged: {
                        deviceEditorContainer.updateModel();
                    }
                }
            }
        }
    }
}


