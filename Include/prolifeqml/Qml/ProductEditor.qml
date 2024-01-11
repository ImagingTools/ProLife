import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0

Item {
    id: productEditor;

    width: 700;
    height: 800;

    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}
    property TreeItemModel orderProductsModel: TreeItemModel {}

    property TreeItemModel productModel: TreeItemModel {}

    property TreeItemModel devicesModel: TreeItemModel {}

    property bool blockUpdatingModel: false;
    property bool centered: true;

    property string orderId;
    property string orderUuid;

    //    property var excludeDeviceIds: [];

    property string productCategory: "";
    property string productId: "";
    property string uuid: "";

    property bool serialNumberEdit: true;

    // ProductEditorDialog reference
    property Item rootItem: null;

    Component.onDestruction: {
        productEditor.productModel.onDataChanged.disconnect(productEditor.onModelChanged);
    }

    onProductIdChanged: {
        productEditor.productModel.SetData("ProductUuid", productEditor.productId);
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
        console.log("getDevicesModel");

        let excludeDeviceIds = []
        for (let i = 0; i < orderProductsModel.GetItemsCount(); i++){
            let categoryId = orderProductsModel.GetData("CategoryId", i);
            if (categoryId === "Hardware"){
                let deviceID = orderProductsModel.GetData("DeviceId", i);
                if (deviceID !== ""){
                    excludeDeviceIds.push(deviceID)
                }
            }
        }

        let resultModel = treeItemModelComp.createObject(null);
        let selectedProductId = productEditor.productModel.GetData("ProductUuid");
        let selectedDeviceId = productEditor.productModel.GetData("DeviceId");

        let index = excludeDeviceIds.indexOf(selectedDeviceId);
        if (index >= 0){
            excludeDeviceIds.splice(index, 1)
        }

        for (let i = 0; i < productEditor.devicesModel.GetItemsCount(); i++){
            let status = productEditor.devicesModel.GetData("Status", i);
            let orderId = productEditor.devicesModel.GetData("OrderUuid", i);
            let deviceId = productEditor.devicesModel.GetData("Id", i);
            let deviceType = productEditor.devicesModel.GetData("ProductUuid", i);

            if (excludeDeviceIds.includes(deviceId)){
                continue;
            }

            if (selectedProductId === deviceType && (orderId === "" || productEditor.orderUuid === orderId) && (status === "Finished" || status === "None") ||
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
        resultModel.SetData("Name", qsTr("New Sensor"), newIndex);
        resultModel.SetData("DeviceId", deviceId, newIndex);
        resultModel.SetData("MacAddress", "", newIndex);
        resultModel.SetData("SerialNumber", "", newIndex);
        resultModel.SetData("LicenseUuid", "", newIndex);
        resultModel.SetData("LicenseId", "", newIndex);
        resultModel.SetData("LicenseName", "", newIndex);
//        resultModel.SetData("IsNew", true, newIndex);

        return resultModel;
    }

    function onModelChanged(){
        if (productEditor.blockUpdatingModel){
            return;
        }

        productEditor.blockUpdatingModel = true;

        let ok = false;

        if (productModel.ContainsKey("LicenseUuid")){
            let licenseUuid = productModel.GetData("LicenseUuid");
            if (licenseUuid !== ""){
                ok = true;
            }
        }

        if (productEditor.productCategory === "Hardware"){
            if (ok){
                ok = false;
                if (productModel.ContainsKey("DeviceId")){
                    let deviceId = productModel.GetData("DeviceId");
                    if (deviceId !== ""){
                        ok = true;
                    }
                }
            }
        }
        else{
            if (productModel.ContainsKey("LicenseUuid")){
                let licenseUuid = productModel.GetData("LicenseUuid");
                if (licenseUuid !== ""){
                    for (let i = 0; i < licensesModel.GetItemsCount(); i++){
                        let uuid = licensesModel.GetData("Id", i);
                        if (uuid === licenseUuid){
                            let licenseId = licensesModel.GetData("LicenseId", i)
                            let licenseName = licensesModel.GetData("LicenseName", i)

                            productModel.SetData("LicenseId", licenseId);
                            productModel.SetData("LicenseName", licenseName);

                            break;
                        }
                    }
                }
            }
        }

        productEditor.rootItem.buttons.setButtonState(Enums.ButtonType.Ok, ok);

        productEditor.blockUpdatingModel = false;
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

            nameId: "ProductName";

            onCurrentIndexChanged: {
                if (productCB.currentIndex >= 0){
                    let productId = productCB.model.GetData("Id", productCB.currentIndex);
                    let categoryId = productCB.model.GetData("CategoryId", productCB.currentIndex);

                    productEditor.productCategory = categoryId;
                    productEditor.productId = productId;

                    if (!productEditor.blockUpdatingModel){
                        productEditor.clearProduct();
                    }

                    productEditor.productModel.SetData("Id", productEditor.uuid);
                    productEditor.productModel.SetData("CategoryId", categoryId);
                    productEditor.productModel.SetData("ProductUuid", productEditor.productId);

                    let productName = productCB.model.GetData("ProductName", productCB.currentIndex);
                    productEditor.productModel.SetData("ProductName", productName);

                    contentLoader.sourceComponent = null;
                    if (categoryId === "Hardware"){
                        contentLoader.sourceComponent = hardwareProductComponent;
                    }
                    else if (categoryId === "Software"){
                        contentLoader.sourceComponent = softwareProductComponent;
                    }

                    contentLoader.item.productLicensesModel = 0;

                    let licensesModel = productCB.model.GetData("Licenses", productCB.currentIndex);
                    if (licensesModel){
                        contentLoader.item.productLicensesModel = licensesModel;
                    }

                    if (contentLoader.item.productLicensesModel){
                        contentLoader.item.productLicensesModel.Refresh()
                    }

                    contentLoader.item.updateGui();
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
        productEditor.productModel.Clear();
        if (productEditor.productCategory === "Hardware"){
            productEditor.productModel.SetData("DeviceId", "");
            productEditor.productModel.SetData("LicenseUuid", "");
            productEditor.productModel.SetData("LicenseId", "");
            productEditor.productModel.SetData("LicenseName", "");
            productEditor.productModel.SetData("MacAddress", "");
            productEditor.productModel.SetData("SerialNumber", "");

        }
        else if (productEditor.productCategory === "Software"){
            productEditor.productModel.SetData("SerialNumber", "");
            productEditor.productModel.SetData("LicenseUuid", "");
            productEditor.productModel.SetData("Expiration", "");
        }
    }

    Component {
        id: hardwareProductComponent;
        HardwareProductEditor {}
    }

    Component {
        id: softwareProductComponent;
        SoftwareProductEditor {
            serialNumberEdit: productEditor.serialNumberEdit;
        }
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


    function getProductName(productId){
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

    function getLicenseName(licenseUuid){
        let retVal = "";
        for (let i = 0; i < licensesModel.GetItemsCount(); i++){
            let id = licensesModel.GetData("Id", i);
            if (id === licenseUuid){
                retVal = licensesModel.GetData("LicenseName", i);
                break;
            }
        }
        return retVal;
    }

    UuidGenerator {
        id: uuidGenerator;
    }

    function started(){
        productEditor.blockUpdatingModel = true;

        console.log("started", productEditor.productModel.toJSON());

        let uuid;
        if (productEditor.productModel.ContainsKey("Id")){
            uuid = productEditor.productModel.GetData("Id");
        }
        else{
            uuid = uuidGenerator.generateUUID();
        }

        productEditor.uuid = uuid;

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
        if (productEditor.productModel.ContainsKey("ProductUuid")){
            productEditor.productId = productEditor.productModel.GetData("ProductUuid")
            for (let i = 0; i < productCB.model.GetItemsCount(); i++){
                let id = productCB.model.GetData("Id", i);
                if (id === productEditor.productId){
                    productCB.currentIndex = i;
                    break;
                }
            }
        }

        productEditor.productModel.onDataChanged.connect(productEditor.onModelChanged);

        productEditor.blockUpdatingModel = false;
    }

    function updateModel(){}
}//Container


