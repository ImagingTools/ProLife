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

        if (documentModel.ContainsKey("Id")){
            instanceIdInput.text = documentModel.GetData("Id");
        }

        if (documentModel.ContainsKey("Comment")){
            commentInput.text = documentModel.GetData("Comment");
        }

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

        documentModel.SetData("Comment", commentInput.text);

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


