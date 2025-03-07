import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import prolifeqml 1.0
import prolifeOrdersSdl 1.0

ViewBase {
    id: orderEditorContainer;

    property TreeItemModel accountsModel: CachedAccountCollection.collectionModel;
    property TreeItemModel productsModel: CachedProductCollection.collectionModel;
    property TreeItemModel devicesModel: CachedDeviceCollection.collectionModel;
    property TreeItemModel softwaresModel: CachedSoftwareCollection.collectionModel;
    property TreeItemModel licensesModel: CachedLicenseCollection.collectionModel

    property OrderData orderData: model ? model : null;

    Component.onCompleted: {
        if (!CachedAccountCollection.completed){
            CachedAccountCollection.updateModel();
        }

        if (!CachedLicenseCollection.completed){
            CachedLicenseCollection.updateModel();
        }

        if (!CachedDeviceCollection.completed){
            CachedDeviceCollection.updateModel();
        }

        if (!CachedProductCollection.completed){
            CachedProductCollection.updateModel();
        }

        if (!CachedSoftwareCollection.completed){
            CachedSoftwareCollection.updateModel();
        }

        CachedDeviceCollection.modelUpdated.connect(orderEditorContainer.doUpdateGui)
        CachedSoftwareCollection.modelUpdated.connect(orderEditorContainer.doUpdateGui)
    }

    Component.onDestruction: {
        CachedDeviceCollection.modelUpdated.disconnect(orderEditorContainer.doUpdateGui)
        CachedSoftwareCollection.modelUpdated.disconnect(orderEditorContainer.doUpdateGui)
    }

    onOrderDataChanged: {
        checkPermissions();
    }

    function setReadOnly(readOnly){
        instanceIdInput.readOnly = readOnly;
        purchaseIdInput.readOnly = readOnly;
        descriptionInput.readOnly = readOnly;
        productsView.readOnly = readOnly;

        customerCB.changeable = !readOnly;
        orderStatusCB.changeable = !readOnly;
    }

    function checkPermissions(){
        if (!orderData){
            return;
        }

        let orderId = orderData.m_id;
        let canAddOrder = PermissionsController.checkPermission("AddOrder");

        if (orderId === "" && canAddOrder){
            instanceIdInput.readOnly = false;
            purchaseIdInput.readOnly = false;
            descriptionInput.readOnly = false;
            customerCB.changeable = true;
            orderStatusCB.changeable = true;
            addProduct.visible = true;
            productsView.readOnly = false;
        }
        else{
            let canChangeDeliveryId = PermissionsController.checkPermission("ChangeDeliveryId");
            instanceIdInput.readOnly = !canChangeDeliveryId;

            let canChangePurchaseOrderId = PermissionsController.checkPermission("ChangePurchaseOrderId");
            purchaseIdInput.readOnly = !canChangePurchaseOrderId;

            let canChangeDescriptionForOrder = PermissionsController.checkPermission("ChangeDescriptionForOrder");
            descriptionInput.readOnly = !canChangeDescriptionForOrder;

            let canChangeCustomer = PermissionsController.checkPermission("ChangeCustomer");
            customerCB.changeable = canChangeCustomer;

            let canChangeOrderStatus = PermissionsController.checkPermission("ChangeOrderStatus");
            orderStatusCB.changeable = canChangeOrderStatus;

            let canChangeOrderProducts = PermissionsController.checkPermission("ChangeOrderProducts");
            addProduct.visible = canChangeOrderProducts;
            productsView.readOnly = !canChangeOrderProducts;

            let ok = canChangeDeliveryId || canChangePurchaseOrderId || canChangeDescriptionForOrder || canChangeCustomer || canChangeOrderStatus || canChangeOrderProducts;

            if (commandsController){
                commandsController.setCommandVisible("Undo", ok);
                commandsController.setCommandVisible("Redo", ok);
                commandsController.setCommandVisible("Save", ok);
            }
        }
    }

    function syncroniseProducts(){
        if (!softwaresModel){
            return;
        }

        if (!devicesModel){
            return;
        }

        model.setUpdateEnabled(false);

        let orderUuid = ""
        if (model.containsKey("Id")){
            orderUuid = model.getData("Id");
        }

        if (model.containsKey("OrderProducts")){
            let orderProducts = model.getData("OrderProducts")

            for (let j = 0; j < orderProducts.getItemsCount(); j++){
                let orderedProductId = orderProducts.getData("Id", j);
                let categoryId = orderProducts.getData("CategoryId", j);

                let productFound = false;

                if (categoryId === "Software"){
                    for (let i = 0; i < softwaresModel.getItemsCount(); i++){
                        let softwareId = softwaresModel.getData("Id", i);
                        let softwareOrderUuid = orderProducts.getData("OrderUuid", i);

                        if (softwareId === orderedProductId){
                            // ????
                            //                            if (orderUuid !== "" && orderUuid !== softwareOrderUuid){
                            //                                orderProducts.setData("ErrorMessage", qsTr("The product has been removed from this order"), j);

                            //                                break;
                            //                            }

                            if (softwaresModel.containsKey("SerialNumber", i)){
                                let serialNumber = softwaresModel.getData("SerialNumber", i)
                                orderProducts.setData("SerialNumber", serialNumber, j);

                                let licenseUuid = softwaresModel.getData("LicenseUuid", i)
                                orderProducts.setData("LicenseUuid", licenseUuid, j);

                                let licenseId = softwaresModel.getData("LicenseId", i)
                                orderProducts.setData("LicenseId", licenseId, j);

                                let licenseName = softwaresModel.getData("LicenseName", i)
                                orderProducts.setData("LicenseName", licenseName, j);

                                let productUuid = softwaresModel.getData("ProductUuid", i)
                                orderProducts.setData("ProductUuid", productUuid, j);

                                let productName = softwaresModel.getData("ProductName", i)
                                orderProducts.setData("ProductName", productName, j);

                                let expiration = softwaresModel.getData("Expiration", i)
                                orderProducts.setData("Expiration", expiration, j);

                                productFound = true;

                                break;
                            }
                        }
                    }
                }
                else if (categoryId === "Hardware") {
                    for (let i = 0; i < devicesModel.getItemsCount(); i++){
                        let hardwareId = devicesModel.getData("Id", i);
                        if (hardwareId === orderedProductId){
                            let macAddress = devicesModel.getData("MacAddress", i);
                            orderProducts.setData("MacAddress", macAddress, j);

                            let serialNumber = devicesModel.getData("SerialNumber", i);
                            orderProducts.setData("SerialNumber", serialNumber, j);

                            let licenseUuid = devicesModel.getData("LicenseUuid", i)
                            orderProducts.setData("LicenseUuid", licenseUuid, j);

                            let licenseId = devicesModel.getData("LicenseId", i)
                            orderProducts.setData("LicenseId", licenseId, j);

                            let licenseName = devicesModel.getData("LicenseName", i)
                            orderProducts.setData("LicenseName", licenseName, j);

                            let productUuid = devicesModel.getData("ProductUuid", i)
                            orderProducts.setData("ProductUuid", productUuid, j);

                            let productName = devicesModel.getData("DeviceType", i)
                            orderProducts.setData("ProductName", productName, j);

                            productFound = true;

                            break;
                        }
                    }
                }

                if (!productFound){
                    // ????
                }

                orderProducts.refresh()
            }
        }

        model.setUpdateEnabled(true);
    }

    function updateGui(){
        console.log("updateGui", orderData.toJson())
        instanceIdInput.text = orderData.m_orderId;
        purchaseIdInput.text = orderData.m_purchaseId;
        descriptionInput.text = orderData.m_description;

        customerCB.currentIndex = -1;
        let customerId = orderData.m_customerId;
        let customerModel = customerCB.model;
        if (customerModel){
            for (let i = 0; i < customerModel.getItemsCount(); i++){
                let id = customerModel.getData("Id", i);
                if (id === customerId){
                    customerCB.currentIndex = i;
                    break;
                }
            }
        }

        orderStatusCB.currentIndex = -1;
        let status = orderData.m_orderStatus;
        let statusModel = orderStatus.statusModel
        if (statusModel){
            orderStatusCB.model = statusModel;
            for (let i = 0; i < statusModel.getItemsCount(); i++){
                let id = statusModel.getData("Id", i);
                if (id === status){
                    orderStatusCB.currentIndex = i;
                    break;
                }
            }
        }

        //        syncroniseProducts();
        productsView.model = 0;
        productsView.model = orderData.m_orderProducts;
    }

    function updateModel(){
        orderData.m_orderId = instanceIdInput.text ;
        orderData.m_purchaseId = purchaseIdInput.text;
        orderData.m_description = descriptionInput.text;

        let selectedAccountId = "";
        if (customerCB.currentIndex >= 0 && customerCB.model){
            selectedAccountId = customerCB.model.getData("Id", customerCB.currentIndex);
        }

        orderData.m_customerId = selectedAccountId;

        if (orderStatusCB.currentIndex >= 0){
            let selectedStatus = orderStatusCB.model.getData("Id", orderStatusCB.currentIndex);
            orderData.m_orderStatus = selectedStatus;
        }
        else{
            orderData.m_orderStatus = "";
        }
    }

    OrderStatus {
        id: orderStatus;
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor2;
    }

	DocumentHistoryPanel {
		id: historyPanel;
		documentId: orderEditorContainer.orderData ? orderEditorContainer.orderData.m_id : "";
		collectionId: "Orders";
		editorFlickable: flickable;
	}

    CustomScrollbar {
        id: scrollbar;
        z: parent.z + 1;

        anchors.right: parent.right;
        anchors.top: flickable.top;
        anchors.bottom: flickable.bottom;

        secondSize: 10;
        targetItem: flickable;

        radius: 2;
    }

    CustomScrollbar{
        id: scrollHoriz;

        z: parent.z + 1;

        anchors.left: flickable.left;
        anchors.right: flickable.right;
        anchors.bottom: flickable.bottom;

        secondSize: 10;

        vertical: false;
        targetItem: flickable;
    }

    Flickable {
        id: flickable;

        anchors.left: parent.left;
        anchors.leftMargin: Style.sizeLargeMargin;

        anchors.top: parent.top;
        anchors.topMargin: Style.sizeLargeMargin;

        anchors.bottom: parent.bottom;
        anchors.bottomMargin: Style.sizeLargeMargin;

        anchors.right: scrollbar.left;
        anchors.rightMargin: Style.sizeLargeMargin;

        contentWidth: content.width;
		contentHeight: Math.max(content.height + 2 * Style.sizeLargeMargin + 100, historyPanel.contentHeight + 2 * Style.sizeLargeMargin);

        boundsBehavior: Flickable.StopAtBounds;

        clip: true;

        Column {
            id: content;

            width: 700;

            spacing: Style.sizeLargeMargin;

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

                        text: qsTr("Enter a 5-digit or 8-digit number");

                        color: Style.errorTextColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSizeXSmall;
                    }
                }

                TextInputElementView {
                    id: instanceIdInput;

                    name: qsTr("Delivery-ID");
                    placeHolderText: qsTr("Enter the delivery-ID");

                    readOnly: orderEditorContainer.readOnly;
                    maximumLength: 8;

                    onEditingFinished: {
                        orderEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: purchaseIdInput;
                    KeyNavigation.backtab: orderStatusCB;

                    bottomComp: acceptableInput ? undefined : errorComp;

                    Component.onCompleted: {
                        validate();
                    }

                    onTextChanged: {
                        validate();
                    }

                    function validate(){
                        let len = instanceIdInput.text.length;

                        let ok1 = instanceIdInput.test("\\d{5}", instanceIdInput.text) && len === 5;
                        let ok2 = instanceIdInput.test("\\d{8}", instanceIdInput.text) && len === 8;

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
                    KeyNavigation.backtab: instanceIdInput;
                }

                TextInputElementView {
                    id: descriptionInput;

                    name: qsTr("Description");
                    placeHolderText: qsTr("Enter the comment");

                    readOnly: orderEditorContainer.readOnly;

                    onEditingFinished: {
                        orderEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: customerCB;
                    KeyNavigation.backtab: purchaseIdInput;
                }

                ComboBoxElementView {
                    id: customerCB;

                    name: qsTr("Customer");

                    model: orderEditorContainer.accountsModel;
                    changeable: !orderEditorContainer.readOnly;
                    isSelectionRequired: true;
                    errorText: qsTr("Please select a customer");

                    onCurrentIndexChanged: {
                        orderEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: orderStatusCB;
                    KeyNavigation.backtab: descriptionInput;

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

                    KeyNavigation.tab: instanceIdInput;
                    KeyNavigation.backtab: customerCB;
                }
            }

            Component {
                id: productFactory;

                OrderedProduct {}
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
                        productsDialog.bodyItem.orderUuid = orderEditorContainer.orderData.m_id;
                        productsDialog.activeProductIndex = productsView.activeProductIndex;
                        productsDialog.bodyItem.orderId = orderEditorContainer.orderData.m_orderId;

                        productsDialog.bodyItem.orderProductsModel = orderEditorContainer.orderData.m_orderProducts;
                        if (productsView.activeProductIndex >= 0){
                            let productModel = orderEditorContainer.orderData.m_orderProducts.get(productsView.activeProductIndex).item;
                            productsDialog.bodyItem.productItem = productModel.copyMe();
                        }
                        else{
							let productItem = productFactory.createObject(orderEditorContainer);
                            productItem.m_id = UuidGenerator.generateUUID();
                            productItem.m_categoryId = "Software";

                            productsDialog.bodyItem.productItem = productItem;
                        }

                        productsDialog.bodyItem.started();
                    }

                    onFinished: {
                        if (buttonId == Enums.ok){

                            let productModel = productsDialog.bodyItem.productItem.copyMe();
                            let actualOrderProducts = orderEditorContainer.orderData.m_orderProducts;

                            let index = productsView.activeProductIndex;
                            if (index < 0){
                                if (actualOrderProducts){
                                    actualOrderProducts.insert(0, {"item": productModel})
                                }
                            }
                            else{
                                if (actualOrderProducts){
                                    actualOrderProducts.set(index, {"item": productModel})
                                }
                            }

                            productsView.model = 0;
                            productsView.model = actualOrderProducts;

                            orderEditorContainer.model.modelChanged([]);
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

                    height: parent.height;

                    spacing: Style.sizeMainMargin;

                    ToolButton {
                        id: expandButton;

                        anchors.verticalCenter: parent.verticalCenter;

                        width: 22;
                        height: width;

                        iconSource: !productsView.expanded ? "../../../" + Style.getIconPath("Icons/DetailedView", Icon.State.On, Icon.Mode.Normal)
                                                           : "../../../" + Style.getIconPath("Icons/CompactView", Icon.State.On, Icon.Mode.Normal);

                        tooltipText: !productsView.expanded ? qsTr("Detailed view") : qsTr("Compact view");

                        onClicked: {
                            productsView.expanded = !productsView.expanded;
                        }
                    }

                    ToolButton {
                        id: addProduct;

                        anchors.verticalCenter: parent.verticalCenter;

                        width: 22;
                        height: width;

                        iconSource: "../../../" + Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal);

                        tooltipText: qsTr("Add a new product");

                        onClicked: {
                            productsView.activeProductIndex = -1;
                            ModalDialogManager.openDialog(productEditorDialog, {});
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
                    font.pixelSize: Style.fontSizeLarge;
                }
            }

            ListView {
                id: productsView;

                width: content.width;
                height: contentHeight;

                boundsBehavior: Flickable.StopAtBounds;
                spacing: Style.sizeLargeMargin;

                cacheBuffer: 1000;

                property int activeProductIndex: -1;
                property int selectedIndex: -1;
                property bool readOnly: false;

                property bool expanded: true;

                delegate: OrderProductDelegate {
                    id: orderProductDelegate;
                    width: productsView.width;
                    readOnly: productsView.readOnly;
                    expanded: productsView.expanded;
                    onEdited: {
                        productsView.activeProductIndex = model.index;
                        ModalDialogManager.openDialog(productEditorDialog, {});
                    }
                    onRemoved: {
                        productsView.activeProductIndex = model.index;
                        ModalDialogManager.openDialog(removeDialog, {"message": qsTr("Remove selected product ?")});
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

                    let orderProducts = orderEditorContainer.orderData.m_orderProducts
                    if (orderProducts){
                        orderProducts.remove(productsView.activeProductIndex);

                        orderEditorContainer.model.modelChanged([]);
                    }
                }
            }
        }
    }
}//Container


