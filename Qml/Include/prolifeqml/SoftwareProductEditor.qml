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

    property bool serialNumberEdit: true;

//    property alias tableElements: licensesTable.elements;
    property alias tableElements: licenseCB.model;
    property bool readOnly: false;

    property int comboBoxHeight: 27;

    function setReadOnly(readOnly){
        serialNumberInput.readOnly = readOnly;
        datePicker.readOnly = readOnly;
        licenseCB.changeable = !readOnly
    }

    function updateGui(){
        if (model.ContainsKey("SerialNumber")){
            serialNumberInput.text = model.GetData("SerialNumber")
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
                checkBox.checkState = Qt.Checked;
            }
            else{
                checkBox.checkState = Qt.Unchecked;
            }

            if (expiration){
                let currentDate = datePicker.getDate();

                if (expiration !== "" && expiration !== currentDate){
                    let date = expiration;
                    let data = date.split("-");
                    datePicker.setDate(Number(data[0]), Number(data[1]) - 1, Number(data[2]));
                }
            }
        }
    }

    function updateModel(){
        model.SetData("SerialNumber", serialNumberInput.text)

        if (checkBox.checkState == Qt.Checked){
            model.SetData("Expiration", datePicker.getDate());
        }
        else{
            model.SetData("Expiration", "");
        }

        if (licenseCB.currentIndex >= 0 && licenseCB.model){
            let selectedId = licenseCB.model.GetData("Id", licenseCB.currentIndex);
            model.SetData("LicenseUuid", selectedId);
        }
        else{
            model.SetData("LicenseUuid", "");
        }
    }

    Text {
        id: serialNumberText;

        anchors.top: parent.top;

        height: visible ? licensesText.height : 0;

        text: qsTr("License Number");
        color: Style.textColor;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSize_common;
    }

    CustomTextField {
        id: serialNumberInput;

        anchors.top: serialNumberText.bottom;
        anchors.topMargin: root.margin;

        height: visible ? root.itemHeight : 0;
        width: parent.width;

        placeHolderText: qsTr("Enter the license number");

        radius: 3;
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

    Text {
        id: licensesText;

        anchors.top: serialNumberInput.bottom;
        anchors.topMargin: root.margin;

        text: qsTr("Licenses");
        color: Style.textColor;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSize_common;
    }

    ComboBox {
        id: licenseCB;

        anchors.top: licensesText.bottom;
        anchors.topMargin: root.margin;

        width: parent.width;
        height: root.comboBoxHeight;

        nameId: "LicenseName";

        model: root.productLicensesModel;

        radius: 3;

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

    Text {
        id: expirationText;

        anchors.top: licenseCB.bottom;
        anchors.topMargin: root.margin;

        text: qsTr("Expiration");
        color: Style.textColor;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSize_common;
    }

    Item {
        anchors.top: expirationText.bottom;
        anchors.topMargin: root.margin;

        width: parent.width;
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

            Component.onCompleted: {
                if (!root.readOnly){
                    let ok = PermissionsController.checkPermission("ChangeLicense");
                    let canEditOrder = PermissionsController.checkPermission("ChangeOrder");
                    if (canEditOrder){
                        ok = true;
                    }

                    datePicker.readOnly = !ok;
                }
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
}//Container


