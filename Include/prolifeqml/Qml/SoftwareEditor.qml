import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import prolifeqml 1.0
import lisaqml 1.0

DocumentData {
    id: root;

    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}

    documentCompleted: CachedProductCollection.completed && CachedOrderCollection.completed;

    property string alertMessage: "";

    Component.onCompleted: {
        CachedProductCollection.updateModel();
        CachedOrderCollection.updateModel();
    }

    onVisibleChanged: {
        if (visible){
            checkInIse();
        }
        else{
            if (root.documentManagerPtr){
                root.documentManagerPtr.setAlertPanel(null);
            }
        }
    }

    Component {
        id: alertComp;
        AlertMessage {
            message: qsTr("The product cannot be edited as it is in use.");
        }
    }

    function checkInIse(){
        if (root.documentModel.ContainsKey("InUse")){
            let inUse = root.documentModel.GetData("InUse")
            if (inUse){
                root.documentManagerPtr.setAlertPanel(alertComp);
                root.blockEditing();

                return;
            }
        }

        root.documentManagerPtr.setAlertPanel(null);
    }

    function documentCanBeSaved(){
        console.log("documentCanBeSaved", root.documentModel.toJSON());

        let ok = false;

        if (root.documentModel.ContainsKey("ProductId")){
            let productId = root.documentModel.GetData("ProductId");
            if (String(productId) !== ""){
                ok = true;
            }
        }

        if (!ok){
            root.documentManagerPtr.openErrorDialog(qsTr("Please select a product"));
        }

        if (ok){
            ok = false;

            if (root.documentModel.ContainsKey("LicenseUuid")){
                let licenseUuid = root.documentModel.GetData("LicenseUuid");
                if (String(licenseUuid) !== ""){
                    ok = true;
                }
            }

            if (!ok){
                root.documentManagerPtr.openErrorDialog(qsTr("Please select a license"));
            }
        }

        return ok;
    }

    function beginDocumentModelChanged(){
        checkInIse();

        softwareProductEditor.productModel = root.documentModel;
    }

//    CollectionDataProvider {
//        id: ordersList;

//        commandId: "Orders";

//        fields: ["Id", "OrderId", "Description"];
//        sortByField: "OrderId";

//        onCollectionModelChanged: {
//            console.log("ordersList onCollectionModelChanged");

//            if (ordersList.collectionModel != null){
//                ordersCB.model = ordersList.collectionModel;
//            }
//        }
//    }

//    CollectionDataProvider {
//        id: productsList;

//        fields: ["Id", "ProductName", "CategoryId", "Licenses"];
//        commandId: "Products";
//        sortByField: "ProductName";

//        Component.onCompleted: {
//            let objectFilter =  productsList.filterModel.AddTreeModel("ObjectFilter")
//            objectFilter.SetData("CategoryId", "Software");
//        }

//        onCollectionModelChanged: {
//            if (productsList.collectionModel != null){
//                root.productsModel = productsList.collectionModel;

//                productCB.model = root.productsModel;
//            }
//        }

//        onFailed: {
//            if (root.documentManagerPtr){
//                let message = qsTr("Error loading products. Please check Lisa connection.");

//                root.documentManagerPtr.openErrorDialog(message);
//                root.documentManagerPtr.showAlertMessage(message);

