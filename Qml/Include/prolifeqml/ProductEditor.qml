import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0

Item {
    id: productEditor;

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
        productEditor.productModel.setData("ProductUuid", productEditor.productId);
    }

    onProductCategoryChanged: {
        productEditor.productModel.setData("CategoryId", productEditor.productCategory);
    }

    function getSoftwareModel(){
        let excludeIds = []
        for (let i = 0; i < orderProductsModel.getItemsCount(); i++){
            let categoryId = orderProductsModel.getData("CategoryId", i);
            if (categoryId === "Software"){
                let id = orderProductsModel.getData("Id", i);
                if (id !== ""){
                    excludeIds.push(id)
                }
            }
        }

        if (productEditor.productModel.containsKey("Id")){
            let id = productEditor.productModel.getData("Id");

            let index = excludeIds.indexOf(id);
            if (index >= 0){
                excludeIds.splice(index, 1)
            }
        }

        let resultModel = treeItemModelComp.createObject(null);

        for (let i = 0; i < productEditor.softwaresModel.getItemsCount(); i++){
            let id = productEditor.softwaresModel.getData("Id", i);

            if (!id || id === ""){
                continue;
            }

            if (excludeIds.includes(id)){
                continue;
            }

            let serialNumber = productEditor.softwaresModel.getData("SerialNumber", i);

            let orderUuid = productEditor.softwaresModel.getData("OrderUuid", i);

            let productUuid = productEditor.softwaresModel.getData("ProductUuid", i);
            let licenseUuid = productEditor.softwaresModel.getData("LicenseUuid", i);

            if ((orderUuid === "" || orderUuid === productEditor.orderUuid) && productUuid === productEditor.productId){
                let index = resultModel.insertNewItem();

                resultModel.copyItemDataFromModel(index, productEditor.softwaresModel, i);
            }
        }

        return resultModel;
    }

    function getDevicesModel(){
        let excludeDeviceIds = []
        for (let i = 0; i < orderProductsModel.getItemsCount(); i++){
            let categoryId = orderProductsModel.getData("CategoryId", i);
            if (categoryId === "Hardware"){
                let deviceID = orderProductsModel.getData("Id", i);
                if (deviceID !== ""){
                    excludeDeviceIds.push(deviceID)
                }
            }
        }

        let resultModel = treeItemModelComp.createObject(null);
        let selectedProductId = productEditor.productModel.getData("ProductUuid");
        let selectedDeviceId = productEditor.productModel.getData("Id");

        let index = excludeDeviceIds.indexOf(selectedDeviceId);
        if (index >= 0){
            excludeDeviceIds.splice(index, 1)
        }

        for (let i = 0; i < productEditor.devicesModel.getItemsCount(); i++){
            let status = productEditor.devicesModel.getData("Status", i);
            let orderId = productEditor.devicesModel.getData("OrderUuid", i);
            let deviceId = productEditor.devicesModel.getData("Id", i);
            let deviceType = productEditor.devicesModel.getData("ProductUuid", i);
            let macAddress = productEditor.devicesModel.getData("MacAddress", i);

            if (!deviceId || deviceId === ""){
                continue;
            }

            if (excludeDeviceIds.includes(deviceId)){
                continue;
            }

            if (selectedProductId === deviceType && (orderId === "" || productEditor.orderUuid === orderId)){
                let index = resultModel.insertNewItem();
                resultModel.copyItemDataFromModel(index, productEditor.devicesModel, i);

                let sMacAddress = "s" + macAddress.split(':').join('');
                resultModel.setData("SMacAddress1", sMacAddress, index);

                let sMacAddress2 = "s:" + macAddress.split(':').join('');
                resultModel.setData("SMacAddress2", sMacAddress2, index);

                let sMacAddress3 = "s:" + macAddress;
                resultModel.setData("SMacAddress3", sMacAddress3, index);

                let sMacAddress4 = "s" + macAddress;
                resultModel.setData("SMacAddress4", sMacAddress4, index);
            }
        }

        return resultModel;
    }

    function onModelChanged(){
        let ok = true;

        let licenseUuid = "";
        if (productModel.containsKey("LicenseUuid")){
            licenseUuid = productModel.getData("LicenseUuid");
        }

        ok = ok && licenseUuid !== "";

        let productUuid = "";
        if (productModel.containsKey("ProductUuid")){
            productUuid = productModel.getData("ProductUuid");
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
            let productId = productCB.model.getData("Id", productCB.currentIndex);
            let categoryId = productCB.model.getData("CategoryId", productCB.currentIndex);

            productEditor.productCategory = categoryId;
            productEditor.productId = productId;

            if (!productEditor.blockUpdatingModel){
                productEditor.clearProduct();
            }

            productEditor.productModel.setData("Id", productEditor.uuid);
            productEditor.productModel.setData("CategoryId", categoryId);
            productEditor.productModel.setData("ProductUuid", productEditor.productId);

            let productName = productCB.model.getData("ProductName", productCB.currentIndex);
            productEditor.productModel.setData("ProductName", productName);

            contentLoader.item.productLicensesModel = 0;

            let licensesModel = productCB.model.getData("Licenses", productCB.currentIndex);
            if (licensesModel){
                contentLoader.item.productLicensesModel = licensesModel;
            }

            if (contentLoader.item.productLicensesModel){
                contentLoader.item.productLicensesModel.refresh()
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
            controlWidth: 500;

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
        productEditor.productModel.clear();

        productEditor.productModel.setData("ProductUuid", "");
        productEditor.productModel.setData("ProductName", "");
        productEditor.productModel.setData("LicenseUuid", "");
        productEditor.productModel.setData("LicenseId", "");
        productEditor.productModel.setData("LicenseName", "");
        productEditor.productModel.setData("IsNew", false);

        if (productEditor.productCategory === "Hardware"){
            productEditor.productModel.setData("MacAddress", "");
        }
        else if (productEditor.productCategory === "Software"){
            productEditor.productModel.setData("SerialNumber", "");
            productEditor.productModel.setData("Expiration", "");
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
        for (let i = 0; i < productEditor.licensesModel.getItemsCount(); i++){
            let productId = productEditor.licensesModel.getData("Id", i);
            if (productId === productEditor.productId){
                if (productEditor.licensesModel.containsKey("Licenses", i)){
                    return productEditor.licensesModel.getData("Licenses", i);
                }
            }
        }

        return null;
    }

    function started(){
        productEditor.blockUpdatingModel = true;

        let uuid;
        if (productEditor.productModel.containsKey("Id")){
            uuid = productEditor.productModel.getData("Id");
        }
        else{
            uuid = UuidGenerator.generateUUID();
        }

        productEditor.uuid = uuid;

        if (productEditor.productModel.containsKey("CategoryId")){
            productEditor.productCategory = productEditor.productModel.getData("CategoryId")
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

        if (productEditor.productModel.containsKey("ProductUuid")){
            productEditor.productId = productEditor.productModel.getData("ProductUuid")

            for (let i = 0; i < productCB.model.getItemsCount(); i++){
                let id = productCB.model.getData("Id", i);
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


