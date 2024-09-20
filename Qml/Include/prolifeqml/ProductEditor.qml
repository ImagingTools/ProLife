import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import prolifeOrdersSdl 1.0
import prolifeLicensesSdl 1.0
import prolifeSensorsSdl 1.0

Item {
    id: productEditor;

    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}
    property ListModel orderProductsModel: ListModel {}

    property TreeItemModel devicesModel: TreeItemModel {}
    property TreeItemModel softwaresModel: TreeItemModel {}

    property bool blockUpdatingModel: false;

    property string orderId;
    property string orderUuid;

    property OrderedProduct productItem;
    property int index: -1;

    property string softwareCategoryId: "Software";
    property string hardwareCategoryId: "Hardware";

    function getSoftwareModel(){
        let excludeIds = []

        for (let i = 0; i < orderProductsModel.count; i++){
            let categoryId = orderProductsModel.get(i).item.m_categoryId;
            if (categoryId === productEditor.softwareCategoryId){
                let id = orderProductsModel.get(i).item.m_id;
                if (id !== ""){
                    excludeIds.push(id)
                }
            }
        }

        let index = excludeIds.indexOf(productItem.m_id);
        if (index >= 0){
            excludeIds.splice(index, 1)
        }

        let resultModel = treeItemModelComp.createObject(null);

        for (let i = 0; i < productEditor.softwaresModel.getItemsCount(); i++){
            let id = productEditor.softwaresModel.getData(SoftwareProductItemTypeMetaInfo.s_id, i);

            if (!id || id === ""){
                continue;
            }

            if (excludeIds.includes(id)){
                continue;
            }

            let serialNumber = productEditor.softwaresModel.getData(SoftwareProductItemTypeMetaInfo.s_serialNumber, i);

            let orderUuid = productEditor.softwaresModel.getData(SoftwareProductItemTypeMetaInfo.s_orderUuid, i);

            let productUuid = productEditor.softwaresModel.getData(SoftwareProductItemTypeMetaInfo.s_productUuid, i);
            let licenseUuid = productEditor.softwaresModel.getData(SoftwareProductItemTypeMetaInfo.s_licenseUuid, i);

            if ((orderUuid === "" || orderUuid === productEditor.orderUuid) && productUuid === productItem.m_productUuid){
                let index = resultModel.insertNewItem();

                resultModel.copyItemDataFromModel(index, productEditor.softwaresModel, i);
            }
        }

        return resultModel;
    }

    function getDevicesModel(){
        let excludeDeviceIds = []
        for (let i = 0; i < orderProductsModel.count; i++){
            let categoryId = orderProductsModel.get(i).item.m_categoryId;
            if (categoryId === "Hardware"){
                let deviceID = orderProductsModel.get(i).item.m_id;
                if (deviceID !== ""){
                    excludeDeviceIds.push(deviceID)
                }
            }
        }

        console.log("excludeDeviceIds1", excludeDeviceIds);

        let resultModel = treeItemModelComp.createObject(null);
        let selectedProductId = productItem.m_productUuid;
        let selectedDeviceId = productItem.m_id;

        let index = excludeDeviceIds.indexOf(selectedDeviceId);
        if (index >= 0){
            excludeDeviceIds.splice(index, 1)
        }

        console.log("excludeDeviceIds2", excludeDeviceIds);

        for (let i = 0; i < productEditor.devicesModel.getItemsCount(); i++){
            let status = productEditor.devicesModel.getData(DeviceItemTypeMetaInfo.s_status, i);
            let orderId = productEditor.devicesModel.getData(DeviceItemTypeMetaInfo.s_orderUuid, i);
            let deviceId = productEditor.devicesModel.getData(DeviceItemTypeMetaInfo.s_id, i);
            let deviceType = productEditor.devicesModel.getData(DeviceItemTypeMetaInfo.s_productUuid, i);
            let macAddress = productEditor.devicesModel.getData(DeviceItemTypeMetaInfo.s_macAddress, i);

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
            if (!productEditor.blockUpdatingModel){
                productEditor.clearProduct();
            }

            productItem.m_productUuid = productCB.model.getData(OrderedProductTypeMetaInfo.s_id, productCB.currentIndex);
            productItem.m_categoryId = productCB.model.getData(OrderedProductTypeMetaInfo.s_categoryId, productCB.currentIndex);
            productItem.m_productName = productCB.model.getData(OrderedProductTypeMetaInfo.s_productName, productCB.currentIndex);

            contentLoader.item.productLicensesModel = 0;

            let licensesModel = productCB.model.getData("Licenses", productCB.currentIndex);
            if (licensesModel){
                contentLoader.item.productLicensesModel = licensesModel;
            }

            if (contentLoader.item.productLicensesModel){
                contentLoader.item.productLicensesModel.refresh()
            }

            if (productItem.m_categoryId === productEditor.hardwareCategoryId){
                contentLoader.item.devicesModel = productEditor.getDevicesModel();
            }

            if (productItem.m_categoryId === productEditor.softwareCategoryId){
                contentLoader.item.softwaresModel = productEditor.getSoftwareModel();
            }

            let productLicensesModel = productEditor.getProductLicensesModel();
            if (productLicensesModel){
                contentLoader.item.productLicensesModel = productLicensesModel;
            }

            contentLoader.item.model = productItem;

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

            visible: productEditor.index === -1;

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
        if (productItem){
            // productItem.m_id = ''
            productItem.m_productUuid = ''
            productItem.m_licenseUuid = ''
            productItem.m_categoryId = ''
            productItem.m_licenseId = ''
            productItem.m_productName = ''
            productItem.m_licenseName = ''
            productItem.m_serialNumber = ''
            productItem.m_inUse = false
            productItem.m_isNew = false
            productItem.m_expiration = ''
            productItem.m_macAddress = ''
        }
    }

    Component {
        id: hardwareProductComponent;
        HardwareProductEditor {
            productIndex: productEditor.index;
        }
    }

    Component {
        id: softwareProductComponent;
        SoftwareProductEditor {
            productIndex: productEditor.index;
        }
    }

    Component {
        id: treeItemModelComp;
        TreeItemModel {}
    }

    function getProductLicensesModel(){
        for (let i = 0; i < productEditor.licensesModel.getItemsCount(); i++){
            let productId = productEditor.licensesModel.getData("Id", i);
            if (productId === productItem.m_productUuid){
                if (productEditor.licensesModel.containsKey("Licenses", i)){
                    return productEditor.licensesModel.getData("Licenses", i);
                }
            }
        }

        return null;
    }

    function started(){
        console.log("started");

        if (!productItem){
            return;
        }

        productEditor.blockUpdatingModel = true;

        if (productItem.m_categoryId === productEditor.softwareCategoryId){
            productEditor.setSoftware();
        }
        else if (productItem.m_categoryId  === productEditor.hardwareCategoryId){
            productEditor.setHardware();
        }
        else{
            console.error("Unknown product type:", productItem.m_categoryId);
            return;
        }

        for (let i = 0; i < productCB.model.getItemsCount(); i++){
            let id = productCB.model.getData(OrderedProductTypeMetaInfo.s_id, i);
            if (id === productItem.m_productUuid){
                productCB.currentIndex = i;
                break;
            }
        }

        if (productCB.currentIndex == -1){
            console.error("Unable to edit product. Error: This product not found!", productItem.m_categoryId);
            return;
        }

        productEditor.blockUpdatingModel = false;
    }
}//Container


