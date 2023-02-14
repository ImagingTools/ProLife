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
        updateGui();

        undoRedoManager.registerModel(documentModel)
    }

    //    onAccountsModelChanged: {
    //        console.log("onAccountsModelChanged", accountsModel);
    //        customerCB.model = accountsModel;
    //    }

    //    onProductsModelChanged: {
    //        console.log("onProductsModelChanged", productsModel);
    //    }
    TreeItemModel {
        id: statusModel;

        Component.onCompleted: {
            let index = statusModel.InsertNewItem();

            statusModel.SetData("Id", "None", index);
            statusModel.SetData("Name", "NONE", index);

            index = statusModel.InsertNewItem();

            statusModel.SetData("Id", "Created", index);
            statusModel.SetData("Name", "CREATED", index);

            index = statusModel.InsertNewItem();

            statusModel.SetData("Id", "InProgress", index);
            statusModel.SetData("Name", "IN_PROGRESS", index);

            index = statusModel.InsertNewItem();

            statusModel.SetData("Id", "Finished", index);
            statusModel.SetData("Name", "FINISHED", index);

            statusCB.model = statusModel;
        }
    }

//    ListModel {
//        id: statusModel;

//        ListElement {
//            Id: "NONE";
//            Name: qsTr("NONE");
//        }

//        ListElement {
//            Id: "CREATED";
//            Name: qsTr("CREATED");
//        }

//        ListElement {
//            Id: "IN_PROGRESS";
//            Name: qsTr("IN_PROGRESS");
//        }

//        ListElement {
//            Id: "CANCELED";
//            Name: qsTr("CANCELED");
//        }

//        ListElement {
//            Id: "ON_HOLD";
//            Name: qsTr("ON_HOLD");
//        }

//        ListElement {
//            Id: "ON_FINISHED";
//            Name: qsTr("ON_FINISHED");
//        }

//        ListElement {
//            Id: "ON_CLOSED";
//            Name: qsTr("ON_CLOSED");
//        }
//    }

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
            serialNumberInput.text = deviceEditorContainer.documentModel.GetData("Description");
        }

        if (deviceEditorContainer.documentModel.ContainsKey("OrderId")){
            serialNumberInput.text = deviceEditorContainer.documentModel.GetData("OrderId");
        }

        if (deviceEditorContainer.documentModel.ContainsKey("SerialNumber")){
            serialNumberInput.text = deviceEditorContainer.documentModel.GetData("SerialNumber");
        }

        if (deviceEditorContainer.documentModel.ContainsKey("MacAddress")){
            macAddressInput.text = deviceEditorContainer.documentModel.GetData("MacAddress");
        }

        if (deviceEditorContainer.documentModel.ContainsKey("Status")){
            let status = deviceEditorContainer.documentModel.GetData("Status");
            for (let i = 0; i < statusModel.GetItemsCount(); i++){
                let id = statusModel.GetData("Id", i);
                if (id === status){
                    statusCB.currentIndex = i;
                    break;
                }
            }
        }

        let productId = documentModel.GetData("ProductId");

        let productModel = productCB.model;
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            let id = productModel.GetData("Id", i);
            if (id === productId){
                productCB.currentIndex = i;
                break;
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

        let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
        deviceEditorContainer.documentModel.SetData("ProductId", selectedProductId);

        let description = descriptionInput.text;
        deviceEditorContainer.documentModel.SetData("Description", description);

        let serialNumber = serialNumberInput.text;
        deviceEditorContainer.documentModel.SetData("SerialNumber", serialNumber);

        let macAddress = macAddressInput.text;
        deviceEditorContainer.documentModel.SetData("MacAddress", macAddress);
        deviceEditorContainer.documentModel.SetData("Id", macAddress);
        deviceEditorContainer.documentModel.SetData("Name", macAddress);

        let index = statusCB.currentIndex;
        if (index >= 0){
            documentModel.SetData("Status", statusModel.GetData("Id", index));
        }

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
            id: titleDeviceName;
            text: qsTr("Device type");
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
                let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
                if (selectedProductId){
                    orderProductsModel.SetData("ProductId", selectedProductId, activeProductIndex);
                    bodyColumn.productCategory = productCB.model.GetData("CategoryId", productCB.currentIndex);
                    orderProductsModel.SetData("CategoryId",  bodyColumn.productCategory, activeProductIndex);
                }

                //                updatePairModel();
                //                if (bodyColumn.productCategory == "Software"){
                //                    updateHardwareCategoryProducts()
                //                }
                //                else{
                //                    updateSoftwareCategoryProducts()
                //                }

                console.log("InstallationEditor onCurrentIndexChanged",productCB.currentIndex, pairCB.model.toJSON());

                if (!deviceEditorContainer.blockUpdatingModel){
                    //   installationEditorContainer.updateModel();
                    deviceEditorContainer.updateGui();
                }
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
            id: titleOrderId;
            text: qsTr("Order ID");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextField {
            id: orderIdInput;

            width: parent.width;
            height: 30;

            readOnly: true;

            borderColor: Style.iconColorOnSelected;
        }

        Text {
            id: titleStatusId;
            text: qsTr("Status");
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
            text: qsTr("MAC-address");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextField {
            id: macAddressInput;

            width: parent.width;
            height: 30;

            placeHolderText: qsTr("Enter MAC-address");

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

} //Container


