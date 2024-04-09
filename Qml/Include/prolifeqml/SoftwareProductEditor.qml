import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtlicgui 1.0
import imtcontrols 1.0

ViewBase {
    id: softwareProductEditor;

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
        expirationEditor.readOnly = readOnly;
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

        let licenseUuid = softwareProductEditor.model.GetData("LicenseUuid");
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

        if (softwareProductEditor.model.ContainsKey("Expiration")){
            let expiration = softwareProductEditor.model.GetData("Expiration");

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
        model.SetData("SerialNumber", serialNumberInput.text)

        if (expirationEditor.getCheckState() === Qt.Checked){
            model.SetData("Expiration", expirationEditor.getDate());
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

    GroupElementView {
        id: group;

        width: parent.width;

        TextInputElementView {
            id: serialNumberInput;

            placeHolderText: qsTr("Enter the license number");
            name: qsTr("License Number");

            readOnly: softwareProductEditor.readOnly;

            Component.onCompleted: {
                if (!softwareProductEditor.readOnly){
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
                softwareProductEditor.doUpdateModel();
            }
        }

        ComboBoxElementView {
            id: licenseCB;

            nameId: "LicenseName";
            name: qsTr("Licenses");

            model: softwareProductEditor.productLicensesModel;

            changeable: !softwareProductEditor.readOnly;

            Component.onCompleted: {
                if (!softwareProductEditor.readOnly){
                    let ok = PermissionsController.checkPermission("ChangeLicense");

                    let canEditOrder = PermissionsController.checkPermission("ChangeOrder");
                    if (canEditOrder){
                        ok = true;
                    }

                    licenseCB.changeable = ok;
                }
            }

            onCurrentIndexChanged: {
                softwareProductEditor.doUpdateModel();
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

                        isActive: licenseCB.currentIndex >= 0 && licenseCB.changeable && !softwareProductEditor.readOnly;

                        onCheckStateChanged: {
                            softwareProductEditor.doUpdateModel();
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

                        readOnly: softwareProductEditor.readOnly;

                        Component.onCompleted: {
                            if (!softwareProductEditor.readOnly){
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
                            softwareProductEditor.doUpdateModel()
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
}//Container


