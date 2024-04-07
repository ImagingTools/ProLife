import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import prolifeqml 1.0
import QtGraphicalEffects 1.0
import Qt5Compat.GraphicalEffects 6.0

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

        checkWidth();
    }

    onWidthChanged: {
        checkWidth();
    }

    LicensesProvider {
        id: licensesProvider;
    }

    function checkWidth(){
        if (width < content.width + scrollbar.width + 50){
            content.width = width - 50;
        }
        else{
            content.width = 700;
        }
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

        color: Style.backgroundColor2;
    }

    CustomScrollbar {
        id: scrollbar;

        anchors.right: parent.right;
        anchors.top: flickable.top;
        anchors.bottom: flickable.bottom;

        secondSize: 10;
        targetItem: flickable;

        radius: 2;
    }

    Flickable {
        id: flickable;

        anchors.top: parent.top;
        anchors.topMargin: Style.size_largeMargin;

        anchors.bottom: parent.bottom;
        anchors.bottomMargin: Style.size_largeMargin;

        anchors.left: parent.left;
        anchors.right: scrollbar.left;

        contentWidth: content.width;
        contentHeight: content.height + 2 * Style.size_largeMargin;

        boundsBehavior: Flickable.StopAtBounds;

        clip: true;

        Column {
            id: content;

            anchors.top: parent.top;

            anchors.left: parent.left;
            anchors.leftMargin: Style.size_largeMargin;

            width: 700;

            spacing: Style.size_largeMargin;

            NumberAnimation {
                id: orderInfoAnimation;

                target: group;
                property: "height";
                duration: 200;
            }

            Rectangle {
                id: orderInfoTitle;

                width: group.width;
                height: titleLicenses.height;

                color: "transparent";

                radius: 3;

                property bool orderInfoOpened: true;

                onOrderInfoOpenedChanged: {
                    if (orderInfoOpened){
                        orderInfoAnimation.from = 0;
                        orderInfoAnimation.to = group.contentHeight;
                    }
                    else{
                        orderInfoAnimation.from = group.contentHeight;
                        orderInfoAnimation.to = 0;
                    }

                    orderInfoAnimation.start();
                }

                ToolButton {
                    id: openButton;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.right: parent.right;
                    anchors.rightMargin: 10;

                    height: 22;
                    width: height;

                    iconSource: orderInfoTitle.orderInfoOpened
                                ? "../../../" + Style.getIconPath("Icons/Up", Icon.State.On, Icon.Mode.Normal)
                                : "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);

                    onClicked: {
                        orderInfoTitle.orderInfoOpened = !orderInfoTitle.orderInfoOpened;
                    }
                }

                Text {
                    id: titleOrderInfo;

                    anchors.left: parent.left;
                    anchors.verticalCenter: parent.verticalCenter;

                    text: qsTr("Order Information");

                    color: Style.textColor;
                    font.family: Style.fontFamilyBold;
                    font.pixelSize: Style.fontSize_title;
                }
            }

            GroupElementView {
                id: group;

                width: content.width;

                TextInputElementView {
                    id: instanceIdInput;

                    name: qsTr("ERP Order-ID");
                    placeHolderText: qsTr("Enter the ERP Order-ID");
                    description: qsTr("ERP Order-ID");

                    readOnly: orderEditorContainer.readOnly;

                    Component.onCompleted: {
                        let ok = PermissionsController.checkPermission("ChangeOrder");
                        instanceIdInput.readOnly = !ok;
                    }

                    onEditingFinished: {
                        orderEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: purchaseIdInput;
                }

                TextInputElementView {
                    id: purchaseIdInput;

                    name: qsTr("Purchase Order-ID");
                    placeHolderText: qsTr("Enter the Purchase-ID");
                    description: qsTr("Purchase Order-ID");

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

                TextInputElementView {
                    id: descriptionInput;

                    name: qsTr("Description");
                    placeHolderText: qsTr("Enter the comment");

                    readOnly: orderEditorContainer.readOnly;

                    onEditingFinished: {
                        orderEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: instanceIdInput;

                    Component.onCompleted: {
                        let ok = PermissionsController.checkPermission("ChangeOrder");
                        descriptionInput.readOnly = !ok;
                    }
                }

                ComboBoxElementView {
                    id: customerCB;

                    name: qsTr("Customer");

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

                ComboBoxElementView {
                    id: orderStatusCB;

                    name: qsTr("Order Status");

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
            }

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
                                    if (actualOrderProducts.GetItemsCount() > 0){
                                        index = actualOrderProducts.InsertNewItem(0);
                                    }
                                    else{
                                        index = actualOrderProducts.InsertNewItem();
                                    }

//                                    index = actualOrderProducts.GetItemsCount();
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

            Rectangle {
                id: productsTitle;

                width: content.width;
                height: titleLicenses.height;

                color: "transparent";

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
                    anchors.verticalCenter: parent.verticalCenter;

                    text: qsTr("Products") + " (" + productsView.count + ")";
                    color: Style.textColor;
                    font.family: Style.fontFamilyBold;
                    font.pixelSize: Style.fontSize_title;
                }
            }

            ListView {
                id: productsView;

                width: content.width;
                height: contentHeight;

                clip: true;
                boundsBehavior: Flickable.StopAtBounds;
                spacing: Style.size_largeMargin;

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

                delegate: OrderProductDelegate {
                    id: orderProductDelegate;

                    width: productsView.width;

                    productsListView: productsView;

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


