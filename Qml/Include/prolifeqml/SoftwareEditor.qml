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

        checkWidth();
    }

    onReadOnlyChanged: {
        console.log("SE onReadOnlyChanged", root.readOnly);

        projectInput.readOnly = root.readOnly;
    }

    onWidthChanged: {
        checkWidth();
    }

    function checkWidth(){
        if (width < bodyColumn.width + scrollbar.width + 50){
            bodyColumn.width = width - 50;
        }
        else{
            bodyColumn.width = 700;
        }
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

        group2.updateGui();
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

        group2.updateModel();
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

    CustomScrollbar {
        id: scrollbar;

        anchors.right: parent.right;
        anchors.top: flickable.top;
        anchors.bottom: flickable.bottom;

        secondSize: 10;
        targetItem: flickable;

        radius: 2;
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

                    Component.onCompleted: {
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

                FilterableComboBoxElementView {
                    id: ordersCB;

                    nameId: "OrderId";
                    name: qsTr("Order");

                    model: CachedOrderCollection.collectionModel;

                    changeable: !root.readOnly;

                    Component.onCompleted: {
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
                    if (root.model.ContainsKey("SerialNumber")){
                        serialNumberInput.text = root.model.GetData("SerialNumber")
                    }
                    else{
                        serialNumberInput.text = "";
                    }

                    let licenseFound = false;

                    let licenseUuid = root.model.GetData("LicenseUuid");
                    if (licenseCB.model){
                        for (let i = 0; i < licenseCB.model.GetItemsCount(); i++){
                            let licenseId = licenseCB.model.GetData("Id", i);
                            if (licenseId == licenseUuid){
                                licenseCB.currentIndex = i;

                                licenseFound = true;

                                break;
                            }
                        }
                    }

                    if (!licenseFound){
                        licenseCB.currentIndex = -1;
                    }

                    if (root.model.ContainsKey("Expiration")){
                        let expiration = root.model.GetData("Expiration");

                        if (expiration && expiration !== "" ){
                            expirationEditor.setCheckState(Qt.Checked);
                        }
                        else{
                            expirationEditor.setCheckState(Qt.Unchecked);
                        }

                        if (expiration){
                            let currentDate = expirationEditor.getDate();

                            if (expiration !== "" && expiration !== currentDate){
                                let date = expiration;
                                let data = date.split("-");
                                expirationEditor.setDate(Number(data[0]), Number(data[1]) - 1, Number(data[2]));
                            }
                        }
                    }
                }

                function updateModel(){
                    root.model.SetData("SerialNumber", serialNumberInput.text)

                    if (expirationEditor.getCheckState() === Qt.Checked){
                        root.model.SetData("Expiration", expirationEditor.getDate());
                    }
                    else{
                        root.model.SetData("Expiration", "");
                    }

                    if (licenseCB.currentIndex >= 0 && licenseCB.model){
                        let selectedId = licenseCB.model.GetData("Id", licenseCB.currentIndex);
                        root.model.SetData("LicenseUuid", selectedId);
                    }
                    else{
                        root.model.SetData("LicenseUuid", "");
                    }
                }

                ComboBoxElementView {
                    id: productCB;

                    name: qsTr("Product");
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

                            root.productLicensesModel = licensesModel;
                        }
                        else{
                            root.productLicensesModel = 0;
                        }

                        root.doUpdateModel();
                    }
                }

                ComboBoxElementView {
                    id: licenseCB;

                    nameId: "LicenseName";
                    name: qsTr("Licenses");

                    model: root.productLicensesModel;

                    changeable: !root.readOnly;

                    Component.onCompleted: {
                        if (!root.readOnly){
                            let ok = PermissionsController.checkPermission("ChangeLicense");

                            let canEditOrder = PermissionsController.checkPermission("ChangeOrder");
                            if (canEditOrder){
                                ok = true;
                            }

                            licenseCB.changeable = ok;
                        }
                    }

                    onCurrentIndexChanged: {
                        root.doUpdateModel();
                    }
                }

                TextInputElementView {
                    id: serialNumberInput;

                    placeHolderText: qsTr("Enter the license number");
                    name: qsTr("License Number");

                    readOnly: root.readOnly;

                    Component.onCompleted: {
                        if (!root.readOnly){
                            let ok = PermissionsController.checkPermission("ChangeLicense");
                            if (!ok){
                                ok = PermissionsController.checkPermission("ChangeLicenseNumber");
                            }

                            let canEditOrder = PermissionsController.checkPermission("ChangeOrder");
                            if (canEditOrder){
                                ok = true;
                            }

                            serialNumberInput.readOnly = !ok;
                        }
                    }

                    onEditingFinished: {
                        root.doUpdateModel();
                    }
                }

                ElementView {
                    id: expirationEditor;

                    name: qsTr("Expiration");

                    property bool readOnly: false;

                    function getDate(){
                        if (datePicker){
                            return datePicker.getDate();
                        }

                        return "";
                    }

                    function setDate(year, month, day){
                        if (datePicker){
                            datePicker.setDate(year, month, day);
                        }
                    }

                    function setCheckState(state){
                        if (checkBox){
                            checkBox.checkState = state;
                        }
                    }

                    function getCheckState(){
                        if (checkBox){
                            return checkBox.checkState;
                        }

                        return Qt.Unchecked;
                    }

                    property DatePicker datePicker: null;
                    property CheckBox checkBox: null;

                    controlComp: expirationComp;

                    Component {
                        id: expirationComp;

                        Item {
                            width: 300;
                            height: 30;

                            CheckBox {
                                id: checkBox;

                                anchors.verticalCenter: parent.verticalCenter;
                                anchors.left: parent.left;

                                onClicked: {
                                    checkBox.checkState = Qt.Checked - checkBox.checkState;
                                }

                                isActive: licenseCB.currentIndex >= 0 && licenseCB.changeable && !root.readOnly;

                                onCheckStateChanged: {
                                    root.doUpdateModel();
                                }

                                Component.onCompleted: {
                                    expirationEditor.checkBox = checkBox;
                                }
                            }

                            Text {
                                id: textUnlimited;

                                anchors.verticalCenter: parent.verticalCenter;
                                anchors.left: checkBox.right;
                                anchors.leftMargin: 5;

                                visible: checkBox.checkState === Qt.Unchecked;

                                font.family: Style.fontFamily;
                                font.pixelSize: Style.fontSize_common;
                                color: Style.textColor;

                                text: qsTr("Unlimited");
                            }

                            DatePicker {
                                id: datePicker;

                                anchors.verticalCenter: parent.verticalCenter;
                                anchors.left: checkBox.right;
                                anchors.leftMargin: 5;

                                visible: checkBox.checkState === Qt.Checked;

                                width: 100;
                                height: 20;

                                currentDayButtonVisible: false;
                                startWithCurrentDay: true;

                                hasDayCombo: false;
                                hasMonthCombo: false;
                                hasYearCombo: false;

                                readOnly: root.readOnly;

                                Component.onCompleted: {
                                    if (!root.readOnly){
                                        let ok = PermissionsController.checkPermission("ChangeLicense");
                                        let canEditOrder = PermissionsController.checkPermission("ChangeOrder");
                                        if (canEditOrder){
                                            ok = true;
                                        }

                                        datePicker.readOnly = !ok;
                                    }

                                    expirationEditor.datePicker = datePicker;
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

                                        datePicker.setDate(year, month, day)
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


