import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import Acf 1.0

DocumentBase {
    id: installationEditorContainer;

    commandsDelegateSourceComp: installationEditorCommandsDelegate;

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
        let activeLicensesModel = documentModel.GetData("ActiveLicenses");
        if (!activeLicensesModel){
            activeLicensesModel = documentModel.AddTreeModel("ActiveLicenses");
        }

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

        instanceIdInput.text = documentModel.GetData("Id");

        let accountId = documentModel.GetData("AccountId");
        let productId = documentModel.GetData("ProductId");

        //        customerCB.currentText = "";
        let customerModel = customerCB.model;
        for (let i = 0; i < customerModel.GetItemsCount(); i++){
            let id = customerModel.GetData("Id", i);
            if (id === accountId){
                customerCB.currentIndex = i;
                break;
            }
        }

        //        productCB.currentText = "";
        let productModel = productCB.model;
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            let id = productModel.GetData("Id", i);
            if (id === productId){
                productCB.currentIndex = i;
                break;
            }
        }

        licensesTable.rowModel.clear();

        let activeLicensesModel = documentModel.GetData("ActiveLicenses");
        if (!activeLicensesModel){
            activeLicensesModel = documentModel.AddTreeModel("ActiveLicenses");
        }

        let licensesModel;
        for (let i = 0; i < licensesProvider.model.GetItemsCount(); i++){
            let id = licensesProvider.model.GetData("Id", i);
            if (id === productId){
                let productLicensesModel = licensesProvider.model.GetData("Licenses", i);
                licensesModel = productLicensesModel;
            }
        }

        if (licensesModel){
            console.log("licensesModel", licensesModel.toJSON());
            console.log("activeLicensesModel", activeLicensesModel.toJSON());
            for (let i = 0; i < licensesModel.GetItemsCount(); i++){
                let licenseId = licensesModel.GetData("Id", i);
                let licenseName = licensesModel.GetData("Name", i);

                let row = {"Id": licenseId, "Name": licenseName, "LicenseState": Qt.Unchecked, "ExpirationState": Qt.Unchecked, "Expiration": ""}

                for (let j = 0; j < activeLicensesModel.GetItemsCount(); j++){
                    let activeLicenseId = activeLicensesModel.GetData("Id", j);
                    let expiration = activeLicensesModel.GetData("Expiration", j);
                    if (licenseId == activeLicenseId){
                        row["LicenseState"] = Qt.Checked;

                        if (expiration == ""){
                            row["ExpirationState"] = Qt.Unchecked;
                        }
                        else{
                            row["ExpirationState"] = Qt.Checked;
                            row["Expiration"] = expiration;
                        }
                    }
                }

                console.log("row addRow", JSON.stringify(row));
                licensesTable.addRow(row);
            }
        }

        blockUpdatingModel = false;
        console.log("End updateGui");
    }

    function updateModel(){
        console.log("Begin updateModel");
        undoRedoManager.beginChanges();

        documentModel.SetData("Id", instanceIdInput.text)

        let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
        documentModel.SetData("ProductId", selectedProductId);

        let selectedAccountId = customerCB.model.GetData("Id", customerCB.currentIndex);
        documentModel.SetData("AccountId", selectedAccountId);

        let activeLicenses = documentModel.AddTreeModel("ActiveLicenses");

        for (let i = 0; i < licensesTable.rowModel.count; i++){
            let rowObj = licensesTable.rowModel.get(i);

            let licenseId = rowObj["Id"];
            let licenseName = rowObj["Name"];
            let expirationState  = rowObj["ExpirationState"];
            let expiration  = rowObj["Expiration"];
            let state = rowObj["LicenseState"];

            console.log("rowObj", JSON.stringify(rowObj));

            if (state == Qt.Checked){

                let index = activeLicenses.InsertNewItem();

                activeLicenses.SetData("Id", licenseId, index);
                activeLicenses.SetData("Name", licenseName, index);

                if (expirationState == Qt.Checked){
                    activeLicenses.SetData("Expiration", expiration, index);
                }
                else{
                    activeLicenses.SetData("Expiration", "", index);
                }
            }
        }

        undoRedoManager.endChanges();
        console.log("End updateModel");
    }

    function addDocument(document){
        let itemId = document["Id"];
        console.log("OrderEditor addDocument", itemId)

        let pageIndex = -1; //this.getDocumentIndexById(itemId);
        if (pageIndex < 0){
            var index = ordersData.InsertNewItem();
            console.log("OrderEditor addDocument index:", index)

            //TODO -> Uuid
            ordersData.SetData("Id", itemId, index);
            ordersData.SetData("Title", document["Name"], index);
            pageIndex = index;
        }

        tabPanel.selectedIndex = pageIndex;
    }

    function closeDocument(itemId){
         let index = this.getDocumentIndexById(itemId);;
         if (index >= 0){
             ordersData.RemoveItem(index);

             if (tabPanel.selectedIndex >= index && index > 0){
                 tabPanel.selectedIndex--;
             }
         }
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

        Text {
            id: titleComment;
            text: qsTr("Comment");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }


        CustomTextField {
            id: commentInput;

            width: parent.width;
            height: 30;

            placeHolderText: qsTr("Enter the comment");

            borderColor: Style.iconColorOnSelected;

            maximumLength: 255;

            onEditingFinished: {
            }
        }

//        Text {
//            id: titleProduct;

//            text: qsTr("Product");
//            color: Style.textColor;
//            font.family: Style.fontFamily;
//            font.pixelSize: Style.fontSize_common;
//        }

//        ComboBox {
//            id: productCB;

//            width: parent.width;
//            height: 23;

//            radius: 3;

//            onCurrentIndexChanged: {
//                console.log("InstallationEditor onCurrentIndexChanged",productCB.currentIndex);

//                if (!blockUpdatingModel){
//                    let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);

//                    //                        commandsDelegate.updateLicenses(selectedProductId);

//                    updateModel();

//                    updateGui();
//                }
//            }
//        }

//        Text {
//            id: titleDependency;

//            text: qsTr("Dependency");
//            color: Style.textColor;
//            font.family: Style.fontFamily;
//            font.pixelSize: Style.fontSize_common;
//        }

//        ComboBox {
//            id: dependencyComboBox;

//            width: parent.width;
//            height: 23;

//            radius: 3;

//            currentText: "RTVision.3d Sensor";

//            model: ListModel {
//                id: modelCategogy;
//                ListElement {
//                    Name: "RTVision.3d Sensor"
//                }
//            }

//            onCurrentIndexChanged: {

//            }
//        }

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
//                     modalDialogManager.openDialog(producteditor, {"productId": ""});
//                    modalDialogManager.openDialog(producteditor);
                    documentManager.addDocument({"Id": "newProduct", "Name": qsTr("New product"), "Source": "../../imtlicgui/InstallationEditor.qml", "CommandsId": "Installation"});

                }