//                root.errorMessage = message;
//            }
//        }
//    }

    function blockEditing(){
        projectInput.readOnly = true;

        ordersCB.enabled = false;
        productCB.enabled = false;

        buttonContainer.enabled = false;

        softwareProductEditor.readOnly = true;
    }

    function updateGui(){
        console.log("Software updateGui start");

        if (root.documentModel.ContainsKey("Project")){
            projectInput.text = root.documentModel.GetData("Project");
        }
        else{
            projectInput.text = "";
        }

        let orderFound = false;
        if (root.documentModel.ContainsKey("OrderUuid")){
            let orderUuid = root.documentModel.GetData("OrderUuid");
            if (ordersCB.model){
                for (let i = 0; i < ordersCB.model.GetItemsCount(); i++){
                    let id = ordersCB.model.GetData("Id", i);
                    if (id === orderUuid){
                        ordersCB.currentIndex = i;

                        orderFound = true;
                        break;
                    }
                }
            }
        }

        if (!orderFound){
            ordersCB.currentIndex = -1;
        }

        let productFound = false;
        if (root.documentModel.ContainsKey("ProductId")){
            let productId = root.documentModel.GetData("ProductId");

            console.log("productId", productId);

            if (productCB.model){
                for (let i = 0; i < productCB.model.GetItemsCount(); i++){
                    let id = productCB.model.GetData("Id", i);
                    console.log("id", id);

                    if (id == productId){
                        console.log("==", id);
                        productCB.currentIndex = i;
                        productFound = true;
                        break;
                    }
                }
            }
        }

        console.log("productFound", productFound);

        if (!productFound){
            productCB.currentIndex = -1;
        }

        softwareProductEditor.updateGui();
    }

    function updateModel(){
        console.log("updateModel");

        root.documentModel.SetData("Project", projectInput.text);

        let canChangeOrder = PermissionsController.checkPermission("ChangeOrder");
        if (canChangeOrder){
            if (ordersCB.model){
                if (ordersCB.currentIndex >= 0){
                    let orderUuid = ordersCB.model.GetData("Id", ordersCB.currentIndex);
                    root.documentModel.SetData("OrderUuid", orderUuid);
                }
                else{
                    root.documentModel.SetData("OrderUuid", "");
                }
            }
        }

        if (productCB.currentIndex >= 0 && productCB.model){
            let selectedId = productCB.model.GetData("Id", productCB.currentIndex);
            root.documentModel.SetData("ProductId", selectedId);
        }
        else{
            root.documentModel.SetData("ProductId", "");
        }

        softwareProductEditor.updateModel();
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

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Column {
        id: bodyColumn;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.leftMargin: 20;

        width: 500;

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
                root.doUpdateModel();
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

                model: CachedOrderCollection.collectionModel;

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ChangeLicense");

                    ordersCB.changeable = ok;
                }

                onCurrentIndexChanged: {
                    root.doUpdateModel();
                }

                onModelChanged: {
                    root.doUpdateGui();
                }
            }

            Button{
                id: buttonContainer;

                anchors.right: parent.right;

                text: qsTr("Clear");

                enabled: ordersCB.changeable && ordersCB.currentIndex >= 0;

                onClicked: {
                    if(root.documentModel.ContainsKey("OrderUuid")){
                        let orderUuid = root.documentModel.GetData("OrderUuid")
                        if (ordersCB.currentIndex != -1){
                            ordersCB.currentIndex = -1;
                        }
                    }
                }

                decorator: ButtonDecorator{
                    width: 70;
                    height: 23;
                    radius: 3;
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

            nameId: "ProductName";

            model: CachedProductCollection.softwareProductsModel;

            Component.onCompleted: {
                let ok = PermissionsController.checkPermission("ChangeLicense");

                productCB.changeable = ok;
            }

            onModelChanged: {
                root.doUpdateGui();
            }

            onCurrentIndexChanged: {
                console.log("productCB onCurrentIndexChanged", productCB.currentIndex);

                if (productCB.currentIndex >= 0){
                    let licensesModel = productCB.model.GetData("Licenses", productCB.currentIndex);
                    if (!licensesModel){
                        licensesModel = productCB.model.AddTreeModel("Licenses", productCB.currentIndex);
                    }

                    softwareProductEditor.productLicensesModel = licensesModel;
                }
                else{
                    softwareProductEditor.productLicensesModel = 0;
                }

                root.doUpdateModel();
            }
        }
    }

    SoftwareProductEditor {
        id: softwareProductEditor;

        anchors.top: bodyColumn.bottom;
        anchors.left: parent.left;
        anchors.leftMargin: 20;
        anchors.topMargin: bodyColumn.spacing;
        anchors.bottom: root.bottom;

        width: bodyColumn.width;

        function onModelChanged(){
            root.doUpdateModel();
        }
    }
    //    }
}//Container


