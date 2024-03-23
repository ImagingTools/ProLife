import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import prolifeqml 1.0

ViewBase {
    id: root;

    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}

    property string alertMessage: "";
    property int comboBoxHeight: 27;

    Component.onCompleted: {
        CachedProductCollection.updateModel();
        CachedOrderCollection.updateModel();
    }

    onReadOnlyChanged: {
        console.log("SE onReadOnlyChanged", root.readOnly);

        projectInput.readOnly = root.readOnly;
    }

    onVisibleChanged: {
        if (visible){
            checkInUse()
        }
        else{
            Events.sendEvent("SetAlertPanel", undefined);
        }
    }

    onModelChanged: {
        console.log("SE onModelChanged", root.model.toJSON());

        checkInUse();

        softwareProductEditor.model = root.model;
    }

    Component {
        id: alertComp;
        AlertMessage {
            message: qsTr("The product cannot be edited as it is in use.");
        }
    }

    function checkInUse(){
        let inUse = root.model.GetData("InUse");
        if (inUse){
            root.readOnly = true;
            Events.sendEvent("SetAlertPanel", alertComp);
        }
        else{
            root.readOnly = false;
            Events.sendEvent("SetAlertPanel", undefined);
        }
    }

    function setReadOnly(readOnly){
        projectInput.readOnly = readOnly;

        ordersCB.changeable = !readOnly;
        productCB.changeable = !readOnly;

        softwareProductEditor.setReadOnly(readOnly);
    }

    function updateGui(){
        console.log("Software updateGui start");

        if (root.model.ContainsKey("Project")){
            projectInput.text = root.model.GetData("Project");
        }
        else{
            projectInput.text = "";
        }

        let orderFound = false;
        if (root.model.ContainsKey("OrderUuid")){
            let orderUuid = root.model.GetData("OrderUuid");
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
        if (root.model.ContainsKey("ProductId")){
            let productId = root.model.GetData("ProductId");

            if (productCB.model){
                for (let i = 0; i < productCB.model.GetItemsCount(); i++){
                    let id = productCB.model.GetData("Id", i);

                    if (id == productId){
                        productCB.currentIndex = i;
                        productFound = true;
                        break;
                    }
                }
            }
        }

        if (!productFound){
            productCB.currentIndex = -1;
        }

        softwareProductEditor.doUpdateGui();
    }

    function updateModel(){
        console.log("updateModel");

        root.model.SetData("Project", projectInput.text);

        let canChangeOrder = PermissionsController.checkPermission("ChangeOrder");
        if (canChangeOrder){
            if (ordersCB.model){
                if (ordersCB.currentIndex >= 0){
                    let orderUuid = ordersCB.model.GetData("Id", ordersCB.currentIndex);
                    root.model.SetData("OrderUuid", orderUuid);
                }
                else{
                    root.model.SetData("OrderUuid", "");
                }
            }
        }

        if (productCB.currentIndex >= 0 && productCB.model){
            let selectedId = productCB.model.GetData("Id", productCB.currentIndex);
            root.model.SetData("ProductId", selectedId);
        }
        else{
            root.model.SetData("ProductId", "");
        }

        softwareProductEditor.doUpdateModel();
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

            readOnly: root.readOnly;

            Component.onCompleted: {
                console.log("project onCompleted", root.readOnly);

                if (!root.readOnly){
                    let ok = PermissionsController.checkPermission("ChangeLicense");
                    console.log("ok", ok);
                    projectInput.readOnly = !ok;
                }
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

            FilterableComboBox {
                id: ordersCB;

                anchors.left: parent.left;

                width: parent.width - buttonContainer.width - 10;
                height: root.comboBoxHeight;

                radius: 3;

                nameId: "OrderId";

                model: CachedOrderCollection.collectionModel;

                changeable: !root.readOnly;

                Component.onCompleted: {
                    console.log("ordersCB onCompleted");
                    console.log("root.readOnly", root.readOnly);
                    if (!root.readOnly){
                        let ok = PermissionsController.checkPermission("ChangeLicense");
                        console.log("ok", ok);

                        ordersCB.changeable = ok;
                    }
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
                    if(root.model.ContainsKey("OrderUuid")){
                        let orderUuid = root.model.GetData("OrderUuid")
                        if (ordersCB.currentIndex != -1){
                            ordersCB.currentIndex = -1;
                        }
                    }
                }

                decorator: ButtonDecorator{
                    width: 70;
                    height: ordersCB.height;

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
            height: root.comboBoxHeight;

            radius: 3;

            nameId: "ProductName";

            model: CachedProductCollection.softwareProductsModel;

            changeable: !root.readOnly

            Component.onCompleted: {
                if (!root.readOnly){
                    let ok = PermissionsController.checkPermission("ChangeLicense");

                    productCB.changeable = ok;
                }
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

        model: root.model;

        readOnly: root.readOnly;

        function onModelChanged(){
            root.doUpdateModel();
        }
    }
    //    }
}//Container