//                Component{
//                    id: producteditor;
//                    InstallationEditor{
//                    }
//                }
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

//    MultiDocWorkspaceView {
//        id: productManager;
//        anchors.top: bodyColumn.bottom;
//        anchors.topMargin: bodyColumn.height + 5;
//        anchors.bottom: parent.bottom;
//        anchors.left: parent.left;
//        anchors.right: parent.right;
//        isCloseEnable: false;

//        Component.onCompleted: {
//            productManager.addDocument({"Id": "Sofware", "ProductId": "Sofware", "Name": "Sofware", "Source": "../../imtlicgui/SoftwareCollectionView.qml", "CommandsId": "Software"});
//            productManager.addDocument({"Id": "Hardware", "ProductId": "Hardware", "Name": "Hardware", "Source": "../../imtlicgui/HardwareCollectionView.qml", "CommandsId": "Hardware"});

//        }
//    }

//    TreeItemModel {
//        id: ordersData;
//    }

//    TabPanel {
//        id: tabPanel;

//        anchors.top: bodyColumn.bottom;
//        anchors.topMargin: 10
//        anchors.left: parent.left;
//        anchors.right: parent.right;

//        visible: true;
//        model: ordersData;

//        Component.onCompleted: {
//            addDocument({"Id":"Software", "Name":"Software"})
//            addDocument({"Id":"Hardware", "Name":"Hardware"})
//        }

//        onCloseItem: {
//            let item = ordersData.GetData("Item", index);
//            item.commandsDelegate.commandHandle("Close");
//        }

//        onRightClicked: {
//            if (tabPanel.selectedIndex < ordersData.GetItemsCount() - 1) {
//                tabPanel.selectedIndex++;
//                tabPanel.viewTabInListView(tabPanel.selectedIndex);
//            }
//        }

