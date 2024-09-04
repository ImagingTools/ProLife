import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import prolifeqml 1.0
import prolifeLicensesSdl 1.0

ViewBase {
    id: root;

    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}

    property var productLicensesModel: TreeItemModel{}

    property string alertMessage: "";
    property int comboBoxHeight: 27;

    property SoftwareProductData softwareProductData: model ? model : null;

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

    // onModelChanged: {
    //     checkPermissions();
    //     checkInUse();
    // }

    onSoftwareProductDataChanged: {
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
        if (!softwareProductData){
            return;
        }

        let softwareId = softwareProductData.m_id;

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
        if (!softwareProductData){
            return;
        }

        let parameters = {"Id": "SoftwareProducts"};

        if (softwareProductData.m_inUse){
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
        if (!softwareProductData){
            console.error("Unable to update GUI for 'SoftwareEditor'. Error: softwareProductData is invalid");
            return;
        }

        projectInput.text = softwareProductData.m_project;

        ordersCB.currentIndex = -1;

        let orderUuid = softwareProductData.m_orderUuid;
        if (ordersCB.model){
            for (let i = 0; i < ordersCB.model.getItemsCount(); i++){
                let id = ordersCB.model.getData("Id", i);
                if (id === orderUuid){
                    ordersCB.currentIndex = i;
                    break;
                }
            }
        }

        productCB.currentIndex = -1;
        let productId = softwareProductData.m_productId;

        if (productCB.model){
            for (let i = 0; i < productCB.model.getItemsCount(); i++){
                let id = productCB.model.getData("Id", i);
                if (id === productId){
                    productCB.currentIndex = i;
                    break;
                }
            }
        }

        group2.updateGui();
    }

    function updateModel(){
        if (!softwareProductData){
            console.error("Unable to update model for 'SoftwareEditor'. Error: softwareProductData is invalid");
            return;
        }

        softwareProductData.m_project = projectInput.text;

        let canChangeOrder = PermissionsController.checkPermission("ChangeOrderForLicense");
        if (canChangeOrder){
            if (ordersCB.model){
                if (ordersCB.currentIndex >= 0){
                    let orderUuid = ordersCB.model.getData("Id", ordersCB.currentIndex);
                    softwareProductData.m_orderUuid = orderUuid;
                }
                else{
                    softwareProductData.m_orderUuid = "";
                }
            }
        }

        if (productCB.currentIndex >= 0 && productCB.model){
            let selectedId = productCB.model.getData("Id", productCB.currentIndex);
            softwareProductData.m_productId = selectedId;
        }
        else{
            softwareProductData.m_productId = "";

        }

        group2.updateModel();
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
                    serialNumberInput.text = root.softwareProductData.m_serialNumber;

                    licenseCB.currentIndex = -1;

                    let licenseUuid = root.softwareProductData.m_licenseUuid;
                    if (licenseCB.model){
                        for (let i = 0; i < licenseCB.model.getItemsCount(); i++){
                            let licenseId = licenseCB.model.getData("Id", i);
                            if (licenseId === licenseUuid){
                                licenseCB.currentIndex = i;

                                break;
                            }
                        }
                    }

                    let expiration = root.softwareProductData.m_expiration;

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

                function updateModel(){
                    root.softwareProductData.m_serialNumber = serialNumberInput.text;

                    if (expirationEditor.datePicker){
                        if (!unlimitedSwitch.checked){
                            root.softwareProductData.m_expiration = expirationEditor.datePicker.getDateAsString();
                        }
                        else{
                            root.softwareProductData.m_expiration = "";
                        }
                    }

                    if (licenseCB.currentIndex >= 0 && licenseCB.model){
                        let selectedId = licenseCB.model.getData("Id", licenseCB.currentIndex);
                        root.softwareProductData.m_licenseUuid = selectedId;
                    }
                    else{
                        root.softwareProductData.m_licenseUuid = "";
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


