import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0

Item {
    id: root;

    //    property TreeItemModel documentModel: TreeItemModel{}
    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}
    property TreeItemModel orderProductsModel: TreeItemModel {}

    property TreeItemModel productModel: TreeItemModel {}

    property bool blockUpdatingModel: false;
    property bool centered: true;

    property string orderId;
    property string orderUuid;

    // ProductEditorDialog reference
    property Item rootItem: null;

    // property string selectedPairId: "";

    Component.onCompleted: {
        loading.visible = true;
    }

    onBlockUpdatingModelChanged: {
        loading.visible = root.blockUpdatingModel;
    }

    width: 550;
    height: 800;

    UuidGenerator {
        id: uuidGenerator;
    }

    function onModelChanged(){
        root.rootItem.buttons.setButtonState("Save", true);
    }

    function started(){
        productCB.model = root.productsModel;

        if (!root.productModel.ContainsKey("Id")){
            let uuid = uuidGenerator.generateUUID();

            root.productModel.SetData("Id", uuid);
        }

        devicesList.updateModel({});

        root.productModel.modelChanged.connect(root.onModelChanged);
    }

    function getProductName(productId){
        let productModel = root.productsModel;
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

    function updatePairModel(){
        let categoryId =  root.productModel.GetData("CategoryId");
        let id = root.productModel.GetData("Id");
        let pairId = root.productModel.GetData("PairId");

        console.log("id", id);
        console.log("pairId", pairId);

        let isPairFinded = false;

        pairCB.currentIndex = -1;
        pairCB.model.Clear();
        if (orderProductsModel){
            for (let i = 0; i < orderProductsModel.GetItemsCount(); i++){
                let currentId = root.orderProductsModel.GetData("Id", i);
                let currentCategoryId = root.orderProductsModel.GetData("CategoryId", i);
                let currentProductId = root.orderProductsModel.GetData("ProductId", i)
                let currentPairId = root.orderProductsModel.GetData("PairId", i)

                console.log("currentCategoryId", currentCategoryId);
                console.log("currentProductId", currentProductId);
                console.log("currentPairId", currentPairId);
                console.log("currentId", currentId);

                if (currentCategoryId === "Pair"){
                    console.log("categoryId", categoryId);
                    if (categoryId === "Software"){
                        console.log("Software");
                        let hardwareProductModel = root.orderProductsModel.GetData("HardwareProduct", i);
                        if (hardwareProductModel){
                            console.log("hardwareProductModel", hardwareProductModel.toJSON());

                            let pairId = hardwareProductModel.GetData("PairId");
                            if (id === pairId){
                                isPairFinded = true;
                            }

                            currentProductId = hardwareProductModel.GetData("ProductId");
                            currentId = id;
                        }
                    }
                    else if (categoryId === "Hardware"){
                        console.log("Hardware");
                        let softwareProductModel = root.orderProductsModel.GetData("SoftwareProduct", i);
                        if (softwareProductModel){
                            console.log("softwareProductModel", softwareProductModel.toJSON());
                            let softwareId = softwareProductModel.GetData("Id");
                            console.log("pairId", pairId);
                            console.log("currentId", currentId);
                            if (pairId === softwareId){
                                isPairFinded = true;
                            }

                            currentProductId = softwareProductModel.GetData("ProductId");
                            currentId = softwareProductModel.GetData("Id");
                        }
                    }
                }
                else if (categoryId === "Hardware" && pairId === currentId && currentCategoryId === "Software" ||
                    categoryId === "Software" && id === currentPairId && currentCategoryId === "Hardware"){
                    isPairFinded = true;
                }

                if (isPairFinded){
                    console.log("result currentProductId", currentProductId);
                    console.log("result currentId", currentId);
                    console.log();

                    let dependencyModel = pairCB.model;
                    let index = dependencyModel.InsertNewItem();
                    dependencyModel.SetData("ProductId", currentProductId, index);
                    dependencyModel.SetData("Id", currentId, 0);
                    dependencyModel.SetData("Name", "#" + (i + 1) + " " + getProductName(currentProductId), index);
                    pairCB.currentIndex = 0;
                    break;
                }
            }
        }
    }

    function updateHardwareCategoryProducts(){
        let productModel = root.orderProductsModel;
        let resultModel = pairCB.model;
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            let productCategory = productModel.GetData("CategoryId", i);

            if (productCategory === "Hardware"){
                let pairId = productModel.GetData("PairId", i);
                if (productModel.GetData("CategoryId", i) === "Hardware" && pairId === ""){
                    let resultIndex = resultModel.InsertNewItem();
                    let productId = productModel.GetData("ProductId", i);
                    resultModel.SetData("ProductId", productId, resultIndex);
                    resultModel.SetData("Id", productModel.GetData("Id", i), resultIndex);
                    resultModel.SetData("Name", "#" + (i + 1) + " " + getProductName(productId), resultIndex);
                }
            }

        }
        console.log("updateHardwareCategoryProducts", resultModel.toJSON())
    }

    function findHardwarePair(id){
        let retVal = ""
        let productsModel = root.orderProductsModel;
        for (let i = 0; i < productsModel.GetItemsCount(); i++){

            if (productsModel.GetData("CategoryId", i) === "Hardware"){
                let pairId = productsModel.GetData("PairId", i)
                if (!pairId){
                    productsModel.SetData("PairId", "", i)
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

    function updateSoftwareCategoryProducts(){
        let productModel = root.orderProductsModel;
        let resultModel = pairCB.model;
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            if (productModel.GetData("CategoryId", i) === "Software"){
                let hardwareId = findHardwarePair(productModel.GetData("Id", i));
                if (hardwareId == ""){
                    let productId = productModel.GetData("ProductId", i);
                    let resultIndex = resultModel.InsertNewItem();
                    resultModel.SetData("ProductId", productId, resultIndex);
                    resultModel.SetData("Id", productModel.GetData("Id", i), resultIndex);
                    resultModel.SetData("MacAddress", productModel.GetData("MacAddress", i), resultIndex);
                    resultModel.SetData("Name", "#" + (i + 1) + " " + getProductName(productId), resultIndex);
                }
            }
        }
        console.log("updateSoftwareCategoryProducts", resultModel.toJSON())
    }



    function updateGui(){
        blockUpdatingModel = true;

        let productId = root.productModel.GetData("ProductId");
        let id = root.productModel.GetData("Id");
        let pairId = root.productModel.GetData("PairId");
        let categoryId = root.productModel.GetData("CategoryId");

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

        deviceCB.currentIndex = -1;
        if (root.productModel.ContainsKey("DeviceId")){
            let deviceId = root.productModel.GetData("DeviceId");
            let deviceModel = deviceCB.model;
            for (let i = 0; i < deviceModel.GetItemsCount(); i++){
                let id = deviceModel.GetData("Id", i);
                if (id === deviceId){
                    deviceCB.currentIndex = i;
                    break;
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
            for (let i = 0; i < licensesModel.GetItemsCount(); i++){
                let licenseId = licensesModel.GetData("Id", i);
                let licenseName = licensesModel.GetData("Name", i);

                let row = {"Id": licenseId, "Name": licenseName, "LicenseState": Qt.Unchecked, "ExpirationState": Qt.Unchecked, "Expiration": ""}

                if (root.productModel.ContainsKey("ActiveLicenses")){
                    let activeLicensesModel = root.productModel.GetData("ActiveLicenses");
                    for (let j = 0; j < activeLicensesModel.GetItemsCount(); j++){
                        let activeLicenseId = activeLicensesModel.GetData("Id", j);
                        let expiration = activeLicensesModel.GetData("Expiration", j);
                        if (licenseId == activeLicenseId){
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
                }

                licensesTable.addRow(row);
            }
        }

        blockUpdatingModel = false;
    }

    function updateModel(){
        if (blockUpdatingModel){
            return;
        }

        let categoryId = root.productModel.GetData("CategoryId");

        if (categoryId == "Hardware"){
            if (deviceCB.currentIndex >= 0){
                let selectedDeviceId = deviceCB.model.GetData("Id", deviceCB.currentIndex);
                root.productModel.SetData("DeviceId", selectedDeviceId);
            }
            else{
                if (root.productModel.ContainsKey("DeviceId")){
                    root.productModel.RemoveData("DeviceId");
                }
            }
        }
        else{
            let activeLicenses = root.productModel.AddTreeModel("ActiveLicenses");
            activeLicenses.Clear();
            for (let i = 0; i < licensesTable.rowModel.count; i++){
                let rowObj = licensesTable.rowModel.get(i);

                let licenseId = rowObj["Id"];
                let licenseName = rowObj["Name"];
                let expirationState  = rowObj["ExpirationState"];
                let expiration  = rowObj["Expiration"];
                let state = rowObj["LicenseState"];

                console.log("rowObj", JSON.stringify(rowObj));

                if (state == Qt.Checked){

                    let index = activeLicenses.InsertNewItem();

                    activeLicenses.SetData("Id", licenseId, index);
                    activeLicenses.SetData("Name", licenseName, index);

                    if (expirationState == Qt.Checked){
                        activeLicenses.SetData("Expiration", expiration, index);
                    }
                    else{
                        activeLicenses.SetData("Expiration", "", index);
                    }
                }
            }
        }

        console.log("updateModel", orderProductsModel.toJSON());
    }

    function clearPairLink(){
        let productsModel =  root.productModel;
        let id = productsModel.GetData("Id");
        let categoryId = productsModel.GetData("CategoryId");
        let pairId = productsModel.GetData("PairId");

        // clear parents data
        if(categoryId == "Hardware"){
            if (pairId){
                productsModel.SetData("PairId", "");
            }
        }
        else{
            for (let i = 0; i < productsModel.GetItemsCount(); i++){
                if (productsModel.GetData("CategoryId", i) == "Hardware" && id == productsModel.GetData("PairId", i)){
                    productsModel.SetData("PairId", "",i);
                }
            }
        }
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Column {
        id: bodyColumn;

        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;
        spacing: 7;
        property string productCategory: "Software";

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
                let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
                if (selectedProductId){
                    root.productModel.SetData("ProductId", selectedProductId);
                    bodyColumn.productCategory = productCB.model.GetData("CategoryId", productCB.currentIndex);
                    root.productModel.SetData("CategoryId",  bodyColumn.productCategory);
                }

                updatePairModel();
                if (bodyColumn.productCategory == "Software"){
                    updateHardwareCategoryProducts()
                }
                else{
                    updateSoftwareCategoryProducts()

                    devicesList.updateModel({});
                }

                console.log("InstallationEditor onCurrentIndexChanged",productCB.currentIndex, pairCB.model.toJSON());

                if (!blockUpdatingModel){
                    root.updateGui();
                }
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

            visible: bodyColumn.productCategory == "Hardware";
        }

        Item {
            width: parent.width;
            height: 23;

            visible: bodyColumn.productCategory == "Hardware";

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

                        if (selectedProductId === deviceType && (root.orderId === "" || root.orderUuid === orderId) && (status === "Finished" || status === "None")||
                                selectedProductId === deviceType && root.orderId === orderId){
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
                    root.updateModel();
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
//                    if (pairCB.currentIndex < 0){

//                    }
                    console.log("pairCB onCurrentIndexChanged", pairCB.currentIndex);

                    productCB.changeable = pairCB.currentIndex < 0;

                    let pairId = pairCB.model.GetData("Id", pairCB.currentIndex);
                    if (pairId){
                        clearPairLink();

                        let pairId = root.productModel.GetData("PairId");
                        let id = root.productModel.GetData("Id");
                        let categoryId = root.productModel.GetData("CategoryId");

                        // set parents data
                        pairId = pairCB.model.GetData("Id", pairCB.currentIndex);

                        if (pairId && pairId !== ""){
                            if(categoryId === "Hardware"){
                                root.productModel.SetData("PairId", pairId);
                            }
                            else{
                                let productsModel = root.orderProductsModel;
                                for (let i = 0; i < productsModel.GetItemsCount(); i++){
                                    if(pairId === productsModel.GetData("Id", i)){
                                        productsModel.SetData("PairId", id, i);
                                    }
                                }
                            }

//                            for (let i = 0; i < productsModel.GetItemsCount(); i++){
//                                if(categoryId === "Software"){
//                                    if (pairId === productsModel.GetData("Id", i)){
//                                        if(productsModel.GetData("CategoryId", i) === "Hardware"){
//                                            productsModel.SetData("PairId", id, i);
//                                        }
//                                    }
//                                }
//                                else{
//                                    if (id === productsModel.GetData("Id", i)){
//                                        if(productsModel.GetData("CategoryId", i) === "Hardware"){
//                                            productsModel.SetData("PairId", pairId, i);
//                                        }
//                                    }
//                                }
//                            }
                        }
                    }

                    if (!blockUpdatingModel){
                        root.updateModel();
                        root.updateGui();
                    }
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
                    clearPairLink();
                    pairCB.currentIndex = -1;
                    pairCB.model.Clear();
                    //updatePairModel();
                    if (bodyColumn.productCategory == "Software"){
//                        updateHardwareCategoryProducts()
                    }
                    else{
                        updateSoftwareCategoryProducts()
                    }
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
        visible: bodyColumn.productCategory == "Software";
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

}//Container


