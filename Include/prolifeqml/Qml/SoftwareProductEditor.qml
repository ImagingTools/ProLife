import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtlicgui 1.0

Rectangle {
    id: root;

    color: Style.backgroundColor;

    property TreeItemModel productModel: TreeItemModel {}
    property TreeItemModel productsModel: null;
    property TreeItemModel orderProductsModel: null;

    property string categoryId: "Software";

    // All licenses from Lisa
    property TreeItemModel licensesModel: null;

    property bool blockUpdatingModel: false;
    onBlockUpdatingModelChanged: {
        loading.visible = root.blockUpdatingModel;
    }

    Component.onCompleted: {
        loading.visible = true;

        root.updateGui();
    }

    UuidGenerator {
        id: uuidGenerator;
    }

    function started(){
        productCB.model = root.productsModel;

        if (!root.productModel.ContainsKey("Id")){
            let uuid = uuidGenerator.generateUUID();

            root.productModel.SetData("Id", uuid);
        }

        if (root.productModel.ContainsKey("CategoryId")){
            let categoryId = root.productModel.GetData("CategoryId");

            root.categoryId = categoryId
        }

        root.updateGui();
    }

    function updateGui(){
        root.blockUpdatingModel = true;

        productCB.currentIndex = -1;
        let productId = "";
        if (root.productModel.ContainsKey("ProductId")){
            productId = root.productModel.GetData("ProductId");

            let productModel = productCB.model;
            if (productModel){
                for (let i = 0; i < productModel.GetItemsCount(); i++){
                    let id = productModel.GetData("Id", i);
                    if (id === productId && productCB.currentIndex != i){
                        productCB.currentIndex = i;
                        break;
                    }
                }
            }
        }

        licensesTable.rowModel.clear();

        let licensesModel;
        if (root.licensesModel){
            for (let i = 0; i < root.licensesModel.GetItemsCount(); i++){
                let id = root.licensesModel.GetData("Id", i);
                if (id === productId){
                    let productLicensesModel = root.licensesModel.GetData("Licenses", i);
                    licensesModel = productLicensesModel;
                }
            }
        }

        if (licensesModel){
            if (root.productModel.ContainsKey("ActiveLicenses")){
                for (let i = 0; i < licensesModel.GetItemsCount(); i++){
                    let licenseId = licensesModel.GetData("Id", i);
                    let licenseName = licensesModel.GetData("Name", i);
                    let row = {"Id": licenseId, "Name": licenseName, "LicenseState": Qt.Unchecked, "ExpirationState": Qt.Unchecked, "Expiration": ""}
                    let activeLicensesModel = root.productModel.GetData("ActiveLicenses");

                    for (let j = 0; j < activeLicensesModel.GetItemsCount(); j++){
                        let activeLicenseId = activeLicensesModel.GetData("Id", j);
                        let expiration = activeLicensesModel.GetData("Expiration", j);
                        if (licenseId === activeLicenseId){
                            row["LicenseState"] = Qt.Checked;

                            if (expiration == ""){
                                row["ExpirationState"] = Qt.Unchecked;
                            }
                            else{
                                row["ExpirationState"] = Qt.Checked;
                                row["Expiration"] = expiration;
                            }
                        }
                    }

                    licensesTable.addRow(row);
                }
            }
        }

        root.blockUpdatingModel = false;
    }

    function updateModel(){
        if (root.blockUpdatingModel){
            return;
        }

        if (productCB.currentIndex >= 0){
            let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
            root.productModel.SetData("ProductId", selectedProductId);
        }
        else{
            root.productModel.SetData("ProductId", "");
        }

        let activeLicenses = root.productModel.AddTreeModel("ActiveLicenses");
        for (let i = 0; i < licensesTable.rowModel.count; i++){
            let rowObj = licensesTable.rowModel.get(i);

            let licenseId = rowObj["Id"];
            let licenseName = rowObj["Name"];
            let expirationState  = rowObj["ExpirationState"];
            let expiration  = rowObj["Expiration"];
            let state = rowObj["LicenseState"];

            if (state === Qt.Checked){
                let index = activeLicenses.InsertNewItem();

                activeLicenses.SetData("Id", licenseId, index);
                activeLicenses.SetData("Name", licenseName, index);

                if (expirationState === Qt.Checked){
                    activeLicenses.SetData("Expiration", expiration, index);
                }
                else{
                    activeLicenses.SetData("Expiration", "", index);
                }
            }
        }
    }

    function clearPairLink(){
        let id = root.productModel.GetData("Id");
        let productsModel =  root.orderProductsModel;
        for (let i = 0; i < productsModel.GetItemsCount(); i++){
            let categoryId = productsModel.GetData("CategoryId", i)
            if (categoryId === "Hardware"){
                let pairId = productsModel.GetData("PairId", i)
                if (id === pairId){
                    productsModel.SetData("PairId", "", i);
                }
            }
        }
    }

    Column {
        id: bodyColumn;

        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;
        spacing: 7;

        Text {
            id: titleProduct;

            text: qsTr("Product");
            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;
        }

        ComboBox {
            id: productCB;

            width: parent.width;
            height: 23;

            radius: 3;

            onCurrentIndexChanged: {
            }

            MouseArea {
                id: disabledMA;

                z: 100;

                anchors.fill: productCB;

                visible: !productCB.changeable;

                onClicked: {
                    console.log("disabledMA onClicked");

                    disabledComboBoxText.visible = true;
                }
            }
        }

        Text {
            id: disabledComboBoxText;
            text: qsTr("Please make a unlink first");
            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            visible: false;

            onVisibleChanged: {
                if (disabledComboBoxText.visible){
                    timer.start();
                }
            }
        }

        Timer {
            id: timer;
            interval: 3000;
            onTriggered: {
                disabledComboBoxText.visible = false;
            }
        }

        Text {
            text: qsTr("Device");
            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;

            visible: root.categoryId === "Hardware";
        }

        Item {
            width: parent.width;
            height: 23;

            visible: root.categoryId === "Hardware";

            CollectionDataProvider {
                id: devicesList;

                commandId: "Devices";

                fields: ["Id", "Name", "DeviceType", "OrderId", "Status"];

                onModelUpdated: {
                    filteringModel.Clear();

                    if (devicesList.collectionModel != null){

                        devicesList.filteringDevicesList();

                        let newIndex = filteringModel.InsertNewItem(0);

                        filteringModel.SetData("Id", "", newIndex);
                        filteringModel.SetData("Name", "New Device", newIndex);

                        deviceCB.model = filteringModel;
                    }

                    root.updateGui();
                }

                TreeItemModel {
                    id: filteringModel;
                }

                function filteringDevicesList(){
                    for (let i = 0; i < devicesList.collectionModel.GetItemsCount(); i++){
                        let status = devicesList.collectionModel.GetData("ProductionStatus", i);
                        let orderId = devicesList.collectionModel.GetData("OrderId", i);
                        let deviceType = devicesList.collectionModel.GetData("DeviceType", i);
                        let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);

                        if (selectedProductId === deviceType && (installationEditorContainer.orderId === "" || installationEditorContainer.orderUuid === orderId) && (status === "Finished" || status === "None")||
                                selectedProductId === deviceType && installationEditorContainer.orderId === orderId){
                            let index = filteringModel.InsertNewItem();
                            filteringModel.CopyItemDataFromModel(index, devicesList.collectionModel, i);
                        }
                    }
                }
            }

            ComboBox {
                id: deviceCB;

                height: parent.height;
                width: parent.width;

                radius: 3;

                onCurrentIndexChanged: {
                    installationEditorContainer.updateModel();
                }
            }

            Component {
                id: saveDialog;
                MessageDialog {
                    onFinished: {
                    }
                }
            }

            Component {
                id: errorDialog;

                ErrorDialog {
                    onFinished: {
                    }
                }
            }
        }

        Text {
            id: titleDependency;

            text: qsTr("Pair link");
            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;
        }

        Item {
            width: parent.width;
            height: 23;
            ComboBox {
                id: pairCB;
                anchors.left: parent.left;
                anchors.right: clearPair.left;
                anchors.rightMargin: 10;
                height: parent.height;

                model: TreeItemModel{}

                radius: 3;

                onCurrentIndexChanged: {
                }
            }

            BaseButton{
                id: clearPair;
                anchors.right: parent.right;
                height: parent.height;
                width: 100;
                enabled: pairCB.currentIndex > -1;
                text: qsTr("Unlink");

                onClicked: {
                    root.clearPairLink();
                }
            }//delegate
        }
    }//Column bodyColumn

    Text {
        id: titleLicenses;
        anchors.top: bodyColumn.bottom;
        anchors.topMargin: 10;
        anchors.left: bodyColumn.left;

        text: qsTr("Licenses");
        color: Style.textColor;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSize_common;
    }

    BasicTableView {
        id: licensesTable;

        anchors.top: titleLicenses.bottom;
        anchors.topMargin: 10;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.left: bodyColumn.left;

        width: bodyColumn.width;
        visible: titleLicenses.visible;

        rowDelegate: LicenseInstanceItemDelegate {root: licensesTable;}

        Component.onCompleted: {
            licensesTable.addColumn({"Id": "Name", "Name": "License Name"});
            licensesTable.addColumn({"Id": "Expiration", "Name": "Expiration"});
        }

        onRowModelDataChanged: {
            console.log("licensesTable onRowModelDataChanged");

            if (!blockUpdatingModel){
                root.updateModel();
            }
        }
    }

    Loading {
        id: loading;

        anchors.fill: parent;
        visible: false;
    }

}// product Editor container
