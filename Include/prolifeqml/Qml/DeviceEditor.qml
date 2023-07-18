import QtQuick 2.12
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import Acf 1.0

DocumentBase {
    id: deviceEditorContainer;

    property TreeItemModel accountsModel: TreeItemModel {}
    property TreeItemModel productsModel: TreeItemModel {}

    property alias orderComboBoxEnabled: orderCB.enabled;
    property alias deviceTypeComboBoxEnabled: productCB.enabled;

    property bool modelsIsLoaded: ordersList.completed && productsList.completed && deviceEditorContainer.modelIsReady;

    property int radius: 3;
    property int spacing: 20;

    Component.onCompleted: {
        ordersList.updateModel();
        productsList.updateModel();
    }

    onModelsIsLoadedChanged: {
        console.log("onModelsIsLoadedChanged", deviceEditorContainer.modelsIsLoaded);
        if (deviceEditorContainer.modelsIsLoaded){
            deviceEditorContainer.updateGui();
            undoRedoManager.registerModel(documentModel);
        }
    }

    function documentCanBeSaved(){
        let ok = deviceEditorContainer.macAddressIsValid();
        if (!ok){
            deviceEditorContainer.documentManager.openErrorDialog("MAC-Address invalid");
        }

        return ok;
    }

    CollectionDataProvider {
        id: productsList;

        commandId: "Products";

        fields: ["Id", "Name", "Description", "CategoryId"];

        property TreeItemModel filteringModel: TreeItemModel {}

        onCollectionModelChanged: {
            console.log("productsList onCollectionModelChanged");
            if (productsList.collectionModel != null){
                productsList.filteringProductCollection();
                productCB.model = filteringModel;
            }
        }

        function filteringProductCollection(){
            let productsModel = productsList.collectionModel;
            if (productsModel){
                for (let i = 0; i < productsModel.GetItemsCount(); i++){
                    let categoryId = productsModel.GetData("CategoryId", i);
                    if (categoryId === "Hardware"){
                        let index = filteringModel.InsertNewItem();
                        filteringModel.CopyItemDataFromModel(index, productsList.collectionModel, i);
                    }
                }
            }
        }
    }

    CollectionDataProvider {
        id: ordersList;

        commandId: "Orders";

        fields: ["Id", "OrderId", "Description"];

        onCollectionModelChanged: {
            console.log("ordersList onCollectionModelChanged");

            if (ordersList.collectionModel != null){
                orderCB.model = ordersList.collectionModel;

                if (deviceEditorContainer.documentModel.ContainsKey("ProductionStatus")){
                    let status = deviceEditorContainer.documentModel.GetData("ProductionStatus");
                    if (status !== ""){
                        let statusModel = stateMachine.getAvailableModel(status);
                        statusCB.model = statusModel;

                        return;
                    }
                }

                statusCB.model = productionStatus.statusModel;
            }
        }
    }

    DeviceProductionStatus {
        id: productionStatus;
    }

    StateMachine {
        id: stateMachine;

        Component.onCompleted: {
            stateMachine.registerModel(productionStatus.statusModel);

            stateMachine.addState("None", ["None", "Accepted"]);
            stateMachine.addState("Accepted", ["Accepted", "InProgress", "Canceled", "OnHold"]);
            stateMachine.addState("InProgress", ["InProgress", "Finished"]);
            stateMachine.addState("Canceled", ["Canceled", "None"]);
            stateMachine.addState("OnHold", ["OnHold", "Accepted", "InProgress"]);
            stateMachine.addState("Finished", ["Finished"]);
        }
    }

    UndoRedoManager {
        id: undoRedoManager;

        documentBase: deviceEditorContainer;

        onModelStateChanged: {
            deviceEditorContainer.updateGui();
        }
    }

    MouseArea {
        anchors.fill: parent;

        onClicked: {
            deviceEditorContainer.forceActiveFocus();
        }
    }

    function onCommandsModelLoaded(){
        let saveExists = deviceEditorContainer.commandsProvider.commandExists("Save");
        if (!saveExists){
            deviceEditorContainer.blockEditing();
        }

        let isOrderEdited = deviceEditorContainer.commandsProvider.commandExists("OrderEdit");
        if (!isOrderEdited){
            orderCB.changeable = false;
        }
    }

    function blockEditing(){
        descriptionInput.readOnly = true;
        serialNumberInput.readOnly = true;
        macAddressInput.readOnly = true;
        statusCB.changeable = false;
        productCB.changeable = false;
        orderCB.changeable = false;
    }

    function updateGui(){
        console.log("DeviceEditor begin updateGui");
        deviceEditorContainer.blockUpdatingModel = true;

        descriptionInput.text = "";
        if (deviceEditorContainer.documentModel.ContainsKey("Description")){
            descriptionInput.text = deviceEditorContainer.documentModel.GetData("Description");
        }

        serialNumberInput.text = "";
        if (deviceEditorContainer.documentModel.ContainsKey("SerialNumber")){
            serialNumberInput.text = deviceEditorContainer.documentModel.GetData("SerialNumber");
        }

        macAddressInput.text = "";
        if (deviceEditorContainer.documentModel.ContainsKey("MacAddress")){
            macAddressInput.text = deviceEditorContainer.documentModel.GetData("MacAddress");
        }

        statusCB.currentIndex = -1;
        if (deviceEditorContainer.documentModel.ContainsKey("ProductionStatus")){
            let status = deviceEditorContainer.documentModel.GetData("ProductionStatus");
            console.log("status", status);
            //            let statusModel = productionStatus.getAvailableModel(status);
            let statusModel = stateMachine.getAvailableModel(status);
            if (statusModel){
                console.log("statusModel", statusModel.toJSON());
                for (let i = 0; i < statusModel.GetItemsCount(); i++){
                    let id = statusModel.GetData("Id", i);
                    if (id === status){
                        console.log("statusCB.currentIndex", i);
                        statusCB.updateIcon(status);
                        statusCB.currentIndex = i;
                        break;
                    }
                }
            }
        }

        productCB.currentIndex = -1;
        if (deviceEditorContainer.documentModel.ContainsKey("DeviceType")){
            let productId = deviceEditorContainer.documentModel.GetData("DeviceType");
            let productModel = productCB.model;
            for (let i = 0; i < productModel.GetItemsCount(); i++){
                let id = productModel.GetData("Id", i);
                if (id === productId){
                    productCB.currentIndex = i;
                    break;
                }
            }
        }

        orderCB.currentIndex = -1;
        if (deviceEditorContainer.documentModel.ContainsKey("OrderId")){
            let orderId = deviceEditorContainer.documentModel.GetData("OrderId");
            let ordersModel = orderCB.model;
            if (ordersModel){
                for (let i = 0; i < ordersModel.GetItemsCount(); i++){
                    let id = ordersModel.GetData("Id", i);
                    if (id === orderId){
                        orderCB.currentIndex = i;
                        break;
                    }
                }
            }
        }

        deviceEditorContainer.blockUpdatingModel = false;
        console.log("DeviceEditor end updateGui");
    }

    function updateModel(){
        console.log("DeviceEditor begin updateModel");
        if (deviceEditorContainer.blockUpdatingModel){
            return;
        }

        undoRedoManager.beginChanges();

        if (productCB.currentIndex >= 0){
            let selectedProductId = productCB.model.GetData("Id", productCB.currentIndex);
            deviceEditorContainer.documentModel.SetData("DeviceType", selectedProductId);
        }
        else{
            deviceEditorContainer.documentModel.SetData("DeviceType", "");
        }

        if (orderCB.currentIndex >= 0){
            let selectedOrderId = orderCB.model.GetData("Id", orderCB.currentIndex);
            deviceEditorContainer.documentModel.SetData("OrderId", selectedOrderId);
        }
        else{
            deviceEditorContainer.documentModel.SetData("OrderId", "");
        }

        let description = descriptionInput.text;
        deviceEditorContainer.documentModel.SetData("Description", description);

        let serialNumber = serialNumberInput.text;
        deviceEditorContainer.documentModel.SetData("SerialNumber", serialNumber);

        if (macAddressInput.acceptableInput){
            let macAddress = macAddressInput.text;
            deviceEditorContainer.documentModel.SetData("MacAddress", macAddress);
        }

        console.log("statusCB.currentIndex", statusCB.currentIndex);
        if (statusCB.currentIndex >= 0){
            let selectedStatus = statusCB.model.GetData("Id", statusCB.currentIndex);
            console.log("selectedStatus", selectedStatus);
            deviceEditorContainer.documentModel.SetData("ProductionStatus", selectedStatus);
        }
        else{
            deviceEditorContainer.documentModel.SetData("ProductionStatus", "");
        }

        undoRedoManager.endChanges();
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    function macAddressIsValid(){
        if (macAddressInput.text === ""){
            return true;
        }

        if (macAddressInput.regExp){
            return macAddressInput.regExp.test(macAddressInput.text);
        }

        return false;
    }

    Column {
        id: bodyColumn;

        width: 500;
        height: childrenRect.height;

        spacing: 7;

        Text {
            id: titleDeviceInformationBlock;
            text: qsTr("Device information");
            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle { ////////////////////
            id: deviceInformationBlockBorders;

            width: parent.width;
            height: deviceInformationBlock.height + 25;

            color: "transparent";

            border.width: 1;
            border.color: Style.borderColor;

            radius: deviceEditorContainer.radius;

            Column {
                id: deviceInformationBlock;

                anchors.centerIn: deviceInformationBlockBorders;

                width: parent.width - 20;

                spacing: deviceEditorContainer.spacing;

                Item {
                    width: parent.width;
                    height: titleDeviceName.height + productCB.height + productCB.anchors.topMargin;

                    Text {
                        id: titleDeviceName;
                        text: qsTr("Device Type");
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }

                    ComboBox {
                        id: productCB;

                        anchors.top: titleDeviceName.bottom;
                        anchors.topMargin: deviceEditorContainer.heightBetweenTitleAndComp;

                        width: parent.width;
                        height: 23;

                        radius: deviceEditorContainer.radius;

                        model: deviceEditorContainer.productsModel;

                        onCurrentIndexChanged: {
                            deviceEditorContainer.updateModel();
                        }
                    }
                }

                TextFieldWithTitle {
                    id: descriptionInput;

                    width: parent.width;

                    title: qsTr("Description");
                    placeHolderText: qsTr("Enter description");

                    onEditingFinished: {
                        let oldText = deviceEditorContainer.documentModel.GetData("Description");
                        if (oldText && oldText !== descriptionInput.text || !oldText && descriptionInput.text !== ""){
                            deviceEditorContainer.updateModel();
                        }
                    }

                    KeyNavigation.tab: serialNumberInput;
                }

                TextFieldWithTitle {
                    id: serialNumberInput;

                    width: parent.width;

                    title: qsTr("Serial Number");
                    placeHolderText: qsTr("Enter serial number");

                    onEditingFinished: {
                        console.log("onEditingFinished", serialNumberInput.text);
                        let oldText = deviceEditorContainer.documentModel.GetData("SerialNumber");
                        if (!oldText && serialNumberInput.text !== "" || oldText && oldText !== serialNumberInput.text){
                            deviceEditorContainer.updateModel();
                        }
                    }

                    KeyNavigation.tab: macAddressInput;
                }

                Item {
                    width: parent.width;

                    height: macAddresInvalidText.visible ?
                                macAddressInput.height + macAddresInvalidText.height + macAddresInvalidText.anchors.topMargin :
                                    macAddressInput.height ;

                    TextFieldWithTitle {
                        id: macAddressInput;

                        width: parent.width;

                        title: qsTr("MAC-Address");
                        placeHolderText: qsTr("Enter MAC-Address");

                        maximumLength: 17;

                        onEditingFinished: {
                            let oldText = deviceEditorContainer.documentModel.GetData("MacAddress");
                            if (oldText && oldText !== macAddressInput.text || !oldText && macAddressInput.text !== ""){
                                deviceEditorContainer.updateModel();
                            }
                        }

                        property var regExp: new RegExp(macAddressRegExp.regExp)
                        onTextChanged: {
                            if (macAddressInput.text === ""){
                                macAddressInput.borderColor = Style.iconColorOnSelected;
                                macAddresInvalidText.visible = false;
                            }
                            else if (regExp){
                                let isValid = regExp.test(macAddressInput.text);
                                if (isValid){
                                    macAddressInput.borderColor = Style.iconColorOnSelected;
                                }
                                else{
                                    macAddressInput.borderColor = Style.errorTextColor;
                                }

                                macAddresInvalidText.visible = !isValid;
                            }
                        }

                        KeyNavigation.tab: macAddressInput;
                    }

                    Text {
                        id: macAddresInvalidText;

                        anchors.top: macAddressInput.bottom;
                        anchors.topMargin: 5;

                        text: qsTr("MAC-Address invalid");
                        color: Style.errorTextColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;

                        visible: false;
                    }

                    RegExpValidator {
                        id: macAddressRegExp;

                        regExp: /^([0-9A-Fa-f]{2}[:]){5}([0-9A-Fa-f]{2})$/;
                    }
                }
            }
        }

        Text {
            id: titleAdditonalInfoId;
            text: qsTr("Additional information");
            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;
        }

        Rectangle {
            id: additionalInfoBorders;

            width: parent.width;
            height: additionalInfoBlock.height + 25;

            color: "transparent";

            border.width: 1;
            border.color: Style.borderColor;

            radius: deviceEditorContainer.radius;

            Column {
                id: additionalInfoBlock;

                anchors.horizontalCenter: additionalInfoBorders.horizontalCenter;
                anchors.verticalCenter: additionalInfoBorders.verticalCenter;

                width: parent.width - 15;

                spacing: deviceEditorContainer.spacing;

                Item {
                    width: parent.width;
                    height: titleStatusId.height + comboBoxItem.height + comboBoxItem.anchors.topMargin;

                    Text {
                        id: titleStatusId;
                        text: qsTr("Production Status");
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }

                    Item {
                        id: comboBoxItem;

                        anchors.top: titleStatusId.bottom;
                        anchors.topMargin: deviceEditorContainer.heightBetweenTitleAndComp;

                        width: parent.width;
                        height: 23;

                        ComboBox {
                            id: statusCB;

                            anchors.left: parent.left;

                            width: parent.width - iconStatus.width - 10;
                            height: 23;

                            radius: deviceEditorContainer.radius;

                            function updateIcon(statusId){
                                if (statusId === "None"){
                                    iconStatus.source = "qrc:/Icons/Light/StateUnknown_On_Active";
                                }
                                else if (statusId === "Canceled"){
                                    iconStatus.source = "qrc:/Icons/Light/Cancel_On_Active";
                                }
                                else if (statusId === "Accepted" || statusId === "InProgress"){
                                    iconStatus.source = "qrc:/Icons/Light/Timeline_On_Active";
                                }
                                else if (statusId === "OnHold"){
                                    iconStatus.source = "qrc:/Icons/Light/Pause_On_Active";
                                }
                                else{
                                    iconStatus.source = "qrc:/Icons/Light/StateOk_On_Active";
                                }
                            }

                            property bool blockingIndexChanged: false;

                            onCurrentIndexChanged: {
                                console.log("statusCB onCurrentIndexChanged", statusCB.currentIndex);

                                if (statusCB.blockingIndexChanged){
                                    return;
                                }

                                if (statusCB.currentIndex >= 0){
                                    deviceEditorContainer.updateModel();

                                    let status = deviceEditorContainer.documentModel.GetData("ProductionStatus");
                                    //                                let statusModel = productionStatus.getAvailableModel(status);

                                    let statusModel = stateMachine.getAvailableModel(status);

                                    statusCB.model = statusModel;
                                    statusCB.updateIcon(status);

                                    statusCB.blockingIndexChanged = true;
                                    statusCB.currentIndex = 0;
                                    statusCB.blockingIndexChanged = false;
                                }
                                else{
                                    statusCB.model = productionStatus.statusModel;
                                }
                            }
                        }

                        Image {
                            id: iconStatus;

                            anchors.verticalCenter: parent.verticalCenter;
                            anchors.left: statusCB.right;
                            anchors.leftMargin: 10;

                            width: 20;
                            height: width;

                            sourceSize.height: height;
                            sourceSize.width: width;
                        }
                    }
                }

                Item {
                    width: parent.width;
                    height: orderCB.height + titleOrderId.height + orderCB.anchors.topMargin;

                    Text {
                        id: titleOrderId;
                        text: qsTr("Order ID");
                        color: Style.textColor;
                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                    }

                    ComboBox {
                        id: orderCB;

                        anchors.top: titleOrderId.bottom;
                        anchors.topMargin: deviceEditorContainer.heightBetweenTitleAndComp;

                        width: parent.width;
                        height: 23;

                        radius: deviceEditorContainer.radius;

                        nameId: "OrderId";

                        onCurrentIndexChanged: {
                            deviceEditorContainer.updateModel();
                        }
                    }
                }
            }
        }
    }

    property int heightBetweenTitleAndComp: 5;
}


