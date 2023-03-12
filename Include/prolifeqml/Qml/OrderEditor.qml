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

    Component.onCompleted: {
        licensesProvider.updateModel();

        accountsList.updateModel({});
    }

    Component {
        id: orderEditorCommandsDelegate;
        OrderEditorCommandsDelegate {}
    }

//    onItemIdChanged: {
//        if (orderEditorContainer.itemId !== ""){
//            if (orderEditorContainer.documentManager != null){
//                orderEditorContainer.documentManager.updateDocumentModel(orderEditorContainer.itemId, {});
//            }
//        }
//    }

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

        fields: ["Id", "Name", "DeviceType", "OrderId", "Status", "MacAddress", "SerialNumber"];

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
        console.log("updateModel");

        if (orderEditorContainer.blockUpdatingModel){
            return;
        }

        undoRedoManager.beginChanges();

        documentModel.SetData("OrderId", instanceIdInput.text)
        documentModel.SetData("Name", instanceIdInput.text);

        console.log("customerCB.currentIndex", customerCB.currentIndex);
        let selectedAccountId = "";
        if (customerCB.currentIndex >= 0){
            selectedAccountId = customerCB.model.GetData("Id", customerCB.currentIndex);

            console.log("customerCB.model", customerCB.model.toJSON());

            console.log("selectedAccountId", selectedAccountId);
        }
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

        spacing: 15;

        Item {
            width: parent.width;
            height: titleInstanceId.height + instanceIdInput.height;

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
                    if (currentId !== instanceIdInput.text && instanceIdInput.text !== ""){
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
                    updateModel();
                }
            }
        }

        Item {
            width: parent.width;
            height: titleOrderStatus.height + orderStatusCB.height;

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

                anchors.top: titleOrderStatus.bottom;
                anchors.topMargin: 5;

                width: parent.width;
                height: 23;

                radius: 3;

                onCurrentIndexChanged: {
                    updateModel();
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

//                tooltipText: qsTr("Add product to this order");

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

                    //                    let productsModel = documentModel.GetData("OrderProducts");
                    //                    if (!productsModel){
                    //                        productsModel = documentModel.AddTreeModel("OrderProducts")
                    //                    }

                    //                    productsModel.InsertNewItem();

                    //                    productsView.model = productsModel;
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

    UuidGenerator {
        id: uuidGenerator;
    }

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

            function createProductsPair(softwareId, hardwareId){
                let softwareIndex = -1;
                let softwareModel = null;

                for (let i = 0; i < productsDialog.orderProductsModel.GetItemsCount(); i++){
                    let categoryId = productsDialog.orderProductsModel.GetData("CategoryId", i);
                    if (categoryId && categoryId !== "Pair"){
                        let productId = productsDialog.orderProductsModel.GetData("Id", i);
                        if (productId === softwareId){
                            softwareIndex = i;
                            softwareModel = productsDialog.orderProductsModel.GetModelFromItem(i);
                            break;
                        }
                    }
                }

                if (softwareIndex > -1){
                    productsDialog.orderProductsModel.RemoveItem(softwareIndex);
                }

                let hardwareIndex = -1;
                let hardwareModel = null;

                for (let i = 0; i < productsDialog.orderProductsModel.GetItemsCount(); i++){
                    let categoryId = productsDialog.orderProductsModel.GetData("CategoryId", i);
                    if (categoryId && categoryId !== "Pair"){
                        let productId = productsDialog.orderProductsModel.GetData("Id", i);
                        if (productId === hardwareId){
                            hardwareIndex = i;
                            hardwareModel = productsDialog.orderProductsModel.GetModelFromItem(i);
                            break;
                        }
                    }
                }

                if (hardwareIndex > -1){
                    productsDialog.orderProductsModel.RemoveItem(hardwareIndex);
                }

                if (softwareModel != null && hardwareModel != null){
                    let index = productsDialog.orderProductsModel.InsertNewItem();

                    productsDialog.orderProductsModel.SetData("Id", uuidGenerator.generateUUID(), index);
                    productsDialog.orderProductsModel.SetData("CategoryId", "Pair", index);

                    let emptySoftwareModel = productsDialog.orderProductsModel.AddTreeModel("SoftwareProduct", index);
                    emptySoftwareModel.Copy(softwareModel);

                    let emptyHardwareModel = productsDialog.orderProductsModel.AddTreeModel("HardwareProduct", index);
                    emptyHardwareModel.Copy(hardwareModel);
                }
            }

            onFinished: {
                if (buttonId == "Save"){
                    productsDialog.bodyItem.updateModel()

                    let equals = false;
                    if (orderEditorContainer.documentModel.ContainsKey("OrderProducts")){
                        let orderProductsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
                        let firstModel = JSON.stringify(orderProductsModel)
                        let secondModel = JSON.stringify(productsDialog.orderProductsModel);
                        equals = _.isEqual(firstModel, secondModel);
                    }

                    if (!equals){
                        undoRedoManager.beginChanges();

                        if (productsDialog.activeProductIndex > -1){

                            console.log("SAVE");
                            if (!orderEditorContainer.documentModel.ContainsKey("OrderProducts")){
                                orderEditorContainer.documentModel.AddTreeModel("OrderProducts");
                            }

                            let softwareId = "";
                            let hardwareId = "";
                            let categoryId = productsDialog.orderProductsModel.GetData("CategoryId", productsDialog.activeProductIndex);
                            if (categoryId === "Software"){
                                softwareId = productsDialog.orderProductsModel.GetData("Id", productsDialog.activeProductIndex);
                                hardwareId = productsDialog.bodyItem.findHardwarePair(softwareId);
                            }
                            else if (categoryId === "Hardware"){
                                hardwareId = productsDialog.orderProductsModel.GetData("Id", productsDialog.activeProductIndex);
                                softwareId = productsDialog.orderProductsModel.GetData("PairId", productsDialog.activeProductIndex);
                            }

                            console.log("softwareId", softwareId);
                            console.log("hardwareId", hardwareId);

                            if (softwareId && softwareId !== "" && hardwareId && hardwareId !== ""){
                                productsDialog.createProductsPair(softwareId, hardwareId);
                            }

                            let orderProductsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
                            orderProductsModel.Copy(productsDialog.orderProductsModel);
                        }

                        undoRedoManager.endChanges();
                        updateGui();

                        //                        undoRedoManager.beginChanges();

                        //                        if (productsDialog.activeProductIndex > -1){
                        //                            if (!orderEditorContainer.documentModel.ContainsKey("OrderProducts")){
                        //                                orderEditorContainer.documentModel.AddTreeModel("OrderProducts");
                        //                                //                            console.log("newProductsModel", orderEditorContainer.documentModel.toJSON());
                        //                            }
                        //                            let orderProductsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
                        //                            orderProductsModel.Copy(productsDialog.orderProductsModel);
                        //                        }

                        //                        undoRedoManager.endChanges();

                        //                        updateGui();
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
        delegate: OrderProductCard {
            id: orderProductDelegate;

            productIndex: model.index;
            devicesModel: orderEditorContainer.devicesModel;

            readOnly: productsView.readOnly;
            createLicenseOnly: productsView.createLicenseOnly;

            licensesProvider: orderEditorContainer.licensesProviderLocal;

            onEdited: {
                productsView.activeProductIndex = model.index;

                var productsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
                if (productsModel){
                    modalDialogManager.openDialog(productEditorDialog, {});
                }
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
                if (model.CategoryId === "Pair"){
                    let productId = "";
                    if (model.HardwareProduct){
                        let hardwareProductModel = model.HardwareProduct;
                        productId = hardwareProductModel.GetData("Id");
                    }

                    if (productId !== ""){
                        let orderId = documentModel.GetData("Id");
                        console.log("onCreateLicenseFile", orderId + "/" + productId);
                        licenseFileController.createLicenseFile(orderId + "/" + productId);
                    }
                }
            }

            onPairEdited: {
                let productModel = model.SoftwareProduct;
                if (categoryId === "Hardware"){
                    productModel = model.HardwareProduct;
                }


            }
        }

        //        delegate: OrderProductView {
        //            productName: "#" + (model.index + 1) + " " + productsView.getProductName(model.ProductId);
        //            productCategory: productsView.getProductCategory(model.ProductId);
        //            pairName: productsView.getPairName(model.index)
        //            macAddress: devicesList.getMacAddress(model.DeviceId);
        //            serialNumber: devicesList.getSerialNumber(model.DeviceId);
        //            licenseName: productsView.getLicenseName(model.index);

        //            commandsModel: commandsModelLocal;

        //            selected: productsView.selectedIndex === model.index;

        //            onEdited: {
        //                productsView.activeProductIndex = model.index;

        //                var productsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
        //                if (productsModel){
        //                    //                    let productModel = productsModel.GetModelFromItem(model.index);

        //                    //                    modalDialogManager.openDialog(productEditorDialog, {"documentModel": productModel});
        //                    //                    console.log("OrderProductView onEdited", productModel.toJSON())
        //                    modalDialogManager.openDialog(productEditorDialog, {});
        //                }
        //            }

        //            onRemoved: {
        //                productsView.activeProductIndex = model.index;

        //                modalDialogManager.openDialog(removeDialog, {"message": qsTr("Remove selected product ?")});
        //            }

        //            onClicked: {
        //                productsView.selectedIndex = model.index;
        //            }

        //            onCreateLicenseFile: {
        //                let orderId = documentModel.GetData("Id");
        //                let productId = model.Id;

        //                if (model.CategoryId === "Software"){
        //                    productId = productsView.findHardwarePair(productId);
        //                }
        //                if (productId){
        //                    console.log("onCreateLicenseFile", orderId + "/" + productId);
        //                    licenseFileController.createLicenseFile(orderId + "/" + productId);
        //                }
        //            }

        //            onPairClicked: {
        //                let id = model.PairId;
        //                if (model.CategoryId === "Software"){
        //                    id = productsView.findHardwarePair(model.Id);
        //                }

        //                let index = productsView.getIndexByPairId(id);
        //                if (index >= 0){
        //                    productsView.selectedIndex = index;
        //                    productsView.positionViewAtIndex(index, ListView.Center);
        //                }
        //            }
        //        }
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


