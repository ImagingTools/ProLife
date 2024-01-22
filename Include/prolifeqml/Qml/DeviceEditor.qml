import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import prolifeqml 1.0

DocumentData {
    id: deviceEditorContainer;

    property TreeItemModel accountsModel: TreeItemModel {}
    property TreeItemModel productsModel: TreeItemModel {}

    property alias orderComboBoxEnabled: orderCB.enabled;
    property alias deviceTypeComboBoxEnabled: productCB.enabled;

    //    documentCompleted: ordersList.completed && productsList.completed;
    documentCompleted: CachedOrderCollection.completed && CachedProductCollection.completed;

    property int radius: 3;
    property int spacing: Style.size_mainMargin;

    property int comboBoxHeight: 27;

    commandsDelegate: DocumentWorkspaceCommandsDelegateBase {
        documentPtr: deviceEditorContainer;

        onCommandActivated: {
            if (commandId == "Bind"){
                if (deviceEditorContainer.isDirty){
                    modalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Please save the document first.")});

                    return;
                }

                let hardwareUuid = deviceEditorContainer.documentId;
                let macAddress = "";

                if (deviceEditorContainer.documentModel.ContainsKey("MacAddress")){
                    macAddress = deviceEditorContainer.documentModel.GetData("MacAddress");
                }

                if (hardwareUuid === "" || macAddress === ""){
                    modalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Please enter the MAC-Address then save the document.")});

                    return;
                }

                modalDialogManager.openDialog(productPairEditorDialog, {"hardwareId": hardwareUuid});
            }
        }
    }

    Component {
        id: saveDialogComp;

        MessageDialog {
            width: 300;

            title: qsTr("Save document");

            buttonsModel: ListModel{
                ListElement{Id: Enums.ButtonType.Yes; Name:qsTr("OK"); Enabled: true}
            }
        }
    }

    Component {
        id: productPairEditorDialog;

        HardwareProductBindingDialog {
        }
    }

    Component.onCompleted: {
        CachedOrderCollection.updateModel();
        CachedProductCollection.updateModel();
    }

    function beginDocumentModelChanged(){
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
    }

    onSaved: {
        if (deviceEditorContainer.documentModel.ContainsKey("ProductionStatus")){

            let status = deviceEditorContainer.documentModel.GetData("ProductionStatus");
            let statusModel = stateMachine.getAvailableModel(status);

            if (statusModel){
                statusCB.model = statusModel;

                doUpdateGui();
            }
        }
    }

    function documentCanBeSaved(){
        let ok = deviceEditorContainer.macAddressIsValid();
        if (!ok && deviceEditorContainer.documentManagerPtr){
            deviceEditorContainer.documentManagerPtr.openErrorDialog(qsTr("MAC-Address invalid"));
        }

        if (ok){
            if (productCB.currentIndex < 0){
                deviceEditorContainer.documentManagerPtr.openErrorDialog(qsTr("Please select a device type"));

                ok = false;
            }
        }

        if (ok){
            if (configurationCB.currentIndex < 0){
                deviceEditorContainer.documentManagerPtr.openErrorDialog(qsTr("Please select a device configuration"));

                ok = false;
            }
        }

        return ok;
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
        if (deviceEditorContainer.documentModel.ContainsKey("Description")){
            descriptionInput.text = deviceEditorContainer.documentModel.GetData("Description");
        }
        else{
            descriptionInput.text = "";
        }

        if (deviceEditorContainer.documentModel.ContainsKey("SerialNumber")){
            serialNumberInput.text = deviceEditorContainer.documentModel.GetData("SerialNumber");
        }
        else{
            serialNumberInput.text = "";
        }

        if (deviceEditorContainer.documentModel.ContainsKey("MacAddress")){
            macAddressInput.text = deviceEditorContainer.documentModel.GetData("MacAddress");
        }
        else{
            macAddressInput.text = "";
        }

        let statusFound = false;
        if (deviceEditorContainer.documentModel.ContainsKey("ProductionStatus")){
            let status = deviceEditorContainer.documentModel.GetData("ProductionStatus");
            //            let statusModel = stateMachine.getAvailableModel(status);
            let statusModel = statusCB.model;
            if (statusModel){
                //                statusCB.model = statusModel;
                for (let i = 0; i < statusModel.GetItemsCount(); i++){
                    let id = statusModel.GetData("Id", i);
                    if (id === status){
                        statusCB.updateIcon(status);
                        statusCB.currentIndex = i;

                        statusFound = true;
                        break;
                    }
                }
            }
        }

        if (!statusFound){
            statusCB.currentIndex = -1;
        }

        let deviceTypeFound = false;
        if (deviceEditorContainer.documentModel.ContainsKey("DeviceType")){
            let productId = deviceEditorContainer.documentModel.GetData("DeviceType");
            let productModel = productCB.model;
            for (let i = 0; i < productModel.GetItemsCount(); i++){
                let id = productModel.GetData("Id", i);
                if (id === productId){
                    productCB.currentIndex = i;

                    deviceTypeFound = true;
                    break;
                }
            }
        }

        if (!deviceTypeFound){
            productCB.currentIndex = -1;
        }

        let configurationTypeFound = false;
        if (deviceEditorContainer.documentModel.ContainsKey("LicenseName")){
            let productId = deviceEditorContainer.documentModel.GetData("LicenseName");
            let model = configurationCB.model;
            if (model){
                for (let i = 0; i < model.GetItemsCount(); i++){
                    let id = model.GetData("Id", i);
                    if (id === productId){
                        configurationCB.currentIndex = i;

                        configurationTypeFound = true;
                        break;
                    }
                }
            }
        }

        if (!configurationTypeFound){
            configurationCB.currentIndex = -1;
        }

        let orderIdFound = false;

        if (deviceEditorContainer.documentModel.ContainsKey("OrderId")){
            let orderId = deviceEditorContainer.documentModel.GetData("OrderId");
            let ordersModel = orderCB.model;
            if (ordersModel){
                for (let i = 0; i < ordersModel.GetItemsCount(); i++){
                    let id = ordersModel.GetData("Id", i);
                    if (id === orderId){
                        orderCB.currentIndex = i;
                        orderIdFound = true;
                        break;
                    }
                }
            }
        }

        if (!orderIdFound){
            orderCB.currentIndex = -1;
        }
    }

    function updateModel(){
        if (productCB.currentIndex >= 0 && productCB.model){
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
                deviceEditorContainer.documentModel.SetData("LicenseName", configurationType);

                configurationExists = true;
            }
        }

        if (!configurationExists){
            deviceEditorContainer.documentModel.SetData("LicenseName", "");
        }

        let canChangeOrder = PermissionsController.checkPermission("ChangeOrder");
        if (canChangeOrder){
            if (orderCB.currentIndex >= 0){
                let selectedOrderId = orderCB.model.GetData("Id", orderCB.currentIndex);
                deviceEditorContainer.documentModel.SetData("OrderId", selectedOrderId);
            }
            else{
                deviceEditorContainer.documentModel.SetData("OrderId", "");
            }
        }

        deviceEditorContainer.documentModel.SetData("Description", descriptionInput.text);
        deviceEditorContainer.documentModel.SetData("SerialNumber", serialNumberInput.text);
        deviceEditorContainer.documentModel.SetData("MacAddress", macAddressInput.text);

        if (statusCB.currentIndex >= 0 && statusCB.model){
            let selectedStatus = statusCB.model.GetData("Id", statusCB.currentIndex);
            deviceEditorContainer.documentModel.SetData("ProductionStatus", selectedStatus);
        }
        else{
            deviceEditorContainer.documentModel.SetData("ProductionStatus", "");
        }
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

        anchors.left: flickable.right;
        anchors.leftMargin: Style.size_mainMargin;
        anchors.top: flickable.top;
        anchors.bottom: flickable.bottom;

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

            spacing: Style.size_mainMargin;

            Text {
                id: titleDeviceInformationBlock;
                text: qsTr("Device information");
                color: Style.textColor;
                font.family: Style.fontFamilyBold;
                font.pixelSize: Style.fontSize_common;
            }

            Rectangle {
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
                            height: deviceEditorContainer.comboBoxHeight;

                            radius: deviceEditorContainer.radius;

                            model: CachedProductCollection.hardwareProductsModel;

                            nameId: "ProductName";

                            Component.onCompleted: {
                                let ok = PermissionsController.checkPermission("ChangeSensor");
                                productCB.changeable = ok;
                            }

                            onCurrentIndexChanged: {
                                let ok = false;
                                if (productCB.currentIndex >= 0){
                                    let model = productCB.model.GetData("Licenses", productCB.currentIndex);
                                    if (model){
                                        configurationCB.model = model;

                                        ok = true;
                                    }
                                }

                                if (!ok){
                                    configurationCB.model = 0;
                                }

                                deviceEditorContainer.doUpdateModel();
                            }

                            onModelChanged: {
                                deviceEditorContainer.doUpdateGui();
                            }
                        }
                    }

                    Item {
                        width: parent.width;
                        height: titleConfigurationName.height + configurationCB.height + configurationCB.anchors.topMargin;

                        Text {
                            id: titleConfigurationName;
                            text: qsTr("Hardware Configuration");
                            color: Style.textColor;
                            font.family: Style.fontFamily;
                            font.pixelSize: Style.fontSize_common;
                        }

                        ComboBox {
                            id: configurationCB;

                            anchors.top: titleConfigurationName.bottom;
                            anchors.topMargin: deviceEditorContainer.heightBetweenTitleAndComp;

                            width: parent.width;
                            height: deviceEditorContainer.comboBoxHeight;

                            radius: deviceEditorContainer.radius;

                            nameId: "LicenseName";

                            Component.onCompleted: {
                                let ok = PermissionsController.checkPermission("ChangeSensor");
                                configurationCB.changeable = ok;
                            }

                            onCurrentIndexChanged: {
                                deviceEditorContainer.doUpdateModel();
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
                            deviceEditorContainer.doUpdateModel();
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
                            deviceEditorContainer.doUpdateModel();
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
                                deviceEditorContainer.doUpdateModel();
                            }

                            property int lastLength: 0;
                            property bool blockSignal: false;;
                            property var regExp: new RegExp(macAddressRegExp.regularExpression)
                            onTextChanged: {
                                if(blockSignal){
                                    return;
                                }

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
                            anchors.topMargin: Style.size_mainMargin;

                            text: qsTr("MAC-Address invalid");
                            color: Style.errorTextColor;
                            font.family: Style.fontFamily;
                            font.pixelSize: Style.fontSize_common;

                            visible: false;
                        }

                        RegularExpressionValidator {
                            id: macAddressRegExp;

                            regularExpression: /^([0-9A-Fa-f]{2}[:]){5}([0-9A-Fa-f]{2})$/;
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
                            height: 30;

                            ComboBox {
                                id: statusCB;

                                anchors.left: parent.left;

                                width: parent.width - iconStatus.width - 2*iconStatus.anchors.leftMargin - buttonContainer.width;
                                height: deviceEditorContainer.comboBoxHeight;

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
                                    deviceEditorContainer.doUpdateModel();
                                    //                                    deviceEditorContainer.updateModel();

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
                                anchors.leftMargin: Style.size_mainMargin;

                                width: 25;
                                height: width;

                                sourceSize.height: height;
                                sourceSize.width: width;
                            }

                            Button{
                                id: buttonContainer;

                                anchors.verticalCenter: parent.verticalCenter;
                                anchors.right: parent.right;

                                text: qsTr("Clear");

                                enabled: statusCB.changeable;

                                onClicked: {
                                    if(deviceEditorContainer.documentModel.ContainsKey("ProductionStatus")){
                                        if (statusCB.currentIndex != -1){
                                            statusCB.currentIndex = -1;
                                        }
                                    }
                                }

                                decorator: ButtonDecorator{
                                    width: 70;
                                    height: statusCB.height;
                                    radius: deviceEditorContainer.radius;
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

                        FilterableComboBox {
                            id: orderCB;

                            anchors.left: parent.left;
                            anchors.top: titleOrderId.bottom;
                            anchors.topMargin: deviceEditorContainer.heightBetweenTitleAndComp;

                            width: parent.width - orderClearButton.width - 10;
                            height: deviceEditorContainer.comboBoxHeight;

                            radius: deviceEditorContainer.radius;

                            nameId: "OrderId";

                            model: CachedOrderCollection.collectionModel;

                            Component.onCompleted: {
                                let canChangeSensor = PermissionsController.checkPermission("ChangeSensor");
                                let canChangeOrder = PermissionsController.checkPermission("ChangeOrder");

                                orderCB.changeable = canChangeSensor && canChangeOrder;
                            }

                            onCurrentIndexChanged: {
                                deviceEditorContainer.doUpdateModel();
                            }

                            onModelChanged: {
                                deviceEditorContainer.doUpdateGui();
                            }
                        }

                        Button {
                            id: orderClearButton;

                            anchors.top: orderCB.top;
                            anchors.right: parent.right;

                            text: qsTr("Clear");

                            enabled: orderCB.changeable;

                            onClicked: {
                                if(deviceEditorContainer.documentModel.ContainsKey("OrderId")){
                                    if (orderCB.currentIndex != -1){
                                        orderCB.currentIndex = -1;
                                    }
                                }
                            }

                            decorator: ButtonDecorator{
                                width: 70;
                                height: orderCB.height;
                                radius: deviceEditorContainer.radius;
                            }
                        }
                    }
                }
            }
        }
    }

    property int heightBetweenTitleAndComp: Style.size_mainMargin;
}


