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

    Component.onCompleted: {
        CachedOrderCollection.updateModel();
        CachedProductCollection.updateModel();
    }

    onModelChanged: {
        checkPermissions();
    }

    function checkPermissions(){
        let deviceId = "";
        if (model.containsKey("Id")){
            deviceId = model.getData("Id");
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

    function checkFinishedStatus(){
        if (macAddressInput.acceptableInput &&
            serialNumberInput.acceptableInput &&
            !ModalDialogManager.dialogIsOpened(confirmSetFinishedStatusDialogComp) &&
            PermissionsController.checkPermission("ChangeProductionStatus")){
            let status = model.getData("ProductionStatus")

            let macAddress = model.getData("MacAddress")
            let serialNumber = model.getData("SerialNumber")

            if (macAddress !== "" && serialNumber !== "" && status !== "Finished"){
                ModalDialogManager.openDialog(confirmSetFinishedStatusDialogComp);
            }
        }
    }

    Component {
        id: confirmSetFinishedStatusDialogComp;
        MessageDialog {
            title: qsTr("Confirm status");
            message: qsTr("Do you want to set the production state of the sensor to Finished ?");

            onFinished: {
                if (buttonId == Enums.yes){
                    let finishedStatusIndex = productionStatus.getStatusIndex("Finished");
                    statusCB.currentIndex = finishedStatusIndex;
                }
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
        if (deviceEditorContainer.model.containsKey("Description")){
            descriptionInput.text = deviceEditorContainer.model.getData("Description");
        }
        else{
            descriptionInput.text = "";
        }

        if (deviceEditorContainer.model.containsKey("SerialNumber")){
            serialNumberInput.text = deviceEditorContainer.model.getData("SerialNumber");
        }
        else{
            serialNumberInput.text = "";
        }

        if (deviceEditorContainer.model.containsKey("MacAddress")){
            macAddressInput.text = deviceEditorContainer.model.getData("MacAddress");
        }
        else{
            macAddressInput.text = "";
        }

        if (deviceEditorContainer.model.containsKey("Project")){
            projectInput.text = deviceEditorContainer.model.getData("Project");
        }
        else{
            projectInput.text = "";
        }

        let statusFound = false;
        if (deviceEditorContainer.model.containsKey("ProductionStatus")){
            let status = deviceEditorContainer.model.getData("ProductionStatus");
            let statusModel = statusCB.model;
            if (statusModel){
                for (let i = 0; i < statusModel.getItemsCount(); i++){
                    let id = statusModel.getData("Id", i);
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
        if (deviceEditorContainer.model.containsKey("DeviceType")){
            let productId = deviceEditorContainer.model.getData("DeviceType");
            let productModel = productCB.model;
            if (productModel){
                for (let i = 0; i < productModel.getItemsCount(); i++){
                    let id = productModel.getData("Id", i);
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
        if (deviceEditorContainer.model.containsKey("LicenseName")){
            let productId = deviceEditorContainer.model.getData("LicenseName");
            let model = configurationCB.model;
            if (model){
                for (let i = 0; i < model.getItemsCount(); i++){
                    let id = model.getData("Id", i);
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

        if (deviceEditorContainer.model.containsKey("OrderId")){
            let orderId = deviceEditorContainer.model.getData("OrderId");
            let ordersModel = orderCB.model;
            if (ordersModel){
                for (let i = 0; i < ordersModel.getItemsCount(); i++){
                    let id = ordersModel.getData("Id", i);
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
            let selectedProductId = productCB.model.getData("Id", productCB.currentIndex);
            deviceEditorContainer.model.setData("DeviceType", selectedProductId);
        }
        else{
            deviceEditorContainer.model.setData("DeviceType", "");
        }

        let configurationExists = false;
        if (configurationCB.model){
            if (configurationCB.currentIndex >= 0){
                let configurationType = configurationCB.model.getData("Id", configurationCB.currentIndex);
                deviceEditorContainer.model.setData("LicenseName", configurationType);

                configurationExists = true;
            }
        }

        if (!configurationExists){
            deviceEditorContainer.model.setData("LicenseName", "");
        }

        let canChangeOrder = PermissionsController.checkPermission("ChangeOrderForSensor");
        if (canChangeOrder){
            if (orderCB.currentIndex >= 0){
                let selectedOrderId = orderCB.model.getData("Id", orderCB.currentIndex);
                deviceEditorContainer.model.setData("OrderId", selectedOrderId);
            }
            else{
                deviceEditorContainer.model.setData("OrderId", "");
            }
        }

        deviceEditorContainer.model.setData("Description", descriptionInput.text);
        deviceEditorContainer.model.setData("SerialNumber", serialNumberInput.text);
        deviceEditorContainer.model.setData("MacAddress", macAddressInput.text);
        deviceEditorContainer.model.setData("Project", projectInput.text);

        if (statusCB.currentIndex >= 0 && statusCB.model){
            let selectedStatus = statusCB.model.getData("Id", statusCB.currentIndex);
            deviceEditorContainer.model.setData("ProductionStatus", selectedStatus);
        }
        else{
            deviceEditorContainer.model.setData("ProductionStatus", "");
        }
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
                    KeyNavigation.backtab: projectInput;

                    onCurrentIndexChanged: {
                        let ok = false;
                        if (productCB.currentIndex >= 0){
                            let model = productCB.model.getData("Licenses", productCB.currentIndex);
                            if (model){
                                configurationCB.model = model;

                                ok = true;
                            }
                        }

                        if (!ok){
                            configurationCB.model = 0;
                        }

                        configurationCB.currentIndex = -1;

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
                        if (configurationCB.currentIndex >= 0){
                            deviceEditorContainer.doUpdateModel();
                        }
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
                        deviceEditorContainer.checkFinishedStatus();
                    }

                    KeyNavigation.tab: macAddressInput;
                    KeyNavigation.backtab: descriptionInput;
                }

                MacAddressElementView {
                    id: macAddressInput;

                    onEditingFinished: {
                        deviceEditorContainer.doUpdateModel();
                        deviceEditorContainer.checkFinishedStatus();
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
                            if ( deviceEditorContainer.model.containsKey("ProductionStatus")){
                                let status = deviceEditorContainer.model.getData("ProductionStatus");
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


