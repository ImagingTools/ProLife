import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0

DocumentData {
    id: orderEditorContainer;

    property TreeItemModel accountsModel: TreeItemModel {}
    property TreeItemModel productsModel: TreeItemModel {}
    property TreeItemModel devicesModel: TreeItemModel {}
    property TreeItemModel licensesModel: TreeItemModel {}

    property string orderUuid: "";

    property alias licensesProviderLocal: licensesProvider;

    property bool serialNumberEdit: true;

    property int radius: 3;
    property int spacing: 25;

    documentCompleted: accountsList.completed && productsList.completed && devicesList.completed && licenseCollection.completed;

    Component.onCompleted: {
        licenseCollection.updateModel();
        accountsList.updateModel();
        devicesList.updateModel();
        productsList.updateModel();

        orderEditorContainer.undoManagerPtr.modelChanged.connect(beginDocumentModelChanged);
    }

    function beginDocumentModelChanged(){
        if (orderEditorContainer.documentModel.ContainsKey("OrderStatus")){
            let status = orderEditorContainer.documentModel.GetData("OrderStatus");
            if (status !== ""){
                let statusModel = stateMachine.getAvailableModel(status);
                orderStatusCB.model = statusModel;
            }
        }

        if (!orderStatusCB.model){
            orderStatusCB.model = orderStatus.statusModel;
        }

        if (!documentModel.ContainsKey("OrderProducts")){
            documentModel.AddTreeModel("OrderProducts")
        }

        if (documentModel.ContainsKey("OrderProducts")){
            productsView.model = documentModel.GetData("OrderProducts");
        }
    }

    onSaved: {
        console.log("saved");

        setBlockingUpdateModel(true);

        if (documentModel.ContainsKey("OrderProducts")){
            let orderProductsModel = documentModel.GetData("OrderProducts");

            for (let i = 0; i < orderProductsModel.GetItemsCount(); i++){
                let categoryId = orderProductsModel.GetData("CategoryId", i);
                if (categoryId === "Hardware"){
                    if (orderProductsModel.ContainsKey("IsNewDevice", i)){
                        orderProductsModel.RemoveData("IsNewDevice", i);
                    }
                }
            }
        }

        setBlockingUpdateModel(false);

        devicesList.updateModel();
    }

    onWidthChanged: {
        console.log("OrderEditor onWidthChanged", width);

        if (width > bodyColumn.width + productsView.width + productsView.anchors.leftMargin * 2){
            productsTitle.anchors.top = bodyColumn.top;
            productsTitle.anchors.topMargin = 0;
            productsTitle.anchors.left = bodyColumn.right;
            productsTitle.anchors.leftMargin = 25;
        }
        else{
            productsTitle.anchors.top = bodyColumn.bottom;
            productsTitle.anchors.topMargin = 15;
            productsTitle.anchors.left = orderEditorContainer.left;
            productsTitle.anchors.leftMargin = 0;
        }
    }

    function documentCanBeSaved(){
        let ok = true;

        if (!instanceIdInput.acceptableInput){
            ok = false;

            orderEditorContainer.documentManagerPtr.openErrorDialog(qsTr("Please enter a valid ERP Order-ID"));
        }

        return ok;
    }

    function updateOrderProductsModel(){
        let orderProductsModel = documentModel.GetData("OrderProducts");
        for (let j = 0; j < orderProductsModel.GetItemsCount(); j++){
            let categoryId = orderProductsModel.GetData("CategoryId", j);

            let hardwareModel = undefined;
            let index = j;

            if (categoryId === "Hardware"){
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

        onFailed: {
            if (orderEditorContainer.documentManagerPtr){
                let message = qsTr("Error loading accounts.");
                orderEditorContainer.documentManagerPtr.openErrorDialog(message);
            }
        }
    }

    CollectionDataProvider {
        id: productsList;
        fields: ["Id", "ProductId", "ProductName", "CategoryId", "Licenses"];
        commandId: "Products";

        onCollectionModelChanged: {
            if (productsList.collectionModel != null){
                orderEditorContainer.productsModel = productsList.collectionModel;
            }
        }

        onFailed: {
            if (orderEditorContainer.documentManagerPtr){
                let message = qsTr("Error loading products. Please check Lisa connection.");
                orderEditorContainer.documentManagerPtr.openErrorDialog(message);
            }
        }
    }

    CollectionDataProvider {
        id: devicesList;

        commandId: "Devices";

        fields: ["Id", "Name", "DeviceType", "OrderId", "OrderUuid", "Status", "MacAddress", "SerialNumber", "ProductUuid", "LicenseUuid", "LicenseId", "LicenseName"];

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

        onFailed: {
            if (orderEditorContainer.documentManagerPtr){
                let message = qsTr("Error loading sensors.");
                orderEditorContainer.documentManagerPtr.openErrorDialog(message);
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

    CollectionDataProvider{
        id: licenseCollection;

        commandId: "Licenses";

        fields: ["Id", "LicenseId", "LicenseName", "ProductId"]

        onCollectionModelChanged: {
            if (licenseCollection.collectionModel != null){
                orderEditorContainer.licensesModel = licenseCollection.collectionModel;
            }
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

    function blockEditing(){
        instanceIdInput.readOnly = true;
        purchaseIdInput.readOnly = true;
        descriptionInput.readOnly = true;
        customerCB.changeable = false;
        orderStatusCB.changeable = false;
        productsView.readOnly = true;
        buttonContainer.enabled = false;

        addProduct.visible = false;
    }

    function updateGui(){
        console.log("updateGui");

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

        let customerFound = false;
        if (documentModel.ContainsKey("CustomerId")){
            let customerId = documentModel.GetData("CustomerId");
            let customerModel = customerCB.model;
            for (let i = 0; i < customerModel.GetItemsCount(); i++){
                let id = customerModel.GetData("Id", i);
                if (id === customerId){
                    customerCB.currentIndex = i;
                    customerFound = true;
                    break;
                }
            }
        }

        if (!customerFound){
            customerCB.currentIndex = -1;
        }

        let statusFound = false;
        if (orderEditorContainer.documentModel.ContainsKey("OrderStatus")){
            let status = orderEditorContainer.documentModel.GetData("OrderStatus");
            let statusModel = stateMachine.getAvailableModel(status);
            if (statusModel){
                orderStatusCB.model = statusModel;
                for (let i = 0; i < statusModel.GetItemsCount(); i++){
                    let id = statusModel.GetData("Id", i);
                    if (id === status){
                        orderStatusCB.currentIndex = i;
                        statusFound = true;
                        break;
                    }
                }
            }
        }

        if (!statusFound){
            orderStatusCB.currentIndex = -1;
        }
    }

    function updateModel(){
        console.log("OrderEditor updateModel", documentModel.toJSON());

        documentModel.SetData("OrderId", instanceIdInput.text)
        documentModel.SetData("PurchaseId", purchaseIdInput.text)

        let selectedAccountId = "";
        if (customerCB.currentIndex >= 0 && customerCB.model){
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

        console.log("OrderEditor end updateModel", documentModel.toJSON());
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

        spacing: orderEditorContainer.spacing;

        Item {
            width: parent.width;
            height: errorInstanceId.visible ?
                        titleInstanceId.height + instanceIdInput.height + errorInstanceId.height + errorInstanceId.anchors.topMargin
                             : titleInstanceId.height + instanceIdInput.height;

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

                radius: orderEditorContainer.radius;

                borderColor: Style.iconColorOnSelected;

                onEditingFinished: {
                    orderEditorContainer.doUpdateModel();
                }

                KeyNavigation.tab: purchaseIdInput;

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ChangeOrder");
                    instanceIdInput.readOnly = !ok;
                }
            }

            Text {
                id: errorInstanceId;

                anchors.top: instanceIdInput.bottom;
                anchors.topMargin: 5;

                text: qsTr("Enter a five-digit number");

                visible: !instanceIdInput.acceptableInput;
                color: Style.errorTextColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }
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

                radius: orderEditorContainer.radius;

                placeHolderText: qsTr("Enter the Purchase-ID");

                borderColor: Style.iconColorOnSelected;

                onEditingFinished: {
                    orderEditorContainer.doUpdateModel();
                }

                KeyNavigation.tab: descriptionInput;

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ChangeOrder");
                    purchaseIdInput.readOnly = !ok;
                }
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

                radius: orderEditorContainer.radius;

                placeHolderText: qsTr("Enter the comment");

                borderColor: Style.iconColorOnSelected;

                onEditingFinished: {
                    orderEditorContainer.doUpdateModel();
                }

                KeyNavigation.tab: instanceIdInput;

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ChangeOrder");
                    descriptionInput.readOnly = !ok;
                }
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

                radius: orderEditorContainer.radius;

                onCurrentIndexChanged: {
                    orderEditorContainer.doUpdateModel();
                }

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ChangeOrder");

                    customerCB.changeable = ok;
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
                anchors.left: parent.left;

                width: parent.width - buttonContainer.width - 10;
                height: 23;

                radius: orderEditorContainer.radius;

                onCurrentIndexChanged: {
                    orderEditorContainer.doUpdateModel();

                    if (orderStatusCB.currentIndex < 0){
                        orderStatusCB.model = orderStatus.statusModel;
                    }
                }

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ChangeOrder");

                    orderStatusCB.changeable = ok;
                }
            }

            BaseButton{
                id: buttonContainer;

                anchors.top: orderStatusCB.top;
                anchors.right: parent.right;

                text: qsTr("Clear");

                decorator: defaultButtonDecorator;

                enabled: orderStatusCB.changeable;

                onClicked: {
                    if(orderEditorContainer.documentModel.ContainsKey("OrderStatus")){
                        if (orderStatusCB.currentIndex != -1){
                            orderStatusCB.currentIndex = -1;
                        }
                    }
                }

                Component{
                    id: defaultButtonDecorator;
                    CommonButtonDecorator{
                        width: 70;
                        height: 23;
                    }
                }
            }
        }
    }//Column bodyColumn

    UuidGenerator {
        id: uuidGenerator;
    }

    Component {
        id: productEditorDialog;

        ProductEditorDialog {
            id: productsDialog;

            onStarted: {
                productsDialog.bodyItem.productsModel = orderEditorContainer.productsModel;
                productsDialog.bodyItem.devicesModel = orderEditorContainer.devicesModel;
                productsDialog.bodyItem.licensesModel = orderEditorContainer.licensesModel;

                productsDialog.bodyItem.orderUuid = orderEditorContainer.documentId;
                productsDialog.bodyItem.serialNumberEdit = orderEditorContainer.serialNumberEdit;
                productsDialog.activeProductIndex = productsView.activeProductIndex;

                if (orderEditorContainer.documentModel.ContainsKey("OrderId")){
                    productsDialog.bodyItem.orderId = orderEditorContainer.documentModel.GetData("OrderId");
                }

                let orderProductsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
                productsDialog.bodyItem.orderProductsModel = orderProductsModel;
                if (productsView.activeProductIndex >= 0){
                    let productModel = orderProductsModel.GetModelFromItem(productsView.activeProductIndex);
                    productsDialog.bodyItem.productModel = productModel;
                }

                productsDialog.bodyItem.started();
            }

            onFinished: {
                if (buttonId == "Save"){
                    let productModel = productsDialog.bodyItem.productModel;
                    let actualOrderProducts = orderEditorContainer.documentModel.GetData("OrderProducts");

                    let index = productsView.activeProductIndex;
                    if (index < 0){
                        if (actualOrderProducts){
                            orderEditorContainer.undoManagerPtr.beginChanges();
                            index = actualOrderProducts.InsertNewItem();
                            actualOrderProducts.CopyItemDataFromModel(index, productModel);
                            orderEditorContainer.undoManagerPtr.endChanges();
                        }
                    }
                    else{
                        if (actualOrderProducts){
                            let actualProductModel = actualOrderProducts.GetModelFromItem(index);
                            let isEqual = actualProductModel.IsEqualWithModel(productModel);
                            if (!isEqual){
                                orderEditorContainer.undoManagerPtr.beginChanges();
                                actualOrderProducts.CopyItemDataFromModel(index, productModel);
                                orderEditorContainer.undoManagerPtr.endChanges();
                            }
                        }
                    }

                    actualOrderProducts.Refresh();
                }
            }
        }
    }

    CustomScrollbar {
        id: scrollbar;

        anchors.left: productsView.right;
        anchors.leftMargin: 5;
        anchors.top: productsView.top;
        anchors.bottom: productsView.bottom;

        backgroundColor: Style.baseColor;

        secondSize: 10;
        targetItem: productsView;

        radius: 2;
    }

    Rectangle {
        id: productsTitle;

        anchors.leftMargin: 25;

        width: productsView.width;
        height: 30;

        color: Style.alternateBaseColor;

        border.width: 1;
        border.color: Style.borderColor;

        radius: 3;

        AuxButton {
            id: addProduct;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: 10;

            height: 22;
            width: height;

            iconSource: "../../../" + Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal);

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

            Component.onCompleted: {
                let ok = PermissionsController.checkPermission("ChangeOrder");

                addProduct.visible = ok;
            }
        }

        Text {
            id: titleLicenses;

            anchors.left: parent.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            text: qsTr("Products");
            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;
        }
    }

    ListView {
        id: productsView;

        anchors.top: productsTitle.bottom;
        anchors.topMargin: productsView.spacing;
        anchors.left: productsTitle.left;
        anchors.leftMargin: productsView.leftMargin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 5;

        width: bodyColumn.width;

        clip: true;
        boundsBehavior: Flickable.StopAtBounds;
        spacing: 5;

        cacheBuffer: 1000;

        property int activeProductIndex: -1;

        property int selectedIndex: -1;

        property bool readOnly: false;
        property bool isLicenseConsuming: false;
        property bool softwareEditing: true;

        Component.onCompleted: {
            let ok = PermissionsController.checkPermission("ChangeOrder");

            productsView.readOnly = !ok;
        }

        delegate: OrderProductCard {
            id: orderProductDelegate;

            width: productsView.width;

            productsListView: productsView;

            productIndex: model.index;

            readOnly: productsView.readOnly;
            isLicenseConsuming: productsView.isLicenseConsuming;

            orderEditorPtr: orderEditorContainer;

            onEdited: {
                productsView.activeProductIndex = model.index;

                modalDialogManager.openDialog(productEditorDialog, {});
            }

            onRemoved: {
                productsView.activeProductIndex = model.index;

                modalDialogManager.openDialog(removeDialog, {"message": qsTr("Remove selected product ?")});
            }
        }
    }

    Component {
        id: removeDialog;

        MessageDialog {
            onFinished: {
                if (buttonId == "Yes"){
                    if (productsView.activeProductIndex < 0){
                        return;
                    }

                    let orderProducts = documentModel.GetData("OrderProducts")

                    orderEditorContainer.undoManagerPtr.beginChanges();

                    orderProducts.SetUpdateEnabled(true);
                    orderProducts.RemoveItem(productsView.activeProductIndex);
                    orderProducts.SetUpdateEnabled(false);

                    orderEditorContainer.undoManagerPtr.endChanges();
                }
            }
        }
    }
}//Container


