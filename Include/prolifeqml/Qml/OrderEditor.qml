import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import Acf 1.0

DocumentBase {
    id: installationEditorContainer;

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

        commandsId: installationEditorContainer.commandsId;

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
            installationEditorContainer.forceActiveFocus();
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

//        let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
//        documentModel.SetData("ProductId", selectedProductId);

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

        RegExpValidator {
            id: regexValid;

            Component.onCompleted: {
                console.log("RegExpValidator onCompleted");
                let regex = settingsProvider.getInstanceMask();
                console.log("regex", regex);

                let re = new RegExp(regex)
                if (re){
                    regexValid.regExp = re;
                }
            }
        }

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
            productsModel: installationEditorContainer.productsModel;
            onStarted: {
                if (installationEditorContainer.documentModel.ContainsKey("OrderProducts")){
                    productsDialog.orderProductsModel = installationEditorContainer.documentModel.GetData("OrderProducts");
                }
                else{
                    productsDialog.orderProductsModel.Clear();
                }

                console.log("ProductEditorDialog onStarted", orderProductsModel.toJSON())
                productsDialog.bodyItem.started();
            }

            onFinished: {
                if (buttonId == "Save"){
                    undoRedoManager.beginChanges();
                    console.log("orderDocumentModel", installationEditorContainer.documentModel.toJSON());
                    if (!installationEditorContainer.documentModel.ContainsKey("OrderProducts")){
                        installationEditorContainer.documentModel.AddTreeModel("OrderProducts");
                        console.log("newProductsModel", installationEditorContainer.documentModel.toJSON());
                    }
                    var productsModel =  installationEditorContainer.documentModel.GetData("OrderProducts");
                    console.log("productsModel", productsModel.toJSON());

                    if (productsView.activeProductIndex == -1){
                        productsView.activeProductIndex = productsModel.InsertNewItem();
                    }

                    var newProductModel = this.contentItem.documentModel;
                    console.log("newProductModel", newProductModel.toJSON());
                    productsModel.CopyItemDataFromModel(productsView.activeProductIndex, newProductModel);
                    console.log("newProductsModel", productsModel.toJSON());
                    let pairId = newProductModel.GetData("PairId");
                    let productId = newProductModel.GetData("ProductId");
                    if (pairId != ""){
                        for (let i = 0; i < productsModel.GetItemsCount(); i++){
                            if (pairId == productsModel.GetData("ProductId", i)){
                                productsModel.SetData("PairId", productId);
                            }
                        }
                    }

                    undoRedoManager.endChanges();
                    updateGui();
                }
            }
        }
    }

    ListView {
        id: productsView;
        anchors.top: bodyColumn.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 5;

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

        function getLicenseName(uuidId){
            let productsModel = productsView.model;
            console.log("getLicenseName", productsModel.toJSON())
            console.log("getLicenseName uuidId", uuidId)
            let retVal = "";
            for (let productIndex = 0; productIndex <productsModel.GetItemsCount(); productIndex++){
                let id = productsModel.GetData("Id", productIndex);
                if (id === uuidId){
                    let activeLicenses = productsModel.GetData("ActiveLicenses");
//                    console.log("getLicenseName activeLicenses", activeLicenses.toJSON())

                    for (let licenseIndex = 0; licenseIndex < activeLicenses.GetItemsCount(); licenseIndex++){
                        if (licenseIndex > 0){
                            retVal += ", ";
                        }
                        retVal += activeLicenses.GetData("Name", licenseIndex);
                        retVal += " ";
                        let expiration = activeLicenses.GetData("Expiration", licenseIndex);
                        if (expiration == 0){
                            expiration = qsTr("Unlimited");
                        }
                        retVal += " " + expiration;

                    };
                    break;
                }
            }
            return retVal;
        }

        OrderCommandsModelObserver {
            productCommandsModel: commandsModelLocal;
            orderCommandsProvider: installationEditorContainer.commandsProvider;
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

                index = commandsModelLocal.InsertNewItem();

                commandsModelLocal.SetData("Id", "CreateLicenseFile", index);
                commandsModelLocal.SetData("Name", "Create License File", index);
                commandsModelLocal.SetData("Icon", "Key", index);
                commandsModelLocal.SetData("IsEnabled", false, index);

                index = commandsModelLocal.InsertNewItem();

                commandsModelLocal.SetData("Id", "Remove", index);
                commandsModelLocal.SetData("Name", "Remove", index);
                commandsModelLocal.SetData("Icon", "Close", index);
                commandsModelLocal.SetData("IsEnabled", false, index);
            }
        }

        delegate: OrderProductView {
            productName: productsView.getProductName(model.ProductId);
            productCategory: productsView.getProductCategory(model.ProductId);
//            pairName: productsView.getProductName(model.PairId);
            pairName: model.PairId;
            macAddress: model.MacAddress;
            serialNumber: model.SerialNumber;
//            licenseExpiration: !model.LicenseExpiration ? "Unlimited" : model.LicenseExpiration == "" ? "Unlimited" : model.LicenseExpiration;
            licenseName: productsView.getLicenseName(model.Id);

            commandsModel: commandsModelLocal;

            onEdited: {
                productsView.activeProductIndex = model.index;

                var productsModel = installationEditorContainer.documentModel.GetData("OrderProducts");
                if (productsModel){
                    let productModel = productsModel.GetModelFromItem(model.index);

                    modalDialogManager.openDialog(productEditorDialog, {"documentModel": productModel});
                }
            }

            onRemoved: {
                productsView.activeProductIndex = model.index;

                modalDialogManager.openDialog(removeDialog, {"message": qsTr("Remove selected product ?")});
            }

            onCreateLicenseFile: {

                let orderId = documentModel.GetData("OrderId");
                let productId = model.Id;
                if (model.CategoryId == "Hardware"){
                    productId = model.PairId;
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


