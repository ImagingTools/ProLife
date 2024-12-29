import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtlicgui 1.0
import prolifeSensorBindingSdl 1.0
import prolifeSensorsSdl 1.0

Item {
    id: productEditor;
    clip: true;

    property int contentHeight: availableLicensesColumn.height;

	property DeviceBindingData bindingModel: null;

    property string productId: ""
    property string hardwareId: "";

    property string productErrorMessage: qsTr("Please select a product");
    property string duplicateErrorMessage: qsTr("License with ID '%1' has already been selected");
    property string licenseErrorMessage: qsTr("License with ID '%1' has already been added");

    signal modelChanged();

    Component.onCompleted: {
        CachedProductCollection.updateModel();
    }

    onBindingModelChanged: {
        if (bindingModel){
            productEditor.hardwareId = bindingModel.m_id;
            productEditor.productId = bindingModel.m_productUuid;
            bindingModel.modelChanged.connect(productEditor.modelChanged);

            updateGui();
        }
    }

    property bool blockUpdatingModel: false;
    onBlockUpdatingModelChanged: {
        loading.visible = blockUpdatingModel;
    }

    function updateGui(){
        blockUpdatingModel = true;

        if (productComboBoxElementView.cbRef){
            let productsCB = productComboBoxElementView.cbRef;

            productsCB.currentIndex = -1;
            if (productsCB.model){
                for (let i = 0; i < productsCB.model.getItemsCount(); i++){
                    let id = productsCB.model.getData("Id", i);
                    if (id === productEditor.productId){
                        productsCB.currentIndex = i;
                        break;
                    }
                }
            }
        }

        if (usedLicensesElementView.collection){
            usedLicensesElementView.collection.updateData();
        }

        blockUpdatingModel = false;
    }

    function checkLicenseId(licenseId){
        if (!usedLicensesElementView.collection){
            return false;
        }

        let bindingElements = usedLicensesElementView.collection.table.elements;
        if (bindingElements){
            for (let i = 0; i < bindingElements.getItemsCount(); i++){
                let id = bindingElements.getData("LicenseId", i)
                if (id === licenseId){
                    return false;
                }
            }
        }

        return true;
    }

    CustomScrollbar {
        id: scrollbar;
        anchors.right: parent.right;
        anchors.top: flickable.top;
        anchors.bottom: flickable.bottom;
        secondSize: Style.size_mainMargin;
        targetItem: flickable;
    }

    Flickable {
        id: flickable;
        anchors.top: parent.top;
        anchors.topMargin: Style.size_largeMargin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: Style.size_largeMargin;
        anchors.left: parent.left;
        anchors.leftMargin: Style.size_largeMargin;
        anchors.right: scrollbar.left;
        anchors.rightMargin: Style.size_largeMargin;
        contentWidth: width;
        contentHeight: content.height + 2 * Style.size_largeMargin;
        boundsBehavior: Flickable.StopAtBounds;
        clip: true;

        Column {
            id: content;
            width: parent.width;

            Item {
                width: parent.width;
                height: Math.max(availableLicensesColumn.height, bindingLicensesColumn.height, buttonsColumn.height)
                Column {
                    id: availableLicensesColumn;
                    anchors.top: parent.top
                    anchors.topMargin: Style.size_mainMargin;
                    anchors.right: parent.horizontalCenter;
                    anchors.rightMargin: buttonsColumn.width;
                    anchors.left: parent.left;
                    anchors.leftMargin: Style.size_mainMargin;
                    spacing: Style.size_mainMargin;

                    ComboBoxElementView {
                        id: productComboBoxElementView;
                        width: parent.width;
                        name: qsTr("Product");
                        nameId: "ProductName";
                        model: CachedProductCollection.softwareProductsModel;
                        changeable: usedLicensesElementView.collection && usedLicensesElementView.collection.table.elementsList.count === 0;
                        bottomComp: currentIndex >= 0 ? undefined : productErrorComp

                        onCurrentIndexChanged: {
                            if (productEditor.blockUpdatingModel){
                                return;
                            }

                            if (currentIndex > -1){
                                productEditor.productId = model.getData("Id", currentIndex);
                            }

                            if (availableLicensesElementView.collection){
                                availableLicensesElementView.collection.updateData();
                            }
                        }
                    }

                    Component {
                        id: productErrorComp;

                        BaseText {
                            color: Style.errorTextColor;
                            text: qsTr("Please select a product");
                        }
                    }

                    ElementView {
                        id: availableLicensesElementView;
                        width: parent.width;
                        name: qsTr("Available licenses");

                        property SoftwareProductCollectionView collection: null;

                        property bool hasSelectedDuplicate: false;

                        topComp: hasSelectedDuplicate ? licenseErrorComp : undefined;

                        Component {
                            id: licenseErrorComp;
                            BaseText {
                                color: Style.errorTextColor;
                                text: qsTr("Selected licenses with the same License-ID");
                            }
                        }

                        bottomComp: Component {
                            Rectangle {
                                id: rectWrap;
                                width: availableLicensesColumn.width;
                                height: 500;

                                Component.onCompleted: {
                                    availableLicensesElementView.collection = softwareProductCollection;
                                }

                                TreeItemModel {
                                    id: collectionHeadersModel2;

                                    Component.onCompleted: {
                                        rectWrap.updateHeaders2();
                                    }
                                }

                                TreeItemModel {
                                    id: filterHeadersModel;

                                    Component.onCompleted: {
                                        let index = filterHeadersModel.insertNewItem();
                                        filterHeadersModel.setData("Id", "LicenseName", index);

                                        index = filterHeadersModel.insertNewItem();
                                        filterHeadersModel.setData("Id", "LicenseId", index);

                                        index = filterHeadersModel.insertNewItem();
                                        filterHeadersModel.setData("Id", "OrderId", index);

                                        index = filterHeadersModel.insertNewItem();
                                        filterHeadersModel.setData("Id", "PurchaseOrderId", index);

                                        index = filterHeadersModel.insertNewItem();
                                        filterHeadersModel.setData("Id", "SerialNumber", index);

                                        index = filterHeadersModel.insertNewItem();
                                        filterHeadersModel.setData("Id", "Customer", index);

                                        softwareProductCollection.collectionFilter.setFilteringInfoIds(filterHeadersModel);
                                    }
                                }

                                function updateHeaders2(){
                                    collectionHeadersModel2.clear();

                                    let index = collectionHeadersModel2.insertNewItem();
                                    collectionHeadersModel2.setData("Id", "LicenseName", index);
                                    collectionHeadersModel2.setData("Name", qsTr("Name"), index);

                                    index = collectionHeadersModel2.insertNewItem();
                                    collectionHeadersModel2.setData("Id", "LicenseId", index);
                                    collectionHeadersModel2.setData("Name", qsTr("Article"), index);

                                    index = collectionHeadersModel2.insertNewItem();
                                    collectionHeadersModel2.setData("Id", "OrderId", index);
                                    collectionHeadersModel2.setData("Name", qsTr("Delivery-ID"), index);

                                    index = collectionHeadersModel2.insertNewItem();
                                    collectionHeadersModel2.setData("Id", "PurchaseOrderId", index);
                                    collectionHeadersModel2.setData("Name", qsTr("Purchase Order-ID"), index);

                                    index = collectionHeadersModel2.insertNewItem();
                                    collectionHeadersModel2.setData("Id", "SerialNumber", index);
                                    collectionHeadersModel2.setData("Name", qsTr("Software-ID"), index);

                                    index = collectionHeadersModel2.insertNewItem();
                                    collectionHeadersModel2.setData("Id", "Customer", index);
                                    collectionHeadersModel2.setData("Name", qsTr("Customer"), index);

                                    softwareProductCollection.tableViewParamsStoredServer = false;
                                    softwareProductCollection.dataController.headersModel  = collectionHeadersModel2;
                                }

                                SoftwareProductCollectionView {
                                    id: softwareProductCollection;
                                    anchors.fill: parent
                                    filterMenu.decorator: Style.filterPanelDecorator;
                                    commandsControllerComp: null;
                                    table.checkable: true;
                                    table.selectable: false;
                                    tableViewParamsStoredServer: false;
                                    commandsViewComp: undefined;
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
                                            availableLicensesElementView.hasSelectedDuplicate = false;
                                            bindButton.enabled = false
                                        }
                                        else{
                                            let ok = true;
                                            for (let i = 0; i < selection.length; i++){
                                                let index = selection[i];

                                                let inUse = softwareProductCollection.table.elements.getData("InUse", index);
                                                if (inUse && !unbindButton.userCanUnbind){
                                                    ok = false;
                                                    break;
                                                }

                                                let licenseId = softwareProductCollection.table.elements.getData("LicenseId", index);
                                                if (!productEditor.checkLicenseId(licenseId)){
                                                    let message = productEditor.licenseErrorMessage.replace("%1", licenseId)
                                                    // productEditor.setError(message)
                                                    availableLicensesElementView.hasSelectedDuplicate = true;
                                                    ok = false;

                                                    break;
                                                }

                                                for (let j = i + 1; j < selection.length; j++){
                                                    let index2 = selection[j];

                                                    let licenseId2 = softwareProductCollection.table.elements.getData("LicenseId", index2);
                                                    if (licenseId === licenseId2){
                                                        let message =  productEditor.duplicateErrorMessage.replace("%1", licenseId);
                                                        // productEditor.setError(message)
                                                        availableLicensesElementView.hasSelectedDuplicate = true;

                                                        ok = false;

                                                        break;
                                                    }
                                                }

                                                if (!ok){
                                                    break;
                                                }
                                            }

                                            if (ok){
                                                availableLicensesElementView.hasSelectedDuplicate = false;
                                                // productEditor.setError("")
                                            }

                                            bindButton.enabled = ok;
                                        }
                                    }

                                    onHeadersChanged: {
                                        softwareProductCollection.table.setColumnContentById("LicenseName", null);
                                        // softwareProductCollection.table.tableDecorator = tableDecoratorModel2;
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

                                            if (!usedLicensesElementView.collection){
                                                return;
                                            }

                                            let elementsModel = usedLicensesElementView.collection.table.elements;

                                            let products = []
                                            let licenseIds = []

                                            for(var i = 0; i < usedLicensesElementView.collection.table.elements.getItemsCount(); i++){
                                                let id = usedLicensesElementView.collection.table.elements.getData("Id", i);
                                                let licenseId = usedLicensesElementView.collection.table.elements.getData("LicenseId", i);

                                                products.push(id)
                                                licenseIds.push(licenseId)
                                            }

                                            let filterModel = softwareProductCollection.collectionFilter.filterModel;

                                            let objectFilter =  filterModel.addTreeModel("ObjectFilter")

                                            let bindingFilterModel = objectFilter.addTreeModel("BindingFilter");
                                            if (productEditor.productId != ""){
                                                bindingFilterModel.setData("ProductUuid", productEditor.productId);
                                            }

                                            if (products.length !== 0){
                                                bindingFilterModel.setData("ExcludeUuids", products.join(';'));
                                            }

                                            if (licenseIds.length !== 0){
                                                bindingFilterModel.setData("LicenseIds", licenseIds.join(';'));
                                            }

                                            bindingFilterModel.setData("HardwareUuidFilter", productEditor.hardwareId);

                                            softwareProductCollection.doUpdateGui();
                                        }
                                    }
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

                Column {
                    id: bindingLicensesColumn

                    anchors.bottom: availableLicensesColumn.bottom;
                    anchors.right: parent.right;
                    anchors.rightMargin: Style.size_mainMargin;
                    anchors.left: parent.horizontalCenter;
                    anchors.leftMargin: buttonsColumn.width;

                    ElementView {
                        id: usedLicensesElementView;
                        width: parent.width;
                        name: qsTr("Used licenses");

                        property SoftwareProductCollectionView collection: null;

                        bottomComp: Component {
                            Rectangle {
                                id: rectWrap;
                                width: parent.width;
                                height: 500;

                                Component.onCompleted: {
                                    usedLicensesElementView.collection = bindingProductsCollection;
                                }

                                Component {
                                    id: lockIconCellComp;

                                    TableCellDelegateBase {
                                        id: cellDelegate
                                        Image {
                                            id: image;

                                            anchors.verticalCenter: parent.verticalCenter;
                                            anchors.left: parent.left;
                                            anchors.leftMargin: Style.size_smallMargin;

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

                                TreeItemModel {
                                    id: collectionHeadersModel;

                                    Component.onCompleted: {
                                        rectWrap.updateHeaders();
                                    }
                                }

                                function updateHeaders(){
                                    collectionHeadersModel.clear();

                                    let index = collectionHeadersModel.insertNewItem();
                                    collectionHeadersModel.setData("Id", "InUse", index);
                                    collectionHeadersModel.setData("Name", "", index);

                                    index = collectionHeadersModel.insertNewItem();
                                    collectionHeadersModel.setData("Id", "LicenseName", index);
                                    collectionHeadersModel.setData("Name", qsTr("Name"), index);

                                    index = collectionHeadersModel.insertNewItem();
                                    collectionHeadersModel.setData("Id", "LicenseId", index);
                                    collectionHeadersModel.setData("Name", qsTr("Article"), index);

                                    index = collectionHeadersModel.insertNewItem();
                                    collectionHeadersModel.setData("Id", "OrderId", index);
                                    collectionHeadersModel.setData("Name", qsTr("Delivery-ID"), index);

                                    index = collectionHeadersModel.insertNewItem();
                                    collectionHeadersModel.setData("Id", "PurchaseOrderId", index);
                                    collectionHeadersModel.setData("Name", qsTr("Purchase Order-ID"), index);

                                    index = collectionHeadersModel.insertNewItem();
                                    collectionHeadersModel.setData("Id", "SerialNumber", index);
                                    collectionHeadersModel.setData("Name", qsTr("Software-ID"), index);

                                    index = collectionHeadersModel.insertNewItem();
                                    collectionHeadersModel.setData("Id", "Customer", index);
                                    collectionHeadersModel.setData("Name", qsTr("Customer"), index);

                                    bindingProductsCollection.tableViewParamsStoredServer = false;
                                    bindingProductsCollection.dataController.headersModel = collectionHeadersModel;
                                }

                                SoftwareProductCollectionView {
                                    id: bindingProductsCollection;
                                    anchors.fill: parent;
                                    commandsControllerComp: null;
                                    filterMenu.decorator: Style.filterPanelDecorator;
                                    hasSort: false;
                                    hasFilter: false;
                                    filterMenuVisible: false;
                                    hasPagination: false;
                                    commandsViewComp: undefined;
                                    tableViewParamsStoredServer: false;
									table.isMultiSelect: false;
                                    // additionalFieldIds: ["OrderUuid","HardwareUuid", "InUse", "ProductUuid", "CustomerUuid"]

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

                                        dataController.collectionId = "SoftwareProducts";

                                        let filterModel = bindingProductsCollection.collectionFilter.filterModel;
                                        let objectFilter =  filterModel.addTreeModel("ObjectFilter")
                                        let bindingFilterModel = objectFilter.addTreeModel("BindingFilter");
                                        bindingFilterModel.setData("HardwareUuid", productEditor.hardwareId);

                                        bindingProductsCollection.doUpdateGui();
                                    }

                                    onSelectionChanged: {
                                        if (selection.length === 0){
                                            unbindButton.enabled = false
                                        }
                                        else{
                                            if (availableLicensesElementView.collection){
                                                availableLicensesElementView.collection.table.resetSelection();
                                            }

                                            let index = selection[0];

                                            let elementsModel = bindingProductsCollection.table.elements;
                                            let inUse = elementsModel.getData("InUse", index);

                                            if (unbindButton.userCanUnbind){
                                                unbindButton.enabled = true;
                                            }
                                            else{
                                                unbindButton.enabled = !inUse;
                                            }
                                        }
                                    }

                                    onElementsChanged: {
                                        if (availableLicensesElementView.collection){
                                            availableLicensesElementView.collection.updateData();
                                        }
                                    }

                                    onHeadersChanged: {
                                        bindingProductsCollection.table.setColumnContentById("InUse", lockIconCellComp);
                                    }
                                }
                            }
                        }
                    }
                }

                Column {
                    id: buttonsColumn;
                    anchors.centerIn: parent;
                    spacing: Style.size_largeMargin;
                    width: Style.size_largeMargin;

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
                            if (!availableLicensesElementView.collection){
                                return;
                            }

                            if (!usedLicensesElementView.collection){
                                return;
                            }

                            let selectedProductIds = []
                            let softwareIds = productEditor.bindingModel.m_softwareIds;
                            if (softwareIds && softwareIds != ""){
                                selectedProductIds = softwareIds.split(';')
                            }

                            let indexes = availableLicensesElementView.collection.table.getCheckedItems();
                            if (indexes.length === 0){
                                return
                            }

                            for (let index of indexes){
                                let id = availableLicensesElementView.collection.table.elements.getData("Id", index);
                                if (!selectedProductIds.includes(id)){
                                    selectedProductIds.push(id)
                                    let newIndex = usedLicensesElementView.collection.table.elements.insertNewItem()
                                    usedLicensesElementView.collection.table.elements.copyItemDataFromModel(newIndex, availableLicensesElementView.collection.table.elements, index);
                                }
                            }

                            let products = selectedProductIds.join(';');
                            productEditor.bindingModel.m_softwareIds = products;

                            availableLicensesElementView.collection.updateData();
                            availableLicensesElementView.collection.table.resetSelection();
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
                            if (!usedLicensesElementView.collection){
                                return;
                            }

                            let selectedProductIds = productEditor.bindingModel.m_softwareIds.split(';')
                            let indexes = usedLicensesElementView.collection.table.tableSelection.selectedIndexes;
                            if (indexes.length === 0){
                                return
                            }

                            let index = indexes[0];
                            let elementsModel = usedLicensesElementView.collection.table.elements;

                            if (!unbindButton.userCanUnbind){
                                if (elementsModel.containsKey("InUse", index)){
                                    let inUse = elementsModel.getData("InUse", index);
                                    if (inUse){
                                        return;
                                    }
                                }
                            }

                            let id = elementsModel.getData("Id", index);
                            if (selectedProductIds.indexOf(id) > -1){
                                elementsModel.removeItem(index)
                                selectedProductIds.splice(selectedProductIds.indexOf(id), 1);
                            }

                            let products = selectedProductIds.join(';');
                            productEditor.bindingModel.m_softwareIds = products;

                            usedLicensesElementView.collection.table.resetSelection();

                            if (availableLicensesElementView.collection){
                                availableLicensesElementView.collection.updateData();
                            }
                        }
                    }
                }
            }
        }
    }

    Loading {
        id: loading;
        anchors.fill: parent;
        visible: false;
        color: Style.backgroundColor2;
    }

    TreeItemModel {
        id: tableDecoratorModel;

        Component.onCompleted: {
            var cellWidthModel = tableDecoratorModel.addTreeModel("CellWidth");

            let index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("WidthPercent", 5, index);

            index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("WidthPercent", -1, index);

            index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("WidthPercent", -1, index);

            index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("WidthPercent", -1, index);

            index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("WidthPercent", -1, index);

            index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("WidthPercent", -1, index);

            index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("WidthPercent", -1, index);
        }
    }

    TreeItemModel {
        id: tableDecoratorModel2;

        Component.onCompleted: {
            var cellWidthModel = tableDecoratorModel2.addTreeModel("CellWidth");

            let index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("Width", -1, index);

            index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("Width", -1, index);

            index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("Width", -1, index);

            index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("Width", -1, index);

            index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("Width", -1, index);

            index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("Width", -1, index);
        }
    }
}//Container


