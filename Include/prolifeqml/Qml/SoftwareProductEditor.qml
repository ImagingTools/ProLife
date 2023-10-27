import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtlicgui 1.0

Item {
    id: root;

    property int itemHeight: 30;
    property int margin: 10;

    property TreeItemModel productModel: TreeItemModel {}
    property var productLicensesModel: TreeItemModel{}

    property bool serialNumberEdit: true;

//    property alias tableElements: licensesTable.elements;
    property alias tableElements: licenseCB.model;
    property bool readOnly: false;

    onReadOnlyChanged: {
        serialNumberInput.readOnly = root.readOnly;

        datePicker.readOnly = root.readOnly;

        licenseCB.changeable = !root.readOnly
    }

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", root.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", root.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
//        root.updateHeaders();
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

        Component.onCompleted: {
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

        onTextChanged: {
            root.productModel.SetData("SerialNumber", serialNumberInput.text);
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
        height: 23;

        nameId: "LicenseName";

        model: root.productLicensesModel;

        radius: 3;

        Component.onCompleted: {
            let ok = PermissionsController.checkPermission("ChangeLicense");

            let canEditOrder = PermissionsController.checkPermission("ChangeOrder");
            if (canEditOrder){
                ok = true;
            }

            licenseCB.changeable = ok;
        }

        onCurrentIndexChanged: {
            if (currentIndex >= 0){
                let selectedLicenseUuid = licenseCB.model.GetData("Id", currentIndex);

                root.productModel.SetData("LicenseUuid", selectedLicenseUuid)
            }
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

            isActive: licenseCB.currentIndex >= 0 && licenseCB.changeable;

            onCheckStateChanged: {
                if (blockUpdatingModel){
                    return;
                }

                if (checkBox.checkState == Qt.Checked){
                    productModel.SetData("Expiration", datePicker.getDate());
                }
                else{
                    productModel.SetData("Expiration", "");
                }
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

            Component.onCompleted: {
                let ok = PermissionsController.checkPermission("ChangeLicense");
                let canEditOrder = PermissionsController.checkPermission("ChangeOrder");
                if (canEditOrder){
                    ok = true;
                }

                datePicker.readOnly = !ok;
            }

            onDateChanged: {
                if (blockUpdatingModel){
                    return;
                }

                console.log("onDateChanged", datePicker.getDate());
                productModel.SetData("Expiration", datePicker.getDate());
            }

//            property string expirationDate: model.Expiration;

//            onExpirationDateChanged: {
//                console.log("onExpirationDateChanged", datePicker.expirationDate);

//                let currentDate = datePicker.getDate();
//                if (expirationDate !== "" && expirationDate !== currentDate){
//                    let date = model.Expiration;
//                    let data = date.split("-");
//                    datePicker.setDate(Number(data[0]), Number(data[1]) - 1, Number(data[2]));
//                }
//            }

//            onDateChanged: {
//                console.log("onDateChanged", datePicker.getDate());
//                model.Expiration = datePicker.getDate();

//                root.dateChanged();
//            }
        }
    }

    property bool blockUpdatingModel: false;

    function updateModel(){
        if (root.blockUpdatingModel){
            return;
        }

        root.productModel.SetData("SerialNumber", serialNumberInput.text);
    }

    function updateGui(){
        console.log("updateGui", root.productModel.toJSON());
        blockUpdatingModel = true;

        licenseCB.currentIndex = -1;

        let licenseUuid = root.productModel.GetData("LicenseUuid");
        if (licenseCB.model){
            for (let i = 0; i < licenseCB.model.GetItemsCount(); i++){
                let licenseId = licenseCB.model.GetData("Id", i);
                let licenseName = licenseCB.model.GetData("LicenseName", i);

                if (licenseId == licenseUuid){
                    licenseCB.currentIndex = i;

                    break;
                }
            }
        }

        if (root.productModel.ContainsKey("Expiration")){
            let expiration = root.productModel.GetData("Expiration");

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

        if (root.productModel.ContainsKey("SerialNumber")){
            let serialNumber = root.productModel.GetData("SerialNumber");
            if (serialNumber){
                serialNumberInput.text = serialNumber;
            }
            else{
                serialNumberInput.text = "";
            }
        }

        blockUpdatingModel = false;
    }


}//Container


