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
        productCB.model = productsModel;
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

        let customerId = documentModel.GetData("OrderCustomer");

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

        documentModel.SetData("Id", instanceIdInput.text)

//        let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
//        documentModel.SetData("ProductId", selectedProductId);

        let selectedAccountId = customerCB.model.GetData("Id", customerCB.currentIndex);
        documentModel.SetData("AccountId", selectedAccountId);

        documentModel.SetData("Comment", descriptionInput.text);

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

        function getLicenseName(licenseId){
            return licenseId;
        }

        delegate: OrderProductView {
            productName: productsView.getProductName(model.ProductId);
            productCategory: productsView.getProductCategory(model.ProductId);
            pairName: productsView.getProductName(model.PairId);
            macAddress: model.MacAddress;
            serialNumber: model.SerialNumber;
            licenseExpiration: model.LicenseExpiration == "" ? "Unlimited" : model.LicenseExpiration;
            licenseName: productsView.getLicenseName(model.LicenseId);
        }


    }


}//Container


