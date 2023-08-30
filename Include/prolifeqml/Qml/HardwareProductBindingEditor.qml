import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Item {
    id: productEditor;

    property int margin: 10;

    property int contentHeight: availableLicensesColumn.height + titleLable.height;

//    property alias collectionModel: softwareProductCollection.collectionModel;
//    property alias table: softwareProductsTable;

    property TreeItemModel bindingModel: TreeItemModel {}

    property string productId: ""

    property string hardwareId: "";

    signal checkedItemsChanged();
    signal modelChanged();



    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", productEditor.onLocalizationChanged);
        console.log("DEBUG::24",softwareProductCollection.modelFilter.toJSON())
//        softwareProductCollection.updateModel();

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

//            softwareProductsTable.uncheckAll();

//            if (softwareProductsTable.elements){
//                for (let i = 0; i < softwareProductsTable.elements.GetItemsCount(); i++){
//                    let id = softwareProductsTable.elements.GetData("Id", i);
//                    if (softwareIds.includes(id)){
//                        softwareProductsTable.checkItem(i);
//                    }
//                }
//            }
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

    Column {
        id: availableLicensesColumn;

//        anchors.verticalCenter: parent.verticalCenter;
        anchors.top: titleLable.bottom
        anchors.topMargin: Style.margin
        anchors.right: parent.horizontalCenter;
        anchors.rightMargin: 20;
        anchors.left: parent.left;
//        anchors.rightMargin: productEditor.margin;
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
                anchors.right: parent.right

                height: 25;
                radius: 3;
                nameId: "Id";

                onVisibleChanged: {
                    if (visible){
                        //                    let objectFilter =  productsList.filterModel.AddTreeModel("ObjectFilter")
                        //                    objectFilter.SetData("CategoryId", "Software");
                        //                    productsList.updateModel()
                    }
                }

                onCurrentIndexChanged: {
                    if (productEditor.blockUpdatingModel){
                        return;
                    }

                    //                productEditor.bindingModel.SetData("SoftwareIds", "");
                    if (productsCB.currentIndex > -1){
                        productEditor.productId = productsCB.model.GetData("Id", productsCB.currentIndex);
                    }
                    softwareProductCollection.updateData();
                    //                productEditor.updateGui();
                }

                CollectionDataProvider {
                    id: productsList;
                    fields: ["Id", "Name", "CategoryId"];
                    commandId: "Products";

                    onCollectionModelChanged: {
                        if (productsList.collectionModel != null){
                            productsCB.model = productsList.collectionModel
                            if (bindingProductsCollection.table.elements.GetItemsCount() > 0){
                                //                            productsCB.currentIndex = -1;
                                productEditor.productId = bindingProductsCollection.table.elements.GetData("ProductId")
                                for (let i = 0; i < productsList.collectionModel.GetItemsCount(); i++){
                                    let id = productsList.collectionModel.GetData("Id", i);
                                    if (id === productEditor.productId){
                                        productsCB.currentIndex = i;
                                    }
                                }
                            }
                            else{
                                //                            softwareProductCollection.updateData();
                            }
                        }
                    }

                    onFailed: {
                        if (orderEditorContainer.documentManager){
                            let message = qsTr("Error loading products. Please check Lisa connection.");

                            orderEditorContainer.documentManager.openErrorDialog(message);
                            orderEditorContainer.documentManager.showAlertMessage(message);

                            orderEditorContainer.errorMessage = message;
                        }
                    }

                }
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

        Rectangle{
            width: parent.width;
            height: 400;

            CollectionViewBase {
                id: softwareProductCollection;
                anchors.fill: parent

                isMultiCheckable: false;

                defaultSortHeaderIndex: 2;

                commands.headerInfoModel: headerInfoModel

                GqlModel {
                    id: headerInfoModel
                    function updateModel(){
                        softwareProductCollection.commands.itemsInfoModel.updateModel();
                    }
                }

                property MainDocumentManager mainDocumentManager: null;

                Component.onCompleted: {
                    console.log("DEBUG::29")
                    softwareProductCollection.pagination.countElements = 9
                    softwareProductCollection.loadData = false;
//                    softwareProductCollection.table.checkable = true
//                    bindingProductsCollection.table.selectable = false;
                    bindingProductsCollection.table.canSelectAll = true;
//                    bindingProductsCollection.table.hoverEnabled = false
                    softwareProductCollection.table.checkedItemsChanged.connect(checkedItemsChanged);
                    softwareProductCollection.table.selectionChanged.connect(selectionItemsChanged);
                }

                function selectionItemsChanged(selection){
                    console.log("DEBUG::60", selection)
                    if (selection.length === 0){
                        bindButton.enabled = false
                    }
                    else{
                        bindButton.enabled = true
                    }
                }

                function checkedItemsChanged(){
                    let selectedProductIds = []
                    let softwareIds = productEditor.bindingModel.GetData("SoftwareIds")
                    if (softwareIds != ""){
                        selectedProductIds = softwareIds.split(';')
                    }
                    console.log("DEBUG::41_1", selectedProductIds)
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
                     console.log("DEBUG::41_2", products, selectedProductIds)
                     updateData()
                     softwareProductCollection.table.properties.clearCheckedItems()
                }

                onHeadersChanged: {
                    console.log("DEBUG::26", softwareProductCollection.modelFilter.toJSON())
                    console.log("DEBUG::27",  softwareProductCollection.tableHeaders.toJSON())
                    console.log("DEBUG::28", softwareProductCollection.commands.headers.toJSON())
                }

                function updateData() {
                    if (visible){
                        console.log("DEBUG::30", softwareProductCollection.modelFilter.toJSON())
                        let objectFilter =  softwareProductCollection.modelFilter.AddTreeModel("ObjectFilter")
                        objectFilter.SetData("Key", "DeviceId");
                        objectFilter.SetData("Value", "");
                        objectFilter.SetData("IsEqual", true);
                        console.log("onCompleted filterModel", softwareProductCollection.modelFilter.toJSON());
                        let filterIdsModel = softwareProductCollection.modelFilter.GetData("FilterIds")
                        filterIdsModel.Clear();

                        for(var i = 0; i < softwareProductCollection.commands.headers.GetItemsCount(); i++){
                            let headerId = softwareProductCollection.commands.headers.GetData("Id", i);
                            if (!softwareProductCollection.commands.fieldsData.includes(headerId)){
                                softwareProductCollection.commands.fieldsData.push(headerId);
                            }
                            filterIdsModel.InsertNewItem()
                            filterIdsModel.SetData("Id", headerId, i);
                        }

                        if (!softwareProductCollection.commands.fieldsData.includes("CustomerUuid")){
                            softwareProductCollection.commands.fieldsData.push("CustomerUuid");
                        }

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

//                        let products = productEditor.bindingModel.GetData("SoftwareIds")
                        if (products != ""){
                            objectFilter.SetData("ExcludeIds", products);
                        }

                        if (customerUuid != ""){
                            objectFilter.SetData("CustomerUuid", customerUuid);
                        }

                        if (productEditor.productId != ""){
                            objectFilter.SetData("ProductId", productEditor.productId);
                        }

                        softwareProductCollection.commandsId = "SoftwareProducts"
                        softwareProductCollection.commands.itemsInfoModel.updateModel();
                        console.log("DEBUG::31", softwareProductCollection.modelFilter.toJSON())
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
        text: qsTr("Binding licenses");
        font.family: Style.fontFamilyBold;
    }

    Column {
        id: bindingLicensesColumn
        anchors.bottom: availableLicensesColumn.bottom;
        anchors.right: parent.right;
        anchors.left: parent.horizontalCenter;
        anchors.leftMargin: 20;
        anchors.rightMargin: productEditor.margin;
//        anchors.leftMargin: productEditor.margin;

        Rectangle{
            width: parent.width;
            height: 400;

            CollectionViewBase {
                id: bindingProductsCollection;
                anchors.fill: parent

//                defaultSortHeaderIndex: 2;
                isMultiCheckable: false;

                hasFilter: false
                hasPagination: false
                hasSort: false

                property MainDocumentManager mainDocumentManager: null;

                commands.headerInfoModel: headerInfoBindingModel

                GqlModel {
                    id: headerInfoBindingModel
                    function updateModel(){
                        bindingProductsCollection.commands.itemsInfoModel.updateModel();
                    }
                }

                Component.onCompleted: {
                    console.log("DEBUG::29")
                    bindingProductsCollection.pagination.countElements = 1000
                    bindingProductsCollection.loadData = false;
                    bindingProductsCollection.table.canSelectAll = false;
                    bindingProductsCollection.table.checkedItemsChanged.connect(checkedItemsChanged);
                    bindingProductsCollection.table.selectionChanged.connect(selectionItemsChanged);
                }

                function selectionItemsChanged(selection){
                    if (selection.length === 0){
                        unbindButton.enabled = false
                    }
                    else{
                        unbindButton.enabled = true
                    }
                }

                onVisibleChanged: {
                    if (visible){
                        let objectFilter =  bindingProductsCollection.modelFilter.AddTreeModel("ObjectFilter")

                        let filterIdsModel = bindingProductsCollection.modelFilter.GetData("FilterIds")
                        filterIdsModel.Clear();

                        for(let i = 0; i < bindingProductsCollection.commands.headers.GetItemsCount(); i++){
                            let headerId = bindingProductsCollection.commands.headers.GetData("Id", i);
                            if (!bindingProductsCollection.commands.fieldsData.includes(headerId)){
                                bindingProductsCollection.commands.fieldsData.push(headerId);
                            }
                            filterIdsModel.InsertNewItem()
                            filterIdsModel.SetData("Id", headerId, i);
                        }

                        if (!bindingProductsCollection.commands.fieldsData.includes("CustomerUuid")){
                            bindingProductsCollection.commands.fieldsData.push("CustomerUuid");
                        }
                        if (!bindingProductsCollection.commands.fieldsData.includes("ProductId")){
                            bindingProductsCollection.commands.fieldsData.push("ProductId");
                        }

                        objectFilter.SetData("HardwareUuid", productEditor.hardwareId);
                        bindingProductsCollection.commandsId = "SoftwareProducts"
                        bindingProductsCollection.commands.itemsInfoModel.updateModel();
                    }
                }

                onElementsChanged: {
                    let objectFilter =  productsList.filterModel.AddTreeModel("ObjectFilter")
                    objectFilter.SetData("CategoryId", "Software");

                    productsList.updateModel()
                }

                function checkedItemsChanged(){
                    let selectedProductIds = []
                    selectedProductIds = productEditor.bindingModel.GetData("SoftwareIds").split(';')
                    let indexes = bindingProductsCollection.table.getCheckedItems();
                    if (indexes.length === 0){
                        return
                    }
                     for (let i = indexes.length - 1; i > -1; i--){
                         let index = indexes[i]
                         let id = bindingProductsCollection.table.elements.GetData("Id", index);
                         if (selectedProductIds.indexOf(id) > -1){
                             bindingProductsCollection.table.elements.RemoveItem(index)
                             selectedProductIds.splice(selectedProductIds.indexOf(id), 1);
                         }
                     }
                     let products = selectedProductIds.join(';');
                     productEditor.bindingModel.SetData("SoftwareIds", products)

                     bindingProductsCollection.table.properties.clearCheckedItems()
                     if (bindingProductsCollection.table.elements.GetItemsCount() == 0){
                         productEditor.productId = ""
                         productsCB.enabled = true
                         productsCB.currentIndex = -1
                     }
                     else{
                          softwareProductCollection.updateData()
                     }
                }

            }
        }

    }

    AuxButton {
        id: bindButton;

        anchors.verticalCenter: bindingLicensesColumn.verticalCenter;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenterOffset: - 25;

        enabled: false;

        width: 18;
        height: 25;

        iconSource: "../../../" + "Icons/" + Style.theme + "/Right.svg";
//        iconSource: "../../../" + "Icons/" + Style.theme + "/Link_On_" + enabled ? "Active" : "Disabled" + ".svg";

        iconWidth: 15;
        iconHeight: iconWidth;

        onClicked: {
            let selectedProductIds = []
            let softwareIds = productEditor.bindingModel.GetData("SoftwareIds")
            if (softwareIds && softwareIds != ""){
                selectedProductIds = softwareIds.split(';')
            }
            console.log("DEBUG::41_1", selectedProductIds)
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
             console.log("DEBUG::41_2", products, selectedProductIds)
             softwareProductCollection.updateData()
             softwareProductCollection.table.tableSelection.selectedIndexes = []
        }
    }

    AuxButton {
        id: unbindButton;

        anchors.verticalCenter: bindingLicensesColumn.verticalCenter;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenterOffset: 25;

        enabled: false;

        width: 18;
        height: 25;

        iconSource: "../../../" + "Icons/" + Style.theme + "/Left.svg";

        iconWidth: 15;
        iconHeight: iconWidth;

        onClicked: {
            let selectedProductIds = []
            selectedProductIds = productEditor.bindingModel.GetData("SoftwareIds").split(';')
            let indexes = bindingProductsCollection.table.tableSelection.selectedIndexes;
            if (indexes.length === 0){
                return
            }
             for (let i = indexes.length - 1; i > -1; i--){
                 let index = indexes[i]
                 let id = bindingProductsCollection.table.elements.GetData("Id", index);
                 if (selectedProductIds.indexOf(id) > -1){
                     console.log("DEBUG::52", index)
                     bindingProductsCollection.table.elements.RemoveItem(index)
                     selectedProductIds.splice(selectedProductIds.indexOf(id), 1);
                 }
             }
             let products = selectedProductIds.join(';');
             productEditor.bindingModel.SetData("SoftwareIds", products)

             bindingProductsCollection.table.tableSelection.selectedIndexes = []
             if (bindingProductsCollection.table.elements.GetItemsCount() == 0){
                 productEditor.productId = ""
                 productsCB.enabled = true
                 productsCB.currentIndex = -1
             }
             else{
                  softwareProductCollection.updateData()
             }
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
        headersModel.SetData("Id", "LicenseName", index);
        headersModel.SetData("Name", qsTr("License name"), index);

        index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "LicenseId", index);
        headersModel.SetData("Name", qsTr("License-ID"), index);

        index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "OrderId", index);
        headersModel.SetData("Name", qsTr("Order-ID"), index);

        index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "SerialNumber", index);
        headersModel.SetData("Name", qsTr("Serial Number"), index);

        index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "Customer", index);
        headersModel.SetData("Name", qsTr("Customer"), index);

        softwareProductCollection.commands.headers  = headersModel;
        bindingProductsCollection.commands.headers = headersModel;
    }
}//Container


