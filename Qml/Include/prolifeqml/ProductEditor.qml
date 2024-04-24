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
    property TreeItemModel softwaresModel: TreeItemModel {}

    property bool blockUpdatingModel: false;
    property bool centered: true;

    property string orderId;
    property string orderUuid;

    property string productCategory: "";
    property string productId: "";
    property string uuid: "";

    property bool serialNumberEdit: true;

    // ProductEditorDialog reference
    property Item rootItem: null;

    Component.onDestruction: {
        productEditor.productModel.dataChanged.disconnect(productEditor.onModelChanged);
    }

    onProductIdChanged: {
        productEditor.productModel.SetData("ProductUuid", productEditor.productId);
    }

    onProductCategoryChanged: {
        productEditor.productModel.SetData("CategoryId", productEditor.productCategory);
    }

    function getSoftwareModel(){
        let excludeIds = []
        for (let i = 0; i < orderProductsModel.GetItemsCount(); i++){
            let categoryId = orderProductsModel.GetData("CategoryId", i);
            if (categoryId === "Software"){
                let id = orderProductsModel.GetData("Id", i);
                if (id !== ""){
                    excludeIds.push(id)
                }
            }
        }

        if (productEditor.productModel.ContainsKey("Id")){
            let id = productEditor.productModel.GetData("Id");

            let index = excludeIds.indexOf(id);
            if (index >= 0){
                excludeIds.splice(index, 1)
            }
        }

        let resultModel = treeItemModelComp.createObject(null);

        for (let i = 0; i < productEditor.softwaresModel.GetItemsCount(); i++){
            let id = productEditor.softwaresModel.GetData("Id", i);

            if (excludeIds.includes(id)){
                continue;
            }

            let orderUuid = productEditor.softwaresModel.GetData("OrderUuid", i);
            let productUuid = productEditor.softwaresModel.GetData("ProductUuid", i);
            let licenseUuid = productEditor.softwaresModel.GetData("LicenseUuid", i);

            if ((orderUuid === "" || orderUuid === productEditor.orderUuid) && productUuid === productEditor.productId){
                let index = resultModel.InsertNewItem();

                resultModel.CopyItemDataFromModel(index, productEditor.softwaresModel, i);
            }
        }

        return resultModel;
    }

    function getDevicesModel(){
        let excludeDeviceIds = []
        for (let i = 0; i < orderProductsModel.GetItemsCount(); i++){
            let categoryId = orderProductsModel.GetData("CategoryId", i);
            if (categoryId === "Hardware"){
                let deviceID = orderProductsModel.GetData("Id", i);
                if (deviceID !== ""){
                    excludeDeviceIds.push(deviceID)
                }
            }
        }

        let resultModel = treeItemModelComp.createObject(null);
        let selectedProductId = productEditor.productModel.GetData("ProductUuid");
        let selectedDeviceId = productEditor.productModel.GetData("Id");

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

            if (selectedProductId === deviceType && (orderId === "" || productEditor.orderUuid === orderId)){
                let index = resultModel.InsertNewItem();
                resultModel.CopyItemDataFromModel(index, productEditor.devicesModel, i);
            }
        }

        return resultModel;
    }

    function onModelChanged(){
        console.log("onModelChanged", productModel.ToJson());

        let ok = true;

        let licenseUuid = "";
        if (productModel.ContainsKey("LicenseUuid")){
            licenseUuid = productModel.GetData("LicenseUuid");
        }

        ok = ok && licenseUuid !== "";

        let productUuid = "";
        if (productModel.ContainsKey("ProductUuid")){
            productUuid = productModel.GetData("ProductUuid");
        }

        ok = ok && productUuid !== "";

        productEditor.rootItem.buttons.setButtonState(Enums.ok, ok);
    }

    function setHardware(){
        segmentedElementView.softwareProductButton.checkable = false;
        segmentedElementView.softwareProductButton.checked = false;

        segmentedElementView.hardwareProductButton.checkable = true;
        segmentedElementView.hardwareProductButton.checked = true;

        productCB.model = CachedProductCollection.hardwareProductsModel;

        productCB.currentIndex = -1;

        contentLoader.sourceComponent = hardwareProductComponent;
    }

    function setSoftware(){
        segmentedElementView.softwareProductButton.checkable = true;
        segmentedElementView.softwareProductButton.checked = true;

        segmentedElementView.hardwareProductButton.checkable = false;
        segmentedElementView.hardwareProductButton.checked = false;

        productCB.model = CachedProductCollection.softwareProductsModel;

        productCB.currentIndex = -1;

        contentLoader.sourceComponent = softwareProductComponent;
    }

    function updateProductModel(){
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

            contentLoader.item.productLicensesModel = 0;

            let licensesModel = productCB.model.GetData("Licenses", productCB.currentIndex);
            if (licensesModel){
                contentLoader.item.productLicensesModel = licensesModel;
            }

            if (contentLoader.item.productLicensesModel){
                contentLoader.item.productLicensesModel.Refresh()
            }

            if (productEditor.productCategory === "Hardware"){
                contentLoader.item.devicesModel = productEditor.getDevicesModel();
            }

            if (productEditor.productCategory === "Software"){
                contentLoader.item.softwaresModel = productEditor.getSoftwareModel();

            }

            let productLicensesModel = productEditor.getProductLicensesModel();
            if (productLicensesModel){
                contentLoader.item.productLicensesModel = productLicensesModel;
            }

            contentLoader.item.model = productEditor.productModel;

            contentLoader.item.doUpdateGui();
        }
    }

    Column {
        id: contentColumn;

        anchors.top: parent.top;
        anchors.topMargin: Style.size_mainMargin;
        anchors.left: parent.left;
        anchors.leftMargin: Style.size_mainMargin;
        anchors.right: parent.right;
        anchors.rightMargin: Style.size_mainMargin;

        spacing: Style.size_mainMargin;

        ElementView {
            id: segmentedElementView;

            width: parent.width;

            name: qsTr("Product Category");
            description: qsTr("Please select the product category you want to create");

            visible: productEditor.rootItem.activeProductIndex === -1;

            property Button softwareProductButton;
            property Button hardwareProductButton;

            controlComp: Component {
                SegmentedButton {
                    anchors.centerIn: parent;

                    height: 40;

                    isExclusive: true;

                    onSelectedIndexChanged: {
                        productEditor.clearProduct();

                        if (selectedIndex == 0){
                            productEditor.setSoftware();
                        }
                        else if (selectedIndex == 1){
                            productEditor.setHardware();
                        }
                    }

                    Button {
                        id: softwareProductButton;

                        anchors.verticalCenter: parent.verticalCenter;

                        checkable: true
                        checked: true

                        iconSource: "../../../../" + Style.getIconPath("Icons/Key", Icon.State.On, Icon.Mode.Normal);
                        text: qsTr("Software");

                        Component.onCompleted: {
                            segmentedElementView.softwareProductButton = softwareProductButton;
                        }
                    }

                    Button {
                        id: hardwareProductButton;

                        anchors.verticalCenter: parent.verticalCenter;

                        checkable: true
                        checked: false;

                        iconSource: "../../../../" + Style.getIconPath("Icons/Sensor", Icon.State.On, Icon.Mode.Normal);

                        text: qsTr("Hardware");

                        Component.onCompleted: {
                            segmentedElementView.hardwareProductButton = hardwareProductButton;
                        }
                    }
                }
            }
        }

        ComboBoxElementView {
            id: productCB;

            width: parent.width;

            name: qsTr("Product");
            nameId: "ProductName";

            onCurrentIndexChanged: {
                productCB.bottomComp = productCB.currentIndex < 0 ? productErrorComp : undefined;

                productEditor.updateProductModel();
            }
        }

        Component {
            id: productErrorComp;

            Text {
                id: selectProductText;

                text: qsTr("Please select a product");
                color: Style.errorTextColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }
        }
    }

    Loader {
        id: contentLoader;

        anchors.top: contentColumn.bottom;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.margins: Style.size_mainMargin;

        width: parent.width;

        visible: productCB.currentIndex >= 0;
    }

    function clearProduct(){
        productEditor.productModel.Clear();

        productEditor.productModel.SetData("ProductUuid", "");
        productEditor.productModel.SetData("ProductName", "");
        productEditor.productModel.SetData("LicenseUuid", "");
        productEditor.productModel.SetData("LicenseId", "");
        productEditor.productModel.SetData("LicenseName", "");
        productEditor.productModel.SetData("IsNew", false);

        if (productEditor.productCategory === "Hardware"){
            productEditor.productModel.SetData("MacAddress", "");
        }
        else if (productEditor.productCategory === "Software"){
            productEditor.productModel.SetData("SerialNumber", "");
            productEditor.productModel.SetData("Expiration", "");
        }
    }

    Component {
        id: hardwareProductComponent;
        HardwareProductEditor {
            model: productEditor.productModel;
            productIndex: productEditor.rootItem.activeProductIndex;
        }
    }

    Component {
        id: softwareProductComponent;
        SoftwareProductEditor {
            serialNumberEdit: productEditor.serialNumberEdit;
            productIndex: productEditor.rootItem.activeProductIndex;
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

    function started(){
        productEditor.blockUpdatingModel = true;

        console.log("started", productEditor.productModel.ToJson());

        let uuid;
        if (productEditor.productModel.ContainsKey("Id")){
            uuid = productEditor.productModel.GetData("Id");
        }
        else{
            uuid = UuidGenerator.generateUUID();
        }

        productEditor.uuid = uuid;

        if (productEditor.productModel.ContainsKey("CategoryId")){
            productEditor.productCategory = productEditor.productModel.GetData("CategoryId")
        }

        // By default
        productEditor.setSoftware();

        if (productEditor.rootItem.activeProductIndex >= 0){
            if (productEditor.productCategory === "Software"){
                productEditor.setSoftware();
            }
            else if (productEditor.productCategory === "Hardware"){
                productEditor.setHardware();
            }
        }

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

        productEditor.productModel.dataChanged.connect(productEditor.onModelChanged);

        productEditor.blockUpdatingModel = false;
    }
}//Container


