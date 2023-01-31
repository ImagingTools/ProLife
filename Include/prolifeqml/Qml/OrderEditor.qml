import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import Acf 1.0

DocumentBase {
    id: orderEditorContainer;

    commandsDelegateSourceComp: orderEditorCommandsDelegate;

    property TreeItemModel accountsModel: TreeItemModel {}
    property TreeItemModel productsModel: TreeItemModel {}

    property bool blockUpdatingModel: false;

    Component.onCompleted: {
        licensesProvider.updateModel();
    }

    Component {
        id: orderEditorCommandsDelegate;
        OrderEditorCommandsDelegate {}
    }

    onDocumentModelChanged: {
        updateGui();

        undoRedoManager.registerModel(documentModel)
    }

    onAccountsModelChanged: {
        console.log("onAccountsModelChanged", accountsModel);
        customerCB.model = accountsModel;
    }

    onProductsModelChanged: {
        console.log("onProductsModelChanged", productsModel);
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: orderEditorContainer.commandsId;

        onModelStateChanged: {
            updateGui();
        }
    }

    LicensesProvider {
        id: licensesProvider;
    }

    MouseArea {
        anchors.fill: parent;

        onClicked: {
            orderEditorContainer.forceActiveFocus();
        }
    }

    function updateGui(){
        console.log("Begin updateGui");
        blockUpdatingModel = true;

        if (documentModel.ContainsKey("OrderId")){
            instanceIdInput.text = documentModel.GetData("OrderId");
        }

        if (documentModel.ContainsKey("Description")){
            descriptionInput.text = documentModel.GetData("Description");
        }

        let customerId = documentModel.GetData("CustomerId");

        //        customerCB.currentText = "";
        let customerModel = customerCB.model;
        for (let i = 0; i < customerModel.GetItemsCount(); i++){
            let id = customerModel.GetData("Id", i);
            if (id === customerId){
                customerCB.currentIndex = i;
                break;
            }
        }
        if (documentModel.ContainsKey("OrderProducts")){
            productsView.model = 0
            productsView.model = documentModel.GetData("OrderProducts");
            console.log("productsView.model",  productsView.model.toJSON())
        }
        else{
            productsView.model = 0;
        }

        blockUpdatingModel = false;
        console.log("End updateGui");
    }

    function updateModel(){
        console.log("Begin updateModel");
        undoRedoManager.beginChanges();

        documentModel.SetData("OrderId", instanceIdInput.text)
        documentModel.SetData("Name", instanceIdInput.text);

        let selectedAccountId = customerCB.model.GetData("Id", customerCB.currentIndex);
        documentModel.SetData("CustomerId", selectedAccountId);

        documentModel.SetData("Description", descriptionInput.text);

        undoRedoManager.endChanges();
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Column {
        id: bodyColumn;

        width: 500;
        height: childrenRect.height;

        spacing: 7;

        Text {
            id: titleInstanceId;
            text: qsTr("Order-ID");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

//        RegExpValidator {
//            id: regexValid;

//            Component.onCompleted: {
//                console.log("RegExpValidator onCompleted");
//                let regex = settingsProvider.getInstanceMask();
//                console.log("regex", regex);

//                let re = new RegExp(regex)
//                if (re){
//                    regexValid.regExp = re;
//                }
//            }
//        }

        CustomTextField {
            id: instanceIdInput;

            width: parent.width;
            height: 30;

            placeHolderText: qsTr("Enter the order-ID");

            borderColor: Style.iconColorOnSelected;

            maximumLength: 17;

            onEditingFinished: {
                let currentId = documentModel.GetData("Id");
                if (currentId != instanceIdInput.text){
                    updateModel();
                }
            }
        }

        Text {
            id: titleCustomer;

            text: qsTr("Customer");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        ComboBox {
            id: customerCB;

            width: parent.width;
            height: 23;

            radius: 3;

            onCurrentIndexChanged: {
                if (!blockUpdatingModel){
                    updateModel();
                }
            }
        }


        Item{
            height: 35;
            anchors.left: parent.left;
            anchors.right: parent.right;

            AuxButton {
                id: addProduct;
                height: 25;
                width: 100;
                anchors.right: parent.right;
                anchors.bottom: parent.bottom;
                textButton: qsTr("Add product");
                hasText: true;
                backgroundColor: Style.baseColor;
                gradient: Gradient {
                    GradientStop { position: 0.0; color: Style.imagingToolsGradient1; }
                    GradientStop { position: 0.97; color: Style.imagingToolsGradient2; }
                    GradientStop { position: 0.98; color: Style.imagingToolsGradient3; }
                    GradientStop { position: 1.0; color: Style.imagingToolsGradient4; }
                }
                onClicked: {
                    productsView.activeProductIndex = -1;
                    modalDialogManager.openDialog(productEditorDialog, {});
                }
            }

            Text {
                id: titleLicenses;
                anchors.left: parent.left;
                anchors.bottom: parent.bottom;
                text: qsTr("Products");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }
        }
    }//Column bodyColumn

    Column {
        id: bodyDescription;
        anchors.left: bodyColumn.right;
        anchors.leftMargin: 10;

        width: 250;
        height: childrenRect.height;
        spacing: 7;

        Text {
            id: titleComment;
            text: qsTr("Description");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextEdit {
            id: descriptionInput;

            width: parent.width;
            height: 60;

            placeHolderText: qsTr("Enter the comment");

            borderColor: Style.iconColorOnSelected;

            onEditingFinished: {
                if (!blockUpdatingModel){
                    updateModel();
                }
            }
        }
    }

    Component {
        id: productEditorDialog;

        ProductEditorDialog {
            id: productsDialog;
            licensesModel: licensesProvider.model;
            productsModel: orderEditorContainer.productsModel;
            onStarted: {
//                if (orderEditorContainer.documentModel.ContainsKey("OrderProducts")){
//                    productsDialog.orderProductsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
//                }
//                else{
//                    productsDialog.orderProductsModel.Clear();
//                }
//                if (productsDialog.orderProductsModel){
//                    let productsModel = productsDialog.orderProductsModel
//                    for (let i = 0; i < productsModel.GetItemsCount(); i++){
//                        if (productsModel.GetData("CategoryId", i) == "Hardware"){
//                            let pairId = productsModel.GetData("PairId", i);
//                            let id = productsModel.GetData("Id", i);
//                            if (pairId && pairId != ""){
//                                for (let index = 0; index < productsModel.GetItemsCount(); index++){
//                                    if (pairId == productsModel.GetData("Id", index)){
//                                        productsModel.SetData("PairId", id, index);
//                                    }
//                                }
//                            }
//                        }
//                        else{
//                            let pairId = productsModel.GetData("PairId", i)
//                            if (!pairId){
//                                 productsModel.SetData("PairId", "", i);
//                            }
//                        }
//                    }
//                    productsDialog.bodyItem.documentModel = productsDialog.orderProductsModel.GetModelFromItem(productsView.activeProductIndex);
//                    console.log("ProductEditorDialog onStarted",  productsDialog.bodyItem.documentModel.toJSON())
//                }
                productsDialog.orderProductsModel.Clear();
                if (orderEditorContainer.documentModel.ContainsKey("OrderProducts")){
                    productsDialog.activeProductIndex = productsView.activeProductIndex;
                   let orderProductsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
                    productsDialog.orderProductsModel.Copy(orderProductsModel);
                }
//                productsDialog.bodyItem.documentModel = productsDialog.orderProductsModel.GetModelFromItem(productsView.activeProductIndex);
                //console.log("ProductEditorDialog onStarted",  productsDialog.bodyItem.documentModel.toJSON())
                productsDialog.bodyItem.started();
            }

//            function clearSoftwareMacAddress(productsModel, id){
//                setSoftwareMacAddress(productsModel, id, "")
//            }

//            function setSoftwareMacAddress(productsModel, id, macAddress){
//                for (let i = 0; i < productsModel.GetItemsCount(); i++){
//                    if (productsModel.GetData("CategoryId", i) == "Software" && id == productsModel.GetData("Id", i)){
//                        productsModel.SetData("MacAddress", macAddress,i);
//                    }
//                }
//            }

            onFinished: {
                if (buttonId == "Save"){
                    productsDialog.bodyItem.updateModel()
                    undoRedoManager.beginChanges();
//                    console.log("orderDocumentModel", orderEditorContainer.documentModel.toJSON());
//                    if (!orderEditorContainer.documentModel.ContainsKey("OrderProducts")){
//                        orderEditorContainer.documentModel.AddTreeModel("OrderProducts");
//                        console.log("newProductsModel", orderEditorContainer.documentModel.toJSON());
//                    }
//                    var productsModel =  orderEditorContainer.documentModel.GetData("OrderProducts");

//                    if (productsView.activeProductIndex == -1){
//                        productsView.activeProductIndex = productsModel.InsertNewItem();
//                    }

//                    const newProductModel = this.contentItem.documentModel;
//                    console.log("newProductModel", newProductModel.toJSON());
//                    productsModel.CopyItemDataFromModel(productsView.activeProductIndex, newProductModel, 0);
//                    console.log("ProductsModel", productsModel.toJSON());
//                    let pairId = newProductModel.GetData("PairId");
//                    let id = newProductModel.GetData("Id");
//                    let macAddress = newProductModel.GetData("MacAddress");
//                    let categoryId = newProductModel.GetData("CategoryId");

//                    // clear parents data
//                    if(categoryId == "Hardware"){
//                        if (pairId){
//                            clearSoftwareMacAddress(productsModel, pairId)
////                            productsModel.SetData("MacAddress", "", productsView.activeProductIndex);
//                            productsModel.SetData("PairId", "", productsView.activeProductIndex);
//                        }
//                    }
//                    else{
//                        for (let i = 0; i < productsModel.GetItemsCount(); i++){
//                            if (productsModel.GetData("CategoryId", i) == "Hardware" && id == productsModel.GetData("PairId", i)){
//                                productsModel.SetData("MacAddress", "", productsView.activeProductIndex);
//                                productsModel.SetData("PairId", "",i);
//                            }
//                        }
//                    }

//                    // set parents data
//                    pairId = this.contentItem.selectedPairId;

//                    if (pairId && pairId != ""){
//                        for (let i = 0; i < productsModel.GetItemsCount(); i++){
//                            if(categoryId == "Software"){
//                                if (pairId == productsModel.GetData("Id", i)){
//                                    if(productsModel.GetData("CategoryId", i) == "Hardware"){
//                                        productsModel.SetData("PairId", id, i);
//                                        setSoftwareMacAddress(productsModel, id, productsModel.GetData("MacAddress", i))
//                                    }
//                                }
//                            }
//                            else{
//                                if (id == productsModel.GetData("Id", i)){
//                                    if(productsModel.GetData("CategoryId", i) == "Hardware"){
//                                        productsModel.SetData("PairId", pairId, i);
//                                        setSoftwareMacAddress(productsModel, pairId, macAddress)
//                                    }
//                                }
//                            }
//                        }
//                    }
                    if (productsDialog.activeProductIndex > -1){
                        if (!orderEditorContainer.documentModel.ContainsKey("OrderProducts")){
                            orderEditorContainer.documentModel.AddTreeModel("OrderProducts");
                            //                            console.log("newProductsModel", orderEditorContainer.documentModel.toJSON());
                        }
                        let orderProductsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
                        orderProductsModel.Copy(productsDialog.orderProductsModel);
                    }

                    undoRedoManager.endChanges();
                    updateGui();
                }
            }
        }
    }

    CustomScrollbar {
        id: scrollbar;
        z: 100;

        anchors.left: productsView.right;
        anchors.leftMargin: 5;
        anchors.top: productsView.top;
        anchors.bottom: productsView.bottom;

        secondSize: 10;
        targetItem: flickable;
    }

    ListView {
        id: productsView;
        anchors.top: bodyColumn.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
//        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 5;

        width: 500;

        clip: true;
        boundsBehavior: Flickable.StopAtBounds;
        spacing: 10;

        property int activeProductIndex: -1;

        Component.onCompleted: {
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

        function getProductCategory(productId){
            let retVal = "";
            for (let i = 0; i < productsModel.GetItemsCount(); i++){
                let id = productsModel.GetData("Id", i);
                if (id === productId){
                    retVal = productsModel.GetData("CategoryId", i);
                    break;
                }
            }
            return retVal;
        }

//        function getPairName(productId, macAddress){

//            if (!macAddress || macAddress == ""){
//                return ""
//            }

//            let retVal = "";
//            let categoryId = getProductCategory(productId);

//            let productsModel = productsView.model;
//            for (let i = 0; i < productsModel.GetItemsCount(); i++){
//                let productMacAddress = productsModel.GetData("MacAddress", i);
//                if (categoryId == "Software" && productMacAddress == macAddress
//                        && productsModel.GetData("CategoryId", i) == "Hardware"){
//                    let modelProductId = productsModel.GetData("ProductId", i)
//                    retVal = getProductName(modelProductId);

//                    break;
//                }
//                if (categoryId == "Hardware" && productMacAddress == macAddress
//                        && productsModel.GetData("CategoryId", i) == "Software"){
//                    let modelProductId = productsModel.GetData("ProductId", i)
//                    retVal = getProductName(modelProductId);

//                    break;
//                }
//            }
//            return retVal;
//        }

        function getPairName(index){
            let productsModel = productsView.model;
            let productId  = productsModel.GetData("ProductId", index);
            let id  = productsModel.GetData("Id", index);
            let pairId  = productsModel.GetData("PairId", index);
            let categoryId = getProductCategory(productId);

            let retVal = "";

            for (let i = 0; i < productsModel.GetItemsCount(); i++){
                let productMacAddress = productsModel.GetData("MacAddress", i);
                if (categoryId == "Software" &&  productsModel.GetData("PairId", i) == id){
                    let modelProductId = productsModel.GetData("ProductId", i)
                    retVal = getProductName(modelProductId);

                    break;
                }
                if (categoryId == "Hardware" && productsModel.GetData("Id", i) == pairId){
                    let modelProductId = productsModel.GetData("ProductId", i)
                    retVal = getProductName(modelProductId);

                    break;
                }
            }
            return retVal;
        }

        function getLicenseName(index){
            let productsModel = productsView.model;
//            console.log("getLicenseName", productsModel.toJSON())
            console.log("getLicenseName", index)
              let retVal = "";
            let activeLicenses = productsModel.GetData("ActiveLicenses", index);
            let productId  = productsModel.GetData("ProductId", index);
//            console.log("getLicenseName activeLicenses", activeLicenses.toJSON())
            if (!activeLicenses){
                return;
            }

            for (let licenseIndex = 0; licenseIndex < activeLicenses.GetItemsCount(); licenseIndex++){
                if (licenseIndex > 0){
                    retVal += ", ";
                }
//                retVal += activeLicenses.GetData("Name", licenseIndex);
                retVal += licensesProvider.getLicenseName(productId, activeLicenses.GetData("Id", licenseIndex))
                retVal += " ";
                let expiration = activeLicenses.GetData("Expiration", licenseIndex);
                if (expiration == 0){
                    expiration = qsTr("Unlimited");
                }
                retVal += " " + expiration;

            };

            return retVal;
        }

        function getSoftwareIdByMacAddress(macAddress){
            let dataModel = documentModel.GetData("OrderProducts");

            for (let i = 0; i < dataModel.GetItemsCount(); i++){
                let category = dataModel.GetData("CategoryId", i);
                if (category === "Software"){
                    let address = dataModel.GetData("MacAddress", i);
                    if (address === macAddress){
                        let id = dataModel.GetData("Id", i);

                        return id;
                    }
                }
            }
        }

        OrderCommandsModelObserver {
            productCommandsModel: commandsModelLocal;
            orderCommandsProvider: orderEditorContainer.commandsProvider;
            addProductButton: addProduct;
        }

        TreeItemModel {
            id: commandsModelLocal;

            Component.onCompleted: {
                let index = commandsModelLocal.InsertNewItem();

                commandsModelLocal.SetData("Id", "Edit", index);
                commandsModelLocal.SetData("Name", "Edit", index);
                commandsModelLocal.SetData("Icon", "Edit", index);
                commandsModelLocal.SetData("IsEnabled", false, index);
                commandsModelLocal.SetData("Visible", true, index);

                index = commandsModelLocal.InsertNewItem();

                commandsModelLocal.SetData("Id", "CreateLicenseFile", index);
                commandsModelLocal.SetData("Name", "Create License File", index);
                commandsModelLocal.SetData("Icon", "Key", index);
                commandsModelLocal.SetData("IsEnabled", false, index);
                commandsModelLocal.SetData("Visible", true, index);

                index = commandsModelLocal.InsertNewItem();

                commandsModelLocal.SetData("Id", "Remove", index);
                commandsModelLocal.SetData("Name", "Remove", index);
                commandsModelLocal.SetData("Icon", "Close", index);
                commandsModelLocal.SetData("IsEnabled", false, index);
                commandsModelLocal.SetData("Visible", true, index);
            }
        }

        delegate: OrderProductView {
            productName: productsView.getProductName(model.ProductId);
            productCategory: productsView.getProductCategory(model.ProductId);
//            pairName: productsView.getPairName(model.ProductId, model.MacAddress);
//            pairName: model.MacAddress;
            pairName: productsView.getPairName(model.index)
            macAddress: model.MacAddress;
            serialNumber: model.SerialNumber;
//            licenseExpiration: !model.LicenseExpiration ? "Unlimited" : model.LicenseExpiration == "" ? "Unlimited" : model.LicenseExpiration;
            licenseName: productsView.getLicenseName(model.index);

            commandsModel: commandsModelLocal;

            onEdited: {
                productsView.activeProductIndex = model.index;

                var productsModel = orderEditorContainer.documentModel.GetData("OrderProducts");
                if (productsModel){
//                    let productModel = productsModel.GetModelFromItem(model.index);

//                    modalDialogManager.openDialog(productEditorDialog, {"documentModel": productModel});
//                    console.log("OrderProductView onEdited", productModel.toJSON())
                    modalDialogManager.openDialog(productEditorDialog, {});
                }
            }

            onRemoved: {
                productsView.activeProductIndex = model.index;

                modalDialogManager.openDialog(removeDialog, {"message": qsTr("Remove selected product ?")});
            }

            onCreateLicenseFile: {
                let orderId = documentModel.GetData("OrderId");
                let productId = model.Id;

                if (model.CategoryId === "Hardware"){
                    productId = productsView.getSoftwareIdByMacAddress(model.MacAddress);
                }

                console.log("onCreateLicenseFile", orderId + "/" + productId);

                licenseFileController.createLicenseFile(orderId + "/" + productId);
            }
        }
    }



    LicenseFileController {
        id: licenseFileController;
    }

    Component {
        id: removeDialog;

        MessageDialog {
            onFinished: {
                if (buttonId == "Yes"){
                    productsView.model.RemoveItem(productsView.activeProductIndex);
                }
            }
        }
    }

}//Container


