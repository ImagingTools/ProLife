import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0

Item {
    id: productEditor;

    width: 550;
    height: 800;

    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}
    property TreeItemModel orderProductsModel: TreeItemModel {}

    property TreeItemModel productModel: TreeItemModel {}

    property TreeItemModel devicesModel: TreeItemModel {}

    property TreeItemModel pairsModel: TreeItemModel{}

    property bool blockUpdatingModel: false;
    property bool centered: true;

    property string orderId;
    property string orderUuid;

    property string productCategory: "";
    property string productId: "";

    // ProductEditorDialog reference
    property Item rootItem: null;

    Component.onDestruction: {
        productEditor.productModel.onDataChanged.disconnect(productEditor.onModelChanged);
    }

    onProductIdChanged: {
        productEditor.productModel.SetData("ProductId", productEditor.productId);
    }

    onProductCategoryChanged: {
        productEditor.productModel.SetData("CategoryId", productEditor.productCategory);
    }

    property TreeItemModel hardwareProductsModel: TreeItemModel {}
    property TreeItemModel softwareProductsModel: TreeItemModel {}

    onProductsModelChanged: {
        hardwareProductsModel.Clear();
        softwareProductsModel.Clear();

        for (let i = 0; i < productEditor.productsModel.GetItemsCount(); i++){
            let categoryId = productEditor.productsModel.GetData("CategoryId", i);
            if (categoryId === "Software"){
                let index = softwareProductsModel.InsertNewItem();
                softwareProductsModel.CopyItemDataFromModel(index, productEditor.productsModel, i)
            }
            else if (categoryId === "Hardware"){
                let index = hardwareProductsModel.InsertNewItem();
                hardwareProductsModel.CopyItemDataFromModel(index, productEditor.productsModel, i)
            }
        }
    }

    function getDevicesModel(){
        let resultModel = treeItemModelComp.createObject(null);
        let selectedProductId = productEditor.productModel.GetData("ProductId");
        for (let i = 0; i < productEditor.devicesModel.GetItemsCount(); i++){
            let status = productEditor.devicesModel.GetData("Status", i);
            let orderId = productEditor.devicesModel.GetData("OrderUuid", i);
            let deviceType = productEditor.devicesModel.GetData("DeviceType", i);

            if (selectedProductId === deviceType && (orderId === "" || productEditor.orderUuid === orderId) && (status === "Finished" || status === "None")||
                    selectedProductId === deviceType && productEditor.orderUuid === orderId){
                let index = resultModel.InsertNewItem();
                resultModel.CopyItemDataFromModel(index, productEditor.devicesModel, i);
            }
        }

        let newIndex = resultModel.InsertNewItem(0);

        let deviceId = uuidGenerator.generateUUID();
        if (productEditor.productModel.ContainsKey("IsNewDevice")){
            deviceId = productEditor.productModel.GetData("DeviceId");
        }

        resultModel.SetData("Id", deviceId, newIndex);
        resultModel.SetData("Name", "New Sensor", newIndex);
        resultModel.SetData("IsNew", true, newIndex);

        return resultModel;
    }

    function onModelChanged(){
        console.log("onModelChanged");
        if (productEditor.productCategory === "Hardware"){
            if (productEditor.productModel.ContainsKey("DeviceId")){
                let deviceId = productEditor.productModel.GetData("DeviceId");
                productEditor.rootItem.buttons.setButtonState("Save", deviceId !== "");
            }
        }
        else{
            productEditor.rootItem.buttons.setButtonState("Save", true);
        }
    }

    Column {
        id: contentColumn;

        anchors.top: parent.top;
        anchors.topMargin: 10;
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
                if (productCB.currentIndex >= 0){
                    let productId = productCB.model.GetData("Id", productCB.currentIndex);
                    let categoryId = productCB.model.GetData("CategoryId", productCB.currentIndex);

                    productEditor.productCategory = categoryId;
                    productEditor.productId = productId;

//                    if (contentLoader.status == Loader.Ready && categoryId === "Hardware"){
//                        contentLoader.item.devicesModel = productEditor.getDevicesModel();
//                    }

                    productEditor.pairsModel = productEditor.getPairsModel();

                    if (!productEditor.blockUpdatingModel){
                        productEditor.clearProduct();
                    }

                    contentLoader.sourceComponent = null;
                    if (categoryId === "Hardware"){
                        contentLoader.sourceComponent = hardwareProductComponent;
                    }
                    else if (categoryId === "Software"){
                        contentLoader.sourceComponent = softwareProductComponent;
                    }

                    contentLoader.item.productLicensesModel = 0;

                    let licensesModel = productEditor.getProductLicensesModel();
                    if (licensesModel){
                        contentLoader.item.productLicensesModel = licensesModel;
                    }

                    if (contentLoader.item.productLicensesModel){
                        contentLoader.item.productLicensesModel.Refresh()
                    }

                    contentLoader.item.updateGui();
                }
            }

            MouseArea {
                id: disabledMA;
                z: 100;
                anchors.fill: parent;
                visible: !productCB.changeable;
                onClicked: {
                    disabledComboBoxText.visible = true;
                }
            }
        }

        Text {
            id: selectProductText;

            text: qsTr("Please select a product");
            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            visible: productCB.currentIndex < 0;
        }

        Timer {
            id: timer;
            interval: 3000;
            onTriggered: {
                disabledComboBoxText.visible = false;
            }
        }

        Text {
            id: disabledComboBoxText;
            text: qsTr("Please make an unlink first");
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

        Text {
            id: titleDependency;

            text: qsTr("Pair link");
            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;

            visible: productCB.currentIndex >= 0;
        }

        Item {
            width: parent.width;
            height: 23;

            visible: productCB.currentIndex >= 0;

            ComboBox {
                id: pairCB;

                anchors.left: parent.left;
                anchors.right: clearPair.left;
                anchors.rightMargin: 10;

                height: parent.height;

                model: productEditor.pairsModel;

                radius: 3;

                onCurrentIndexChanged: {
                    productCB.changeable = pairCB.currentIndex < 0;

                    if (productEditor.blockUpdatingModel){
                        return;
                    }

                    if (pairCB.currentIndex >= 0){
                        let softwareId = productEditor.pairsModel.GetData("Id", pairCB.currentIndex);
                        productEditor.onUnlink();
                        productEditor.onLink(softwareId);
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
                    pairCB.currentIndex = -1;

                    productEditor.onUnlink();
                }
            }
        }
    }

    function onLink(pairId){
        productEditor.productModel.SetData("PairId", pairId);

        let orderProductId = productEditor.productModel.GetData("Id");
        for (let i = 0; i < productEditor.orderProductsModel.GetItemsCount(); i++){
            let id = productEditor.orderProductsModel.GetData("Id", i);
            if (id === pairId){
                productEditor.orderProductsModel.SetData("PairId", orderProductId, i);
                break;
            }
        }
    }

    function onUnlink(){
        let pairId = productEditor.productModel.GetData("PairId");
        productEditor.productModel.SetData("PairId", "");

        for (let i = 0; i < productEditor.orderProductsModel.GetItemsCount(); i++){
            let id = productEditor.orderProductsModel.GetData("Id", i);
            if (id === pairId){
                productEditor.orderProductsModel.SetData("PairId", "", i);
                break;
            }
        }
    }

    Loader {
        id: contentLoader;

        anchors.top: contentColumn.bottom;
        anchors.topMargin: 7;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;
//        anchors.bottomMargin: 10;

        width: parent.width;

        onLoaded: {
            if (productEditor.productCategory === "Hardware"){
                contentLoader.item.devicesModel = productEditor.getDevicesModel();
            }

            let productLicensesModel = productEditor.getProductLicensesModel();
            if (productLicensesModel){
                contentLoader.item.productLicensesModel = productLicensesModel;
            }

            contentLoader.item.productModel = productEditor.productModel;
            contentLoader.item.updateGui();
        }
    }

    function clearProduct(){
        if (productEditor.productCategory === "Hardware"){
            productEditor.productModel.SetData("CategoryId", "Hardware");

            productEditor.productModel.SetData("DeviceId", "");
            productEditor.productModel.SetData("ModelTypeId", "");
            productEditor.productModel.SetData("MacAddress", "");
            productEditor.productModel.SetData("SerialNumber", "");

            if (productEditor.productModel.ContainsKey("ActiveLicenses")){
                productEditor.productModel.RemoveData("ActiveLicenses");
            }
        }
        else if (productEditor.productCategory === "Software"){
            productEditor.productModel.SetData("CategoryId", "Software");

            productEditor.productModel.AddTreeModel("ActiveLicenses")

            if (productEditor.productModel.ContainsKey("DeviceId")){
                productEditor.productModel.RemoveData("DeviceId");
            }

            if (productEditor.productModel.ContainsKey("ModelTypeId")){
                productEditor.productModel.RemoveData("ModelTypeId");
            }

            if (productEditor.productModel.ContainsKey("IsNewDevice")){
                productEditor.productModel.RemoveData("IsNewDevice");
            }

            if (productEditor.productModel.ContainsKey("DeviceNotExists")){
                productEditor.productModel.RemoveData("DeviceNotExists");
            }

            if (productEditor.productModel.ContainsKey("MacAddress")){
                productEditor.productModel.RemoveData("MacAddress");
            }

            if (productEditor.productModel.ContainsKey("SerialNumber")){
                productEditor.productModel.RemoveData("SerialNumber");
            }
        }

        clearPairLink();
    }

    Component {
        id: hardwareProductComponent;
        HardwareProductEditor {}
    }

    Component {
        id: softwareProductComponent;
        SoftwareProductEditor {}
    }

    Component {
        id: treeItemModelComp;
        TreeItemModel {}
    }

    function getProductLicensesModel(){
        for (let i = 0; i < productEditor.licensesModel.GetItemsCount(); i++){
            let productId = productEditor.licensesModel.GetData("Id", i);
            if (productId === productEditor.productId){
                if (productEditor.licensesModel.ContainsKey("Licenses", i)){
                    return productEditor.licensesModel.GetData("Licenses", i);
                }
            }
        }

        return null;
    }

    function getPairsModel(){
        let resultModel = treeItemModelComp.createObject(null);
        for (let i = 0; i < productEditor.orderProductsModel.GetItemsCount(); i++){
            let currentCategoryId = productEditor.orderProductsModel.GetData("CategoryId", i);
            if (productEditor.productCategory !== currentCategoryId && currentCategoryId !== "Pair"){
                let  index = resultModel.InsertNewItem();

                let currentCategoryId = productEditor.orderProductsModel.GetData("Id", i);
                let productId = productEditor.orderProductsModel.GetData("ProductId", i);
                let productName = productEditor.getProductName(productId);
                let cardId = productEditor.orderProductsModel.GetData("Id", i);

                let pairId = productEditor.productModel.GetData("PairId")

                let number = i + 1;
                if (pairId !== "" && pairId === cardId){
                    number = productEditor.rootItem.activeProductIndex + 1;
                }

                resultModel.SetData("Id", cardId, index);
                resultModel.SetData("Name", "#" + number + " " + productName, index);
                resultModel.SetData("ProductId", productId, index);
            }
        }

        return resultModel;
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

    function getAllHardwareProducts(){

    }

    UuidGenerator {
        id: uuidGenerator;
    }

    function started(){
        productEditor.blockUpdatingModel = true;

        console.log("started", productEditor.productModel.toJSON());

        if (!productEditor.productModel.ContainsKey("Id")){
            let uuid = uuidGenerator.generateUUID();
            productEditor.productModel.SetData("Id", uuid);
        }

        if (productEditor.productModel.ContainsKey("CategoryId")){
            productEditor.productCategory = productEditor.productModel.GetData("CategoryId")
        }

        let allProductsModel = productEditor.productsModel;
        if (productEditor.rootItem.activeProductIndex >= 0){
            if (productEditor.productCategory === "Software"){
                allProductsModel = softwareProductsModel;
            }
            else if (productEditor.productCategory === "Hardware"){
                allProductsModel = hardwareProductsModel;
            }
        }

        productCB.model = allProductsModel;

        productCB.currentIndex = -1;
        if (productEditor.productModel.ContainsKey("ProductId")){
            productEditor.productId = productEditor.productModel.GetData("ProductId")
            for (let i = 0; i < productCB.model.GetItemsCount(); i++){
                let id = productCB.model.GetData("Id", i);
                if (id === productEditor.productId){
                    productCB.currentIndex = i;
                    break;
                }
            }
        }

//        productEditor.pairsModel = productEditor.getPairsModel();

        if (productEditor.productModel.ContainsKey("PairId")){
            let pairId = productEditor.productModel.GetData("PairId");
            if (pairId !== ""){
                for (let i = 0; i < productEditor.pairsModel.GetItemsCount(); i++){
                    let id = productEditor.pairsModel.GetData("Id", i);
                    if (id === pairId){
                        pairCB.currentIndex = i;
                        break;
                    }
                }
            }
        }

        productEditor.productModel.onDataChanged.connect(productEditor.onModelChanged);

        productEditor.blockUpdatingModel = false;
    }

    function updateModel(){}

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
}//Container


