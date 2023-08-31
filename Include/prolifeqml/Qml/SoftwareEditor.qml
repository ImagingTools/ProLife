import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: root;

    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}

    property bool documentReady: modelIsReady && productsList.completed && ordersList.completed;

    Component.onCompleted: {
        productsList.updateModel();
        ordersList.updateModel();
    }

    onDocumentReadyChanged: {
        if (root.documentReady){
            root.updateGui();
            undoRedoManager.registerModel(root.documentModel);
        }
    }

    CollectionDataProvider {
        id: ordersList;

        commandId: "Orders";

        fields: ["Id", "OrderId", "Description"];

        onCollectionModelChanged: {
            console.log("ordersList onCollectionModelChanged");

            if (ordersList.collectionModel != null){
                ordersCB.model = ordersList.collectionModel;
            }
        }
    }

    CollectionDataProvider {
        id: productsList;

        fields: ["Id", "Name", "CategoryId", "Licenses"];
        commandId: "Products";

        onCollectionModelChanged: {
            if (productsList.collectionModel != null){
                root.productsModel = productsList.collectionModel;

                productCB.model = root.productsModel;
            }
        }

        onFailed: {
            if (root.documentManager){
                let message = qsTr("Error loading products. Please check Lisa connection.");

                root.documentManager.openErrorDialog(message);
                root.documentManager.showAlertMessage(message);

                root.errorMessage = message;
            }
        }
    }

    function updateGui(){
        console.log("Software updateGui", root.documentModel.toJSON());

        root.blockUpdatingModel = true;

        if (root.documentModel.ContainsKey("SerialNumber")){
            let serialNumber = root.documentModel.GetData("SerialNumber");
            softwareProductEditor.productModel.SetData("SerialNumber", serialNumber);
        }
        else{
            softwareProductEditor.productModel.SetData("SerialNumber", "")
        }

        if (root.documentModel.ContainsKey("Project")){
            projectInput.text = root.documentModel.GetData("Project");
        }

        ordersCB.currentIndex = -1;

        if (root.documentModel.ContainsKey("OrderUuid")){
            let orderUuid = root.documentModel.GetData("OrderUuid");

            for (let i = 0; i < ordersCB.model.GetItemsCount(); i++){
                let id = ordersCB.model.GetData("Id", i);
                if (id == orderUuid){
                    ordersCB.currentIndex = i;
                    break;
                }
            }
        }

        let expiration = "";
        if (root.documentModel.ContainsKey("Expiration")){
            expiration = root.documentModel.GetData("Expiration");
        }

        if (root.documentModel.ContainsKey("LicenseId")){
            let licenseId = root.documentModel.GetData("LicenseId");

            let activeLicenses = softwareProductEditor.productModel.AddTreeModel("ActiveLicenses");
            activeLicenses.InsertNewItem();
            activeLicenses.SetData("Id", licenseId);
            activeLicenses.SetData("Expiration", expiration);
        }

        productCB.currentIndex = -1;
        if (root.documentModel.ContainsKey("ProductId")){
            let productId = root.documentModel.GetData("ProductId");

            for (let i = 0; i < productCB.model.GetItemsCount(); i++){
                let id = productCB.model.GetData("Id", i);
                if (id == productId){
                    productCB.currentIndex = i;
                    break;
                }
            }
        }

        if (productCB.currentIndex >= 0){
            let licensesModel = productCB.model.GetData("Licenses", productCB.currentIndex);
            if (!licensesModel){
                licensesModel = productCB.model.AddTreeModel("Licenses", productCB.currentIndex);
            }
            softwareProductEditor.productLicensesModel = licensesModel;
        }

        softwareProductEditor.updateGui();

        root.blockUpdatingModel = false;
    }

    function updateModel(){
        if (root.blockUpdatingModel){
            return;
        }

        console.log("updateModel", softwareProductEditor.productModel.toJSON());

        undoRedoManager.beginChanges();

        if (softwareProductEditor.productModel.ContainsKey("SerialNumber")){
            let serialNumber = softwareProductEditor.productModel.GetData("SerialNumber");
            root.documentModel.SetData("SerialNumber", serialNumber)
        }

        let licenseId = "";
        let expiration = "";
        let activeLicensesModel = softwareProductEditor.productModel.GetData("ActiveLicenses");
        if (activeLicensesModel){
            licenseId =  activeLicensesModel.GetData("Id");
            expiration =  activeLicensesModel.GetData("Expiration");
        }

        root.documentModel.SetData("Project", projectInput.text);

        //        let productId = productCB.model.GetData("Id", productCB.currentIndex);
        //        root.documentModel.SetData("ProductId", productId);

        if (ordersCB.model){
            if (ordersCB.currentIndex >= 0){
                let orderUuid = ordersCB.model.GetData("Id", ordersCB.currentIndex);
                root.documentModel.SetData("OrderUuid", orderUuid);
            }
            else{
                root.documentModel.SetData("OrderUuid", "");
            }
        }

        root.documentModel.SetData("LicenseId", licenseId);
        root.documentModel.SetData("Expiration", expiration);

        undoRedoManager.endChanges();
    }

    function getProductLicensesModel(){
        for (let i = 0; i < root.licensesModel.GetItemsCount(); i++){
            let productId = root.licensesModel.GetData("Id", i);
            if (productId === root.productId){
                if (root.licensesModel.ContainsKey("Licenses", i)){
                    return root.licensesModel.GetData("Licenses", i);
                }
            }
        }

        return null;
    }

    UndoRedoManager {
        id: undoRedoManager;

        documentBase: root;

        onModelStateChanged: {
            root.updateGui();
        }
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Flickable {
        id: flickable;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.leftMargin: 20;

        width: 450;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: bodyColumn;

            width: flickable.width;

            spacing: 7;

            Text {
                id: titleProject;

                color: Style.textColor;
                font.family: Style.fontFamilyBold;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Project");
            }

            CustomTextField {
                id: projectInput;

                height: 30;
                width: bodyColumn.width;

                placeHolderText: qsTr("Enter the project");

                onEditingFinished: {
                    let oldText = root.documentModel.GetData("Project");
                    if (oldText && oldText !== projectInput.text || !oldText && projectInput.text !== ""){
                        root.updateModel();
                    }
                }
            }

            Text {
                id: titleOrder;

                text: qsTr("Order");
                color: Style.textColor;
                font.family: Style.fontFamilyBold;
                font.pixelSize: Style.fontSize_common;
            }

            Item {
                width: parent.width;
                height: 23;

                ComboBox {
                    id: ordersCB;

                    anchors.left: parent.left;

                    width: parent.width - buttonContainer.width - 10;
                    height: 23;

                    radius: 3;

                    nameId: "OrderId";

                    onCurrentIndexChanged: {
                        console.log("onCurrentIndexChanged", ordersCB.currentIndex);
                        if (root.blockUpdatingModel){
                            return;
                        }

                        root.updateModel();
                    }
                }

                BaseButton{
                    id: buttonContainer;

                    anchors.right: parent.right;

                    text: qsTr("Clear");

                    decorator: defaultButtonDecorator;

                    onClicked: {
                        if(root.documentModel.ContainsKey("OrderUuid")){
                            let orderUuid = root.documentModel.GetData("OrderUuid")
                            if (ordersCB.currentIndex != -1){
                                ordersCB.currentIndex = -1;
                            }
                        }
                    }

                    Component{
                        id: defaultButtonDecorator;
                        CommonButtonDecorator{
                            width: 70;
                            height: 23;
                        }
                    }
                }//delegate
            }

            Text {
                id: titleProduct;

                text: qsTr("Product");
                color: Style.textColor;
                font.family: Style.fontFamilyBold;
                font.pixelSize: Style.fontSize_common;
            }

            ComboBox {
                id: productCB;

                width: parent.width;
                height: 23;

                radius: 3;

                onCurrentIndexChanged: {
                    console.log("onCurrentIndexChanged", productCB.currentIndex);
                    if (root.blockUpdatingModel){
                        return;
                    }

                    if (productCB.currentIndex >= 0){
                        let productId = productCB.model.GetData("Id", productCB.currentIndex);
                        root.documentModel.SetData("ProductId", productId);

                        let licensesModel = productCB.model.GetData("Licenses", productCB.currentIndex);
                        if (!licensesModel){
                            licensesModel = productCB.model.AddTreeModel("Licenses", productCB.currentIndex);
                        }

                        let expiration = "";
                        if (root.documentModel.ContainsKey("Expiration")){
                            expiration = root.documentModel.GetData("Expiration");
                        }

                        if (root.documentModel.ContainsKey("LicenseId")){
                            let licenseId = root.documentModel.GetData("LicenseId");

                            let activeLicenses = softwareProductEditor.productModel.AddTreeModel("ActiveLicenses");
                            activeLicenses.InsertNewItem();
                            activeLicenses.SetData("Id", licenseId);
                            activeLicenses.SetData("Expiration", expiration);
                        }

                        softwareProductEditor.productLicensesModel = licensesModel;

                        softwareProductEditor.updateGui()
                    }
                }
            }

            SoftwareProductEditor {
                id: softwareProductEditor;

                width: bodyColumn.width;
                height: 400;

                Component.onCompleted: {
                    softwareProductEditor.productModel.dataChanged.connect(softwareProductEditor.onModelChanged);
                }

                function onModelChanged(){
                    console.log("onModelChanged", softwareProductEditor.productModel.toJSON());

                    root.updateModel()
                }
            }
        }
    }
}//Container


