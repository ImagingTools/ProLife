import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import prolifeqml 1.0

ViewBase {
    id: deviceEditorContainer;

    anchors.fill: parent;

    property TreeItemModel accountsModel: TreeItemModel {}
    property TreeItemModel productsModel: TreeItemModel {}

    property alias orderComboBoxEnabled: orderCB.enabled;
    property alias deviceTypeComboBoxEnabled: productCB.enabled;

    property int radius: 3;
    property int spacing: Style.size_mainMargin;

    property int comboBoxHeight: 27;

    commandsDelegateComp: Component {ViewCommandsDelegateBase {
            view: deviceEditorContainer;
            onCommandActivated: {
                if (commandId == "Bind"){
                    let hardwareUuid = "";
                    if (deviceEditorContainer.model.ContainsKey("Id")){
                        hardwareUuid = deviceEditorContainer.model.GetData("Id")
                    }

                    if (hardwareUuid === ""){
                        modalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Please save the document first"), "title": qsTr("Warning message")});

                        return;
                    }

                    let macAddress = "";

                    if (deviceEditorContainer.model.ContainsKey("MacAddress")){
                        macAddress = deviceEditorContainer.model.GetData("MacAddress");
                    }

                    if (!macAddressValidator.isValid(macAddress)){
                        modalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Please enter a valid MAC-Address")});

                        return;
                    }

                    let title = qsTr("Add license to sensor '%1'");
                    title = title.replace("%1", macAddress);

                    modalDialogManager.openDialog(productPairEditorDialog, {"hardwareId": hardwareUuid, "title": title});
                }
            }
        }
    }

    MacAddressValidator {
        id: macAddressValidator;
    }

    Component.onCompleted: {
        CachedOrderCollection.updateModel();
        CachedProductCollection.updateModel();
    }

    onModelChanged: {
        checkPermissions();
    }

    function checkPermissions(){
        console.log("checkPermissions");
        let deviceId = "";
        if (model.ContainsKey("Id")){
            deviceId = model.GetData("Id");
        }

        let canAddSensor = PermissionsController.checkPermission("AddSensor");
        if (deviceId === "" && canAddSensor){
            descriptionInput.readOnly = false;
            serialNumberInput.readOnly = false;
            macAddressInput.readOnly = false;
            projectInput.readOnly = false;
            statusCB.changeable = true;
            productCB.changeable = true;
            orderCB.changeable = true;
            configurationCB.changeable = true;
        }
        else{
            let canChangeDescription = PermissionsController.checkPermission("ChangeDescriptionForSensor");
            descriptionInput.readOnly = !canChangeDescription;

            let canChangeSerialNumber = PermissionsController.checkPermission("ChangeSerialNumberForSensor");
            serialNumberInput.readOnly = !canChangeSerialNumber;

            let canChangeMacAddress = PermissionsController.checkPermission("ChangeMacAddress");
            macAddressInput.readOnly = !canChangeMacAddress;

            let canChangeOrder = PermissionsController.checkPermission("ChangeOrderForSensor");
            orderCB.changeable = canChangeOrder;

            let canChangeProductionStatus = PermissionsController.checkPermission("ChangeProductionStatus");
            statusCB.changeable = canChangeProductionStatus;

            let canChangeProject = PermissionsController.checkPermission("ChangeProjectForSensor");
            projectInput.readOnly = !canChangeProject;

            let canChangeConfiguration = PermissionsController.checkPermission("ChangeHardwareConfiguration");
            configurationCB.changeable = canChangeConfiguration;

            let canChangeDevice = PermissionsController.checkPermission("ChangeDeviceType");
            productCB.changeable = canChangeDevice;

            let ok =
                canChangeDescription ||
                canChangeSerialNumber ||
                canChangeMacAddress ||
                canChangeOrder||
                canChangeProductionStatus ||
                canChangeProject ||
                canChangeConfiguration ||
                canChangeDevice;

            if (commandsController){
                commandsController.setCommandVisible("Undo", ok);
                commandsController.setCommandVisible("Redo", ok);
                commandsController.setCommandVisible("Save", ok);
            }
        }
    }

    Component {
        id: saveDialogComp;

        ErrorDialog {
            width: 300;

            title: qsTr("Warning message");
        }
    }

    Component {
        id: productPairEditorDialog;

        HardwareProductBindingDialog {
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

    MouseArea {
        anchors.fill: parent;

        onClicked: {
            deviceEditorContainer.forceActiveFocus();
        }
    }

    function setReadOnly(readOnly){
        descriptionInput.readOnly = readOnly;
        serialNumberInput.readOnly = readOnly;
        macAddressInput.readOnly = readOnly;
        statusCB.changeable = !readOnly;
        productCB.changeable = !readOnly;
        orderCB.changeable = !readOnly;
        configurationCB.changeable = !readOnly;
    }

    function updateGui(){
        console.log("DeviceEditor updateGui");
        if (deviceEditorContainer.model.ContainsKey("Description")){
            descriptionInput.text = deviceEditorContainer.model.GetData("Description");
        }
        else{
            descriptionInput.text = "";
        }

        if (deviceEditorContainer.model.ContainsKey("SerialNumber")){
            serialNumberInput.text = deviceEditorContainer.model.GetData("SerialNumber");
        }
        else{
            serialNumberInput.text = "";
        }

        if (deviceEditorContainer.model.ContainsKey("MacAddress")){
            macAddressInput.text = deviceEditorContainer.model.GetData("MacAddress");
        }
        else{
            macAddressInput.text = "";
        }

        if (deviceEditorContainer.model.ContainsKey("Project")){
            projectInput.text = deviceEditorContainer.model.GetData("Project");
        }
        else{
            projectInput.text = "";
        }

        let statusFound = false;
        if (deviceEditorContainer.model.ContainsKey("ProductionStatus")){
            let status = deviceEditorContainer.model.GetData("ProductionStatus");
            let statusModel = statusCB.model;
            if (statusModel){
                for (let i = 0; i < statusModel.GetItemsCount(); i++){
                    let id = statusModel.GetData("Id", i);
                    if (id === status){
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
        if (deviceEditorContainer.model.ContainsKey("DeviceType")){
            let productId = deviceEditorContainer.model.GetData("DeviceType");
            let productModel = productCB.model;
            if (productModel){
                for (let i = 0; i < productModel.GetItemsCount(); i++){
                    let id = productModel.GetData("Id", i);
                    if (id === productId){
                        productCB.currentIndex = i;

                        deviceTypeFound = true;
                        break;
                    }
                }
            }
        }

        if (!deviceTypeFound){
            productCB.currentIndex = -1;
        }

        let configurationTypeFound = false;
        if (deviceEditorContainer.model.ContainsKey("LicenseName")){
            let productId = deviceEditorContainer.model.GetData("LicenseName");
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

        if (deviceEditorContainer.model.ContainsKey("OrderId")){
            let orderId = deviceEditorContainer.model.GetData("OrderId");
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
            deviceEditorContainer.model.SetData("DeviceType", selectedProductId);
        }
        else{
            deviceEditorContainer.model.SetData("DeviceType", "");
        }

        let configurationExists = false;
        if (configurationCB.model){
            if (configurationCB.currentIndex >= 0){
                let configurationType = configurationCB.model.GetData("Id", configurationCB.currentIndex);
                deviceEditorContainer.model.SetData("LicenseName", configurationType);

                configurationExists = true;
            }
        }

        if (!configurationExists){
            deviceEditorContainer.model.SetData("LicenseName", "");
        }

        let canChangeOrder = PermissionsController.checkPermission("ChangeOrderForSensor");
        if (canChangeOrder){
            if (orderCB.currentIndex >= 0){
                let selectedOrderId = orderCB.model.GetData("Id", orderCB.currentIndex);
                deviceEditorContainer.model.SetData("OrderId", selectedOrderId);
            }
            else{
                deviceEditorContainer.model.SetData("OrderId", "");
            }
        }

        deviceEditorContainer.model.SetData("Description", descriptionInput.text);
        deviceEditorContainer.model.SetData("SerialNumber", serialNumberInput.text);
        deviceEditorContainer.model.SetData("MacAddress", macAddressInput.text);
        deviceEditorContainer.model.SetData("Project", projectInput.text);

        if (statusCB.currentIndex >= 0 && statusCB.model){
            let selectedStatus = statusCB.model.GetData("Id", statusCB.currentIndex);
            deviceEditorContainer.model.SetData("ProductionStatus", selectedStatus);
        }
        else{
            deviceEditorContainer.model.SetData("ProductionStatus", "");
        }
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

        z: parent.z + 1;

        anchors.right: parent.right;
        anchors.top: flickable.top;
        anchors.bottom: flickable.bottom;

        secondSize: 10;
        targetItem: flickable;

        radius: 2;
    }

    CustomScrollbar{
        id: scrollHoriz;

        z: parent.z + 1;

        anchors.left: flickable.left;
        anchors.right: flickable.right;
        anchors.bottom: flickable.bottom;

        secondSize: 10;

        vertical: false;
        targetItem: flickable;
    }

    Flickable {
        id: flickable;

        anchors.left: parent.left;
        anchors.leftMargin: Style.size_largeMargin;

        anchors.top: parent.top;
        anchors.topMargin: Style.size_largeMargin;

        anchors.bottom: parent.bottom;
        anchors.bottomMargin: Style.size_largeMargin;

        anchors.right: scrollbar.left;
        anchors.rightMargin: Style.size_largeMargin;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height + 2 * Style.size_largeMargin;

        boundsBehavior: Flickable.StopAtBounds;

        clip: true;

        Column {
            id: bodyColumn;

            width: 700;

            spacing: Style.size_largeMargin;

            GroupHeaderView {
                width: parent.width;

                title: qsTr("Device Information");
                groupView: deviceInformationGroup;
            }

            GroupElementView {
                id: deviceInformationGroup;

                width: parent.width;

                ComboBoxElementView {
                    id: productCB;

                    name: qsTr("Device Type");

                    model: CachedProductCollection.hardwareProductsModel;

                    nameId: "ProductName";

                    KeyNavigation.tab: configurationCB;
                    KeyNavigation.backtab: orderCB;

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

                ComboBoxElementView {
                    id: configurationCB;

                    name: qsTr("Hardware Configuration");
                    nameId: "LicenseName";

                    KeyNavigation.tab: descriptionInput;
                    KeyNavigation.backtab: productCB;

                    onCurrentIndexChanged: {
                        deviceEditorContainer.doUpdateModel();
                    }
                }

                TextInputElementView {
                    id: descriptionInput;

                    name: qsTr("Description");
                    placeHolderText: qsTr("Enter description");

                    onEditingFinished: {
                        deviceEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: serialNumberInput;
                    KeyNavigation.backtab: configurationCB;
                }

                TextInputElementView {
                    id: serialNumberInput;

                    name: qsTr("Serial Number");

                    placeHolderText: qsTr("Enter serial number");

                    onEditingFinished: {
                        deviceEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: macAddressInput;
                    KeyNavigation.backtab: descriptionInput;
                }

                MacAddressElementView {
                    id: macAddressInput;

                    onEditingFinished: {
                        deviceEditorContainer.doUpdateModel();
                    }

                    KeyNavigation.tab: orderCB;
                    KeyNavigation.backtab: serialNumberInput;
                }
            }

            GroupHeaderView {
                id: additionalHeaderView;

                width: parent.width;

                groupView: additionalInformationGroup;
                title: qsTr("Additional Information");
            }

            GroupElementView {
                id: additionalInformationGroup;

                width: parent.width;

                FilterableComboBoxElementView {
                    id: orderCB;

                    name: qsTr("Order-ID");

                    nameId: "OrderId";

                    filteringFields: ["OrderId", "OrderCustomer"];

                    model: CachedOrderCollection.collectionModel;

                    KeyNavigation.tab: statusCB;
                    KeyNavigation.backtab: macAddressInput;

                    delegate: Component {
                        FilterableComboBoxDelegate {
                            width: comboBoxRef ? comboBoxRef.width : 0;
                            comboBoxRef: orderCB.cbRef;

                            description: qsTr("Customer") + ": " + model.OrderCustomer;
                        }
                    }

                    onCurrentIndexChanged: {
                        deviceEditorContainer.doUpdateModel();
                    }

                    onModelChanged: {
                        deviceEditorContainer.doUpdateGui();
                    }
                }

                ClearableComboBoxElementView {
                    id: statusCB;

                    name: qsTr("Production Status");
                    model: productionStatus.statusModel;

                    property bool blockingIndexChanged: false;

                    KeyNavigation.tab: projectInput;
                    KeyNavigation.backtab: orderCB;

                    onCurrentIndexChanged: {
                        deviceEditorContainer.doUpdateModel();

                        if (statusCB.currentIndex >= 0){
                            if ( deviceEditorContainer.model.ContainsKey("ProductionStatus")){
                                let status = deviceEditorContainer.model.GetData("ProductionStatus");
                            }
                        }
                        else{
                            statusCB.model = productionStatus.statusModel;
                        }
                    }
                }

                TextInputElementView {
                    id: projectInput;

                    name: qsTr("Project");
                    placeHolderText: qsTr("Enter the project");

                    readOnly: deviceEditorContainer.readOnly;

                    KeyNavigation.tab: productCB;
                    KeyNavigation.backtab: statusCB;

                    onEditingFinished: {
                        deviceEditorContainer.doUpdateModel();
                    }
                }
            }
        }
    }
}


