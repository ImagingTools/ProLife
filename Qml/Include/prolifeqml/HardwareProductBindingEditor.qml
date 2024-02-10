import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtguigql 1.0

Item {
    id: productEditor;

    property int margin: 10;

    property int contentHeight: availableLicensesColumn.height + titleLable.height;

    property TreeItemModel bindingModel: TreeItemModel {}

    property string productId: ""

    property string hardwareId: "";

    property Item rootItem: null
    signal modelChanged();

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", productEditor.onLocalizationChanged);
        bindingModel.dataChanged.connect(productEditor.modelChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", productEditor.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
//        productEditor.updateHeaders();
    }

    property bool bindingModelReady: false;
    property string startSoftwareIds: "";
    property string startProductId: "";

    onBindingModelChanged: {
        bindingModel.dataChanged.connect(productEditor.modelChanged);

        productEditor.bindingModelReady = true;

        if (productEditor.bindingModel.ContainsKey("SoftwareIds")){
            let softwareIds = productEditor.bindingModel.GetData("SoftwareIds");

            productEditor.startSoftwareIds = softwareIds;
        }
    }

    property bool blockUpdatingModel: false;
    onBlockUpdatingModelChanged: {
        loading.visible = blockUpdatingModel;
    }

    onHardwareIdChanged: {
        let filterModel = bindingProductsCollection.collectionFilter.filterModel;
        let objectFilter =  filterModel.AddTreeModel("ObjectFilter")
        let bindingFilterModel = objectFilter.AddTreeModel("BindingFilter");
        bindingFilterModel.SetData("HardwareUuid", productEditor.hardwareId);

        bindingProductsCollection.doUpdateGui();
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

        if (productsCB.currentIndex < 0){
            productEditor.setError(0);
        }

        if (productEditor.bindingModel.ContainsKey("SoftwareIds")){
            let software = productEditor.bindingModel.GetData("SoftwareIds");
            let softwareIds = software.split(';')
        }

        blockUpdatingModel = false;
    }

    function updateModel(){
        if (productEditor.blockUpdatingModel){
            return;
        }

        let selectedProductIds = []
        let indexes = softwareProductCollection.table.getCheckedItems();
        for (let index of indexes){
            let id = softwareProductCollection.table.elements.GetData("Id", index);
            selectedProductIds.push(id)
        }

        let products = selectedProductIds.join(';');
        productEditor.bindingModel.SetData("SoftwareIds", products)
    }

    function checkLicenseId(licenseId){
        let bindingElements = bindingProductsCollection.table.elements;
        if (bindingElements){
            for (let i = 0; i < bindingElements.GetItemsCount(); i++){
                let id = bindingElements.GetData("LicenseId", i)
                if (id == licenseId){
                    bindButton.enabled = false
                    productEditor.setError(1);

                    return false;
                }
            }
        }

        productEditor.setError(-1);

        return true;
    }

    TreeItemModel {
        id: productsModel;
    }

    BaseText {
        id: titleLable
        anchors.top: parent.top
        anchors.topMargin: Style.margin
        anchors.horizontalCenter: availableLicensesColumn.horizontalCenter
        text: qsTr("Available licenses");
        font.family: Style.fontFamilyBold;
    }

    function setError(errorType){
        console.log("setError", errorType);
        if (errorType === 0){
            console.log("errorType === 0");

            errorText.text = qsTr("Please select a product");
        }
        else if (errorType === 1){
            errorText.text = qsTr("A license with this ID has already been added");
        }
        else{
            console.log("else");
            errorText.text = "";
        }
    }

    Column {
        id: availableLicensesColumn;

        anchors.top: titleLable.bottom
        anchors.topMargin: Style.margin
        anchors.right: parent.horizontalCenter;
        anchors.rightMargin: 20;
        anchors.left: parent.left;
        anchors.leftMargin: productEditor.margin;

        spacing: 10;

        Item {
            width: parent.width;
            height: 25;

            BaseText {
                id: productLable
                anchors.verticalCenter: parent.verticalCenter

                text: qsTr("Product");
                font.family: Style.fontFamilyBold;
            }

            ComboBox {
                id: productsCB;

                anchors.left: productLable.right
                anchors.leftMargin: Style.margin
                anchors.right: lockImage.left;
                anchors.rightMargin: 10;

                height: 30;

                radius: 3;
                nameId: "ProductName";

                enabled: bindingProductsCollection.table.elementsList.count == 0;

                Component.onCompleted: {
                    if (productsCB.currentIndex < 0){
                        productEditor.setError(0);
                    }
                }

                onCurrentIndexChanged: {
                    if (productEditor.blockUpdatingModel){
                        return;
                    }

                    if (productsCB.currentIndex < 0){
                        productEditor.setError(0);
                    }
                    else{
                        productEditor.setError(-1);
                    }

                    if (productsCB.currentIndex > -1){
                        productEditor.productId = productsCB.model.GetData("Id", productsCB.currentIndex);
                    }

                    softwareProductCollection.updateData();
                }

                CollectionDataProvider {
                    id: productsList;
                    fields: ["Id", "ProductId", "ProductName", "CategoryId"];
                    commandId: "Products";

                    onCollectionModelChanged: {
                        if (productsList.collectionModel != null){
                            productsCB.model = productsList.collectionModel
                            if (bindingProductsCollection.table.elements.GetItemsCount() > 0){
                                productEditor.productId = bindingProductsCollection.table.elements.GetData("ProductUuid")
                                for (let i = 0; i < productsList.collectionModel.GetItemsCount(); i++){
                                    let id = productsList.collectionModel.GetData("Id", i);
                                    if (id === productEditor.productId){
                                        productsCB.currentIndex = i;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Image {
                id: lockImage;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right

                width: 18;
                height: 18;

                source: "../../../../" + Style.getIconPath("Icons/Lock", Icon.State.Off, Icon.Mode.Normal);

                sourceSize.width: width;
                sourceSize.height: height;

                visible: !productsCB.enabled;
            }
        }

        Text {
            id: errorText;

            width: parent.width;
            height: 15;

            color: Style.errorTextColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            visible: errorText.text !== "";
        }

        Rectangle{
            width: parent.width;
            height: 400;

            SoftwareProductCollectionView {
                id: softwareProductCollection;

                anchors.fill: parent

                commandsController: null;

                dataController: CollectionRepresentation {
                    collectionId: "SoftwareProducts";

                    Component.onCompleted: {
                        additionalFieldIds.push("OrderUuid");
                        additionalFieldIds.push("HardwareUuid");
                        additionalFieldIds.push("InUse");
                        additionalFieldIds.push("ProductUuid");
                        additionalFieldIds.push("CustomerUuid");
                    }

                    function updateModel(){
                    }
                }

                onSelectionChanged: {
                    if (selection.length <= 0){
                        bindButton.enabled = false
                    }
                    else{
                        bindingProductsCollection.table.resetSelection();

                        let index = selection[0];
                        let licenseId = softwareProductCollection.table.elements.GetData("LicenseId", index);

                        let inUse = softwareProductCollection.table.elements.GetData("InUse", index);
                        if (inUse && !unbindButton.userCanUnbind){
                            bindButton.enabled = false;

                            return;
                        }

                        let ok = productEditor.checkLicenseId(licenseId);
                        bindButton.enabled = ok;
                    }
                }

                onHeadersChanged: {
                    softwareProductCollection.table.setColumnContentComponent(0, pairComp);
                    softwareProductCollection.table.tableDecorator = tableDecoratorModel;
                }

                function updateData() {
                    if (visible){
                        if (productEditor.productId === ""){
                            return;
                        }

                        let elementsModel = bindingProductsCollection.table.elements;
                        let products = ""
                        let customerUuid = ""
                        for(var i = 0; i < bindingProductsCollection.table.elements.GetItemsCount(); i++){
                            let id = bindingProductsCollection.table.elements.GetData("Id", i);
                            if (i > 0){
                                products += ";"
                            }
                            else{
                                customerUuid = bindingProductsCollection.table.elements.GetData("CustomerUuid", i);
                            }

                            products += id
                        }

                        let filterModel = softwareProductCollection.collectionFilter.filterModel;

                        let objectFilter =  filterModel.AddTreeModel("ObjectFilter")

                        let bindingFilterModel = objectFilter.AddTreeModel("BindingFilter");
                        if (productEditor.productId != ""){
                            bindingFilterModel.SetData("ProductUuid", productEditor.productId);
                        }

                        if (products != ""){
                            bindingFilterModel.SetData("ExcludeUuids", products);
                        }

                        bindingFilterModel.SetData("HardwareUuidFilter", productEditor.hardwareId);

                        softwareProductCollection.doUpdateGui();
                    }
                }
            }
        }


        BaseText {
            id: message;
            color: Style.errorTextColor;

            visible: false;
        }
    }

    BaseText {
        anchors.top: parent.top
        anchors.topMargin: Style.margin
        anchors.horizontalCenter: bindingLicensesColumn.horizontalCenter
        text: qsTr("Used licenses");
        font.family: Style.fontFamilyBold;
    }

    Column {
        id: bindingLicensesColumn

        anchors.bottom: availableLicensesColumn.bottom;
        anchors.right: parent.right;
        anchors.rightMargin: productEditor.margin;
        anchors.left: parent.horizontalCenter;
        anchors.leftMargin: 20;

        Rectangle{
            width: parent.width;
            height: 400;

            SoftwareProductCollectionView {
                id: bindingProductsCollection;

                anchors.fill: parent

                commandsController: null;

//                hasPagination: false;
                hasSort: false;
                hasFilter: false;

                dataController: CollectionRepresentation {
                    collectionId: "SoftwareProducts";

                    Component.onCompleted: {
                        additionalFieldIds.push("OrderUuid");
                        additionalFieldIds.push("HardwareUuid");
                        additionalFieldIds.push("InUse");
                        additionalFieldIds.push("ProductUuid");
                        additionalFieldIds.push("CustomerUuid");
                    }

                    function updateModel(){
                    }
                }

                onSelectionChanged: {
                    if (selection.length === 0){
                        unbindButton.enabled = false
                    }
                    else{
                        softwareProductCollection.table.resetSelection();

                        let index = selection[0];

                        let elementsModel = bindingProductsCollection.table.elements;
                        let inUse = elementsModel.GetData("InUse", index);

                        if (unbindButton.userCanUnbind){
                            unbindButton.enabled = true;
                        }
                        else{
                            unbindButton.enabled = !inUse;
                        }
                    }
                }

                onHeadersChanged: {
                    bindingProductsCollection.table.setColumnContentComponent(0, pairComp);
                    bindingProductsCollection.table.tableDecorator = tableDecoratorModel;
                }

//                onVisibleChanged: {
//                    if (visible){
//                        let filterModel = bindingProductsCollection.collectionFilter.filterModel;

//                        let objectFilter =  filterModel.AddTreeModel("ObjectFilter")
//                        let bindingFilterModel = objectFilter.AddTreeModel("BindingFilter");

//                        bindingFilterModel.SetData("HardwareUuid", productEditor.hardwareId);

//                        bindingProductsCollection.doUpdateGui();
//                    }
//                }

                onElementsChanged: {
                    let objectFilter =  productsList.filterModel.AddTreeModel("ObjectFilter")
                    objectFilter.SetData("CategoryId", "Software");

                    if (bindingProductsCollection.table.elements.GetItemsCount() > 0){
                        productEditor.startProductId = bindingProductsCollection.table.elements.GetData("ProductUuid")
                    }

                    productsList.updateModel()
                }
            }
        }
    }

    ToolButton {
        id: bindButton;

        anchors.verticalCenter: bindingLicensesColumn.verticalCenter;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenterOffset: - 25;

        enabled: false;

        width: 18;
        height: 25;

        iconSource: enabled ? "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal):
                              "../../../" + Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled)

        tooltipText: qsTr("Bind to the sensor");

        property bool userCanBind: false;

        Component.onCompleted: {
            bindButton.userCanBind = PermissionsController.checkPermission("BindSensor");
        }

        onClicked: {
            let selectedProductIds = []
            let softwareIds = productEditor.bindingModel.GetData("SoftwareIds")
            if (softwareIds && softwareIds != ""){
                selectedProductIds = softwareIds.split(';')
            }
            let indexes = softwareProductCollection.table.tableSelection.selectedIndexes;
            if (indexes.length === 0){
                return
            }

            for (let index of indexes){
                let id = softwareProductCollection.table.elements.GetData("Id", index);
                if (!selectedProductIds.includes(id)){
                    selectedProductIds.push(id)
                    let newIndex = bindingProductsCollection.table.elements.InsertNewItem()
                    bindingProductsCollection.table.elements.CopyItemDataFromModel(newIndex, softwareProductCollection.table.elements, index);
                }
            }
            let products = selectedProductIds.join(';');
            productEditor.bindingModel.SetData("SoftwareIds", products)

            console.log("productEditor.bindingModel", productEditor.bindingModel.toJSON());
            softwareProductCollection.updateData()

            softwareProductCollection.table.resetSelection();
        }
    }

    ToolButton {
        id: unbindButton;

        anchors.verticalCenter: bindingLicensesColumn.verticalCenter;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenterOffset: 25;

        enabled: false;

        width: 18;
        height: 25;

        iconSource: enabled ? "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal):
                              "../../../" + Style.getIconPath("Icons/Left", Icon.State.Off, Icon.Mode.Disabled)

        property bool userCanUnbind: false;

        tooltipText: qsTr("Unbind from the sensor");

        Component.onCompleted: {
            unbindButton.userCanUnbind = PermissionsController.checkPermission("UnbindSensor");
        }

        onClicked: {
            console.log("productEditor.bindingModel", productEditor.bindingModel.toJSON());
            let selectedProductIds = []
            selectedProductIds = productEditor.bindingModel.GetData("SoftwareIds").split(';')
            let indexes = bindingProductsCollection.table.tableSelection.selectedIndexes;
            if (indexes.length === 0){
                return
            }

            let index = indexes[0];
            let elementsModel = bindingProductsCollection.table.elements;

            if (!unbindButton.userCanUnbind){
                if (elementsModel.ContainsKey("InUse", index)){
                    let inUse = elementsModel.GetData("InUse", index);
                    if (inUse){
                        return;
                    }
                }
            }

            let id = elementsModel.GetData("Id", index);
            if (selectedProductIds.indexOf(id) > -1){
                elementsModel.RemoveItem(index)
                selectedProductIds.splice(selectedProductIds.indexOf(id), 1);
            }

            let products = selectedProductIds.join(';');
            productEditor.bindingModel.SetData("SoftwareIds", products)

            bindingProductsCollection.table.resetSelection();

            softwareProductCollection.updateData()
        }
    }

    Loading {
        id: loading;

        anchors.fill: parent;

        visible: false;

        color: Style.baseColor;
    }

    TreeItemModel {
        id: collectionHeadersModel;

        Component.onCompleted: {
            productEditor.updateHeaders();
        }
    }

    function updateHeaders(){
        collectionHeadersModel.Clear();

        let index = collectionHeadersModel.InsertNewItem();
        collectionHeadersModel.SetData("Id", "InUse", index);
        collectionHeadersModel.SetData("Name", "", index);

        index = collectionHeadersModel.InsertNewItem();
        collectionHeadersModel.SetData("Id", "LicenseName", index);
        collectionHeadersModel.SetData("Name", qsTr("License name"), index);

        index = collectionHeadersModel.InsertNewItem();
        collectionHeadersModel.SetData("Id", "LicenseId", index);
        collectionHeadersModel.SetData("Name", qsTr("License-ID"), index);

        index = collectionHeadersModel.InsertNewItem();
        collectionHeadersModel.SetData("Id", "OrderId", index);
        collectionHeadersModel.SetData("Name", qsTr("Order-ID"), index);

        index = collectionHeadersModel.InsertNewItem();
        collectionHeadersModel.SetData("Id", "SerialNumber", index);
        collectionHeadersModel.SetData("Name", qsTr("Serial Number"), index);

        index = collectionHeadersModel.InsertNewItem();
        collectionHeadersModel.SetData("Id", "Customer", index);
        collectionHeadersModel.SetData("Name", qsTr("Customer"), index);

        softwareProductCollection.dataController.headersModel  = collectionHeadersModel;
        bindingProductsCollection.dataController.headersModel = collectionHeadersModel;
    }

    TreeItemModel {
        id: tableDecoratorModel;

        Component.onCompleted: {
            var cellWidthModel = tableDecoratorModel.AddTreeModel("CellWidth");

            let index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", 30, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);
        }
    }

    Component {
        id: pairComp;
        Item {
            Image {
                id: image;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 5;

                width: 18;
                height: width;

                source: "../../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal);

                sourceSize.width: width;
                sourceSize.height: height;
            }

            Component.onCompleted: {
                let loader = parent;
                let tableCellDelegate = loader.parent;

                let value = tableCellDelegate.getValue();
                if (value){
                    image.source = "../../../../" + Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Normal);
                }
                else{
                    image.source = "";
                }
            }
        }
    }
}//Container


