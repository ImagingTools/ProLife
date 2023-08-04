import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Item {
    id: productEditor;

    property int margin: 10;

    property int contentHeight: bodyColumn.height;

    property alias collectionModel: softwareProductCollection.collectionModel;
    property alias table: softwareProductsTable;

    property TreeItemModel bindingModel: TreeItemModel {}

    property string productId: ""

    signal checkedItemsChanged();
    signal modelChanged();

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", productEditor.onLocalizationChanged);

        softwareProductCollection.updateModel();

        bindingModel.dataChanged.connect(productEditor.modelChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", productEditor.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        productEditor.updateHeaders();
    }

    property bool bindingModelReady: false;
    property bool allCompleted: bindingModelReady && softwareProductCollection.modelReady;
    onAllCompletedChanged: {
        if (allCompleted){
            productEditor.productId = productEditor.getCurrentSoftwareProductId();

            productEditor.updateGui();
        }
    }

    onBindingModelChanged: {
        bindingModel.dataChanged.connect(productEditor.modelChanged);

        productEditor.bindingModelReady = true;
    }

    property bool blockUpdatingModel: false;
    onBlockUpdatingModelChanged: {
        loading.visible = blockUpdatingModel;
    }

    function updateGui(){
        console.log("updateGui", productEditor.productId);

        blockUpdatingModel = true;

        productsCB.currentIndex = -1;
        for (let i = 0; i < productsModel.GetItemsCount(); i++){
            let id = productsModel.GetData("Id", i);
            if (id === productEditor.productId){
                productsCB.currentIndex = i;
            }
        }

        if (productsCB.currentIndex >= 0){
            productEditor.createElementsModel(productId);
        }

        if (productEditor.bindingModel.ContainsKey("SoftwareIds")){
            let software = productEditor.bindingModel.GetData("SoftwareIds");
            let softwareIds = software.split(';')

            softwareProductsTable.uncheckAll();

            if (softwareProductsTable.elements){
                for (let i = 0; i < softwareProductsTable.elements.GetItemsCount(); i++){
                    let id = softwareProductsTable.elements.GetData("Id", i);
                    if (softwareIds.includes(id)){
                        softwareProductsTable.checkItem(i);
                    }
                }
            }
        }

        blockUpdatingModel = false;
    }

    function updateModel(){
        if (productEditor.blockUpdatingModel){
            return;
        }

        let selectedProductIds = []
        let indexes = softwareProductsTable.getCheckedItems();
        for (let index of indexes){
            let id = softwareProductsTable.elements.GetData("Id", index);
            selectedProductIds.push(id)
        }

        let products = selectedProductIds.join(';');
        productEditor.bindingModel.SetData("SoftwareIds", products)
    }

    TreeItemModel {
        id: productsModel;
    }

    function getCurrentSoftwareProductId(){
        let retVal = ""
        if (productEditor.bindingModel.ContainsKey("SoftwareIds")){
            let software = productEditor.bindingModel.GetData("SoftwareIds");
            let softwareIds = software.split(';')

            if (softwareIds.length > 0){
                for (let i = 0; i < softwareProductCollection.collectionModel.GetItemsCount(); i++){
                    let id = softwareProductCollection.collectionModel.GetData("Id", i);
                    if (softwareIds.includes(id)){
                        let productId = softwareProductCollection.collectionModel.GetData("ProductId", i);
                        return productId;
                    }
                }
            }
        }

        return retVal;
    }

    function createProductsModel(){
        productsModel.Clear();

        let productIds = []

        for (let i = 0; i < softwareProductCollection.collectionModel.GetItemsCount(); i++){
            let productId = softwareProductCollection.collectionModel.GetData("ProductId", i);

            if (!productIds.includes(productId)){
                productIds.push(productId);
            }
        }

        for (let productId of productIds){
            let index = productsModel.InsertNewItem();

            productsModel.SetData("Id", productId, index);
        }

        productsCB.model = productsModel;
    }

    TreeItemModel {
        id: elementsModel;
    }

    function createElementsModel(productId){
        elementsModel.Clear();

        for (let i = 0; i < softwareProductCollection.collectionModel.GetItemsCount(); i++){
            let currentProductId = softwareProductCollection.collectionModel.GetData("ProductId", i);
             if (currentProductId === productId){
                 let index = elementsModel.InsertNewItem();

                 elementsModel.CopyItemDataFromModel(index, softwareProductCollection.collectionModel, i)
             }
        }

        softwareProductsTable.elements = elementsModel;
    }

    CollectionDataProvider {
        id: softwareProductCollection;

        commandId: "SoftwareProducts";

        fields: ["Id", "ProductId", "OrderId", "SerialNumber", "Customer"]

        property bool modelReady: false;

        onModelUpdated: {
            productEditor.createProductsModel();
            softwareProductCollection.modelReady = true;
        }

        onStateModelChanged: {
            if (softwareProductCollection.stateModel === "Loading"){
                loading.start();
            }
            else{
                loading.stop();
            }
        }
    }

    Column {
        id: bodyColumn;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        anchors.left: parent.left;
        anchors.rightMargin: productEditor.margin;
        anchors.leftMargin: productEditor.margin;

        spacing: 10;

        BaseText {
            text: qsTr("Product");

            font.family: Style.fontFamilyBold;
        }

        ComboBox {
            id: productsCB;

            width: parent.width;
            height: 25;

            radius: 3;

            nameId: "Id";

            onCurrentIndexChanged: {
                if (productEditor.blockUpdatingModel){
                    return;
                }

                productEditor.bindingModel.SetData("SoftwareIds", "");

                productEditor.productId = productsCB.model.GetData("Id", productsCB.currentIndex);

                productEditor.updateGui();
            }
        }

        Text {
            id: selectProductText;

            text: qsTr("Please select a product");
            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            visible: productsCB.currentIndex < 0;
        }

        AuxTable {
            id: softwareProductsTable;

            width: parent.width;
            height: 500;

            radius: 0;

            isMultiSelect: false;

            checkable: true;

            cacheBuffer: 10000;

            onCheckedItemsChanged: {
                if (productEditor.blockUpdatingModel){
                    return;
                }

                productEditor.updateModel();

                productEditor.checkedItemsChanged();
            }
        }

        BaseText {
            id: message;
            color: Style.errorTextColor;

            visible: false;
        }
    }

    Loading {
        id: loading;

        anchors.fill: parent;

        visible: false;
    }

    TreeItemModel {
        id: headersModel;

        Component.onCompleted: {
            productEditor.updateHeaders();
        }
    }

    function updateHeaders(){
        headersModel.Clear();

        let index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "ProductId", index);
        headersModel.SetData("Name", qsTr("Product"), index);

        index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "OrderId", index);
        headersModel.SetData("Name", qsTr("Order-ID"), index);

        index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "SerialNumber", index);
        headersModel.SetData("Name", qsTr("Serial Number"), index);

        index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "Customer", index);
        headersModel.SetData("Name", qsTr("Customer"), index);

        softwareProductsTable.headers = headersModel;
    }
}//Container


