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

    property string orderUuid: "";

    property alias licensesProviderLocal: licensesProvider;

    property bool creatingLicenseFileFlag: false;

    Component.onCompleted: {
        licensesProvider.updateModel();

        accountsList.updateModel({});
    }

    Component {
        id: orderEditorCommandsDelegate;
        OrderEditorCommandsDelegate {}
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

    onSaved: {
        console.log("Order onSaved", orderEditorContainer.creatingLicenseFileFlag);
        if (orderEditorContainer.creatingLicenseFileFlag){
            let result = orderEditorContainer.createLicenseFile(productsView.activeProductIndex);
            if (result){
                orderEditorContainer.creatingLicenseFileFlag = false;
            }
        }
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

        fields: ["Id", "Name", "DeviceType", "OrderId", "Status", "MacAddress", "SerialNumber"];

        onModelUpdated: {
            if (devicesList.collectionModel != null){
                orderEditorContainer.devicesModel = devicesList.collectionModel;

                if (orderEditorContainer.documentModel.ContainsKey("OrderStatus")){
                    let status = orderEditorContainer.documentModel.GetData("OrderStatus");
                    let statusModel = stateMachine.getAvailableModel(status);
                    orderStatusCB.model = statusModel;
                }
                else{
                    orderStatusCB.model = orderStatus.statusModel;
                }

                orderEditorContainer.updateGui();
                undoRedoManager.registerModel(documentModel);
            }
        }

        function getMacAddress(deviceId){
            if (!deviceId){
                return null;
            }

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

        commandsId: orderEditorContainer.documentUuid;
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
        if (documentModel.ContainsKey("CustomerId")){
            let customerId = documentModel.GetData("CustomerId");
            let customerModel = customerCB.model;
            for (let i = 0; i < customerModel.GetItemsCount(); i++){
                let id = customerModel.GetData("Id", i);
                if (id === customerId){
                    customerCB.currentIndex = i;
                    break;
                }
            }
        }

        orderStatusCB.currentIndex = -1;
        if (orderEditorContainer.documentModel.ContainsKey("OrderStatus")){
            let status = orderEditorContainer.documentModel.GetData("OrderStatus");
            console.log("status",  status)
            let statusModel = stateMachine.getAvailableModel(status);
            if (statusModel){
                for (let i = 0; i < statusModel.GetItemsCount(); i++){
                    let id = statusModel.GetData("Id", i);
                    if (id === status){
                        orderStatusCB.currentIndex = i;
                        break;
                    }
                }
            }
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
        console.log("updateModel");

        if (orderEditorContainer.blockUpdatingModel){
            return;
        }

        undoRedoManager.beginChanges();

        documentModel.SetData("Id", orderEditorContainer.itemId);
        documentModel.SetData("OrderId", instanceIdInput.text)
        documentModel.SetData("Name", instanceIdInput.text);

        let selectedAccountId = "";
        if (customerCB.currentIndex >= 0){
            selectedAccountId = customerCB.model.GetData("Id", customerCB.currentIndex);
        }
        documentModel.SetData("CustomerId", selectedAccountId);

        if (orderStatusCB.currentIndex >= 0){
            let selectedStatus = orderStatusCB.model.GetData("Id", orderStatusCB.currentIndex);
            orderEditorContainer.documentModel.SetData("OrderStatus", selectedStatus);
        }
        else{
            orderEditorContainer.documentModel.SetData("OrderStatus", "");
        }

        documentModel.SetData("Description", descriptionInput.text);

        undoRedoManager.endChanges();
    }

    OrderStatus {
        id: orderStatus;
    }

    StateMachine {
        id: stateMachine;

        Component.onCompleted: {
            stateMachine.registerModel(orderStatus.statusModel);

            stateMachine.addState("None", ["None", "Created"]);
            stateMachine.addState("Created", ["Created", "InProgress", "Canceled", "OnHold"]);
            stateMachine.addState("InProgress", ["InProgress", "OnHold", "Finished"]);
            stateMachine.addState("Canceled", ["Canceled", "None", "Closed"]);
            stateMachine.addState("OnHold", ["OnHold", "Created", "InProgress"]);
            stateMachine.addState("Finished", ["Finished", "Closed"]);
            stateMachine.addState("Closed", ["Closed"]);
        }
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Column {
        id: bodyColumn;

        anchors.top: parent.top;
        //        anchors.bottom: productsView.top;

        width: 500;
        //        height: childrenRect.height;

        spacing: 15;

        Item {
            width: parent.width;
            height: titleInstanceId.height + instanceIdInput.height;

            RegExpValidator {
                id: regexValid;

                Component.onCompleted: {
                    console.log("RegExpValidator onCompleted");
                    let regex = instanceMaskProvider.getInstanceMask();
                    console.log("regex", regex);

                    let re = new RegExp(regex)
                    if (re){
                        regexValid.regExp = re;
                    }
                }
            }

            Text {
                id: titleInstanceId;
                text: qsTr("Order-ID");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: instanceIdInput;

                anchors.top: titleInstanceId.bottom;
                anchors.topMargin: 5;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the order-ID");

                borderColor: Style.iconColorOnSelected;

                onEditingFinished: {
                    let currentId = documentModel.GetData("OrderId");
                    if (currentId && currentId !== instanceIdInput.text || !currentId && instanceIdInput.text !== ""){
                        orderEditorContainer.updateModel();
                    }
                }

                KeyNavigation.tab: descriptionInput;
            }
        }

        Item {
            width: parent.width;
            height: titleComment.height + descriptionInput.height;

            Text {
                id: titleComment;
                text: qsTr("Description");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextEdit {
                id: descriptionInput;

                anchors.top: titleComment.bottom;
                anchors.topMargin: 5;

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

                KeyNavigation.tab: instanceIdInput;
            }
        }

        Item {
            width: parent.width;
            height: titleCustomer.height + customerCB.height;

            Text {
                id: titleCustomer;

                text: qsTr("Customer");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            ComboBox {
                id: customerCB;

                anchors.top: titleCustomer.bottom;
                anchors.topMargin: 5;

                width: parent.width;
                height: 23;

                radius: 3;

                onCurrentIndexChanged: {
                    orderEditorContainer.updateModel();
                }
            }
        }

        Item {
            width: parent.width;
            height: titleOrderStatus.height + orderStatusCB.height;

            Text {
                id: titleOrderStatus;

                text: qsTr("Order Status");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            ComboBox {
                id: orderStatusCB;

                anchors.top: titleOrderStatus.bottom;
                anchors.topMargin: 5;

                width: parent.width;
                height: 23;

                radius: 3;

                property bool blockingIndexChanged: false;

                onCurrentIndexChanged: {
                    if (orderStatusCB.blockingIndexChanged){
                        return;
                    }

                    if (orderStatusCB.currentIndex >= 0){
                        orderEditorContainer.updateModel();

                        let status = orderEditorContainer.documentModel.GetData("OrderStatus");
                        let statusModel = stateMachine.getAvailableModel(status);

                        orderStatusCB.model = statusModel;

                        orderStatusCB.blockingIndexChanged = true;
                        orderStatusCB.currentIndex = 0;
                        orderStatusCB.blockingIndexChanged = false;
                    }
                    else{
                        orderStatusCB.model = orderStatus.statusModel;
                    }
                }
            }
        }

        Item{
            anchors.left: parent.left;
            anchors.right: parent.right;

            height: 25;

            AuxButton {
                id: addProduct;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;

                height: 22;
                width: height;

                iconSource: "../../../Icons/" + Style.theme + "/Add_On_Normal.svg";

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
                anchors.verticalCenter: parent.verticalCenter;

                text: qsTr("Products");
                color: Style.textColor;
                font.family: Style.fontFamilyBold;
                font.pixelSize: Style.fontSize_common;
            }
        }
    }//Column bodyColumn

    function unlinkProducts(linkId){
        console.log("unlinkProducts", linkId);
        if (orderEditorContainer.documentModel.ContainsKey("OrderProducts")){
            let orderProductsModel = orderEditorContainer.documentModel.GetData("OrderProducts");

            console.log("orderProductsModel1", orderProductsModel.toJSON());

            let linkIndex = -1;
            for (let i = 0; i < orderProductsModel.GetItemsCount(); i++){
                let id = orderProductsModel.GetData("Id", i);
                if (id === linkId){
                    linkIndex = i;
                    break;
                }
            }

            if (linkIndex >= 0){
                let categoryId = orderProductsModel.GetData("CategoryId", linkIndex);
                if (categoryId === "Pair"){
                    let softwareProductModel = orderProductsModel.GetData("SoftwareProduct", linkIndex);
                    let index = orderProductsModel.InsertNewItem();
                    orderProductsModel.CopyItemDataFromModel(index, softwareProductModel);

                    let hardwareProductModel = orderProductsModel.GetData("HardwareProduct", linkIndex);
                    hardwareProductModel.SetData("PairId", "");
                    index = orderProductsModel.InsertNewItem();
                    orderProductsModel.CopyItemDataFromModel(index, hardwareProductModel);

                    orderProductsModel.RemoveItem(linkIndex);

                    console.log("orderProductsModel2", orderProductsModel.toJSON());

                    return true;
                }
            }


        }

        return false;
    }

    Component {
        id: productEditorDialog;

        ProductEditorDialog {
            id: productsDialog;
            licensesModel: licensesProvider.model;
            productsModel: orderEditorContainer.productsModel;

            orderUuid: orderEditorContainer.orderUuid;
            onStarted: {
                console.log("productsDialog.productModel", productsDialog.productModel.toJSON())
                productsDialog.isPairEditing = false;
                productsDialog.activeProductIndex = productsView.activeProductIndex;

                if (orderEditorContainer.documentModel.ContainsKey("OrderId")){
                    productsDialog.orderId = orderEditorContainer.documentModel.GetData("OrderId");
                }

                if (orderEditorContainer.documentModel.ContainsKey("OrderProducts")){
                    let orderProductsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
                    productsDialog.orderProductsModel.Copy(orderProductsModel);

                    if (productsView.activeProductIndex >= 0){
                        let productModel = productsDialog.orderProductsModel.GetModelFromItem(productsView.activeProductIndex);
                        if (productModel){
                            let category = productModel.GetData("CategoryId");
                            if (category === "Pair"){
                                productsDialog.isPairEditing = true;
                                if (productsView.softwareEditing){
                                    productModel = productModel.GetData("SoftwareProduct")
                                }
                                else{
                                    productModel = productModel.GetData("HardwareProduct")
                                }
                            }
                        }

                        productsDialog.productModel.Copy(productModel);
                    }
                }

                productsDialog.bodyItem.started();
            }

            onFinished: {
                if (buttonId == "Save"){
                    console.log("productsDialog.productModel", productsDialog.productModel);

                    productsDialog.bodyItem.updateModel()

                    let index = productsView.activeProductIndex;
                    if (productsDialog.isPairEditing){
                        console.log("productsDialog.productModel", productsDialog.productModel.toJSON());
                        if (index >= 0){
                            let pairUuid = productsDialog.orderProductsModel.GetData("Id", index);
                            let editingProductCategory = productsDialog.productModel.GetData("CategoryId");
                            let softwareProductUuid = "";
                            let hardwareProductUuid = "";
                            if (editingProductCategory === "Software"){
                                productsDialog.orderProductsModel.SetData("SoftwareProduct", productsDialog.productModel, index);
                                softwareProductUuid = productsDialog.productModel.GetData("Id");
                                hardwareProductUuid = productsDialog.bodyItem.findHardwarePair(softwareProductUuid);
                            }
                            else if (editingProductCategory === "Hardware"){
                                productsDialog.orderProductsModel.SetData("HardwareProduct", productsDialog.productModel, index);
                                hardwareProductUuid = productsDialog.productModel.GetData("Id");
                                softwareProductUuid = productsDialog.productModel.GetData("PairId");
                            }

                            console.log("productsDialog.orderProductsModel", productsDialog.orderProductsModel.toJSON());

                            if (!productsDialog.pairIsValid(pairUuid)){
                                console.log("!productsDialog.pairIsValid");

                                console.log("before unpairProducts", productsDialog.orderProductsModel.toJSON());
                                productsDialog.unpairProducts(pairUuid);

                                console.log("after unpairProducts", productsDialog.orderProductsModel.toJSON());

                                console.log("softwareProductUuid", softwareProductUuid);
                                console.log("hardwareProductUuid", hardwareProductUuid);
                                if (softwareProductUuid !== "" && hardwareProductUuid !== ""){
                                    productsDialog.createProductsPair(softwareProductUuid, hardwareProductUuid);
                                }
                            }

                            console.log("productsDialog.orderProductsModel", productsDialog.orderProductsModel.toJSON());
                        }
                    }
                    else{
                        if (index < 0){
                            index = productsDialog.orderProductsModel.InsertNewItem();
                        }

                        productsDialog.orderProductsModel.CopyItemDataFromModel(index, productsDialog.productModel);

                        let softwareId = "";
                        let hardwareId = "";

                        let categoryId = productsDialog.productModel.GetData("CategoryId")
                        let productId = productsDialog.productModel.GetData("Id")

                        if (categoryId === "Software"){
                            softwareId = productId;
                            hardwareId = productsDialog.bodyItem.findHardwarePair(softwareId);
                        }
                        else if (categoryId === "Hardware"){
                            hardwareId = productId;
                            softwareId = productsDialog.orderProductsModel.GetData("PairId", index);
                        }

                        if (softwareId && softwareId !== "" && hardwareId && hardwareId !== ""){
                            productsDialog.createProductsPair(softwareId, hardwareId);
                        }
                    }

                    if (!orderEditorContainer.documentModel.ContainsKey("OrderProducts")){
                        orderEditorContainer.documentModel.AddTreeModel("OrderProducts");
                    }

                    console.log("productsDialog.orderProductsModel", productsDialog.orderProductsModel.toJSON());

                    undoRedoManager.beginChanges();
                    let orderProductsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
                    orderProductsModel.Copy(productsDialog.orderProductsModel);
                    undoRedoManager.endChanges();

                    updateGui();


                    productsDialog.productModel.Clear();
                    productsDialog.orderProductsModel.Clear();
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

        property bool readOnly: false;
        property bool createLicenseOnly: false;

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

        function getMacAddressFromCurrentPair(){
            if (productsView.activeProductIndex >= 0){
                let categoryId = productsView.model.GetData("CategoryId", productsView.activeProductIndex);
                if (categoryId === "Pair"){
                    let hardwareProductModel = productsView.model.GetData("HardwareProduct", productsView.activeProductIndex);
                    if (hardwareProductModel){
                        let deviceId = hardwareProductModel.GetData("DeviceId");
                        let macAddress = devicesList.getMacAddress(deviceId);
                        return macAddress;
                    }
                }
            }

            return null;
        }

        OrderCommandsModelObserver {
            productCommandsModel: commandsModelLocal;
            orderCommandsProvider: orderEditorContainer.commandsProvider;
            addProductButton: addProduct;

            listView: productsView;
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

        property bool softwareEditing: true;

        delegate: OrderProductCard {
            id: orderProductDelegate;

            productIndex: model.index;
            devicesModel: orderEditorContainer.devicesModel;

            readOnly: productsView.readOnly;
            createLicenseOnly: productsView.createLicenseOnly;

            licensesProvider: orderEditorContainer.licensesProviderLocal;

            onEdited: {
                productsView.activeProductIndex = model.index;

                modalDialogManager.openDialog(productEditorDialog, {});
            }

            onRemoved: {
                productsView.activeProductIndex = model.index;

                modalDialogManager.openDialog(removeDialog, {"message": qsTr("Remove selected product ?")});
            }

            onUnlinked: {
                let linkId = model.Id;
                undoRedoManager.beginChanges();
                orderEditorContainer.unlinkProducts(linkId);
                undoRedoManager.endChanges();

                updateGui();
            }

            onCreateLicenseFile: {
                productsView.activeProductIndex = model.index;

                orderEditorContainer.createLicenseFile(model.index);
                //                if (model.CategoryId === "Pair"){
                //                    let orderUuid = orderEditorContainer.itemId;
                //                    if (orderUuid === "" || orderEditorContainer.isDirty){
                //                        modalDialogManager.openDialog(errorDialog, {"message": qsTr("To create a license, you need to save the current order. Save the order ?")});
                //                    }
                //                    else{
                //                        let productId = "";
                //                        if (model.HardwareProduct){
                //                            let hardwareProductModel = model.HardwareProduct;
                //                            productId = hardwareProductModel.GetData("Id");
                //                        }

                //                        if (productId !== ""){
                //                            licenseFileController.createLicenseFile(orderUuid + "/" + productId);
                //                        }
                //                    }
                //                }
            }

            onPairEdited: {
                productsView.activeProductIndex = model.index;

                if (categoryId === "Software"){
                    productsView.softwareEditing = true;
                }
                else{
                    productsView.softwareEditing = false;
                }

                modalDialogManager.openDialog(productEditorDialog, {});
            }
        }
    }

    LicenseFileController {
        id: licenseFileController;

        productProvider: productsView;
    }

    function createLicenseFile(pairIndex){
        console.log("createLicenseFile", pairIndex);
        if (pairIndex >= 0){
            let categoryId = productsView.model.GetData("CategoryId", pairIndex);
            if (categoryId === "Pair"){
                let orderUuid = orderEditorContainer.itemId;
                if (orderUuid === "" || orderEditorContainer.isDirty){
                    modalDialogManager.openDialog(errorDialog, {"message": qsTr("To create a license, you need to save the current order. Save the order ?")});
                }
                else{
                    let productId = "";
                    if (productsView.model.ContainsKey("HardwareProduct", pairIndex)){
                        let hardwareProductModel = productsView.model.GetData("HardwareProduct", pairIndex);
                        productId = hardwareProductModel.GetData("Id");
                    }

                    if (productId !== ""){
                        licenseFileController.createLicenseFile(orderUuid + "/" + productId);

                        return true;
                    }
                }
            }
        }

        return false;
    }

    Component {
        id: errorDialog;

        MessageDialog {
            title: qsTr("Save Order");
            onFinished: {
                if (buttonId == "Yes"){
                    Events.sendEvent(orderEditorContainer.documentUuid + "CommandActivated", "Save");

                    orderEditorContainer.creatingLicenseFileFlag = true;
                }
            }
        }
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


