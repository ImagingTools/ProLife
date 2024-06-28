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

    property var productLicensesModel: TreeItemModel{}

    property string alertMessage: "";
    property int comboBoxHeight: 27;

    Component.onCompleted: {
        CachedProductCollection.updateModel();
        CachedOrderCollection.updateModel();
    }

    onVisibleChanged: {
        if (visible){
            checkInUse()
        }
        else{
            let parameters = {"Id": "SoftwareProducts", "AlertPanelComp": undefined};
            Events.sendEvent("SetAlertPanel", parameters);
        }
    }

    onModelChanged: {
        checkPermissions();
        checkInUse();
    }

    Component {
        id: alertComp;
        AlertMessage {
            message: qsTr("The product cannot be edited as it is in use.");
        }
    }

    function checkPermissions(){
        let softwareId = "";
        if (model.containsKey("Id")){
            softwareId = model.getData("Id");
        }

        let canAddLicense = PermissionsController.checkPermission("AddLicense");
        if (softwareId === "" && canAddLicense){
            projectInput.readOnly = false;

            ordersCB.changeable = true;
            productCB.changeable = true;
            licenseCB.changeable = true;

            serialNumberInput.readOnly = false;
            expirationEditor.readOnly = false;
            unlimitedSwitch.readOnly = false;
        }
        else{
            let canChangeProject = PermissionsController.checkPermission("ChangeProjectForLicense");
            projectInput.readOnly = !canChangeProject;

            let canChangeOrder = PermissionsController.checkPermission("ChangeOrderForLicense");
            ordersCB.changeable = canChangeOrder;

            let canChangeProduct = PermissionsController.checkPermission("ChangeProductForLicense");
            productCB.changeable = canChangeProduct;

            let canChangeLicense = PermissionsController.checkPermission("ChangeProductLicenses");
            licenseCB.changeable = canChangeLicense;

            let canChangeLicenseNumber = PermissionsController.checkPermission("ChangeLicenseNumber");
            serialNumberInput.readOnly = !canChangeLicenseNumber;

            let canChangeExpiration = PermissionsController.checkPermission("ChangeExpiration");
            expirationEditor.readOnly = !canChangeExpiration;
            unlimitedSwitch.readOnly = !canChangeExpiration;

            let ok =
                canChangeProject ||
                canChangeOrder ||
                canChangeProduct ||
                canChangeLicense ||
                canChangeLicenseNumber ||
                canChangeExpiration;

            if (commandsController){
                commandsController.setCommandVisible("Undo", ok);
                commandsController.setCommandVisible("Redo", ok);
                commandsController.setCommandVisible("Save", ok);
            }
        }
    }

    function checkInUse(){
        if (!root.model){
            return;
        }

        let parameters = {"Id": "SoftwareProducts"};
        let inUse = root.model.getData("InUse");

        if (inUse){
            root.readOnly = true;
            parameters["AlertPanelComp"] = alertComp
            Events.sendEvent("SetAlertPanel", parameters);
        }
        else{
            root.readOnly = false;
            parameters["AlertPanelComp"] = undefined
            Events.sendEvent("SetAlertPanel", parameters);
        }
    }

    function setReadOnly(readOnly){
        projectInput.readOnly = readOnly;

        ordersCB.changeable = !readOnly;
        productCB.changeable = !readOnly;
        licenseCB.changeable = !readOnly;

        serialNumberInput.readOnly = readOnly;
        expirationEditor.readOnly = readOnly;
        unlimitedSwitch.readOnly = readOnly;
    }

    function updateGui(){
        if (root.model.containsKey("Project")){
            projectInput.text = root.model.getData("Project");
        }
        else{
            projectInput.text = "";
        }

        let orderFound = false;
        if (root.model.containsKey("OrderUuid")){
            let orderUuid = root.model.getData("OrderUuid");
            if (ordersCB.model){
                for (let i = 0; i < ordersCB.model.getItemsCount(); i++){
                    let id = ordersCB.model.getData("Id", i);
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
        if (root.model.containsKey("ProductId")){
            let productId = root.model.getData("ProductId");

            if (productCB.model){
                for (let i = 0; i < productCB.model.getItemsCount(); i++){
                    let id = productCB.model.getData("Id", i);
                    if (id === productId){
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

        group2.updateGui();
    }

    function updateModel(){
        console.log("updateModel", model.toJson())
        root.model.setData("Project", projectInput.text);

        let canChangeOrder = PermissionsController.checkPermission("ChangeOrderForLicense");
        if (canChangeOrder){
            if (ordersCB.model){
                if (ordersCB.currentIndex >= 0){
                    let orderUuid = ordersCB.model.getData("Id", ordersCB.currentIndex);
                    root.model.setData("OrderUuid", orderUuid);
                }
                else{
                    root.model.setData("OrderUuid", "");
                }
            }
        }

        if (productCB.currentIndex >= 0 && productCB.model){
            let selectedId = productCB.model.getData("Id", productCB.currentIndex);
            root.model.setData("ProductId", selectedId);
        }
        else{
            root.model.setData("ProductId", "");
        }

        group2.updateModel();
        console.log("end updateModel", model.toJson())
    }

    function getProductLicensesModel(){
        for (let i = 0; i < root.licensesModel.getItemsCount(); i++){
            let productId = root.licensesModel.getData("Id", i);
            if (productId === root.productId){
                if (root.licensesModel.containsKey("Licenses", i)){
                    return root.licensesModel.getData("Licenses", i);
                }
            }
        }

        return null;
    }

    CustomScrollbar {
        id: scrollbar;

        z: parent.z + 1;

        anchors.right: parent.right;
        anchors.top: flickable.top;
        anchors.bottom: flickable.bottom;

        secondSize: 10;
        targetItem: flickable;
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

                title: qsTr("Software Information");
                groupView: group;
            }

            GroupElementView {
                id: group;

                width: parent.width;

                TextInputElementView {
                    id: projectInput;

                    name: qsTr("Project");
                    placeHolderText: qsTr("Enter the project");

                    readOnly: root.readOnly;

                    KeyNavigation.tab: ordersCB;
                    KeyNavigation.backtab: expirationEditor;

                    onEditingFinished: {
                        root.doUpdateModel();
                    }
                }

                FilterableComboBoxElementView {
                    id: ordersCB;

                    nameId: "OrderId";
                    name: qsTr("Order");

                    filteringFields: ["OrderId", "OrderCustomer"];

                    model: CachedOrderCollection.collectionModel;

                    changeable: !root.readOnly;

                    KeyNavigation.tab: productCB;
                    KeyNavigation.backtab: projectInput;

                    delegate: Component {
                        FilterableComboBoxDelegate {
                            width: comboBoxRef ? comboBoxRef.width : 0;
                            comboBoxRef: ordersCB.cbRef;

                            description: qsTr("Customer") + ": " + model.OrderCustomer;
                        }
                    }

                    onCurrentIndexChanged: {
                        root.doUpdateModel();
                    }

                    onModelChanged: {
                        root.doUpdateGui();
                    }
                }
            }

            GroupHeaderView {
                width: parent.width;

                title: qsTr("License Information");
                groupView: group2;
            }

            GroupElementView {
                id: group2;

                width: parent.width;

                function updateGui(){
                    if (root.model.containsKey("SerialNumber")){
                        serialNumberInput.text = root.model.getData("SerialNumber")
                    }
                    else{
                        serialNumberInput.text = "";
                    }

                    let licenseFound = false;

                    let licenseUuid = root.model.getData("LicenseUuid");
                    if (licenseCB.model){
                        for (let i = 0; i < licenseCB.model.getItemsCount(); i++){
                            let licenseId = licenseCB.model.getData("Id", i);
                            if (licenseId === licenseUuid){
                                licenseCB.currentIndex = i;

                                licenseFound = true;

                                break;
                            }
                        }
                    }

                    if (!licenseFound){
                        licenseCB.currentIndex = -1;
                    }

                    if (root.model.containsKey("Expiration")){
                        let expiration = root.model.getData("Expiration");

                        if (expiration && expiration !== "" ){
                            unlimitedSwitch.switchRef.setChecked(false);
                        }
                        else{
                            unlimitedSwitch.switchRef.setChecked(true);
                        }

                        if (expirationEditor.datePicker){
                            if (expiration){
                                let currentDate = expirationEditor.datePicker.getDateAsString()

                                if (expiration !== "" && expiration !== currentDate){
                                    let date = expiration;
                                    let data = date.split("-");

                                    expirationEditor.datePicker.setDate(Number(data[0]), Number(data[1]) - 1, Number(data[2]));
                                }
                            }
                        }
                    }
                }

                function updateModel(){
                    root.model.setData("SerialNumber", serialNumberInput.text)

                    if (expirationEditor.datePicker){
                        if (!unlimitedSwitch.checked){
                            root.model.setData("Expiration", expirationEditor.datePicker.getDateAsString());
                        }
                        else{
                            root.model.setData("Expiration", "");
                        }
                    }

                    if (licenseCB.currentIndex >= 0 && licenseCB.model){
                        let selectedId = licenseCB.model.getData("Id", licenseCB.currentIndex);
                        root.model.setData("LicenseUuid", selectedId);
                    }
                    else{
                        root.model.setData("LicenseUuid", "");
                    }
                }

                ComboBoxElementView {
                    id: productCB;

                    name: qsTr("Product");
                    nameId: "ProductName";

                    model: CachedProductCollection.softwareProductsModel;

                    changeable: !root.readOnly

                    KeyNavigation.tab: licenseCB;
                    KeyNavigation.backtab: ordersCB;

                    onModelChanged: {
                        root.doUpdateGui();
                    }

                    onCurrentIndexChanged: {
                        if (productCB.currentIndex >= 0){
                            let licensesModel = productCB.model.getData("Licenses", productCB.currentIndex);
                            if (!licensesModel){
                                licensesModel = productCB.model.addTreeModel("Licenses", productCB.currentIndex);
                            }

                            root.productLicensesModel = licensesModel;
                        }
                        else{
                            root.productLicensesModel = 0;
                        }

                        licenseCB.currentIndex = -1;

                        root.doUpdateModel();
                    }
                }

                ComboBoxElementView {
                    id: licenseCB;

                    nameId: "LicenseName";
                    name: qsTr("Licenses");

                    model: root.productLicensesModel;

                    KeyNavigation.tab: serialNumberInput;
                    KeyNavigation.backtab: productCB;

                    onCurrentIndexChanged: {
                        if (currentIndex >= 0){
                            root.doUpdateModel();
                        }
                    }
                }

                TextInputElementView {
                    id: serialNumberInput;

                    placeHolderText: qsTr("Enter the software-ID");
                    name: qsTr("Software-ID");

                    KeyNavigation.tab: unlimitedSwitch;
                    KeyNavigation.backtab: licenseCB;

                    onEditingFinished: {
                        root.doUpdateModel();
                    }
                }
            }

            GroupHeaderView {
                width: parent.width;

                title: qsTr("Expiration Information");
                groupView: expirationGroup;
            }

            GroupElementView {
                id: expirationGroup;

                width: parent.width;

                SwitchElementView {
                    id: unlimitedSwitch;

                    name: qsTr("Unlimited");
                    description: unlimitedSwitch.checked ? qsTr("Deactivate it if you want to set the expiration date manually") : qsTr("Activate it if you want to set an unlimited expiration date");

                    onCheckedChanged: {
                        root.doUpdateModel();
                    }

                    readOnly: root.readOnly;

                    KeyNavigation.tab: expirationEditor;
                    KeyNavigation.backtab: serialNumberInput;

                    onSwitchRefChanged: {
                        if (switchRef){
                            switchRef.readOnly = unlimitedSwitch.readOnly;
                        }
                    }
                }

                ElementView {
                    id: expirationEditor;

                    name: qsTr("Expiration");

                    property bool readOnly: root.readOnly;

                    property DatePicker datePicker: null;

                    onReadOnlyChanged: {
                        if (datePicker){
                            datePicker.readOnly = readOnly;
                        }
                    }

                    visible: !unlimitedSwitch.checked;

                    controlComp: datePickerComp;

                    KeyNavigation.tab: projectInput;
                    KeyNavigation.backtab: unlimitedSwitch;

                    onDatePickerChanged: {
                        if (datePicker){
                            datePicker.readOnly = readOnly;
                        }
                    }

                    Component {
                        id: datePickerComp;

                        Item {
                            width: 300;
                            height: 30;

                            DatePicker {
                                id: datePicker_;

                                anchors.right: parent.right;

                                readOnly: expirationEditor.readOnly;

                                width: contentWidth;
                                height: parent.height;

                                currentDayButtonVisible: false;
                                startWithCurrentDay: true;

                                hasDayCombo: false;
                                hasMonthCombo: false;
                                hasYearCombo: false;

                                textFieldBorderColor: Style.borderColor;

                                textFieldWidthDay: 30;
                                textFieldWidthYear: 45;
                                textFieldWidthMonth: 90;

                                textFieldHeight: height;

                                mainMargin: Style.size_mainMargin;

                                Component.onCompleted: {
                                   datePicker_.readOnly = expirationEditor.readOnly

                                   expirationEditor.datePicker = datePicker_;
                                }

                                onDateChanged: {
                                    root.doUpdateModel()
                                }

                                onCompletedChanged: {
                                    if (completed){
                                        var date_ = new Date();

                                        let day = date_.getDay();
                                        let year = date_.getFullYear() + 1;
                                        let month = date_.getMonth();

                                        datePicker_.setDate(year, month, day)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}//Container


