import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0

DocumentBase {
    id: root;

    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}

    property bool documentReady: modelIsReady && productsList.completed && ordersList.completed;

    property string alertMessage: "";

    Component.onCompleted: {
        productsList.updateModel();
        ordersList.updateModel();
    }

    onVisibleChanged: {
        if (visible){
            if (root.alertMessage !== ""){
                root.documentManager.showAlertMessage(root.alertMessage);
            }
        }
        else{
            root.documentManager.hideAlertMessage();
        }
    }

    onDocumentReadyChanged: {
        if (root.documentReady){
            if (root.documentModel.ContainsKey("InUse")){
                let inUse = root.documentModel.GetData("InUse")
                if (inUse){
                    root.alertMessage = qsTr("The product cannot be edited as it is in use.");
                    root.documentManager.showAlertMessage(root.alertMessage);

                    root.blockEditing();
                }
            }

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

        Component.onCompleted: {
            let objectFilter =  productsList.filterModel.AddTreeModel("ObjectFilter")
            objectFilter.SetData("CategoryId", "Software");
        }

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

    function blockEditing(){
        projectInput.readOnly = true;

        ordersCB.enabled = false;
        productCB.enabled = false;

        buttonContainer.enabled = false;

        softwareProductEditor.readOnly = true;
    }

    function documentCanBeSaved(){
        return true;
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

            if (ordersCB.model){
                for (let i = 0; i < ordersCB.model.GetItemsCount(); i++){
                    let id = ordersCB.model.GetData("Id", i);
                    if (id == orderUuid){
                        ordersCB.currentIndex = i;
                        break;
                    }
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

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ChangeLicense");

                    projectInput.readOnly = !ok;
                }

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

                    Component.onCompleted: {
                        let ok = PermissionsController.checkPermission("ChangeLicense");

                        ordersCB.changeable = ok;
                    }

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

                    enabled: ordersCB.changeable;

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
                }
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

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ChangeLicense");

                    productCB.changeable = ok;
                }

                onCurrentIndexChanged: {
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
                    root.updateModel()
                }
            }
        }
    }
}//Container


