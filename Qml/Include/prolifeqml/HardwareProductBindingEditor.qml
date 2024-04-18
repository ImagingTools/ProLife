import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtlicgui 1.0

Item {
    id: productEditor;

    property int margin: 10;

    property int contentHeight: availableLicensesColumn.height + titleLable.height;

    property TreeItemModel bindingModel: TreeItemModel {}

    property string productId: ""
    property string hardwareId: "";

    property string productErrorMessage: qsTr("Please select a product");
    property string duplicateErrorMessage: qsTr("License with ID '%1' has already been selected");
    property string licenseErrorMessage: qsTr("License with ID '%1' has already been added");

    signal modelChanged();

    Component.onCompleted: {
        bindingModel.dataChanged.connect(productEditor.modelChanged);

        CachedProductCollection.updateModel();
    }

    onBindingModelChanged: {
        if (productEditor.bindingModel.ContainsKey("Id")){
            let id = productEditor.bindingModel.GetData("Id")

            productEditor.hardwareId = id;
        }

        if (productEditor.bindingModel.ContainsKey("ProductUuid")){
            let productId = productEditor.bindingModel.GetData("ProductUuid")

            productEditor.productId = productId;
        }

        bindingModel.dataChanged.connect(productEditor.modelChanged);

        updateGui();
    }

    property bool blockUpdatingModel: false;
    onBlockUpdatingModelChanged: {
        loading.visible = blockUpdatingModel;
    }

    function updateGui(){
        blockUpdatingModel = true;

        productsCB.currentIndex = -1;
        if (productsCB.model){
            for (let i = 0; i < productsCB.model.GetItemsCount(); i++){
                let id = productsCB.model.GetData("Id", i);
                if (id === productEditor.productId){
                    productsCB.currentIndex = i;
                    break;
                }
            }
        }

        if (productsCB.currentIndex < 0){
            productEditor.setError(productErrorMessage);
        }
        else{
            productEditor.setError("")
        }

        bindingProductsCollection.updateData();

        blockUpdatingModel = false;
    }

    function checkLicenseId(licenseId){
        let bindingElements = bindingProductsCollection.table.elements;
        if (bindingElements){
            for (let i = 0; i < bindingElements.GetItemsCount(); i++){
                let id = bindingElements.GetData("LicenseId", i)
                if (id === licenseId){
                    return false;
                }
            }
        }

        return true;
    }

    TreeItemModel {
        id: productsModel;
    }

    BaseText {
        id: titleLable

        anchors.horizontalCenter: availableLicensesColumn.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: Style.size_mainMargin

        text: qsTr("Available licenses");
        font.family: Style.fontFamilyBold;
    }

    function setError(message){
        errorText.text = message;
    }

    Column {
        id: availableLicensesColumn;

        anchors.top: titleLable.bottom
        anchors.topMargin: Style.size_mainMargin;
        anchors.right: parent.horizontalCenter;
        anchors.rightMargin: buttonsColumn.width;
        anchors.left: parent.left;
        anchors.leftMargin: Style.size_mainMargin;

        spacing: Style.size_mainMargin;

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
                anchors.leftMargin: Style.size_mainMargin;
                anchors.right: lockImage.left;
                anchors.rightMargin: Style.size_mainMargin;

                height: 30;

                radius: 3;
                nameId: "ProductName";

                model: CachedProductCollection.softwareProductsModel;

                changeable: bindingProductsCollection.table.elementsList.count === 0;

                Component.onCompleted: {
                    if (productsCB.currentIndex < 0){
                        productEditor.setError(productEditor.productErrorMessage);
                    }
                }

                onModelChanged: {
                    productEditor.updateGui();
                }

                onCurrentIndexChanged: {
                    if (productEditor.blockUpdatingModel){
                        return;
                    }

                    if (productsCB.currentIndex < 0){
                        productEditor.setError(productEditor.productErrorMessage);
                    }
                    else{
                        productEditor.setError("");
                    }

                    if (productsCB.currentIndex > -1){
                        productEditor.productId = productsCB.model.GetData("Id", productsCB.currentIndex);
                    }

                    softwareProductCollection.updateData();
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

                visible: !productsCB.changeable;
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

                filterMenu.decorator: Style.filterPanelDecorator;

                commandsControllerComp: null;

                table.checkable: true;
                table.selectable: false;

                dataControllerComp:
                    Component {
                    CollectionRepresentation {
                        id: softwareDataController;

                        Component.onCompleted: {
                            additionalFieldIds.push("OrderUuid");
                            additionalFieldIds.push("HardwareUuid");
                            additionalFieldIds.push("InUse");
                            additionalFieldIds.push("ProductUuid");
                            additionalFieldIds.push("CustomerUuid");
                        }

                        function updateModel(){}
                    }
                }

                function registerDocumentInfo(){}

                onCheckedItemsChanged: {
                    let selection = softwareProductCollection.table.getCheckedItems();
                    if (selection.length <= 0){
                        productEditor.setError("")
                        bindButton.enabled = false
                    }
                    else{
                        let ok = true;
                        for (let i = 0; i < selection.length; i++){
                            let index = selection[i];

                            let inUse = softwareProductCollection.table.elements.GetData("InUse", index);
                            if (inUse && !unbindButton.userCanUnbind){
                                ok = false;
                                break;
                            }

                            let licenseId = softwareProductCollection.table.elements.GetData("LicenseId", index);
                            if (!productEditor.checkLicenseId(licenseId)){
                                let message = productEditor.licenseErrorMessage.replace("%1", licenseId)
                                productEditor.setError(message)

                                ok = false;

                                break;
                            }

                            for (let j = i + 1; j < selection.length; j++){
                                let index2 = selection[j];

                                let licenseId2 = softwareProductCollection.table.elements.GetData("LicenseId", index2);
                                if (licenseId === licenseId2){
                                    let message =  productEditor.duplicateErrorMessage.replace("%1", licenseId);
                                    productEditor.setError(message)

                                    ok = false;

                                    break;
                                }
                            }

                            if (!ok){
                                break;
                            }
                        }

                        if (ok){
                            productEditor.setError("")
                        }

                        bindButton.enabled = ok;
                    }
                }

                onHeadersChanged: {
                    softwareProductCollection.table.setColumnContentComponent(0, null);
                    softwareProductCollection.table.tableDecorator = tableDecoratorModel2;
                }

                function updateData() {
                    if (!dataController){
                        return;
                    }

                    if (visible){
                        if (productEditor.productId === ""){
                            return;
                        }

                        dataController.collectionId = "SoftwareProducts"

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
        anchors.horizontalCenter: bindingLicensesColumn.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: Style.size_mainMargin;

        text: qsTr("Used licenses");
        font.family: Style.fontFamilyBold;
    }

    Column {
        id: bindingLicensesColumn

        anchors.bottom: availableLicensesColumn.bottom;
        anchors.right: parent.right;
        anchors.rightMargin: Style.size_mainMargin;
        anchors.left: parent.horizontalCenter;
        anchors.leftMargin: buttonsColumn.width;

        Rectangle {
            width: parent.width;
            height: 400;

            SoftwareProductCollectionView {
                id: bindingProductsCollection;

                anchors.fill: parent

                commandsControllerComp: null;

                filterMenu.decorator: Style.filterPanelDecorator;

                hasSort: false;
                hasFilter: false;
                filterMenuVisible: false;
                hasPagination: false;

                dataControllerComp:
                    Component {CollectionRepresentation {
                        id: bindingDataController;

                        Component.onCompleted: {
                            additionalFieldIds.push("OrderUuid");
                            additionalFieldIds.push("HardwareUuid");
                            additionalFieldIds.push("InUse");
                            additionalFieldIds.push("ProductUuid");
                            additionalFieldIds.push("CustomerUuid");
                        }

                        function updateModel(){}
                    }
                }

                function registerDocumentInfo(){}

                function updateData(){
                    if (!dataController){
                        return;
                    }

                    console.log("bindingProductsCollection updateData", dataController);

                    dataController.collectionId = "SoftwareProducts";

                    let filterModel = bindingProductsCollection.collectionFilter.filterModel;
                    let objectFilter =  filterModel.AddTreeModel("ObjectFilter")
                    let bindingFilterModel = objectFilter.AddTreeModel("BindingFilter");
                    bindingFilterModel.SetData("HardwareUuid", productEditor.hardwareId);

                    bindingProductsCollection.doUpdateGui();
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

                onElementsChanged: {
                    softwareProductCollection.updateData();
                }

                onHeadersChanged: {
                    bindingProductsCollection.table.setColumnContentComponent(0, pairComp);
                    bindingProductsCollection.table.tableDecorator = tableDecoratorModel;
                }
            }
        }
    }

    Column {
        id: buttonsColumn;

        anchors.centerIn: parent;

        spacing: Style.size_largeMargin;

        width: 20;

        ToolButton {
            id: bindButton;

            anchors.horizontalCenter: parent.horizontalCenter;

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

                let indexes = softwareProductCollection.table.getCheckedItems();
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

                softwareProductCollection.updateData()

                softwareProductCollection.table.resetSelection();
            }
        }

        ToolButton {
            id: unbindButton;

            anchors.horizontalCenter: parent.horizontalCenter;

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
    }

    Loading {
        id: loading;

        anchors.fill: parent;

        visible: false;

        color: Style.baseColor;
    }

    TreeItemModel {
        id: filterHeadersModel;

        Component.onCompleted: {
            let index = filterHeadersModel.InsertNewItem();
            filterHeadersModel.SetData("Id", "LicenseName", index);

            index = filterHeadersModel.InsertNewItem();
            filterHeadersModel.SetData("Id", "LicenseId", index);

            index = filterHeadersModel.InsertNewItem();
            filterHeadersModel.SetData("Id", "OrderId", index);

            index = filterHeadersModel.InsertNewItem();
            filterHeadersModel.SetData("Id", "PurchaseOrderId", index);

            index = filterHeadersModel.InsertNewItem();
            filterHeadersModel.SetData("Id", "SerialNumber", index);

            index = filterHeadersModel.InsertNewItem();
            filterHeadersModel.SetData("Id", "Customer", index);

            softwareProductCollection.collectionFilter.setFilteringInfoIds(filterHeadersModel);
        }
    }

    TreeItemModel {
        id: collectionHeadersModel2;

        Component.onCompleted: {
            productEditor.updateHeaders2();
        }
    }

    TreeItemModel {
        id: collectionHeadersModel;

        Component.onCompleted: {
            productEditor.updateHeaders();
        }
    }

    function updateHeaders2(){
        collectionHeadersModel2.Clear();

        let index = collectionHeadersModel2.InsertNewItem();
        collectionHeadersModel2.SetData("Id", "LicenseName", index);
        collectionHeadersModel2.SetData("Name", qsTr("Name"), index);

        index = collectionHeadersModel2.InsertNewItem();
        collectionHeadersModel2.SetData("Id", "LicenseId", index);
        collectionHeadersModel2.SetData("Name", qsTr("Article"), index);

        index = collectionHeadersModel2.InsertNewItem();
        collectionHeadersModel2.SetData("Id", "OrderId", index);
        collectionHeadersModel2.SetData("Name", qsTr("Delivery-ID"), index);

        index = collectionHeadersModel2.InsertNewItem();
        collectionHeadersModel2.SetData("Id", "PurchaseOrderId", index);
        collectionHeadersModel2.SetData("Name", qsTr("Purchase Order-ID"), index);

        index = collectionHeadersModel2.InsertNewItem();
        collectionHeadersModel2.SetData("Id", "SerialNumber", index);
        collectionHeadersModel2.SetData("Name", qsTr("Software-ID"), index);

        index = collectionHeadersModel2.InsertNewItem();
        collectionHeadersModel2.SetData("Id", "Customer", index);
        collectionHeadersModel2.SetData("Name", qsTr("Customer"), index);

        softwareProductCollection.dataController.headersModel  = collectionHeadersModel2;
    }

    function updateHeaders(){
        collectionHeadersModel.Clear();

        let index = collectionHeadersModel.InsertNewItem();
        collectionHeadersModel.SetData("Id", "InUse", index);
        collectionHeadersModel.SetData("Name", "", index);

        index = collectionHeadersModel.InsertNewItem();
        collectionHeadersModel.SetData("Id", "LicenseName", index);
        collectionHeadersModel.SetData("Name", qsTr("Name"), index);

        index = collectionHeadersModel.InsertNewItem();
        collectionHeadersModel.SetData("Id", "LicenseId", index);
        collectionHeadersModel.SetData("Name", qsTr("Article"), index);

        index = collectionHeadersModel.InsertNewItem();
        collectionHeadersModel.SetData("Id", "OrderId", index);
        collectionHeadersModel.SetData("Name", qsTr("Delivery-ID"), index);

        index = collectionHeadersModel.InsertNewItem();
        collectionHeadersModel.SetData("Id", "PurchaseOrderId", index);
        collectionHeadersModel.SetData("Name", qsTr("Purchase Order-ID"), index);

        index = collectionHeadersModel.InsertNewItem();
        collectionHeadersModel.SetData("Id", "SerialNumber", index);
        collectionHeadersModel.SetData("Name", qsTr("Software-ID"), index);

        index = collectionHeadersModel.InsertNewItem();
        collectionHeadersModel.SetData("Id", "Customer", index);
        collectionHeadersModel.SetData("Name", qsTr("Customer"), index);

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

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);
        }
    }

    TreeItemModel {
        id: tableDecoratorModel2;

        Component.onCompleted: {
            var cellWidthModel = tableDecoratorModel2.AddTreeModel("CellWidth");

            let index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

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

        TableCellDelegateBase {
            id: cellDelegate
            Image {
                id: image;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 5;

                width: 18;
                height: width;

                sourceSize.width: width;
                sourceSize.height: height;
            }

            onRowIndexChanged: {
                if (!rowDelegate){
                    return
                }

                let value = cellDelegate.getValue();
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


