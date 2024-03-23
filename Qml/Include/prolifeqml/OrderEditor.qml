import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import prolifeqml 1.0

ViewBase {
    id: orderEditorContainer;

    property TreeItemModel accountsModel: CachedAccountCollection.collectionModel;
    property TreeItemModel productsModel: CachedProductCollection.collectionModel;
    property TreeItemModel devicesModel: CachedDeviceCollection.collectionModel;
    property TreeItemModel licensesModel: CachedLicenseCollection.collectionModel

    property string orderUuid: "";

    property alias licensesProviderLocal: licensesProvider;

    property bool serialNumberEdit: true;

    property int radius: 3;
    property int spacing: 25;

    property int comboBoxHeight: 27;

    Component.onCompleted: {
        CachedAccountCollection.updateModel();
        CachedLicenseCollection.updateModel();
        CachedDeviceCollection.updateModel();
        CachedProductCollection.updateModel();
    }

    onWidthChanged: {
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

    LicensesProvider {
        id: licensesProvider;
    }

    function setReadOnly(readOnly){
        instanceIdInput.readOnly = readOnly;
        purchaseIdInput.readOnly = readOnly;
        descriptionInput.readOnly = readOnly;
        productsView.readOnly = readOnly;

        customerCB.changeable = !readOnly;
        orderStatusCB.changeable = !readOnly;
    }

    function updateGui(){
        if (model.ContainsKey("OrderId")){
            instanceIdInput.text = model.GetData("OrderId");
        }
        else{
            instanceIdInput.text = "";
        }

        if (model.ContainsKey("PurchaseId")){
            purchaseIdInput.text = model.GetData("PurchaseId");
        }
        else{
            purchaseIdInput.text = "";
        }

        if (model.ContainsKey("Description")){
            descriptionInput.text = model.GetData("Description");
        }
        else{
            descriptionInput.text = "";
        }

        let customerFound = false;
        if (model.ContainsKey("CustomerId")){
            let customerId = model.GetData("CustomerId");
            let customerModel = customerCB.model;
            if (customerModel){
                for (let i = 0; i < customerModel.GetItemsCount(); i++){
                    let id = customerModel.GetData("Id", i);
                    if (id === customerId){
                        customerCB.currentIndex = i;
                        customerFound = true;
                        break;
                    }
                }
            }
        }

        if (!customerFound){
            customerCB.currentIndex = -1;
        }

        let statusFound = false;
        if (orderEditorContainer.model.ContainsKey("OrderStatus")){
            let status = orderEditorContainer.model.GetData("OrderStatus");
//            let statusModel = stateMachine.getAvailableModel(status);
            let statusModel = stateMachine.stateModel;
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

        console.log("Order updateGui", model.toJSON());

        if (model.ContainsKey("OrderProducts")){
            productsView.model = model.GetTreeItemModel("OrderProducts");

            productsView.model.Refresh();
        }
        else{
            productsView.model = 0;
        }

        model.Refresh();
    }

    function updateModel(){
        console.log("Order updateModel");

        model.SetData("OrderId", instanceIdInput.text)
        model.SetData("PurchaseId", purchaseIdInput.text)

        let selectedAccountId = "";
        if (customerCB.currentIndex >= 0 && customerCB.model){
            selectedAccountId = customerCB.model.GetData("Id", customerCB.currentIndex);
        }

        model.SetData("CustomerId", selectedAccountId);

        if (orderStatusCB.currentIndex >= 0){
            let selectedStatus = orderStatusCB.model.GetData("Id", orderStatusCB.currentIndex);
            orderEditorContainer.model.SetData("OrderStatus", selectedStatus);
        }
        else{
            orderEditorContainer.model.SetData("OrderStatus", "");
        }

        model.SetData("Description", descriptionInput.text);

        if (!model.ContainsKey("OrderProducts")){
            model.AddTreeModel("OrderProducts")
        }
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

            RegularExpressionValidator {
                id: regexValid;

                Component.onCompleted: {
                    let regex = "\\d{5}";

                    let re = new RegExp(regex)
                    if (re){
                        regexValid.regularExpression = re;
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

                readOnly: orderEditorContainer.readOnly;

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
                readOnly: orderEditorContainer.readOnly;

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

            TextEditCustom {
                id: descriptionInput;

                anchors.top: titleComment.bottom;
                anchors.topMargin: 5;

                width: parent.width;
                height: 60;

                radius: orderEditorContainer.radius;

                placeHolderText: qsTr("Enter the comment");

                borderColor: Style.iconColorOnSelected;
                readOnly: orderEditorContainer.readOnly;

                onEditingFinished: {
//                    orderEditorContainer.doUpdateModel();
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
                height: orderEditorContainer.comboBoxHeight;

                radius: orderEditorContainer.radius;

                model: orderEditorContainer.accountsModel;
                changeable: !orderEditorContainer.readOnly;

                onCurrentIndexChanged: {
                    orderEditorContainer.doUpdateModel();
                }

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ChangeOrder");

                    customerCB.changeable = ok;
                }

                onModelChanged: {
                    orderEditorContainer.doUpdateGui();
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
                height: orderEditorContainer.comboBoxHeight;

                radius: orderEditorContainer.radius;

                changeable: !orderEditorContainer.readOnly;

                model: orderStatus.statusModel;

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

            Button{
                id: buttonContainer;

                anchors.top: orderStatusCB.top;
                anchors.right: parent.right;

                text: qsTr("Clear");

                enabled: orderStatusCB.changeable;

                onClicked: {
                    if(orderEditorContainer.model.ContainsKey("OrderStatus")){
                        if (orderStatusCB.currentIndex != -1){
                            orderStatusCB.currentIndex = -1;
                        }
                    }
                }

                decorator: ButtonDecorator{
                    width: 70;
                    height: orderStatusCB.height;
                    radius: orderEditorContainer.radius;
                }
            }
        }
    }//Column bodyColumn

    Component {
        id: productEditorDialog;

        ProductEditorDialog {
            id: productsDialog;

            onStarted: {
                productsDialog.bodyItem.productsModel = orderEditorContainer.productsModel;
                productsDialog.bodyItem.devicesModel = orderEditorContainer.devicesModel;
                productsDialog.bodyItem.licensesModel = orderEditorContainer.licensesModel;

                if (orderEditorContainer.model.ContainsKey("Id")){
                    productsDialog.bodyItem.orderUuid = orderEditorContainer.model.GetData("Id");
                }

                productsDialog.bodyItem.serialNumberEdit = orderEditorContainer.serialNumberEdit;
                productsDialog.activeProductIndex = productsView.activeProductIndex;

                if (orderEditorContainer.model.ContainsKey("OrderId")){
                    productsDialog.bodyItem.orderId = orderEditorContainer.model.GetData("OrderId");
                }

                let orderProductsModel = orderEditorContainer.model.GetData("OrderProducts");
                productsDialog.bodyItem.orderProductsModel = orderProductsModel;
                if (productsView.activeProductIndex >= 0){
                    let productModel = orderProductsModel.GetModelFromItem(productsView.activeProductIndex);
                    productsDialog.bodyItem.productModel = productModel;
                }

                productsDialog.bodyItem.started();
            }

            onFinished: {
                if (buttonId == Enums.ok){
                    let productModel = productsDialog.bodyItem.productModel;
                    let actualOrderProducts = orderEditorContainer.model.GetData("OrderProducts");

                    console.log("productModel", productModel.toJSON());

                    let index = productsView.activeProductIndex;
                    if (index < 0){
                        console.log("actualOrderProducts1", actualOrderProducts.toJSON());

                        if (actualOrderProducts){
                            index = actualOrderProducts.GetItemsCount();
                            actualOrderProducts.CopyItemDataFromModel(index, productModel);
                        }

                        console.log("actualOrderProducts2", actualOrderProducts.toJSON());
                    }
                    else{
                        if (actualOrderProducts){
                            let actualProductModel = actualOrderProducts.GetModelFromItem(index);
                            let isEqual = actualProductModel.IsEqualWithModel(productModel);
                            if (!isEqual){
                                actualOrderProducts.CopyItemDataFromModel(index, productModel);
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

        ToolButton {
            id: addProduct;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: 10;

            height: 22;
            width: height;

            iconSource: "../../../" + Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal);

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
                if (buttonId == Enums.yes){
                    if (productsView.activeProductIndex < 0){
                        return;
                    }

                    if (orderEditorContainer.model.ContainsKey("OrderProducts")){
                        let orderProducts = orderEditorContainer.model.GetTreeItemModel("OrderProducts")
                        if (orderProducts){
                            orderProducts.RemoveItem(productsView.activeProductIndex);
                        }
                    }
                }
            }
        }
    }
}//Container


