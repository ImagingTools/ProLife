import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0

Item {
    id: installationEditorContainer;

    //    property TreeItemModel documentModel: TreeItemModel{}
    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}
    property TreeItemModel orderProductsModel: TreeItemModel {}
    property TreeItemModel softwareModel: TreeItemModel{}
    property TreeItemModel hardwareModel: TreeItemModel{}

    property bool blockUpdatingModel: false;
    property bool centered: true;
    property int activeProductIndex: -1;

    property string orderId;
    property string orderUuid;

    // ProductEditorDialog reference
    property Item rootItem: null;

    // property string selectedPairId: "";

    onOrderUuidChanged: {
        console.log("onOrderUuidChanged", orderUuid);
    }

    width: 550;
    height: 800;

    UuidGenerator {
        id: uuidGenerator;
    }

    function onModelChanged(){
        installationEditorContainer.rootItem.buttons.setButtonState("Save", true);
    }

    function started(){
        productCB.model = installationEditorContainer.productsModel;
        if (activeProductIndex > -1){
           // productCB.enabled = false;
        }

        if (installationEditorContainer.activeProductIndex == -1){
            installationEditorContainer.activeProductIndex = orderProductsModel.InsertNewItem();
            if (!orderProductsModel.ContainsKey("Id", activeProductIndex)){
                let uuid = uuidGenerator.generateUUID();
                orderProductsModel.SetData("Id", uuid, activeProductIndex);
            }
        }

        devicesList.updateModel({});

        orderProductsModel.modelChanged.connect(installationEditorContainer.onModelChanged);
        console.log("started orderProductsModel", orderProductsModel.toJSON());

       // updateGui();
    }

    function getProductName(productId){
        let productModel = installationEditorContainer.productsModel;
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
        let categoryId = orderProductsModel.GetData("CategoryId", activeProductIndex)
        let id = orderProductsModel.GetData("Id", activeProductIndex)
        let isPairFinded = false;

        pairCB.currentIndex = -1;
        pairCB.model.Clear();
        if (orderProductsModel){
            for (let i = 0; i < orderProductsModel.GetItemsCount(); i++){
                //  let modelMacAddress = productModel.GetData("PairId", i);
                if (categoryId == "Hardware" && orderProductsModel.GetData("PairId", activeProductIndex) === orderProductsModel.GetData("Id", i)
                        && orderProductsModel.GetData("CategoryId", i) == "Software"){
                    isPairFinded = true;
                }
                if (categoryId == "Software" && id === orderProductsModel.GetData("PairId", i)
                        && orderProductsModel.GetData("CategoryId", i) == "Hardware"){
                    isPairFinded = true;
                }
                if (isPairFinded){
                    let dependencyModel = pairCB.model;
                    let index = dependencyModel.InsertNewItem();
                    dependencyModel.SetData("ProductId", orderProductsModel.GetData("ProductId", i), index);
                    dependencyModel.SetData("Id", orderProductsModel.GetData("Id", i), 0);
                    //                    dependencyModel.SetData("MacAddress", orderProductsModel.GetData("MacAddress", i), index);
                    let productId = orderProductsModel.GetData("ProductId", i);
                    dependencyModel.SetData("Name", "#" + (i + 1) + " " + getProductName(productId), index);
                    pairCB.currentIndex = 0;
                    break;
                }
            }
        }
    }

    function updateHardwareCategoryProducts(){
        let productModel = installationEditorContainer.orderProductsModel;
        let resultModel = pairCB.model;
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            let pairId = productModel.GetData("PairId", i);
            if (!pairId){
                productModel.SetData("PairId", "", i)
                pairId = ""
            }
            if (productModel.GetData("CategoryId", i) === "Hardware" && pairId === ""){
                let resultIndex = resultModel.InsertNewItem();
                let productId = productModel.GetData("ProductId", i);
                resultModel.SetData("ProductId", productId, resultIndex);
                resultModel.SetData("Id", productModel.GetData("Id", i), resultIndex);
                resultModel.SetData("Name", "#" + (i + 1) + " " + getProductName(productId), resultIndex);
            }
        }
        console.log("updateHardwareCategoryProducts", resultModel.toJSON())
    }

    function findHardwarePair(id){
        let retVal = ""
        let productsModel = installationEditorContainer.orderProductsModel;
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
        let productModel = installationEditorContainer.orderProductsModel;
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

        let productId = orderProductsModel.GetData("ProductId", activeProductIndex);
        let id = orderProductsModel.GetData("Id", activeProductIndex);
        let pairId = orderProductsModel.GetData("PairId", activeProductIndex);
        let categoryId = orderProductsModel.GetData("CategoryId", activeProductIndex);

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
        if (installationEditorContainer.orderProductsModel.ContainsKey("DeviceId", installationEditorContainer.activeProductIndex)){
            let deviceId = installationEditorContainer.orderProductsModel.GetData("DeviceId", installationEditorContainer.activeProductIndex);
            let deviceModel = deviceCB.model;
            for (let i = 0; i < deviceModel.GetItemsCount(); i++){
                let id = deviceModel.GetData("Id", i);
                if (id === deviceId){
                    deviceCB.currentIndex = i;
                    break;
                }
            }
        }

        //        if (orderProductsModel.ContainsKey("SerialNumber", activeProductIndex)){
        //            serialNumberInput.text = orderProductsModel.GetData("SerialNumber", activeProductIndex);
        //        }

        //        if (orderProductsModel.ContainsKey("MacAddress", activeProductIndex)){
        //            macAddressInput.text = orderProductsModel.GetData("MacAddress", activeProductIndex);
        //        }

        licensesTable.rowModel.clear();

//        let activeLicensesModel = orderProductsModel.GetData("ActiveLicenses", activeProductIndex);
//        if (!activeLicensesModel){
//            activeLicensesModel = orderProductsModel.AddTreeModel("ActiveLicenses", activeProductIndex);
//        }

        let licensesModel;
        if (installationEditorContainer.licensesModel){
            for (let i = 0; i < installationEditorContainer.licensesModel.GetItemsCount(); i++){
                let id = installationEditorContainer.licensesModel.GetData("Id", i);
                if (id === productId){
                    let productLicensesModel = installationEditorContainer.licensesModel.GetData("Licenses", i);
                    licensesModel = productLicensesModel;
                }
            }
        }

        if (licensesModel){
            for (let i = 0; i < licensesModel.GetItemsCount(); i++){
                let licenseId = licensesModel.GetData("Id", i);
                let licenseName = licensesModel.GetData("Name", i);

                let row = {"Id": licenseId, "Name": licenseName, "LicenseState": Qt.Unchecked, "ExpirationState": Qt.Unchecked, "Expiration": ""}

                if (orderProductsModel.ContainsKey("ActiveLicenses", activeProductIndex)){
                    let activeLicensesModel = orderProductsModel.GetData("ActiveLicenses", activeProductIndex);
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

        let categoryId = orderProductsModel.GetData("CategoryId", activeProductIndex);

        if (categoryId == "Hardware"){
            //            orderProductsModel.SetData("SerialNumber", serialNumberInput.text, activeProductIndex);
            //            orderProductsModel.SetData("MacAddress", macAddressInput.text, activeProductIndex);

            if (deviceCB.currentIndex >= 0){
                let selectedDeviceId = deviceCB.model.GetData("Id", deviceCB.currentIndex);
                installationEditorContainer.orderProductsModel.SetData("DeviceId", selectedDeviceId, installationEditorContainer.activeProductIndex);
            }
            else{
                if (installationEditorContainer.orderProductsModel.ContainsKey("DeviceId", installationEditorContainer.activeProductIndex)){
                    installationEditorContainer.orderProductsModel.RemoveData("DeviceId", installationEditorContainer.activeProductIndex);
                }
//                installationEditorContainer.orderProductsModel.SetData("DeviceId", "", installationEditorContainer.activeProductIndex);
            }
        }
        else{
            let activeLicenses = orderProductsModel.AddTreeModel("ActiveLicenses", activeProductIndex);
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
        if (installationEditorContainer.activeProductIndex == -1){
            return;
        }
        let productsModel =  installationEditorContainer.orderProductsModel;
        let id = productsModel.GetData("Id", activeProductIndex);
        let categoryId = productsModel.GetData("CategoryId", installationEditorContainer.activeProductIndex);
        let pairId = productsModel.GetData("PairId", installationEditorContainer.activeProductIndex);

        // clear parents data
        if(categoryId == "Hardware"){
            if (pairId){
                productsModel.SetData("PairId", "", installationEditorContainer.activeProductIndex);
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
        property string productCategory;

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
                    orderProductsModel.SetData("ProductId", selectedProductId, activeProductIndex);
                    bodyColumn.productCategory = productCB.model.GetData("CategoryId", productCB.currentIndex);
                    orderProductsModel.SetData("CategoryId",  bodyColumn.productCategory, activeProductIndex);
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
                    installationEditorContainer.updateGui();
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

                fields: ["Id", "Name", "DeviceType", "OrderId", "ProductionStatus"];

                onModelUpdated: {
                    filteringModel.Clear();

                    if (devicesList.collectionModel != null){

                        devicesList.filteringDevicesList();

                        let newIndex = filteringModel.InsertNewItem(0);

//                        let newIndex = devicesList.collectionModel.InsertNewItem(0);

//                        devicesList.collectionModel.SetData("Id", "", newIndex);
//                        devicesList.collectionModel.SetData("Name", "New Device", newIndex);

                        filteringModel.SetData("Id", "", newIndex);
                        filteringModel.SetData("Name", "New Device", newIndex);

                        deviceCB.model = filteringModel;
//                        deviceCB.model = devicesList.collectionModel;
                    }

                    installationEditorContainer.updateGui();
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
//                    if (pairCB.currentIndex < 0){

//                    }
                    console.log("pairCB onCurrentIndexChanged", pairCB.currentIndex);

                    productCB.changeable = pairCB.currentIndex < 0;

                    let pairId = pairCB.model.GetData("Id", pairCB.currentIndex);
                    if (pairId){
                        let productsModel = installationEditorContainer.orderProductsModel;

                        clearPairLink();

                        let activeProductIndex = installationEditorContainer.activeProductIndex;
                        let pairId = productsModel.GetData("PairId", activeProductIndex);
                        let id = productsModel.GetData("Id", activeProductIndex);
                        //                        let macAddress = productsModel.GetData("MacAddress", activeProductIndex);
                        let categoryId = productsModel.GetData("CategoryId", activeProductIndex);


                        // set parents data
                        pairId = pairCB.model.GetData("Id", pairCB.currentIndex);;

                        if (pairId && pairId != ""){
                            for (let i = 0; i < productsModel.GetItemsCount(); i++){
                                if(categoryId == "Software"){
                                    if (pairId == productsModel.GetData("Id", i)){
                                        if(productsModel.GetData("CategoryId", i) == "Hardware"){
                                            productsModel.SetData("PairId", id, i);
                                        }
                                    }
                                }
                                else{
                                    if (id == productsModel.GetData("Id", i)){
                                        if(productsModel.GetData("CategoryId", i) == "Hardware"){
                                            productsModel.SetData("PairId", pairId, i);
                                        }
                                    }
                                }
                            }
                        }

                    }

                    if (!blockUpdatingModel){
                        installationEditorContainer.updateModel();
                        installationEditorContainer.updateGui();
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
                        updateHardwareCategoryProducts()
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
                installationEditorContainer.updateModel();
            }
        }
    }

}//Container


