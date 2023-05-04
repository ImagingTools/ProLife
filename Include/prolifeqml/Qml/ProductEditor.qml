import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0

Item {
    id: productEditor;

    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}
    property TreeItemModel orderProductsModel: TreeItemModel {}

    property TreeItemModel productModel: TreeItemModel {}

    property TreeItemModel devicesModel: TreeItemModel {}

    property bool blockUpdatingModel: false;
    property bool centered: true;

    property string orderId;
    property string orderUuid;

    // ProductEditorDialog reference
    property Item rootItem: null;

    Component.onCompleted: {
        loading.visible = true;
    }

    onBlockUpdatingModelChanged: {
        loading.visible = productEditor.blockUpdatingModel;
    }

    TreeItemModel {
        id: filteringModel;
    }

    function devicesListUpdate(){
        filteringModel.Clear();

        if (productEditor.devicesModel != null){

            productEditor.filteringDevicesList();

            let newIndex = filteringModel.InsertNewItem(0);

            filteringModel.SetData("Id", "", newIndex);
            filteringModel.SetData("Name", "New Device", newIndex);

            deviceCB.model = filteringModel;
        }
    }

    function filteringDevicesList(){
        for (let i = 0; i < productEditor.devicesModel.GetItemsCount(); i++){
            let status = productEditor.devicesModel.GetData("Status", i);
            let orderId = productEditor.devicesModel.GetData("OrderUuid", i);
            let deviceType = productEditor.devicesModel.GetData("DeviceType", i);
            let selectedProductId = productEditor.productModel.GetData("ProductId");

            if (selectedProductId === deviceType && (productEditor.orderUuid === "" || productEditor.orderUuid === orderId) && (status === "Finished" || status === "None")||
                    selectedProductId === deviceType && productEditor.orderUuid === orderId){
                let index = filteringModel.InsertNewItem();
                filteringModel.CopyItemDataFromModel(index, productEditor.devicesModel, i);
            }
        }
    }

    width: 550;
    height: 800;

    UuidGenerator {
        id: uuidGenerator;
    }

    function onModelChanged(){
        productEditor.rootItem.buttons.setButtonState("Save", true);
    }

    TreeItemModel {
        id: productsFilteringModel;
    }

    function productsModelFilter(categoryId){
        for (let i = 0; i < productEditor.productsModel.GetItemsCount(); i++){
            if (productEditor.productsModel.GetData("CategoryId", i) === categoryId){
                let index = productsFilteringModel.InsertNewItem();

                productsFilteringModel.CopyItemDataFromModel(index, productEditor.productsModel, i);
            }
        }

        return productsFilteringModel;
    }

    function started(){
        if (productEditor.rootItem.isPairEditing){
            let categoryId = productEditor.productModel.GetData("CategoryId");
            productCB.model = productEditor.productsModelFilter(categoryId);
        }
        else{
            productCB.model = productEditor.productsModel;
        }

        if (!productEditor.productModel.ContainsKey("Id")){
            let uuid = uuidGenerator.generateUUID();

            productEditor.productModel.SetData("Id", uuid);
        }

        if (productEditor.productModel.ContainsKey("CategoryId")){
            bodyColumn.productCategory = productEditor.productModel.GetData("CategoryId")
        }

        productEditor.devicesListUpdate();
        productEditor.updateGui();

        productEditor.productModel.modelChanged.connect(productEditor.onModelChanged);
        productEditor.orderProductsModel.modelChanged.connect(productEditor.onModelChanged);
    }

    function getProductName(productId){
        let productModel = productEditor.productsModel;
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
        let categoryId = productEditor.productModel.GetData("CategoryId");
        if (categoryId === "Hardware"){
            productEditor.productModel.SetData("PairId", "");
            return true;
        }

        return false;
    }

    function setPairHardware(softwareId){
        let categoryId = productEditor.productModel.GetData("CategoryId");
        if (categoryId === "Hardware"){
            productEditor.productModel.SetData("PairId", softwareId);
            return true;
        }

        return false;
    }

    function setPairSoftware(hardwareId){
        let softwareId = productEditor.productModel.GetData("Id");
        let categoryId = productEditor.productModel.GetData("CategoryId");
        if (categoryId === "Software"){
            for (let i = 0; i < productEditor.orderProductsModel.GetItemsCount(); i++){
                let currentCategoryId = productEditor.orderProductsModel.GetData("CategoryId", i);
                let currentProductId = productEditor.orderProductsModel.GetData("ProductId", i)
                if (currentCategoryId === "Hardware"){
                    let currentId = productEditor.orderProductsModel.GetData("Id", i);
                    if (currentId === hardwareId){
                        productEditor.orderProductsModel.SetData("PairId", softwareId, i);
                        return true;
                    }
                }
                else if (currentCategoryId === "Pair"){
                    let hardwareModel = productEditor.orderProductsModel.GetData("HardwareProduct", i);
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
        let softwareId = productEditor.productModel.GetData("Id");
        let categoryId = productEditor.productModel.GetData("CategoryId");
        if (categoryId === "Software"){
            for (let i = 0; i < productEditor.orderProductsModel.GetItemsCount(); i++){
                let currentCategoryId = productEditor.orderProductsModel.GetData("CategoryId", i);
                let currentProductId = productEditor.orderProductsModel.GetData("ProductId", i)
                if (currentCategoryId === "Hardware"){
                    let currentPairId = productEditor.orderProductsModel.GetData("PairId", i);
                    if (currentPairId === softwareId){
                        productEditor.orderProductsModel.SetData("PairId", "", i);
                        return true;
                    }
                }
                else if (currentCategoryId === "Pair"){
                    let hardwareModel = productEditor.orderProductsModel.GetData("HardwareProduct", i);
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
        productEditor.blockUpdatingPairModel = true;

        pairCB.currentIndex = -1;
        let pairId = productEditor.getPairId();
        for (let i = 0; i < pairCB.model.GetItemsCount(); i++){
            let id = pairCB.model.GetData("Id", i);
            if (pairId === id){
                pairCB.currentIndex = i;
                break;
            }
        }

        productEditor.blockUpdatingPairModel = false;
    }

    function updatePairModel(){
        if (productEditor.blockUpdatingPairModel){
            return;
        }

        let categoryId = productEditor.productModel.GetData("CategoryId");
        if (categoryId === "Hardware"){
            productEditor.resetPairHardware();
            if (pairCB.currentIndex >= 0){
                let selectedSoftwareId = pairCB.model.GetData("Id", pairCB.currentIndex);
                productEditor.setPairHardware(selectedSoftwareId);
            }
        }
        else if (categoryId === "Software"){
            productEditor.resetPairSoftware();
            if (pairCB.currentIndex >= 0){
                let selectedHardwareId = pairCB.model.GetData("Id", pairCB.currentIndex);
                productEditor.setPairSoftware(selectedHardwareId);
            }
        }
    }

    function updateHardwareCategoryProducts(){
        let productModel = productEditor.orderProductsModel;

        let id = productEditor.productModel.GetData("Id");
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
    }

    function findHardwarePair(id){
        let retVal = ""
        let productsModel = productEditor.orderProductsModel;
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
        let id = productEditor.productModel.GetData("Id");
        let categoryId = productEditor.productModel.GetData("CategoryId");

        if (categoryId === "Hardware"){
            let pairId = productEditor.productModel.GetData("PairId");
            return pairId;
        }
        else if (categoryId === "Software"){
            for (let i = 0; i < productEditor.orderProductsModel.GetItemsCount(); i++){
                let currentId = productEditor.orderProductsModel.GetData("Id", i);
                let currentCategoryId = productEditor.orderProductsModel.GetData("CategoryId", i);
                if (currentCategoryId === "Hardware"){
                    let currentPairId = productEditor.orderProductsModel.GetData("PairId", i);
                    if (currentPairId === id){
                        return currentId;
                    }
                }
                else if (currentCategoryId === "Pair"){
                    let hardwareModel = productEditor.orderProductsModel.GetData("HardwareProduct", i);
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
        let id = productEditor.productModel.GetData("Id")
        let categoryId = productEditor.productModel.GetData("CategoryId")
        let productModel = productEditor.orderProductsModel;
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
    }

    function updateGui(){
        blockUpdatingModel = true;

        let productId = productEditor.productModel.GetData("ProductId");
        let id = productEditor.productModel.GetData("Id");
        let pairId = productEditor.productModel.GetData("PairId");
        let categoryId = productEditor.productModel.GetData("CategoryId");

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
        if (productEditor.productModel.ContainsKey("DeviceId")){
            let deviceId = productEditor.productModel.GetData("DeviceId");
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
        if (productEditor.licensesModel){
            for (let i = 0; i < productEditor.licensesModel.GetItemsCount(); i++){
                let id = productEditor.licensesModel.GetData("Id", i);
                if (id === productId){
                    let productLicensesModel = productEditor.licensesModel.GetData("Licenses", i);
                    licensesModel = productLicensesModel;
                }
            }
        }

        if (licensesModel){
            for (let i = 0; i < licensesModel.GetItemsCount(); i++){
                let licenseId = licensesModel.GetData("Id", i);
                let licenseName = licensesModel.GetData("Name", i);

                let row = {"Id": licenseId, "Name": licenseName, "LicenseState": Qt.Unchecked, "ExpirationState": Qt.Unchecked, "Expiration": ""}

                if (productEditor.productModel.ContainsKey("ActiveLicenses")){
                    let activeLicensesModel = productEditor.productModel.GetData("ActiveLicenses");
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

        productEditor.updatePairGui();

        blockUpdatingModel = false;
    }

    function updateModel(){
        if (blockUpdatingModel){
            return;
        }

        if (productCB.currentIndex >= 0){
            let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
            let selectedCategoryId = productCB.model.GetData("CategoryId", productCB.currentIndex);
            productEditor.productModel.SetData("ProductId", selectedProductId);
            productEditor.productModel.SetData("CategoryId", selectedCategoryId);
        }
        else{
            productEditor.productModel.SetData("ProductId", "");
            productEditor.productModel.SetData("CategoryId", "");
        }

        let categoryId = productEditor.productModel.GetData("CategoryId");
        bodyColumn.productCategory = categoryId;
        if (categoryId === "Hardware"){
            if (deviceCB.currentIndex >= 0){
                let selectedDeviceId = deviceCB.model.GetData("Id", deviceCB.currentIndex);
                productEditor.productModel.SetData("DeviceId", selectedDeviceId);
            }
            else{
                if (productEditor.productModel.ContainsKey("DeviceId")){
                    productEditor.productModel.RemoveData("DeviceId");
                }
            }
        }
        else{
            let activeLicenses = productEditor.productModel.AddTreeModel("ActiveLicenses");
            activeLicenses.Clear();
            for (let i = 0; i < licensesTable.rowModel.count; i++){
                let rowObj = licensesTable.rowModel.get(i);

                let licenseId = rowObj["Id"];
                let licenseName = rowObj["Name"];
                let expirationState  = rowObj["ExpirationState"];
                let expiration  = rowObj["Expiration"];
                let state = rowObj["LicenseState"];

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
    }

    function clearPairLink(){
        let id = productEditor.productModel.GetData("Id");
        let categoryId = productEditor.productModel.GetData("CategoryId");
        let pairId = productEditor.productModel.GetData("PairId");

        // clear parents data
        if(categoryId === "Hardware"){
            productEditor.productModel.SetData("PairId", "");
        }
        else if (categoryId === "Software"){
            if (productEditor.rootItem.isPairEditing){
                if (productEditor.rootItem.activeProductIndex >= 0){
                    if (productEditor.orderProductsModel.ContainsKey("HardwareProduct", productEditor.rootItem.activeProductIndex)){
                        let hardwareProductModel = productEditor.orderProductsModel.GetData("HardwareProduct", productEditor.rootItem.activeProductIndex);
                        hardwareProductModel.SetData("PairId", "");
                    }
                }
            }
            else{
                for (let i = 0; i < productEditor.orderProductsModel.GetItemsCount(); i++){
                    if (productEditor.orderProductsModel.GetData("CategoryId", i) === "Hardware" && id === productEditor.orderProductsModel.GetData("PairId", i)){
                        productEditor.orderProductsModel.SetData("PairId", "",i);
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
                productEditor.updateModel();
                if (productCB.currentIndex >= 0){
                    let categoryId = productCB.model.GetData("CategoryId", productCB.currentIndex);

                    pairCB.model.Clear();
                    if (categoryId === "Software"){
                        productEditor.updateHardwareCategoryProducts();
                    }
                    else if (categoryId === "Hardware"){
                        productEditor.updateSoftwareCategoryProducts();
                    }

                    productEditor.devicesListUpdate();

                    if (!productEditor.blockUpdatingModel){
                        productEditor.updateGui();
                    }
                }

            }

            MouseArea {
                id: disabledMA;

                z: 100;

                anchors.fill: productCB;

                visible: !productCB.changeable;

                onClicked: {
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

            ComboBox {
                id: deviceCB;

                height: parent.height;
                width: parent.width;

                radius: 3;

                onCurrentIndexChanged: {
                    productEditor.updateModel();
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
                    productEditor.updatePairModel();
                    productCB.changeable = pairCB.currentIndex < 0;
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
            productEditor.updateModel();
        }
    }

    Loading {
        id: loading;

        anchors.fill: parent;
        visible: false;
    }

}//Container


