import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import Acf 1.0

DocumentBase {
    id: orderEditorContainer;

    // Загрузка моделей: Licenses -> Accounts -> Products -> Devices -> DocumentModel

    commandsDelegateSourceComp: orderEditorCommandsDelegate;

    property TreeItemModel accountsModel: TreeItemModel {}
    property TreeItemModel productsModel: TreeItemModel {}
    property TreeItemModel devicesModel: TreeItemModel {}

    property bool blockUpdatingModel: false;

    property string orderUuid: "";

    Component.onCompleted: {
        licensesProvider.updateModel();

        accountsList.updateModel({});
    }

    Component {
        id: orderEditorCommandsDelegate;
        OrderEditorCommandsDelegate {}
    }

    onOrderUuidChanged: {

    }

    onDocumentModelChanged: {
        updateGui();

        console.log("onDocumentModelChanged");
        if (documentModel.ContainsKey("Id")){

            orderEditorContainer.orderUuid = documentModel.GetData("Id");
            console.log("onDocumentModelChanged", orderEditorContainer.orderUuid);
        }

        undoRedoManager.registerModel(documentModel)
    }

    onProductsModelChanged: {
        console.log("onProductsModelChanged", productsModel);
    }

    CollectionDataProvider {
        id: accountsList;
        fields: ["Id", "Name"];
        commandId: "Accounts";

        onCollectionModelChanged: {
            if (accountsList.collectionModel != null){
                customerCB.model = accountsList.collectionModel;

                productsList.updateModel({});
            }
        }
    }

    CollectionDataProvider {
        id: productsList;
        fields: ["Id", "Name", "CategoryId"];
        commandId: "Products";

        onCollectionModelChanged: {
            if (productsList.collectionModel != null){
                orderEditorContainer.productsModel = productsList.collectionModel;

                devicesList.updateModel({});
            }
        }
    }

    CollectionDataProvider {
        id: devicesList;

        commandId: "Devices";

        fields: ["Id", "Name", "DeviceType", "OrderId", "ProductionStatus", "MacAddress", "SerialNumber"];

        onModelUpdated: {
            if (devicesList.collectionModel != null){

                let newIndex = devicesList.collectionModel.InsertNewItem(0);

                devicesList.collectionModel.SetData("Id", "", newIndex);
                devicesList.collectionModel.SetData("Name", "New Device", newIndex);

                orderEditorContainer.devicesModel = devicesList.collectionModel;

                orderEditorContainer.updateGui();
                undoRedoManager.registerModel(documentModel);
            }
        }

        function getMacAddress(deviceId){
            if (devicesList.collectionModel != null){
                for (let i = 0; i < devicesList.collectionModel.GetItemsCount(); i++){
                    let id = devicesList.collectionModel.GetData("Id", i);
                    if (id === deviceId){
                        let macAddress = devicesList.collectionModel.GetData("MacAddress", i);

                        return macAddress;
                    }
                }
            }

            return null;
        }

        function getSerialNumber(deviceId){
            if (devicesList.collectionModel != null){
                for (let i = 0; i < devicesList.collectionModel.GetItemsCount(); i++){
                    let id = devicesList.collectionModel.GetData("Id", i);
                    if (id === deviceId){
                        let serialNumber = devicesList.collectionModel.GetData("SerialNumber", i);

                        return serialNumber;
                    }
                }
            }

            return null;
        }
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: orderEditorContainer.commandsId;
        documentBase: orderEditorContainer;

        onModelStateChanged: {
            updateGui();
        }
    }

    LicensesProvider {
        id: licensesProvider;
    }

    MouseArea {
        anchors.fill: parent;

        onClicked: {
            orderEditorContainer.forceActiveFocus();
        }
    }

    function updateGui(){
        console.log("Begin updateGui");
        orderEditorContainer.blockUpdatingModel = true;

        if (documentModel.ContainsKey("OrderId")){
            instanceIdInput.text = documentModel.GetData("OrderId");
        }
        else{
            instanceIdInput.text = "";
        }

        if (documentModel.ContainsKey("Description")){
            descriptionInput.text = documentModel.GetData("Description");
        }
        else{
            descriptionInput.text = "";
        }

        customerCB.currentIndex = -1;
        let customerId = documentModel.GetData("CustomerId");
        let customerModel = customerCB.model;
        for (let i = 0; i < customerModel.GetItemsCount(); i++){
            let id = customerModel.GetData("Id", i);
            if (id === customerId){
                customerCB.currentIndex = i;
                break;
            }
        }

        orderStatusCB.currentIndex = -1;
        if (documentModel.ContainsKey("OrderStatus")){
            let orderStatus = documentModel.GetData("OrderStatus");

            orderStatusCB.currentIndex = orderStatus;
        }

        if (documentModel.ContainsKey("OrderProducts")){
            productsView.model = 0
            productsView.model = documentModel.GetData("OrderProducts");
            console.log("productsView.model",  productsView.model.toJSON())
        }
        else{
            productsView.model = 0;
        }

        orderEditorContainer.blockUpdatingModel = false;
        console.log("End updateGui");
    }

    function updateModel(){
        if (orderEditorContainer.blockUpdatingModel){
            return;
        }

        undoRedoManager.beginChanges();

        documentModel.SetData("OrderId", instanceIdInput.text)
        documentModel.SetData("Name", instanceIdInput.text);

        let selectedAccountId = customerCB.model.GetData("Id", customerCB.currentIndex);
        documentModel.SetData("CustomerId", selectedAccountId);

        if (orderStatusCB.currentIndex > -1){
            documentModel.SetData("OrderStatus", orderStatusCB.currentIndex);
        }

        documentModel.SetData("Description", descriptionInput.text);

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
            id: titleInstanceId;
            text: qsTr("Order-ID");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        //        RegExpValidator {
        //            id: regexValid;

        //            Component.onCompleted: {
        //                console.log("RegExpValidator onCompleted");
        //                let regex = settingsProvider.getInstanceMask();
        //                console.log("regex", regex);

        //                let re = new RegExp(regex)
        //                if (re){
        //                    regexValid.regExp = re;
        //                }
        //            }
        //        }

        CustomTextField {
            id: instanceIdInput;

            width: parent.width;
            height: 30;

            placeHolderText: qsTr("Enter the order-ID");

            borderColor: Style.iconColorOnSelected;

            onEditingFinished: {
                let currentId = documentModel.GetData("OrderId");
                if (currentId !== instanceIdInput.text && instanceIdInput.text !== ""){
                    orderEditorContainer.updateModel();
                }
            }
        }

        Text {
            id: titleComment;
            text: qsTr("Description");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextEdit {
            id: descriptionInput;

            width: parent.width;
            height: 60;

            placeHolderText: qsTr("Enter the comment");

            borderColor: Style.iconColorOnSelected;

            onEditingFinished: {
                let oldText = orderEditorContainer.documentModel.GetData("Description");
                if (oldText !== descriptionInput.text && descriptionInput.text !== ""){
                    orderEditorContainer.updateModel();
                }
            }
        }

        Text {
            id: titleCustomer;

            text: qsTr("Customer");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        ComboBox {
            id: customerCB;

            width: parent.width;
            height: 23;

            radius: 3;

            onCurrentIndexChanged: {
                updateModel();
            }
        }

        TreeItemModel {
            id: orderStatusModel;

            Component.onCompleted: {
                let index = orderStatusModel.InsertNewItem();

                // 0
                orderStatusModel.SetData("Id", "None", index);
                orderStatusModel.SetData("Name", qsTr("None"), index);

                index = orderStatusModel.InsertNewItem();

                // 1
                orderStatusModel.SetData("Id", "Created", index);
                orderStatusModel.SetData("Name", qsTr("Created"), index);

                index = orderStatusModel.InsertNewItem();

                // 2
                orderStatusModel.SetData("Id", "InProgress", index);
                orderStatusModel.SetData("Name", qsTr("In Progress"), index);

                index = orderStatusModel.InsertNewItem();

                // 3
                orderStatusModel.SetData("Id", "Canceled", index);
                orderStatusModel.SetData("Name", qsTr("Canceled"), index);

                index = orderStatusModel.InsertNewItem();

                // 4
                orderStatusModel.SetData("Id", "OnHold", index);
                orderStatusModel.SetData("Name", qsTr("On Hold"), index);

                index = orderStatusModel.InsertNewItem();

                // 5
                orderStatusModel.SetData("Id", "Finished", index);
                orderStatusModel.SetData("Name", qsTr("Finished"), index);

                index = orderStatusModel.InsertNewItem();

                // 6
                orderStatusModel.SetData("Id", "Closed", index);
                orderStatusModel.SetData("Name", qsTr("Closed"), index);

                orderStatusCB.model = orderStatusModel;
            }
        }

        Text {
            id: titleOrderStatus;

            text: qsTr("Order Status");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        ComboBox {
            id: orderStatusCB;

            width: parent.width;
            height: 23;

            radius: 3;

            onCurrentIndexChanged: {
                updateModel();
            }
        }

        Item{
            height: 35;
            anchors.left: parent.left;
            anchors.right: parent.right;

            AuxButton {
                id: addProduct;
                height: 25;
                width: 100;
                anchors.right: parent.right;
                anchors.bottom: parent.bottom;
                textButton: qsTr("Add product");
                hasText: true;
                backgroundColor: Style.baseColor;
                gradient: Gradient {
                    GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
                    GradientStop { position: 0.97; color: Style.imagingToolsGradient2; }
                    GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
                    GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
                }
                onClicked: {
                    productsView.activeProductIndex = -1;
                    modalDialogManager.openDialog(productEditorDialog, {});
                }
            }

            Text {
                id: titleLicenses;
                anchors.left: parent.left;
                anchors.bottom: parent.bottom;
                text: qsTr("Products");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }
        }
    }//Column bodyColumn

    Component {
        id: productEditorDialog;

        ProductEditorDialog {
            id: productsDialog;
            licensesModel: licensesProvider.model;
            productsModel: orderEditorContainer.productsModel;

            orderUuid: orderEditorContainer.orderUuid;
            onStarted: {
                productsDialog.orderProductsModel.Clear();
                if (orderEditorContainer.documentModel.ContainsKey("OrderProducts")){
                    productsDialog.activeProductIndex = productsView.activeProductIndex;
                    let orderProductsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
                    productsDialog.orderProductsModel.Copy(orderProductsModel);
                }

                if (orderEditorContainer.documentModel.ContainsKey("OrderId")){
                    productsDialog.orderId = orderEditorContainer.documentModel.GetData("OrderId");
                }

                productsDialog.bodyItem.started();
            }

            onFinished: {
                if (buttonId == "Save"){
                    productsDialog.bodyItem.updateModel()
                    undoRedoManager.beginChanges();

                    if (productsDialog.activeProductIndex > -1){
                        if (!orderEditorContainer.documentModel.ContainsKey("OrderProducts")){
                            orderEditorContainer.documentModel.AddTreeModel("OrderProducts");
                            //                            console.log("newProductsModel", orderEditorContainer.documentModel.toJSON());
                        }
                        let orderProductsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
                        orderProductsModel.Copy(productsDialog.orderProductsModel);
                    }

                    undoRedoManager.endChanges();
                    updateGui();
                }
            }
        }
    }

    CustomScrollbar {
        id: scrollbar;
        z: 100;

        anchors.left: productsView.right;
        anchors.leftMargin: 5;
        anchors.top: productsView.top;
        anchors.bottom: productsView.bottom;

        backgroundColor: Style.baseColor;

        secondSize: 10;
        targetItem: productsView;
    }

    ListView {
        id: productsView;
        anchors.top: bodyColumn.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        //        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 5;

        width: 500;

        clip: true;
        boundsBehavior: Flickable.StopAtBounds;
        spacing: 10;

        property int activeProductIndex: -1;

        property int selectedIndex: -1;

        Component.onCompleted: {
        }

        function getProductName(productId){
            let retVal = "";
            for (let i = 0; i < productsModel.GetItemsCount(); i++){
                let id = productsModel.GetData("Id", i);
                if (id === productId){
                    retVal = productsModel.GetData("Name", i);
                    break;
                }
            }
            return retVal;
        }

        function getProductCategory(productId){
            let retVal = "";
            for (let i = 0; i < productsModel.GetItemsCount(); i++){
                let id = productsModel.GetData("Id", i);
                if (id === productId){
                    retVal = productsModel.GetData("CategoryId", i);
                    break;
                }
            }
            return retVal;
        }

        function getPairName(index){
            let productsModel = productsView.model;
            let productId  = productsModel.GetData("ProductId", index);
            let id  = productsModel.GetData("Id", index);
            let pairId  = productsModel.GetData("PairId", index);
            let categoryId = getProductCategory(productId);

            let retVal = "";

            for (let i = 0; i < productsModel.GetItemsCount(); i++){
                ;
                if (categoryId == "Software" &&  productsModel.GetData("PairId", i) == id){
                    let modelProductId = productsModel.GetData("ProductId", i)
                    retVal = "#" + (i + 1) + " " + getProductName(modelProductId);
                    //                    let productMacAddress = productsModel.GetData("MacAddress", i)
                    //                    retVal += " (" + productMacAddress + ")";

                    break;
                }
                if (categoryId == "Hardware" && productsModel.GetData("Id", i) == pairId){
                    let modelProductId = productsModel.GetData("ProductId", i)
                    retVal = "#" + (i + 1) + " " + getProductName(modelProductId);

                    break;
                }
            }
            return retVal;
        }

        function getLicenseName(index){
            let productsModel = productsView.model;
            //            console.log("getLicenseName", productsModel.toJSON())
            console.log("getLicenseName", index)
            let retVal = "";
            let activeLicenses = productsModel.GetData("ActiveLicenses", index);
            let productId  = productsModel.GetData("ProductId", index);
            //            console.log("getLicenseName activeLicenses", activeLicenses.toJSON())
            if (!activeLicenses){
                return;
            }

            for (let licenseIndex = 0; licenseIndex < activeLicenses.GetItemsCount(); licenseIndex++){
                if (licenseIndex > 0){
                    retVal += ", ";
                }
                //                retVal += activeLicenses.GetData("Name", licenseIndex);
                retVal += licensesProvider.getLicenseName(productId, activeLicenses.GetData("Id", licenseIndex))
                retVal += " ";
                let expiration = activeLicenses.GetData("Expiration", licenseIndex);
                if (expiration == 0){
                    expiration = qsTr("Unlimited");
                }
                retVal += " " + expiration;

            };

            return retVal;
        }

        function getSoftwareIdByMacAddress(macAddress){
            let dataModel = documentModel.GetData("OrderProducts");

            for (let i = 0; i < dataModel.GetItemsCount(); i++){
                let category = dataModel.GetData("CategoryId", i);
                if (category === "Software"){
                    let address = dataModel.GetData("MacAddress", i);
                    if (address === macAddress){
                        let id = dataModel.GetData("Id", i);

                        return id;
                    }
                }
            }
        }

        function findHardwarePair(id){
            let retVal = ""
            let productsModel = productsView.model;
            for (let i = 0; i < productsModel.GetItemsCount(); i++){

                if (productsModel.GetData("CategoryId", i) === "Hardware"){
                    let pairId = productsModel.GetData("PairId", i)
                    if (!pairId){
                        //productsModel.SetData("PairId", "", i)
                        pairId = ""
                    }
                    if (pairId === id){
                        retVal = productsModel.GetData("Id", i)
                        break
                    }
                }

            }

            return retVal
        }

        function getIndexByPairId(pairId){
            let productsModel = productsView.model;
            for (let i = 0; i < productsModel.GetItemsCount(); i++){
                if (productsModel.GetData("Id", i) === pairId){
                    return i;
                }
            }

            return -1;
        }

        OrderCommandsModelObserver {
            productCommandsModel: commandsModelLocal;
            orderCommandsProvider: orderEditorContainer.commandsProvider;
            addProductButton: addProduct;
        }

        TreeItemModel {
            id: commandsModelLocal;

            Component.onCompleted: {
                let index = commandsModelLocal.InsertNewItem();

                commandsModelLocal.SetData("Id", "Edit", index);
                commandsModelLocal.SetData("Name", "Edit", index);
                commandsModelLocal.SetData("Icon", "Edit", index);
                commandsModelLocal.SetData("IsEnabled", false, index);
                commandsModelLocal.SetData("Visible", true, index);

                index = commandsModelLocal.InsertNewItem();

                commandsModelLocal.SetData("Id", "CreateLicenseFile", index);
                commandsModelLocal.SetData("Name", "Create License File", index);
                commandsModelLocal.SetData("Icon", "Key", index);
                commandsModelLocal.SetData("IsEnabled", false, index);
                commandsModelLocal.SetData("Visible", true, index);

                index = commandsModelLocal.InsertNewItem();

                commandsModelLocal.SetData("Id", "Remove", index);
                commandsModelLocal.SetData("Name", "Remove", index);
                commandsModelLocal.SetData("Icon", "Close", index);
                commandsModelLocal.SetData("IsEnabled", false, index);
                commandsModelLocal.SetData("Visible", true, index);
            }
        }

        delegate: OrderProductView {
            productName: "#" + (model.index + 1) + " " + productsView.getProductName(model.ProductId);
            productCategory: productsView.getProductCategory(model.ProductId);
            pairName: productsView.getPairName(model.index)
            macAddress: devicesList.getMacAddress(model.DeviceId);
            serialNumber: devicesList.getSerialNumber(model.DeviceId);
            licenseName: productsView.getLicenseName(model.index);

            commandsModel: commandsModelLocal;

            selected: productsView.selectedIndex === model.index;

            onEdited: {
                productsView.activeProductIndex = model.index;

                var productsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
                if (productsModel){
                    //                    let productModel = productsModel.GetModelFromItem(model.index);

                    //                    modalDialogManager.openDialog(productEditorDialog, {"documentModel": productModel});
                    //                    console.log("OrderProductView onEdited", productModel.toJSON())
                    modalDialogManager.openDialog(productEditorDialog, {});
                }
            }

            onRemoved: {
                productsView.activeProductIndex = model.index;

                modalDialogManager.openDialog(removeDialog, {"message": qsTr("Remove selected product ?")});
            }

            onClicked: {
                productsView.selectedIndex = model.index;
            }

            onCreateLicenseFile: {
                let orderId = documentModel.GetData("Id");
                let productId = model.Id;

                if (model.CategoryId === "Software"){
                    productId = productsView.findHardwarePair(productId);
                }
                if (productId){
                    console.log("onCreateLicenseFile", orderId + "/" + productId);
                    licenseFileController.createLicenseFile(orderId + "/" + productId);
                }
            }

            onPairClicked: {
                let id = model.PairId;
                if (model.CategoryId === "Software"){
                    id = productsView.findHardwarePair(model.Id);
                }

                let index = productsView.getIndexByPairId(id);
                if (index >= 0){
                    productsView.selectedIndex = index;
                    productsView.positionViewAtIndex(index, ListView.Center);
                }
            }
        }
    }

    LicenseFileController {
        id: licenseFileController;
    }

    Component {
        id: removeDialog;

        MessageDialog {
            onFinished: {
                if (buttonId == "Yes"){
                    productsView.model.RemoveItem(productsView.activeProductIndex);

                    orderEditorContainer.updateModel();
                }
            }
        }
    }

}//Container


