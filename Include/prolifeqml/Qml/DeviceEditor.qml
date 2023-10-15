import QtQuick 2.12
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import imtauthgui 1.0
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

    onVisibleChanged: {
        if (visible){
            if (deviceEditorContainer.errorMessage !== ""){
                deviceEditorContainer.documentManager.showAlertMessage(deviceEditorContainer.errorMessage);
            }
        }
        else{
            deviceEditorContainer.documentManager.hideAlertMessage();
        }
    }

    onModelsIsLoadedChanged: {
        if (deviceEditorContainer.modelsIsLoaded){
            if (deviceEditorContainer.documentModel.ContainsKey("DeviceType")){
                let productId = deviceEditorContainer.documentModel.GetData("DeviceType");
                let productModel = productCB.model;
                for (let i = 0; i < productModel.GetItemsCount(); i++){
                    let id = productModel.GetData("Id", i);
                    if (id === productId){
                        let licensesModel = productModel.GetData("Licenses", i);
                        if (licensesModel){
                            configurationCB.model = licensesModel;
                        }

                        break;
                    }
                }
            }

            if (deviceEditorContainer.documentModel.ContainsKey("ProductionStatus")){
                let status = deviceEditorContainer.documentModel.GetData("ProductionStatus");
                if (status !== ""){
                    let statusModel = stateMachine.getAvailableModel(status);
                    statusCB.model = statusModel;
                }
            }

            if (!statusCB.model){
                statusCB.model = productionStatus.statusModel;
            }

            deviceEditorContainer.updateGui();
            undoRedoManager.registerModel(documentModel);
        }
    }

    onSaved: {
        if (deviceEditorContainer.documentModel.ContainsKey("ProductionStatus")){
            let status = deviceEditorContainer.documentModel.GetData("ProductionStatus");
            let statusModel = stateMachine.getAvailableModel(status);

            statusCB.model = statusModel;

            deviceEditorContainer.updateGui();
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

        fields: ["Id", "ProductName", "Description", "CategoryId", "Licenses"];

        Component.onCompleted: {
            let objectFilter =  productsList.filterModel.AddTreeModel("ObjectFilter")
            objectFilter.SetData("CategoryId", "Hardware");
        }

        onCollectionModelChanged: {
            console.log("productsList onCollectionModelChanged");
            if (productsList.collectionModel != null){
                productCB.model = productsList.collectionModel;
            }
        }

        onFailed: {
            if (deviceEditorContainer.documentManager){
                let message = qsTr("Error loading products. Please check Lisa connection.");
                deviceEditorContainer.documentManager.openErrorDialog(message);
                deviceEditorContainer.documentManager.showAlertMessage(message);
                deviceEditorContainer.errorMessage = message;
            }
        }
    }

    CollectionDataProvider {
        id: ordersList;

        commandId: "Orders";

        fields: ["Id", "OrderId", "Description"];

        onCollectionModelChanged: {
            if (ordersList.collectionModel != null){
                orderCB.model = ordersList.collectionModel;
            }
        }

        onFailed: {
            if (deviceEditorContainer.documentManager){
                let message = qsTr("Error loading orders");
                deviceEditorContainer.documentManager.openErrorDialog(message);
                deviceEditorContainer.documentManager.showAlertMessage(message);
                deviceEditorContainer.errorMessage = message;
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
        orderClearButton.enabled = false
        buttonContainer.enabled = false;

        configurationCB.changeable = false;
    }

    function updateGui(){
        console.log("UpdateGui start");

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
            let statusModel = stateMachine.getAvailableModel(status);
//            let statusModel = statusCB.model;
            if (statusModel){
                statusCB.model = statusModel;
                for (let i = 0; i < statusModel.GetItemsCount(); i++){
                    let id = statusModel.GetData("Id", i);
                    if (id === status){
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

        configurationCB.currentIndex = -1;
        if (deviceEditorContainer.documentModel.ContainsKey("ConfigurationType")){
            let productId = deviceEditorContainer.documentModel.GetData("ConfigurationType");
            let model = configurationCB.model;
            if (model){
                for (let i = 0; i < model.GetItemsCount(); i++){
                    let id = model.GetData("Id", i);
                    if (id === productId){
                        configurationCB.currentIndex = i;
                        break;
                    }
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

        console.log("UpdateGui end");
    }

    function updateModel(){
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

        let configurationExists = false;
        if (configurationCB.model){
            if (configurationCB.currentIndex >= 0){
                let configurationType = configurationCB.model.GetData("Id", configurationCB.currentIndex);
                deviceEditorContainer.documentModel.SetData("ConfigurationType", configurationType);

                configurationExists = true;
            }
        }

        if (!configurationExists){
            deviceEditorContainer.documentModel.SetData("ConfigurationType", "");
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

        if (statusCB.currentIndex >= 0){
            let selectedStatus = statusCB.model.GetData("Id", statusCB.currentIndex);
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

    CustomScrollbar {
        id: scrollbar;
        z: 100;

        anchors.left: flickable.right;
        anchors.leftMargin: 5;
        anchors.top: flickable.top;
        anchors.bottom: flickable.bottom;

        backgroundColor: Style.baseColor;

        secondSize: 10;
        targetItem: flickable;

        radius: 2;
    }

    Flickable {
        id: flickable;

        anchors.top: parent.top;
        anchors.bottom: parent.bottom;

        width: bodyColumn.width;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height;

        boundsBehavior: Flickable.StopAtBounds;

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

                            nameId: "ProductName";

                            Component.onCompleted: {
                                let ok = PermissionsController.checkPermission("ChangeSensor");
                                productCB.changeable = ok;
                            }

                            onCurrentIndexChanged: {
                                if (productCB.currentIndex >= 0){
                                    let model = productCB.model.GetData("Licenses", productCB.currentIndex);
                                    if (model){
                                        configurationCB.model = model;
                                    }
                                    else{
                                        configurationCB.model = 0;
                                    }

                                    configurationCB.currentIndex = -1;
                                }

                                deviceEditorContainer.updateModel();
                            }
                        }
                    }

                    Item {
                        width: parent.width;
                        height: titleConfigurationName.height + configurationCB.height + configurationCB.anchors.topMargin;

                        Text {
                            id: titleConfigurationName;
                            text: qsTr("Configuration Type");
                            color: Style.textColor;
                            font.family: Style.fontFamily;
                            font.pixelSize: Style.fontSize_common;
                        }

                        ComboBox {
                            id: configurationCB;

                            anchors.top: titleConfigurationName.bottom;
                            anchors.topMargin: deviceEditorContainer.heightBetweenTitleAndComp;

                            width: parent.width;
                            height: 23;

                            radius: deviceEditorContainer.radius;

                            nameId: "LicenseName";

                            Component.onCompleted: {
                                let ok = PermissionsController.checkPermission("ChangeSensor");
                                configurationCB.changeable = ok;
                            }

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

                        Component.onCompleted: {
                            let ok = PermissionsController.checkPermission("ChangeSensor");
                            if (!ok){
                                ok = PermissionsController.checkPermission("ChangeSensorDescription");
                            }

                            descriptionInput.readOnly = !ok;
                        }

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

                        Component.onCompleted: {
                            let ok = PermissionsController.checkPermission("ChangeSensor");
                            if (!ok){
                                ok = PermissionsController.checkPermission("ChangeSerialNumber");
                            }

                            serialNumberInput.readOnly = !ok;
                        }

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

                            Component.onCompleted: {
                                let ok = PermissionsController.checkPermission("ChangeSensor");
                                if (!ok){
                                    ok = PermissionsController.checkPermission("ChangeMacAddress");
                                }

                                macAddressInput.readOnly = !ok;
                            }

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

                                width: parent.width - iconStatus.width - 2*iconStatus.anchors.leftMargin - buttonContainer.width;
                                height: 23;

                                radius: deviceEditorContainer.radius;

                                function updateIcon(statusId){
                                    if (statusId === "None"){
                                        iconStatus.source = "../../../" + Style.getIconPath("Icons/StateUnknown", Icon.State.On, Icon.Mode.Normal);
                                    }
                                    else if (statusId === "Canceled"){
                                        iconStatus.source = "../../../" + Style.getIconPath("Icons/Cancel", Icon.State.On, Icon.Mode.Normal);
                                    }
                                    else if (statusId === "Accepted" || statusId === "InProgress"){
                                        iconStatus.source = "../../../" + Style.getIconPath("Icons/Timeline", Icon.State.On, Icon.Mode.Normal);
                                    }
                                    else if (statusId === "OnHold"){
                                        iconStatus.source = "../../../" + Style.getIconPath("Icons/Pause", Icon.State.On, Icon.Mode.Normal);
                                    }
                                    else{
                                        iconStatus.source = "../../../" + Style.getIconPath("Icons/StateOk", Icon.State.On, Icon.Mode.Normal);
                                    }
                                }

                                property bool blockingIndexChanged: false;

                                Component.onCompleted: {
                                    let ok = PermissionsController.checkPermission("ChangeSensor");
                                    if (!ok){
                                        ok = PermissionsController.checkPermission("ChangeProductionStatus");
                                    }

                                    statusCB.changeable = ok;
                                }

                                onCurrentIndexChanged: {
                                    deviceEditorContainer.updateModel();

                                    if (statusCB.currentIndex >= 0){
                                        if ( deviceEditorContainer.documentModel.ContainsKey("ProductionStatus")){
                                            let status = deviceEditorContainer.documentModel.GetData("ProductionStatus");
                                            statusCB.updateIcon(status);
                                        }
                                    }
                                    else{
                                        iconStatus.source = "";
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

                            BaseButton{
                                id: buttonContainer;

                                anchors.verticalCenter: parent.verticalCenter;
                                anchors.right: parent.right;

                                text: qsTr("Clear");

                                decorator: defaultButtonDecorator;

                                enabled: statusCB.changeable;

                                onClicked: {
                                    if(deviceEditorContainer.documentModel.ContainsKey("ProductionStatus")){
                                        if (statusCB.currentIndex != -1){
                                            statusCB.currentIndex = -1;
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
                    }

                    Item {
                        width: parent.width;
                        height: orderCB.height + titleOrderId.height + orderCB.anchors.topMargin;

                        Text {
                            id: titleOrderId;
                            text: qsTr("Order-ID");
                            color: Style.textColor;
                            font.family: Style.fontFamily;
                            font.pixelSize: Style.fontSize_common;
                        }

                        ComboBox {
                            id: orderCB;

                            anchors.left: parent.left;
                            anchors.top: titleOrderId.bottom;
                            anchors.topMargin: deviceEditorContainer.heightBetweenTitleAndComp;

                            width: parent.width - orderClearButton.width - 10;
                            height: 23;

                            radius: deviceEditorContainer.radius;

                            nameId: "OrderId";

                            Component.onCompleted: {
                                let ok = PermissionsController.checkPermission("ChangeSensor");
                                orderCB.changeable = ok;
                            }

                            onCurrentIndexChanged: {
                                deviceEditorContainer.updateModel();
                            }
                        }

                        BaseButton {
                            id: orderClearButton;

                            anchors.top: orderCB.top;
                            anchors.right: parent.right;

                            text: qsTr("Clear");

                            decorator: defaultButtonDecorator;

                            enabled: orderCB.changeable;

                            onClicked: {
                                if(deviceEditorContainer.documentModel.ContainsKey("OrderId")){
                                    if (orderCB.currentIndex != -1){
                                        orderCB.currentIndex = -1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    property int heightBetweenTitleAndComp: 5;
}


