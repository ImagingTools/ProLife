import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtlicgui 1.0
import imtcontrols 1.0

ViewBase {
    id: root;

    height: content.height;

    property int itemHeight: 30;

    property var productLicensesModel: TreeItemModel{}

    property TreeItemModel softwaresModel: TreeItemModel{}

    property bool serialNumberEdit: true;

    //    property alias tableElements: licensesTable.elements;
    property alias tableElements: licenseCB.model;
    property bool readOnly: false;

    property bool isNewSoftware: switchNewLicense.checked;
    property int productIndex: -1;

    property bool isNewProduct: root.model.GetData("IsNew") ? root.model.GetData("IsNew") : false;

    function setReadOnly(readOnly){
        serialNumberInput.readOnly = readOnly;
        expirationElementView.datePicker.readOnly = readOnly;
        licenseCB.changeable = !readOnly
    }

    function updateGui(){
        let isNew = model.GetData("IsNew")

        if (isNew){
            switchNewLicense.checked = true;

            licenseCB.currentIndex = -1;

            let licenseUuid = root.model.GetData("LicenseUuid");
            if (licenseCB.model){
                for (let i = 0; i < licenseCB.model.GetItemsCount(); i++){
                    let id = licenseCB.model.GetData("Id", i);
                    if (id === licenseUuid){
                        licenseCB.currentIndex = i;

                        break;
                    }
                }
            }

            if (model.ContainsKey("SerialNumber")){
                serialNumberInput.text = model.GetData("SerialNumber")
            }
            else{
                serialNumberInput.text = "";
            }

            if (root.model.ContainsKey("Expiration")){
                let expiration = root.model.GetData("Expiration");

                if (expiration && expiration !== "" ){
                    expirationElementView.checkBox.checkState = Qt.Checked;
                }
                else{
                    expirationElementView.checkBox.checkState = Qt.Unchecked;
                }

                if (expiration){
                    let currentDate = expirationElementView.datePicker.getDate();

                    if (expiration !== "" && expiration !== currentDate){
                        let date = expiration;
                        let data = date.split("-");
                        expirationElementView.datePicker.setDate(Number(data[0]), Number(data[1]) - 1, Number(data[2]));
                    }
                }
            }
        }
        else{
            switchNewLicense.checked = false;

            createdLicenseCb.currentIndex = -1;

            let licenseUuid = root.model.GetData("LicenseUuid");

            if (createdLicenseCb.model){
                for (let i = 0; i < createdLicenseCb.model.GetItemsCount(); i++){
                    let id = createdLicenseCb.model.GetData("LicenseUuid", i);

                    if (id === licenseUuid){
                        createdLicenseCb.currentIndex = i;

                        break;
                    }
                }
            }

            if (model.ContainsKey("SerialNumber")){
                softwareValue.text = model.GetData("SerialNumber")
            }
            else{
                softwareValue.text = "";
            }
        }
    }

    function updateModel(){
        root.model.SetData("IsNew", isNewSoftware);

        if (isNewSoftware){
            if (licenseCB.currentIndex >= 0 && licenseCB.model){
                let selectedId = licenseCB.model.GetData("Id", licenseCB.currentIndex);
                root.model.SetData("LicenseUuid", selectedId);

                let licenseId = licenseCB.model.GetData("LicenseId", licenseCB.currentIndex);
                root.model.SetData("LicenseId", licenseId);

                let licenseName = licenseCB.model.GetData("LicenseName", licenseCB.currentIndex);
                root.model.SetData("LicenseName", licenseName);
            }
            else{
                root.model.SetData("LicenseUuid", "");
                root.model.SetData("LicenseId", "");
                root.model.SetData("LicenseName", "");
            }

            root.model.SetData("SerialNumber", serialNumberInput.text)

            if (expirationElementView.checkBox.checkState == Qt.Checked){
                root.model.SetData("Expiration", expirationElementView.datePicker.getDate());
            }
            else{
                root.model.SetData("Expiration", "");
            }
        }
        else{
            root.model.SetData("LicenseUuid", "");
            root.model.SetData("LicenseId", "");
            root.model.SetData("LicenseName", "");
            root.model.SetData("SerialNumber", "");
            root.model.SetData("Expiration", "");

            if (createdLicenseCb.currentIndex >= 0){
                let id = createdLicenseCb.model.GetData("Id", createdLicenseCb.currentIndex);
                root.model.SetData("Id", id);

                let licenseUuid = createdLicenseCb.model.GetData("LicenseUuid", createdLicenseCb.currentIndex);
                root.model.SetData("LicenseUuid", licenseUuid);

                let licenseID = createdLicenseCb.model.GetData("LicenseId", createdLicenseCb.currentIndex);
                root.model.SetData("LicenseId", licenseID);

                let licenseName = createdLicenseCb.model.GetData("LicenseName", createdLicenseCb.currentIndex);
                root.model.SetData("LicenseName", licenseName);

                let serialNumber = createdLicenseCb.model.GetData("SerialNumber", createdLicenseCb.currentIndex);
                root.model.SetData("SerialNumber", serialNumber);

                let expiration = createdLicenseCb.model.GetData("Expiration", createdLicenseCb.currentIndex);
                root.model.SetData("Expiration", expiration);
            }
        }
    }

    Column {
        id: content;

        width: parent.width;

        spacing: Style.size_mainMargin;

        SwitchElementView {
            id: switchNewLicense;

            width: parent.width;

            name: qsTr("New License");

            visible: root.productIndex == -1 || root.isNewProduct;

            onCheckedChanged: {
                createdLicenseCb.visible = !checked;

                root.doUpdateModel();
            }
        }

        FilterableComboBoxElementView {
            id: createdLicenseCb;

            width: parent.width;

            model: root.softwaresModel;

            changeable: !root.readOnly;

            name: qsTr("License")

            nameId: "Name"

            filteringFields: ["SerialNumber", "ProductName", "LicenseId", "LicenseName"];

            bottomComp: currentIndex < 0 ? licenseTypeErrorComp : undefined;

            delegate: Component {
                FilterableComboBoxDelegate {
                    width: comboBoxRef ? comboBoxRef.width : 0;
                    comboBoxRef: createdLicenseCb.cbRef;

                    text: model.SerialNumber === "" ? model.ProductName + " (" + qsTr("No software-ID") + ")" : model.ProductName + " (" + model.SerialNumber+ ")";

                    property string article: qsTr("Article");
                    property string notSpecified: qsTr("not specified");

                    description: model.LicenseId !== "" ? article + ": " + model.LicenseName + " (" + model.LicenseId + ")": article + ": " + notSpecified;
                }
            }

            onCurrentIndexChanged: {
                if (currentIndex >= 0){
                    if (createdLicenseCb.model.ContainsKey("LicenseName", currentIndex)){
                        let licenseName = createdLicenseCb.model.GetData("LicenseName", currentIndex)

                        typeValue.text = licenseName;
                    }

                    if (createdLicenseCb.model.ContainsKey("LicenseId", currentIndex)){
                        let licenseId = createdLicenseCb.model.GetData("LicenseId", currentIndex)

                        articleValue.text = licenseId;
                    }

                    if (createdLicenseCb.model.ContainsKey("SerialNumber", currentIndex)){
                        let serialNumber = createdLicenseCb.model.GetData("SerialNumber", currentIndex)

                        softwareValue.text = serialNumber;
                    }

                    if (createdLicenseCb.model.ContainsKey("Expiration", currentIndex)){
                        let expiration = createdLicenseCb.model.GetData("Expiration", currentIndex)

                        if (expiration === ""){
                            expirationValue.text = qsTr("Unlimited");
                        }
                        else{
                            expirationValue.text = expiration;
                        }
                    }
                }

                root.doUpdateModel();
            }
        }

        Column {
            id: softwareContent;

            width: parent.width;

            spacing: parent.spacing;

            visible: !root.isNewSoftware && createdLicenseCb.currentIndex >= 0;

            TextElementView {
                id: typeValue;

                width: parent.width;

                name: qsTr("License Type");
            }

            TextElementView {
                id: articleValue;

                width: parent.width;

                name: qsTr("Article Number");
            }

            TextElementView {
                id: softwareValue;

                width: parent.width;

                name: qsTr("Software-ID");
            }

            TextElementView {
                id: expirationValue;

                width: parent.width;

                name: qsTr("Expiration");
            }
        }

        Column {
            id: newSoftwareContent;

            width: parent.width;

            spacing: parent.spacing;

            visible: root.isNewSoftware;

            FilterableComboBoxElementView {
                id: licenseCB;

                width: parent.width;

                name: qsTr("License Types");
                nameId: "LicenseName";

                model: root.productLicensesModel;

                changeable: !root.readOnly;

                bottomComp: currentIndex < 0 ? licenseTypeErrorComp : undefined;
                filteringFields: ["LicenseName", "LicenseId"];

                delegate: Component {
                    FilterableComboBoxDelegate {
                        width: licenseCB.width;
                        comboBoxRef: licenseCB.cbRef;

                        text: model.LicenseName;
                        description: model.LicenseId;
                    }
                }

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
                    if (currentIndex >= 0){
                        if (licenseCB.model.ContainsKey("LicenseId", currentIndex)){
                            let licenseId = licenseCB.model.GetData("LicenseId", currentIndex)

                            newArticleValue.text = licenseId
                        }
                    }

                    root.doUpdateModel();
                }
            }

            Component {
                id: licenseTypeErrorComp;

                Text {
                    id: selectSensorText;

                    text: qsTr("Please select a license");
                    color: Style.errorTextColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }
            }

            TextInputElementView {
                id: serialNumberInput;

                width: parent.width;

                name: qsTr("Software-ID");
                placeHolderText: qsTr("Enter the software-ID");

                readOnly: root.readOnly;

                visible: licenseCB.currentIndex >= 0;

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
                id: expirationElementView;

                width: parent.width;

                name: qsTr("Expiration");

                visible: licenseCB.currentIndex >= 0;

                property CheckBox checkBox;
                property DatePicker datePicker;

                controlComp: Component {
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
                                expirationElementView.checkBox = checkBox;
                            }
                        }

                        Text {
                            id: textUnlimited;

                            anchors.verticalCenter: parent.verticalCenter;
                            anchors.left: checkBox.right;
                            anchors.leftMargin: Style.size_mainMargin;

                            visible: checkBox.checkState === Qt.Unchecked;

                            font.family: Style.fontFamily;
                            font.pixelSize: Style.fontSize_common;
                            color: Style.textColor;

                            text: qsTr("Unlimited");
                        }

                        DatePicker {
                            id: datePicker_;

                            anchors.verticalCenter: parent.verticalCenter;
                            anchors.left: checkBox.right;
                            anchors.leftMargin: Style.size_mainMargin;

                            visible: checkBox.checkState === Qt.Checked;

                            width: 100;
                            height: parent.height;

                            currentDayButtonVisible: false;
                            startWithCurrentDay: true;

                            readOnly: root.readOnly;

                            hasDayCombo: false;
                            hasMonthCombo: false;
                            hasYearCombo: false;

                            textFieldWidthDay: 30;
                            textFieldWidthYear: 45;
                            textFieldWidthMonth: 90;

                            textFieldHeight: height;

                            textFieldBorderColor: Style.borderColor;
                            mainMargin: Style.size_mainMargin;

                            Component.onCompleted: {
                                if (!root.readOnly){
                                    let ok = PermissionsController.checkPermission("ChangeLicense");
                                    let canEditOrder = PermissionsController.checkPermission("ChangeOrder");
                                    if (canEditOrder){
                                        ok = true;
                                    }

                                    datePicker_.readOnly = !ok;
                                }

                                expirationElementView.datePicker = datePicker_;
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

            TextElementView {
                id: newArticleValue;

                width: parent.width;

                name: qsTr("Article Number");

                visible: parent.visible && licenseCB.currentIndex >= 0;
            }
         }
    }


}//Container