//        onLeftClicked: {
//            if (tabPanel.selectedIndex > 0) {
//                tabPanel.selectedIndex--;
//                tabPanel.viewTabInListView(tabPanel.selectedIndex);
//            }
//        }
//    }

    ListView {
        id: productsView;
        anchors.top: bodyColumn.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.right: parent.right;
        model: productsInfoModel;
//        anchors.top: tabPanel.bottom;
       // anchors.topMargin: 20;//thumbnailDecoratorContainer.mainMargin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 5;

        clip: true;
        boundsBehavior: Flickable.StopAtBounds;
        spacing: 10;
//        orientation: ListView.Horizontal;

        Component.onCompleted: {
            productsInfoModel.InsertNewItem();
            productsInfoModel.SetData("Name", "RTVision.3d")
            productsInfoModel.SetData("LinkId", "RTVision.3dSensor")
            productsInfoModel.SetData("Comment", "")
            productsInfoModel.SetData("Category", "Software")
            productsInfoModel.SetData("LicenseName", "Standard")
            productsInfoModel.SetData("ProductionStatus", "Ordered")
            productsInfoModel.InsertNewItem();
            productsInfoModel.SetData("Name", "RTVision.3d Sensor",1)
            productsInfoModel.SetData("LinkId", "RTVision.3d",1)
            productsInfoModel.SetData("Comment", "",1)
            productsInfoModel.SetData("Category", "Hardware",1)
            productsInfoModel.SetData("ProductionStatus", "Ordered",1)
            productsInfoModel.SetData("Manufacturer", "QUISS",1)
            productsInfoModel.SetData("MacAddress", "12:23:23:12:34",1)
            productsInfoModel.SetData("SerialNumber", "2345671",1)
        }

        delegate: Rectangle {
            height: 85;
            width: 500;
            radius: 10;
            Text {
                id: productName;
                anchors.left: parent.left;
                anchors.leftMargin: 5;
                anchors.top: parent.top;
                anchors.topMargin: 5;
                text: model.Name;
                color: Style.textColor;
                font.family: Style.fontFamilyBold;
                font.pixelSize: Style.fontSize_common;
                font.bold: true;
            }
            Text {
                id: productCategory;
                anchors.top: parent.top;
                anchors.topMargin: 5;
                anchors.left: productName.right;
                anchors.leftMargin: 5;
                text: "(" + model.Category + ")";
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                font.bold: true;
            }
            Text {
                id: productionStatus;
                anchors.right: parent.right;
                anchors.rightMargin: 5;
                anchors.top: parent.top;
                anchors.topMargin: 5;
                text: model.ProductionStatus;
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }
            Rectangle {
                anchors.top: productName.bottom;
                anchors.topMargin: 5;
                height: 1;
                width: parent.width;
                color: Style.textColor;
            }

            Text {
                id: licenseName;
                anchors.left: parent.left;
                anchors.leftMargin: 5;
                anchors.top: productName.bottom;
                anchors.topMargin: 10;
                text: qsTr("License: ") + model.LicenseName + qsTr("Data expired: 01.01.2024");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                visible: model.Category === "Software";
            }

            Text {
                id: linkedName;
                anchors.top: licenseName.bottom;
                anchors.topMargin: 5;
                anchors.left: parent.left;
                anchors.leftMargin: 5;
                text: qsTr("Associated:");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
//                font.underline: true;
            }
            Text {
                id: linked;
                anchors.top: licenseName.bottom;
                anchors.topMargin: 5;
                anchors.left: linkedName.right;
                anchors.leftMargin: 5;
                text:  model.LinkId;
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                font.underline: true;
            }

            Text {
                id: macAddress;
                anchors.top: productName.bottom;
                anchors.topMargin: 10;
                anchors.left: parent.left;
                anchors.leftMargin: 5;
                text: qsTr("Mac address: ") + model.MacAddress;
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                visible: model.Category === "Hardware";
            }

            Text {
                id: serialNumber;
                anchors.top: macAddress.top;
                anchors.right: parent.right;
                anchors.rightMargin: 5;
                text: qsTr("Serial number: ") + model.SerialNumber;
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                visible: macAddress.visible;
            }
        }


    }

    GqlModel {
        id: productsInfoModel;

        function updateModel() {
            //console.log( "gqlModelBaseContainer updateModel", gqlModelBaseContainer.gqlModelItemsInfo, gqlModelBaseContainer.itemId);
            var query = Gql.GqlRequest("query", "OrderedProductsInfo");

            var viewParams = Gql.GqlObject("viewParams");
            viewParams.InsertField("Offset", offset);
            viewParams.InsertField("Count", count);
            viewParams.InsertField("FilterModel");
            var jsonString = modelFilter.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")
            viewParams.InsertField("FilterModel", jsonString);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertFieldObject(viewParams);

//            if (itemId){
//                inputParams.InsertField("Id", itemId);
//            }

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("Id");
            queryFields.InsertField("");


            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("gqlModelBaseContainer query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, itemsInfoModel);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (itemsInfoModel.ContainsKey("errors")){
                    return;
                }

                if (itemsInfoModel.ContainsKey("data")){
                    dataModelLocal = itemsInfoModel.GetData("data");
                    if (dataModelLocal.ContainsKey(gqlModelBaseContainer.gqlModelItemsInfo)){
                        dataModelLocal = dataModelLocal.GetData(gqlModelBaseContainer.gqlModelItemsInfo);
                        if (dataModelLocal.ContainsKey("items")){
                            gqlModelBaseContainer.items = dataModelLocal.GetData("items");
                            gqlModelBaseContainer.table.selectedIndex = -1;
                        }

                        if (dataModelLocal.ContainsKey("notification")){
                            dataModelLocal = dataModelLocal.GetData("notification");
                            if (dataModelLocal.ContainsKey("PagesCount")){
                                dataModelLocal = dataModelLocal.GetData("PagesCount");

                                pagination.pagesSize = dataModelLocal;
                            }
                        }
                    }
                }
            }
        }
    }

}//Container


