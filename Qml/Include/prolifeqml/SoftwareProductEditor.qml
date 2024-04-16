import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtlicgui 1.0
import imtcontrols 1.0

ViewBase {
    id: root;

    property int itemHeight: 30;
    property int margin: 10;

    property var productLicensesModel: TreeItemModel{}

    property TreeItemModel softwaresModel: TreeItemModel{}

    property bool serialNumberEdit: true;

    //    property alias tableElements: licensesTable.elements;
    property alias tableElements: licenseCB.model;
    property bool readOnly: false;

    property int comboBoxHeight: 27;

    property bool isNewSoftware: switchNewLicense.checked;

    function setReadOnly(readOnly){
        serialNumberInput.readOnly = readOnly;
        expirationElementView.datePicker.readOnly = readOnly;
        licenseCB.changeable = !readOnly
    }

    function updateGui(){
        console.log("Software updateGui", model.toJSON());

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

            console.log("licenseUuid", licenseUuid);
            console.log("licenseCB.model", createdLicenseCb.model);

            if (createdLicenseCb.model){
                for (let i = 0; i < createdLicenseCb.model.GetItemsCount(); i++){
                    let id = createdLicenseCb.model.GetData("LicenseUuid", i);

                    if (id === licenseUuid){
                        createdLicenseCb.currentIndex = i;

                        break;
                    }
                }
            }
        }
    }

    function updateModel(){
        model.SetData("IsNew", isNewSoftware);

        if (isNewSoftware){
            if (licenseCB.currentIndex >= 0 && licenseCB.model){
                let selectedId = licenseCB.model.GetData("Id", licenseCB.currentIndex);
                model.SetData("LicenseUuid", selectedId);

                let licenseId = licenseCB.model.GetData("LicenseId", licenseCB.currentIndex);
                model.SetData("LicenseId", licenseId);

                let licenseName = licenseCB.model.GetData("LicenseName", licenseCB.currentIndex);
                model.SetData("LicenseName", licenseName);
            }
            else{
                model.SetData("LicenseUuid", "");
                model.SetData("LicenseId", "");
                model.SetData("LicenseName", "");
            }

            model.SetData("SerialNumber", serialNumberInput.text)

            if (expirationElementView.checkBox.checkState == Qt.Checked){
                model.SetData("Expiration", expirationElementView.datePicker.getDate());
            }
            else{
                model.SetData("Expiration", "");
            }
        }
        else{
            model.SetData("LicenseUuid", "");
            model.SetData("LicenseId", "");
            model.SetData("LicenseName", "");
            model.SetData("SerialNumber", "");
            model.SetData("Expiration", "");

            if (createdLicenseCb.currentIndex >= 0){
                let id = createdLicenseCb.model.GetData("Id", createdLicenseCb.currentIndex);
                model.SetData("Id", id);

                let licenseUuid = createdLicenseCb.model.GetData("LicenseUuid", createdLicenseCb.currentIndex);
                model.SetData("LicenseUuid", licenseUuid);

                let licenseID = createdLicenseCb.model.GetData("LicenseId", createdLicenseCb.currentIndex);
                model.SetData("LicenseId", licenseID);

                let licenseName = createdLicenseCb.model.GetData("LicenseName", createdLicenseCb.currentIndex);
                model.SetData("LicenseName", licenseName);

                let serialNumber = createdLicenseCb.model.GetData("SerialNumber", createdLicenseCb.currentIndex);
                model.SetData("SerialNumber", serialNumber);

                let expiration = createdLicenseCb.model.GetData("Expiration", createdLicenseCb.currentIndex);
                model.SetData("Expiration", expiration);
            }
        }
    }

    Column {
        width: parent.width;

        spacing: Style.size_mainMargin;

        SwitchElementView {
            id: switchNewLicense;

            width: parent.width;

            name: qsTr("New License");

            onCheckedChanged: {
                createdLicenseCb.visible = !checked;

                root.doUpdateModel();
            }
        }

        ComboBoxElementView {
            id: createdLicenseCb;

            width: parent.width;

            model: root.softwaresModel;

            changeable: !root.readOnly;

            name: qsTr("License")

            onCurrentIndexChanged: {
                if (currentIndex >= 0){
                    if (createdLicenseCb.model.ContainsKey("LicenseName", currentIndex)){
                        let licenseName = createdLicenseCb.model.GetData("LicenseName", currentIndex)

                        typeValue.text = licenseName;
                    }

                    if (createdLicenseCb.model.ContainsKey("SerialNumber", currentIndex)){
                        let serialNumber = createdLicenseCb.model.GetData("SerialNumber", currentIndex)

                        softwareValue.text = serialNumber;
                    }

                    if (createdLicenseCb.model.ContainsKey("Expiration", currentIndex)){
                        let expiration = createdLicenseCb.model.GetData("Expiration", currentIndex)

                        expirationValue.text = expiration;
                    }
                }

                root.doUpdateModel();
            }
        }

        Column {
            id: softwareContent;

            width: parent.width;

            spacing: parent.spacing;

            visible: !root.isNewSoftware;

            TextElementView {
                id: typeValue;

                width: parent.width;

                name: qsTr("License Type");
            }

            TextElementView {
                id: softwareValue;

                width: parent.width;

                name: qsTr("License-ID");
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

            ComboBoxElementView {
                id: licenseCB;

                width: parent.width;

                name: qsTr("License Types");
                nameId: "LicenseName";

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

                width: parent.width;

                name: qsTr("Software-ID");
                placeHolderText: qsTr("Enter the software-ID");

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
                id: expirationElementView;

                width: parent.width;

                name: qsTr("Expiration");

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

                            readOnly: root.readOnly;

                            hasDayCombo: false;
                            hasMonthCombo: false;
                            hasYearCombo: false;

                            textFieldBorderColor: Style.borderColor;

                            Component.onCompleted: {
                                if (!root.readOnly){
                                    let ok = PermissionsController.checkPermission("ChangeLicense");
                                    let canEditOrder = PermissionsController.checkPermission("ChangeOrder");
                                    if (canEditOrder){
                                        ok = true;
                                    }

                                    datePicker.readOnly = !ok;
                                }

                                expirationElementView.datePicker = datePicker;
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


}//Container


