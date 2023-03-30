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

    Component.onDestruction: {
        console.log("ProductEditor onDestruction");
    }

    onBlockUpdatingModelChanged: {
        loading.visible = root.blockUpdatingModel;
    }

    onProductModelChanged: {
        console.log("onProductModelChanged", root.productModel);

        if (root.productModel == null && root.productModel === undefined){
            loading.visible = true;
        }
        else{
            loading.visible = false;
        }
    }

    width: 550;
    height: 800;

    UuidGenerator {
        id: uuidGenerator;
    }

    function onModelChanged(){
        console.log("onModelChanged");
        root.rootItem.buttons.setButtonState("Save", true);
    }

    TreeItemModel {
        id: productsFilteringModel;
    }

    function productsModelFilter(categoryId){
        for (let i = 0; i < root.productsModel.GetItemsCount(); i++){
            if (root.productsModel.GetData("CategoryId", i) === categoryId){
                let index = productsFilteringModel.InsertNewItem();

                productsFilteringModel.CopyItemDataFromModel(index, root.productsModel, i);
            }
        }

        return productsFilteringModel;
    }

    function started(){
        console.log("started", root.productModel.toJSON())

        if (root.rootItem.isPairEditing){
            let categoryId = root.productModel.GetData("CategoryId");
            productCB.model = root.productsModelFilter(categoryId);
        }
        else{
            productCB.model = root.productsModel;
        }

        if (!root.productModel.ContainsKey("Id")){
            let uuid = uuidGenerator.generateUUID();

            root.productModel.SetData("Id", uuid);
        }

        if (root.productModel.ContainsKey("CategoryId")){
            bodyColumn.productCategory = root.productModel.GetData("CategoryId")
        }

        devicesList.updateModel({});

        root.productModel.modelChanged.connect(root.onModelChanged);
        root.orderProductsModel.modelChanged.connect(root.onModelChanged);

        console.log("end", root.productModel.toJSON())
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

    function resetPairHardware(){
        console.log("resetPairHardware");
        let categoryId = root.productModel.GetData("CategoryId");
        if (categoryId === "Hardware"){
            root.productModel.SetData("PairId", "");
            return true;
        }

        return false;
    }

    function setPairHardware(softwareId){
        console.log("setPairHardware");
        let categoryId = root.productModel.GetData("CategoryId");
        if (categoryId === "Hardware"){
            root.productModel.SetData("PairId", softwareId);
            return true;
        }

        return false;
    }

    function setPairSoftware(hardwareId){
        console.log("setPairSoftware");
        let softwareId = root.productModel.GetData("Id");
        let categoryId = root.productModel.GetData("CategoryId");
        if (categoryId === "Software"){
            for (let i = 0; i < root.orderProductsModel.GetItemsCount(); i++){
                let currentCategoryId = root.orderProductsModel.GetData("CategoryId", i);
                let currentProductId = root.orderProductsModel.GetData("ProductId", i)
                if (currentCategoryId === "Hardware"){
                    let currentId = root.orderProductsModel.GetData("Id", i);
                    if (currentId === hardwareId){
                        root.orderProductsModel.SetData("PairId", softwareId, i);
                        return true;
                    }
                }
                else if (currentCategoryId === "Pair"){
                    let hardwareModel = root.orderProductsModel.GetData("HardwareProduct", i);
                    if (hardwareModel){
                        let currentId = hardwareModel.GetData("Id");
                        if (currentId === hardwareId){
                            hardwareModel.SetData("PairId", softwareId);
                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }

    function resetPairSoftware(){
        console.log("resetPairSoftware");
        let softwareId = root.productModel.GetData("Id");
        let categoryId = root.productModel.GetData("CategoryId");
        if (categoryId === "Software"){
            for (let i = 0; i < root.orderProductsModel.GetItemsCount(); i++){
                let currentCategoryId = root.orderProductsModel.GetData("CategoryId", i);
                let currentProductId = root.orderProductsModel.GetData("ProductId", i)
                if (currentCategoryId === "Hardware"){
                    let currentPairId = root.orderProductsModel.GetData("PairId", i);
                    if (currentPairId === softwareId){
                        root.orderProductsModel.SetData("PairId", "", i);
                        return true;
                    }
                }
                else if (currentCategoryId === "Pair"){
                    let hardwareModel = root.orderProductsModel.GetData("HardwareProduct", i);
                    if (hardwareModel){
                        let currentPairId = hardwareModel.GetData("PairId");
                        if (currentPairId === softwareId){
                            hardwareModel.SetData("PairId", "");
                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }

    property bool blockUpdatingPairModel: false;

    function updatePairGui(){
        console.log("updatePairGui");
        root.blockUpdatingPairModel = true;

        pairCB.currentIndex = -1;
        let pairId = root.getPairId();
        console.log("pairId", pairId);
        for (let i = 0; i < pairCB.model.GetItemsCount(); i++){
            let id = pairCB.model.GetData("Id", i);
            console.log("id", id);
            if (pairId === id){
                pairCB.currentIndex = i;
                break;
            }
        }

        root.blockUpdatingPairModel = false;
    }

    function updatePairModel(){
        console.log("updatePairModel");
        if (root.blockUpdatingPairModel){
            return;
        }

        let categoryId = root.productModel.GetData("CategoryId");
        if (categoryId === "Hardware"){
            root.resetPairHardware();
            if (pairCB.currentIndex >= 0){
                let selectedSoftwareId = pairCB.model.GetData("Id", pairCB.currentIndex);
                root.setPairHardware(selectedSoftwareId);
            }
        }
        else if (categoryId === "Software"){
            root.resetPairSoftware();
            if (pairCB.currentIndex >= 0){
                let selectedHardwareId = pairCB.model.GetData("Id", pairCB.currentIndex);
                root.setPairSoftware(selectedHardwareId);
            }
        }
    }

    function updateHardwareCategoryProducts(){
        console.log("updateHardwareCategoryProducts")
        let productModel = root.orderProductsModel;
        console.log("root.orderProductsModel", root.orderProductsModel.toJSON())

        let id = root.productModel.GetData("Id");
        let resultModel = pairCB.model;
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            let productCategory = productModel.GetData("CategoryId", i);
            if (productCategory === "Hardware"){
                let pairId = "";
                if (productModel.ContainsKey("PairId", i)){
                    pairId = productModel.GetData("PairId", i);
                }
                else{
                    productModel.SetData("PairId", "")
                }

                if (productModel.GetData("CategoryId", i) === "Hardware" && pairId === ""){
                    let resultIndex = resultModel.InsertNewItem();
                    let productId = productModel.GetData("ProductId", i);
                    resultModel.SetData("ProductId", productId, resultIndex);
                    resultModel.SetData("Id", productModel.GetData("Id", i), resultIndex);
                    resultModel.SetData("Name", "#" + (i + 1) + " " + getProductName(productId), resultIndex);
                }
            }
            else if (productCategory === "Pair"){
                let hardwareModel = productModel.GetData("HardwareProduct", i);
                let softwareModel = productModel.GetData("SoftwareProduct", i);
                if (softwareModel.GetData("Id") === id){
                    let resultIndex = resultModel.InsertNewItem();
                    let productId = hardwareModel.GetData("ProductId");
                    resultModel.SetData("ProductId", productId, resultIndex);
                    resultModel.SetData("Id", hardwareModel.GetData("Id"), resultIndex);
                    resultModel.SetData("Name", "#" + (i + 1) + " " + getProductName(productId), resultIndex);
                }
            }
        }
        console.log("updateHardwareCategoryProducts", resultModel.toJSON())
    }

    function findHardwarePair(id){
        console.log("findHardwarePair");
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

    function getPairId(){
        console.log("getPairId");
        let id = root.productModel.GetData("Id");
        let categoryId = root.productModel.GetData("CategoryId");

        console.log("id", id);
        console.log("categoryId", categoryId);

        if (categoryId === "Hardware"){
            let pairId = root.productModel.GetData("PairId");
            return pairId;
        }
        else if (categoryId === "Software"){
            for (let i = 0; i < root.orderProductsModel.GetItemsCount(); i++){
                let currentId = root.orderProductsModel.GetData("Id", i);
                let currentCategoryId = root.orderProductsModel.GetData("CategoryId", i);
                if (currentCategoryId === "Hardware"){
                    let currentPairId = root.orderProductsModel.GetData("PairId", i);
                    if (currentPairId === id){
                        return currentId;
                    }
                }
                else if (currentCategoryId === "Pair"){
                    let hardwareModel = root.orderProductsModel.GetData("HardwareProduct", i);
                    currentId = hardwareModel.GetData("Id");
                    let currentPairId = hardwareModel.GetData("PairId");
                    if (currentPairId === id){
                        return currentId;
                    }
                }
            }
        }

        return "";
    }

    function updateSoftwareCategoryProducts(){
        console.log("updateSoftwareCategoryProducts");
        console.log("root.orderProductsModel", root.orderProductsModel.toJSON());
        let id = root.productModel.GetData("Id")
        let categoryId = root.productModel.GetData("CategoryId")
        let productModel = root.orderProductsModel;
        let resultModel = pairCB.model;
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            let currentCategoryId = productModel.GetData("CategoryId", i);
            if (currentCategoryId === "Software"){
                let hardwareId = findHardwarePair(productModel.GetData("Id", i));
                if (hardwareId == ""){
                    let productId = productModel.GetData("ProductId", i);
                    let resultIndex = resultModel.InsertNewItem();
                    resultModel.SetData("ProductId", productId, resultIndex);
                    resultModel.SetData("Id", productModel.GetData("Id", i), resultIndex);
                    resultModel.SetData("Name", "#" + (i + 1) + " " + getProductName(productId), resultIndex);
                }
            }
            else if (currentCategoryId === "Pair"){
                let hardwareModel = productModel.GetData("HardwareProduct", i);
                let softwareModel = productModel.GetData("SoftwareProduct", i);

                let softwareId = softwareModel.GetData("Id");
                let hardwareId = hardwareModel.GetData("Id");

                if (hardwareId === id){
                    let resultIndex = resultModel.InsertNewItem();
                    resultModel.SetData("ProductId", softwareModel.GetData("ProductId"), resultIndex);
                    resultModel.SetData("Id", softwareId, resultIndex);
                    resultModel.SetData("Name", "#" + (i + 1) + " " + getProductName(softwareModel.GetData("ProductId")), resultIndex);
                }
            }
        }
        console.log("updateSoftwareCategoryProducts", resultModel.toJSON())
    }

    function updateGui(){
        console.log("updateGui");
        blockUpdatingModel = true;

        let productId = root.productModel.GetData("ProductId");
        let id = root.productModel.GetData("Id");
        let pairId = root.productModel.GetData("PairId");
        let categoryId = root.productModel.GetData("CategoryId");

        productCB.currentIndex = -1;
        let productModel = productCB.model;
        if (productModel){
            for (let i = 0; i < productModel.GetItemsCount(); i++){
                let id = productModel.GetData("Id", i);
                if (id === productId){
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

        root.updatePairGui();

        blockUpdatingModel = false;
    }

    function updateModel(){
        console.log("updateModel", blockUpdatingModel);
        if (blockUpdatingModel){
            return;
        }

        if (productCB.currentIndex >= 0){
            let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
            let selectedCategoryId = productCB.model.GetData("CategoryId", productCB.currentIndex);
            root.productModel.SetData("ProductId", selectedProductId);
            root.productModel.SetData("CategoryId", selectedCategoryId);
        }
        else{
            root.productModel.SetData("ProductId", "");
            root.productModel.SetData("CategoryId", "");
        }

        let categoryId = root.productModel.GetData("CategoryId");
//        console.log("categoryId", categoryId);
//        pairCB.model.Clear();
//        if (categoryId === "Software"){
//            root.updateHardwareCategoryProducts();
//        }
//        else if (categoryId === "Hardware"){
//            root.updateSoftwareCategoryProducts();
//        }

        bodyColumn.productCategory = categoryId;
        if (categoryId === "Hardware"){
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

//        root.updatePairModel();

        console.log("updateModel end", orderProductsModel.toJSON());
    }

    function clearPairLink(){
        console.log("clearPairLink");
        let id = root.productModel.GetData("Id");
        let categoryId = root.productModel.GetData("CategoryId");
        let pairId = root.productModel.GetData("PairId");

        // clear parents data
        if(categoryId === "Hardware"){
            root.productModel.SetData("PairId", "");
        }
        else if (categoryId === "Software"){
            if (root.rootItem.isPairEditing){
                if (root.rootItem.activeProductIndex >= 0){
                    if (root.orderProductsModel.ContainsKey("HardwareProduct", root.rootItem.activeProductIndex)){
                        let hardwareProductModel = root.orderProductsModel.GetData("HardwareProduct", root.rootItem.activeProductIndex);
                        hardwareProductModel.SetData("PairId", "");
                    }
                }
            }
            else{
                for (let i = 0; i < root.orderProductsModel.GetItemsCount(); i++){
                    if (root.orderProductsModel.GetData("CategoryId", i) === "Hardware" && id === root.orderProductsModel.GetData("PairId", i)){
                        root.orderProductsModel.SetData("PairId", "",i);
                    }
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
                console.log("productCB onCurrentIndexChanged", productCB.currentIndex);

                root.updateModel();
                if (productCB.currentIndex >= 0){
                    let categoryId = productCB.model.GetData("CategoryId", productCB.currentIndex);

                    pairCB.model.Clear();
                    if (categoryId === "Software"){
                        root.updateHardwareCategoryProducts();
                    }
                    else if (categoryId === "Hardware"){
                        root.updateSoftwareCategoryProducts();
                    }

                    devicesList.devicesListUpdate();

                    if (!root.blockUpdatingModel){
                        root.updateGui();
                    }
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

                fields: ["Id", "Name", "DeviceType", "OrderId", "Status", "OrderUuid"];

                onModelUpdated: {
                    devicesList.devicesListUpdate();

                    root.updateGui();
                }

                TreeItemModel {
                    id: filteringModel;
                }

                function devicesListUpdate(){
                    filteringModel.Clear();

                    if (devicesList.collectionModel != null){

                        devicesList.filteringDevicesList();

                        let newIndex = filteringModel.InsertNewItem(0);

                        filteringModel.SetData("Id", "", newIndex);
                        filteringModel.SetData("Name", "New Device", newIndex);

                        deviceCB.model = filteringModel;
                    }

//                    root.updateGui();
                }

                function filteringDevicesList(){
                    console.log("filteringDevicesList");
                    console.log("productCB.model", productCB.model.toJSON());

                    for (let i = 0; i < devicesList.collectionModel.GetItemsCount(); i++){
                        let status = devicesList.collectionModel.GetData("Status", i);
                        let orderId = devicesList.collectionModel.GetData("OrderUuid", i);
                        let deviceType = devicesList.collectionModel.GetData("DeviceType", i);
//                        let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
                        let selectedProductId = root.productModel.GetData("ProductId");

                        console.log("root.orderUuid", root.orderUuid);
                        console.log("deviceType", deviceType);
                        console.log("status", status);
                        console.log("selectedProductId", selectedProductId);

                        if (selectedProductId === deviceType && (root.orderUuid === "" || root.orderUuid === orderId) && (status === "Finished" || status === "None")||
                                selectedProductId === deviceType && root.orderUuid === orderId){
                            let index = filteringModel.InsertNewItem();
                            filteringModel.CopyItemDataFromModel(index, devicesList.collectionModel, i);

                            console.log("filteringModel", filteringModel.toJSON());

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
                    console.log("deviceCB onCurrentIndexChanged", deviceCB.currentIndex);
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
                    console.log("pairCB onCurrentIndexChanged", pairCB.currentIndex);
                    root.updatePairModel();
                    productCB.changeable = pairCB.currentIndex < 0;

//                    if (!root.blockUpdatingPairModel){
//                        updatePairGui();
//                    }
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
                    updatePairModel();
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

            root.updateModel();
        }
    }

    Loading {
        id: loading;

        anchors.fill: parent;
        visible: false;
    }

}//Container


