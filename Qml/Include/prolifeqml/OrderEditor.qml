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
    property TreeItemModel softwaresModel: CachedSoftwareCollection.collectionModel;
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
        CachedSoftwareCollection.updateModel();

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
            let statusModel = orderStatus.statusModel
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

        productsView.model = 0;

        if (model.ContainsKey("OrderProducts")){
            productsView.model = model.GetTreeItemModel("OrderProducts");

            productsView.model.Refresh();
        }
    }

    function updateModel(){
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

            GroupHeaderView {
                title: qsTr("Order Information");
                width: content.width;
                groupView: group;
            }

            GroupElementView {
                id: group;

                width: content.width;

                Component {
                    id: errorComp;

                    Text {
                        id: errorInstanceId;

                        text: qsTr("Enter a 5-digit or 10-digit number");

                        color: Style.errorTextColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }
                }

                TextInputElementView {
                    id: instanceIdInput;

                    name: qsTr("Delivery-ID");
                    placeHolderText: qsTr("Enter the delivery-ID");

                    readOnly: orderEditorContainer.readOnly;
                    maximumLength: 10;

                    Component.onCompleted: {
                        let ok = PermissionsController.checkPermission("ChangeOrder");
                        instanceIdInput.readOnly = !ok;
                    }

                    onEditingFinished: {
                        orderEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: purchaseIdInput;

                    onAcceptableInputChanged: {
                        instanceIdInput.bottomComp = acceptableInput ? undefined : errorComp;
                    }

                    onTextChanged: {
                        let len = instanceIdInput.text.length;

                        let ok1 = instanceIdInput.test("\\d{5}", instanceIdInput.text) && len === 5;
                        let ok2 = instanceIdInput.test("\\d{10}", instanceIdInput.text) && len === 10;

                        instanceIdInput.bottomComp = ok1 || ok2 ? undefined : errorComp;
                    }

                    function test(regex, text){
                        let re = new RegExp(regex)
                        if (re){
                            return re.test(text);
                        }

                        return false;
                    }
                }

                TextInputElementView {
                    id: purchaseIdInput;

                    name: qsTr("Purchase Order-ID");
                    placeHolderText: qsTr("Enter the Purchase-ID");

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
                        productsDialog.bodyItem.softwaresModel = orderEditorContainer.softwaresModel;
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
                            orderEditorContainer.model.SetUpdateEnabled(false);

                            let productModel = productsDialog.bodyItem.productModel;
                            let actualOrderProducts = orderEditorContainer.model.GetData("OrderProducts");

                            let index = productsView.activeProductIndex;
                            if (index < 0){
                                if (actualOrderProducts){
                                    index = actualOrderProducts.InsertNewItem(0);
                                    actualOrderProducts.CopyItemDataFromModel(index, productModel);
                                }
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

                            orderEditorContainer.model.SetUpdateEnabled(true);
                            orderEditorContainer.model.dataChanged(null, null);

                            actualOrderProducts.Refresh();
                        }
                    }
                }
            }

            Item {
                id: productsTitle;

                width: content.width;
                height: titleLicenses.height;

                Row {
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.right: parent.right;
                    anchors.rightMargin: Style.size_mainMargin;

                    height: parent.height;

                    ToolButton {
                        id: addProduct;

                        width: 22;
                        height: width;

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

                boundsBehavior: Flickable.StopAtBounds;
                spacing: Style.size_largeMargin;

                cacheBuffer: 1000;

                property int activeProductIndex: -1;
                property int selectedIndex: -1;
                property bool readOnly: false;

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ChangeOrder");

                    productsView.readOnly = !ok;
                }

                delegate: OrderProductDelegate {
                    id: orderProductDelegate;

                    width: productsView.width;

                    readOnly: productsView.readOnly;

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


