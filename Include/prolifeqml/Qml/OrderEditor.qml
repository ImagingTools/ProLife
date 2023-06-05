import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import Acf 1.0

DocumentBase {
    id: orderEditorContainer;

    property TreeItemModel accountsModel: TreeItemModel {}
    property TreeItemModel productsModel: TreeItemModel {}
    property TreeItemModel devicesModel: TreeItemModel {}

    property string orderUuid: "";

    property alias licensesProviderLocal: licensesProvider;

    property bool creatingLicenseFileFlag: false;

    property bool modelsIsLoaded: accountsList.completed && productsList.completed && devicesList.completed && licensesProvider.completed && orderEditorContainer.modelIsReady;

    onModelsIsLoadedChanged: {
        if (orderEditorContainer.modelsIsLoaded){
            orderEditorContainer.blockUpdatingModel = true;

            if (!documentModel.ContainsKey("OrderProducts")){
                documentModel.AddTreeModel("OrderProducts")
            }

            orderEditorContainer.updateGui();
            undoRedoManager.registerModel(documentModel);
        }
    }

    Component.onCompleted: {
        licensesProvider.updateModel();

        accountsList.updateModel({});
        productsList.updateModel({});
        devicesList.updateModel({});
    }

    onSaved: {
        console.log("saved");
        if (orderEditorContainer.creatingLicenseFileFlag){
            let result = orderEditorContainer.createLicenseFile(productsView.activeProductIndex);
            if (result){
                orderEditorContainer.creatingLicenseFileFlag = false;
            }
        }

        orderEditorContainer.blockUpdatingModel = true;
        if (documentModel.ContainsKey("OrderProducts")){
            let orderProductsModel = documentModel.GetData("OrderProducts");

            for (let i = 0; i < orderProductsModel.GetItemsCount(); i++){
                let categoryId = orderProductsModel.GetData("CategoryId", i);
                if (categoryId === "Pair"){
                    let hardwareModel = orderProductsModel.GetData("HardwareProduct", i);
                    if (hardwareModel.ContainsKey("IsNewDevice")){
                        hardwareModel.RemoveData("IsNewDevice");
                    }
                }
                else if (categoryId === "Hardware"){
                    if (orderProductsModel.ContainsKey("IsNewDevice", i)){
                        orderProductsModel.RemoveData("IsNewDevice", i);
                    }
                }
            }
        }

        devicesList.updateModel();

        orderEditorContainer.blockUpdatingModel = false;
    }

    function documentCanBeSaved(){
        let ok = true;

        let orderProductsModel = documentModel.GetData("OrderProducts");
        for (let j = 0; j < orderProductsModel.GetItemsCount(); j++){
            let categoryId = orderProductsModel.GetData("CategoryId", j);

            let hardwareModel = undefined;
            let index = j;
            if (categoryId === "Pair"){
                hardwareModel = orderProductsModel.GetData("HardwareProduct", j);
                index = 0;
            }
            else if (categoryId === "Hardware"){
                hardwareModel = orderProductsModel;
                index = j;
            }

            if (hardwareModel){
                if (hardwareModel.ContainsKey("DeviceNotExists", index)){
                    ok = false;
                    break;
                }
            }
        }

        if (!ok){
            let message = qsTr("Sensor detection error. Please select a new sensor.");
            orderEditorContainer.documentManager.openErrorDialog(message);

            if (orderEditorContainer.creatingLicenseFileFlag){
                orderEditorContainer.creatingLicenseFileFlag = false;
            }
        }

        return ok;
    }

    function updateOrderProductsModel(){
        let orderProductsModel = documentModel.GetData("OrderProducts");
        for (let j = 0; j < orderProductsModel.GetItemsCount(); j++){
            let categoryId = orderProductsModel.GetData("CategoryId", j);

            let hardwareModel = undefined;
            let index = j;
            if (categoryId === "Pair"){
                hardwareModel = orderProductsModel.GetData("HardwareProduct", j);
                index = 0;
            }
            else if (categoryId === "Hardware"){
                hardwareModel = orderProductsModel;
                index = j;
            }

            if (hardwareModel){
                let deviceId = hardwareModel.GetData("DeviceId", index);
                let isNew = hardwareModel.GetData("IsNewDevice", index);
                let deviceIdFound = false;
                if (!isNew){
                    for (let i = 0; i < orderEditorContainer.devicesModel.GetItemsCount(); i++){
                        let id = orderEditorContainer.devicesModel.GetData("Id", i);
                        if (deviceId === id){
                            deviceIdFound = true;
                            let macAddress = orderEditorContainer.devicesModel.GetData("MacAddress", i);
                            hardwareModel.SetData("MacAddress", macAddress, index);

                            let serialNumber = orderEditorContainer.devicesModel.GetData("SerialNumber", i);
                            hardwareModel.SetData("SerialNumber", serialNumber, index);

                            break;
                        }
                    }
                }

                if (isNew){
                    hardwareModel.SetData("MacAddress", "", index);
                    hardwareModel.SetData("SerialNumber", "", index);
                }

                if (!deviceIdFound && !isNew){
                    hardwareModel.SetData("DeviceNotExists", true, index);
                    hardwareModel.SetData("MacAddress", "", index);
                    hardwareModel.SetData("SerialNumber", "", index);
                }
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
            }
        }
    }

    CollectionDataProvider {
        id: devicesList;

        commandId: "Devices";

        fields: ["Id", "Name", "DeviceType", "OrderId", "OrderUuid", "Status", "MacAddress", "SerialNumber"];

        onModelUpdated: {
            if (devicesList.collectionModel != null){
                orderEditorContainer.devicesModel = devicesList.collectionModel;

                if (orderEditorContainer.documentModel.ContainsKey("OrderStatus")){
                    let status = orderEditorContainer.documentModel.GetData("OrderStatus");
                    if (status !== ""){
                        let statusModel = stateMachine.getAvailableModel(status);
                        orderStatusCB.model = statusModel;

                        return;
                    }
                }

                orderStatusCB.model = orderStatus.statusModel;
            }
        }

//        onCompletedChanged: {
//            orderEditorContainer.documentManager.documentLoading = !devicesList.completed;
//        }

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

            return "";
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

            return "";
        }
    }

    UndoRedoManager {
        id: undoRedoManager;

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

    function onCommandsModelLoaded(){
        let saveExists = orderEditorContainer.commandsProvider.commandExists("Save");
        if (!saveExists){
            orderEditorContainer.blockEditing();
        }

        let createLicenseExists = orderEditorContainer.commandsProvider.commandExists("CreateLicenseFile");
        if (createLicenseExists){
            productsView.isLicenseConsuming = true;
        }
    }

    function blockEditing(){
        instanceIdInput.readOnly = true;
        descriptionInput.readOnly = true;
        customerCB.changeable = false;
        orderStatusCB.changeable = false;
        productsView.readOnly = true;

        addProduct.visible = false;
    }

    function updateGui(){
        console.log("updateGui");
       // orderEditorContainer.documentManager.showLoading = true;
        orderEditorContainer.blockUpdatingModel = true;

        if (documentModel.ContainsKey("OrderId")){
            instanceIdInput.text = documentModel.GetData("OrderId");
        }
        else{
            instanceIdInput.text = "";
        }

        if (documentModel.ContainsKey("PurchaseId")){
            purchaseIdInput.text = documentModel.GetData("PurchaseId");
        }
        else{
            purchaseIdInput.text = "";
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

            orderEditorContainer.updateOrderProductsModel();
            productsView.model = 0
            productsView.model = documentModel.GetData("OrderProducts");
        }
        else{
            productsView.model = 0;
        }

        orderEditorContainer.blockUpdatingModel = false;
    }

    function updateModel(){
        if (orderEditorContainer.blockUpdatingModel){
            return;
        }

        undoRedoManager.beginChanges();

        documentModel.SetData("OrderId", instanceIdInput.text)
        documentModel.SetData("PurchaseId", purchaseIdInput.text)
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

        if (!documentModel.ContainsKey("OrderProducts")){
            documentModel.AddTreeModel("OrderProducts")
        }

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

        width: 500;

        spacing: 15;

        Item {
            width: parent.width;
            height: titleInstanceId.height + instanceIdInput.height;

            RegExpValidator {
                id: regexValid;

                Component.onCompleted: {
                    let regex = "\\d{5}";

                    let re = new RegExp(regex)
                    if (re){
                        regexValid.regExp = re;
                        instanceIdInput.textInputValidator = regexValid;
                    }
                }
            }

            Text {
                id: titleInstanceId;
                text: qsTr("ERP Order-ID");
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

                placeHolderText: qsTr("Enter the ERP Order-ID");

//                maximumLength: 5;

                borderColor: Style.iconColorOnSelected;

                onEditingFinished: {
                    let currentId = documentModel.GetData("OrderId");
                    if (currentId && currentId !== instanceIdInput.text || !currentId && instanceIdInput.text !== ""){
                        orderEditorContainer.updateModel();
                    }
                }

                KeyNavigation.tab: purchaseIdInput;
            }
        }

        Text {
            id: errorInstanceId;
            text: qsTr("Enter a five-digit number");

            visible: !instanceIdInput.acceptableInput;
            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        Item {
            width: parent.width;
            height: titleInstanceId.height + instanceIdInput.height;

            Text {
                id: titlePurchaseId;
                text: qsTr("Purchase Order-ID");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: purchaseIdInput;

                anchors.top: titlePurchaseId.bottom;
                anchors.topMargin: 5;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the Purchase-ID");

                borderColor: Style.iconColorOnSelected;

                onEditingFinished: {
                    let currentId = documentModel.GetData("PurchaseId");
                    if (currentId && currentId !== purchaseIdInput.text || !currentId && purchaseIdInput.text !== ""){
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

//                tooltipText: qsTr("Add a new product");

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

    function unlinkProducts(pairIndex){
        if (orderEditorContainer.documentModel.ContainsKey("OrderProducts")){
            let orderProductsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
            if (pairIndex >= 0){
                let categoryId = orderProductsModel.GetData("CategoryId", pairIndex);
                if (categoryId === "Pair"){
                    let softwareProductModel = orderProductsModel.GetData("SoftwareProduct", pairIndex);
                    softwareProductModel.SetData("PairId", "");
                    let index = orderProductsModel.InsertNewItem();
                    orderProductsModel.CopyItemDataFromModel(index, softwareProductModel);

                    let hardwareProductModel = orderProductsModel.GetData("HardwareProduct", pairIndex);
                    hardwareProductModel.SetData("PairId", "");
                    index = orderProductsModel.InsertNewItem();
                    orderProductsModel.CopyItemDataFromModel(index, hardwareProductModel);

                    orderProductsModel.RemoveItem(pairIndex);
                    return true;
                }
            }
        }

        return false;
    }

    UuidGenerator {
        id: uuidGenerator;
    }

    Component {
        id: productEditorDialog;

        ProductEditorDialog {
            id: productsDialog;

            function unpairProducts(pairId){
                let orderProductsModel = productsDialog.bodyItem.orderProductsModel;

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
                        let index = orderProductsModel.InsertNewItem();

                        let softwareKeys = softwareProductModel.GetKeys();
                        for (let i = 0; i < softwareKeys.length; i++){
                            orderProductsModel.SetData(softwareKeys[i], softwareProductModel.GetData(softwareKeys[i]), index);
                        }

                        let hardwareProductModel = orderProductsModel.GetData("HardwareProduct", linkIndex);
                        hardwareProductModel.SetData("PairId", "");
                        index = orderProductsModel.InsertNewItem();

                        let hardwareKeys = hardwareProductModel.GetKeys();
                        for (let i = 0; i < hardwareKeys.length; i++){
                            orderProductsModel.SetData(hardwareKeys[i], hardwareProductModel.GetData(hardwareKeys[i]), index);
                        }

                        orderProductsModel.RemoveItem(linkIndex);

                        return true;
                    }
                }

                return false;
            }

            function pairIsValid(pairId){
                let orderProductsModel = productsDialog.bodyItem.orderProductsModel;

                for (let i = 0; i < orderProductsModel.GetItemsCount(); i++){
                    let id = orderProductsModel.GetData("Id", i);
                    if (id === pairId){
                        let hardwareProductModel = orderProductsModel.GetData("HardwareProduct", i);
                        let softwareProductModel = orderProductsModel.GetData("SoftwareProduct", i);

                        let hardwarePairId = hardwareProductModel.GetData("PairId");
                        let softwareId = softwareProductModel.GetData("Id");

                        if (hardwarePairId === softwareId){
                            return true;
                        }
                    }
                }

                return false;
            }

            function createProductsPair(softwareId, hardwareId){
                let softwareIndex = -1;
                let softwareModel = null;

                for (let i = 0; i < productsDialog.bodyItem.orderProductsModel.GetItemsCount(); i++){
                    let categoryId = productsDialog.bodyItem.orderProductsModel.GetData("CategoryId", i);
                    if (categoryId && categoryId !== "Pair"){
                        let productId = productsDialog.bodyItem.orderProductsModel.GetData("Id", i);
                        if (productId === softwareId){
                            softwareIndex = i;
                            softwareModel = productsDialog.bodyItem.orderProductsModel.GetModelFromItem(i);

                            break;
                        }
                    }
                }

                if (softwareIndex > -1){
                    productsDialog.bodyItem.orderProductsModel.RemoveItem(softwareIndex);
                }

                let hardwareIndex = -1;
                let hardwareModel = null;

                for (let i = 0; i < productsDialog.bodyItem.orderProductsModel.GetItemsCount(); i++){
                    let categoryId = productsDialog.bodyItem.orderProductsModel.GetData("CategoryId", i);
                    if (categoryId && categoryId !== "Pair"){
                        let productId = productsDialog.bodyItem.orderProductsModel.GetData("Id", i);
                        if (productId === hardwareId){
                            hardwareIndex = i;
                            productsDialog.bodyItem.orderProductsModel.SetData("PairId", softwareId, i);
                            hardwareModel = productsDialog.bodyItem.orderProductsModel.GetModelFromItem(i);
                            break;
                        }
                    }
                }

                if (hardwareIndex > -1){
                    productsDialog.bodyItem.orderProductsModel.RemoveItem(hardwareIndex);
                }

                if (softwareModel != null && hardwareModel != null){
                    let index = productsDialog.bodyItem.orderProductsModel.InsertNewItem();

                    productsDialog.bodyItem.orderProductsModel.SetData("Id", uuidGenerator.generateUUID(), index);
                    productsDialog.bodyItem.orderProductsModel.SetData("CategoryId", "Pair", index);

                    let emptySoftwareModel = productsDialog.bodyItem.orderProductsModel.AddTreeModel("SoftwareProduct", index);
                    let softwareKeys = softwareModel.GetKeys();
                    for (let i = 0; i < softwareKeys.length; i++){
                        emptySoftwareModel.SetData(softwareKeys[i], softwareModel.GetData(softwareKeys[i]));
                    }

                    let emptyHardwareModel = productsDialog.bodyItem.orderProductsModel.AddTreeModel("HardwareProduct", index);

                    let hardwareKeys = hardwareModel.GetKeys();
                    for (let i = 0; i < hardwareKeys.length; i++){
                        emptyHardwareModel.SetData(hardwareKeys[i], hardwareModel.GetData(hardwareKeys[i]));
                    }
                }
            }

            onStarted: {
                productsDialog.bodyItem.productsModel = orderEditorContainer.productsModel;
                productsDialog.bodyItem.licensesModel = licensesProvider.model;
                productsDialog.bodyItem.orderUuid = orderEditorContainer.itemId;

                productsDialog.isPairEditing = false;
                productsDialog.activeProductIndex = productsView.activeProductIndex;

                if (orderEditorContainer.documentModel.ContainsKey("OrderId")){
                    productsDialog.bodyItem.orderId = orderEditorContainer.documentModel.GetData("OrderId");
                }

                let orderProductsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
                productsDialog.bodyItem.orderProductsModel.Copy(orderProductsModel);
                if (productsView.activeProductIndex >= 0){
                    let productModel = orderProductsModel.GetModelFromItem(productsView.activeProductIndex);
                    if (productModel){
                        let category = productModel.GetData("CategoryId");
                        if (category === "Pair"){
                            productsDialog.isPairEditing = true;
                            productsDialog.unlink(productsView.activeProductIndex);
                            if (productsView.softwareEditing){
                                productModel = productModel.GetData("SoftwareProduct")
                            }
                            else{
                                productModel = productModel.GetData("HardwareProduct")
                            }
                        }
                    }

                    productsDialog.bodyItem.productModel.Copy(productModel);
                }

                productsDialog.bodyItem.devicesModel = orderEditorContainer.devicesModel;

                productsDialog.bodyItem.started();
            }

            function unlink(pairIndex){
                let model = productsDialog.bodyItem.orderProductsModel;
                let categoryId = model.GetData("CategoryId", pairIndex);
                if (categoryId === "Pair"){
                    let hardwareModel = model.GetData("HardwareProduct", pairIndex);
                    let softwareModel = model.GetData("SoftwareProduct", pairIndex);

                    let newHardwareModel = hardwareModel.CopyMe();
                    let newSoftwareModel = softwareModel.CopyMe();

                    model.RemoveItem(pairIndex);

                    model.InsertNewItem(pairIndex);

                    if (productsView.softwareEditing){
                        model.CopyItemDataFromModel(pairIndex, newSoftwareModel);
                    }
                    else{
                        model.CopyItemDataFromModel(pairIndex, newHardwareModel);
                    }

                    let lastIndex = model.InsertNewItem();

                    if (productsView.softwareEditing){
                        model.CopyItemDataFromModel(lastIndex, newHardwareModel);
                    }
                    else{
                        model.CopyItemDataFromModel(lastIndex, newSoftwareModel);
                    }
                }
            }

            onFinished: {
                if (buttonId == "Save"){
                    let orderProductsModel = productsDialog.bodyItem.orderProductsModel;
                    let productModel = productsDialog.bodyItem.productModel;

                    let index = productsView.activeProductIndex;
                    if (index < 0){
                        index = orderProductsModel.InsertNewItem();
                    }

                    orderProductsModel.CopyItemDataFromModel(index, productModel);
                    let id = productModel.GetData("Id");
                    let pairId = productModel.GetData("PairId");
                    if (pairId && pairId !== ""){
                        for (let i = 0; i < orderProductsModel.GetItemsCount(); i++){
                            let externId = orderProductsModel.GetData("Id", i);
                            if (pairId === externId){
                                let secondPairId = orderProductsModel.GetData("PairId", i);
                                if (secondPairId === id){
                                    productsDialog.createPair(id, externId);
                                }
                            }
                        }
                    }

                    let actualOrderProducts = orderEditorContainer.documentModel.GetData("OrderProducts");
                    let isEqual = orderProductsModel.IsEqualWithModel(actualOrderProducts);
                    if (!isEqual){
                        actualOrderProducts.Copy(orderProductsModel);
                        undoRedoManager.makeChanges();
                        orderEditorContainer.modelChanged();
                        updateGui();
                    }
                }
            }

            function createPair(firstId, secondId){
                let firstIndex = -1;
                let secondIndex = -1;

                let orderProductsModel = productsDialog.bodyItem.orderProductsModel;
                for (let i = 0; i < orderProductsModel.GetItemsCount(); i++){
                    let id = orderProductsModel.GetData("Id", i);
                    if (id === firstId){
                        firstIndex = i;
                        if (secondIndex >= 0){
                            break;
                        }
                    }

                    if (id === secondId){
                        secondIndex = i;
                        if (firstIndex >= 0){
                            break;
                        }
                    }
                }

                if (firstIndex >= 0 && secondIndex >= 0){
                    let firstModel = orderProductsModel.GetModelFromItem(firstIndex);
                    let secondModel = orderProductsModel.GetModelFromItem(secondIndex);

                    if (firstIndex < secondIndex){
                        secondIndex -= 1;
                    }

                    orderProductsModel.RemoveItem(firstIndex);
                    orderProductsModel.RemoveItem(secondIndex);

                    let lastIndex = orderProductsModel.InsertNewItem();

                    orderProductsModel.SetData("Id", uuidGenerator.generateUUID(), lastIndex);
                    orderProductsModel.SetData("CategoryId", "Pair", lastIndex);

                    let firstModelCategory = firstModel.GetData("CategoryId");
                    let secondModelCategory = secondModel.GetData("CategoryId");

                    if (firstModelCategory !== secondModelCategory){
                        if (firstModelCategory === "Software"){
                            orderProductsModel.SetData("SoftwareProduct", firstModel, lastIndex);
                        }
                        else{
                            orderProductsModel.SetData("SoftwareProduct", secondModel, lastIndex);
                        }

                        if (firstModelCategory === "Software"){
                            orderProductsModel.SetData("HardwareProduct", secondModel, lastIndex);
                        }
                        else{
                            orderProductsModel.SetData("HardwareProduct", firstModel, lastIndex);
                        }
                    }
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

        cacheBuffer: 1000;

        property int activeProductIndex: -1;

        property int selectedIndex: -1;

        property bool readOnly: false;
        property bool isLicenseConsuming: false;
        property bool softwareEditing: true;

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

        delegate: OrderProductCard {
            id: orderProductDelegate;

            width: productsView.width;

            productsListView: productsView;

            productIndex: model.index;
            devicesModel: orderEditorContainer.devicesModel;

            readOnly: productsView.readOnly;
            isLicenseConsuming: productsView.isLicenseConsuming;

            licensesProvider: orderEditorContainer.licensesProviderLocal;
            orderEditorPtr: orderEditorContainer;

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
                orderEditorContainer.unlinkProducts(model.index);
                undoRedoManager.endChanges();

                updateGui();
            }

            onCreateLicenseFile: {
                productsView.activeProductIndex = model.index;
                orderEditorContainer.createLicenseFile(model.index);
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


