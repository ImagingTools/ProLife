import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

ViewBase {
    id: root;

    height: content.height;

    property int margin: 10;

    property var productLicensesModel: TreeItemModel{}
    property TreeItemModel devicesModel: TreeItemModel{}

    property alias deviceIndex: deviceCB.currentIndex;

    property bool isNewDevice: switchNewSensor.checked;
    property int productIndex: -1;

    function updateGui(){
        let isNew = model.GetData("IsNew")
        if (isNew){
            switchNewSensor.checked = true;

            macAddressInput.text = "";

            if (root.model.ContainsKey("MacAddress")){
                let macAddress = root.model.GetData("MacAddress");
                macAddressInput.text = macAddress;
            }

            typesCB.currentIndex = -1;

            if (root.model.ContainsKey("LicenseUuid")){
                let licenseUuid = root.model.GetData("LicenseUuid");

                if (typesCB.model){
                    for (let i = 0; i < typesCB.model.GetItemsCount(); i++){
                        let id = typesCB.model.GetData("Id", i);
                        if (id === licenseUuid){
                            typesCB.currentIndex = i;
                            break;
                        }
                    }
                }
            }
        }
        else{
            switchNewSensor.checked = false;

            deviceCB.currentIndex = -1;
            if (root.model.ContainsKey("Id")){
                let deviceId = root.model.GetData("Id")
                if (deviceCB.model){
                    for (let i = 0; i < deviceCB.model.GetItemsCount(); i++){
                        let id = deviceCB.model.GetData("Id", i);
                        if (id === deviceId){
                            deviceCB.currentIndex = i;
                            break;
                        }
                    }
                }
            }
        }
    }

    function updateModel(){
        root.model.SetData("IsNew", isNewDevice);

        if (isNewDevice){
            if (typesCB.currentIndex >= 0){
                let index = typesCB.currentIndex;

                let uuid = root.productLicensesModel.GetData("Id", index);
                let licenseId = root.productLicensesModel.GetData("LicenseId", index);
                let licenseName = root.productLicensesModel.GetData("LicenseName", index);

                root.model.SetData("LicenseUuid", uuid);
                root.model.SetData("LicenseId", licenseId);
                root.model.SetData("LicenseName", licenseName);
            }
            else{
                root.model.SetData("LicenseUuid", "");
                root.model.SetData("LicenseId", "");
                root.model.SetData("LicenseName", "");
            }

            root.model.SetData("MacAddress", macAddressInput.text);
        }
        else{
            if (deviceCB.currentIndex >= 0){
                let deviceId = deviceCB.model.GetData("Id", deviceCB.currentIndex);
                root.model.SetData("Id", deviceId);

                if (deviceCB.model.ContainsKey("LicenseUuid", deviceCB.currentIndex)){
                    let configurationType = deviceCB.model.GetData("LicenseUuid", deviceCB.currentIndex);
                    root.model.SetData("LicenseUuid", configurationType);
                }
                else{
                    root.model.SetData("LicenseUuid", "");
                }

                if (deviceCB.model.ContainsKey("LicenseId", deviceCB.currentIndex)){
                    let licenseId = deviceCB.model.GetData("LicenseId", deviceCB.currentIndex);
                    root.model.SetData("LicenseId", licenseId);
                }
                else{
                    root.model.SetData("LicenseId", "");
                }

                if (deviceCB.model.ContainsKey("LicenseName", deviceCB.currentIndex)){
                    let licenseName = deviceCB.model.GetData("LicenseName", deviceCB.currentIndex);
                    root.model.SetData("LicenseName", licenseName);
                }
                else{
                    root.model.SetData("LicenseName", "");
                }

                if (deviceCB.model.ContainsKey("MacAddress", deviceCB.currentIndex)){
                    let macAddress = deviceCB.model.GetData("MacAddress", deviceCB.currentIndex);
                    root.model.SetData("MacAddress", macAddress);
                }
                else{
                    root.model.SetData("MacAddress", "");
                }

                if (deviceCB.model.ContainsKey("SerialNumber", deviceCB.currentIndex)){
                    let serialNumber = deviceCB.model.GetData("SerialNumber", deviceCB.currentIndex);
                    root.model.SetData("SerialNumber", serialNumber);
                }
                else{
                    root.model.SetData("SerialNumber", "");
                }
            }
            else{
                root.model.SetData("LicenseUuid", "");
                root.model.SetData("LicenseId", "");
                root.model.SetData("LicenseName", "");
                root.model.SetData("MacAddress", "");
                root.model.SetData("SerialNumber", "");
            }
        }
    }

    Column {
        id: content;

        width: parent.width;

        spacing: Style.size_mainMargin;

        SwitchElementView {
            id: switchNewSensor;

            width: parent.width;

            name: qsTr("New Sensor");

            visible: root.productIndex == -1 || root.model.GetData("IsNew");

            onCheckedChanged: {
                deviceCB.visible = !checked;

                root.doUpdateModel();
            }
        }

        FilterableComboBoxElementView {
            id: deviceCB;

            width: parent.width;

            model: root.devicesModel;

            name: qsTr("Hardware-ID");
            nameId: "DeviceType";

            bottomComp: currentIndex < 0 ? sensorErrorComp : undefined;

            filteringFields: ["SMacAddress", "DeviceType"];

            delegate: Component {
                FilterableComboBoxDelegate {
                    width: deviceCB.width;
                    comboBoxRef: deviceCB.cbRef;

                    description: model.MacAddress === "" ? qsTr("MAC Address") + ": " + qsTr("not specified"): qsTr("MAC Address") + ": " + model.MacAddress;
                }
            }

            onCurrentIndexChanged: {
                if (deviceCB.currentIndex >= 0 && deviceCB.model){
                    if (deviceCB.model.ContainsKey("LicenseName", deviceCB.currentIndex)){
                        let licenseName = deviceCB.model.GetData("LicenseName", deviceCB.currentIndex)

                        deviceTypeText.text = licenseName;
                    }

                    if (deviceCB.model.ContainsKey("MacAddress", deviceCB.currentIndex)){
                        let macAddress = deviceCB.model.GetData("MacAddress", deviceCB.currentIndex)

                        macAddressText.text = macAddress;
                    }
                }

                root.doUpdateModel();
            }
        }

        Component {
            id: sensorErrorComp;

            Text {
                id: selectSensorText;

                text: qsTr("Please select a sensor");
                color: Style.errorTextColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }
        }

        Column {
            width: parent.width;

            spacing: content.spacing;

            visible: root.isNewDevice;

            FilterableComboBoxElementView {
                id: typesCB;

                width: parent.width;

                model: root.productLicensesModel;

                name: qsTr("Types");
                nameId: "LicenseName";

                bottomComp: currentIndex < 0 ? typeSensorErrorComp : undefined;

                onCurrentIndexChanged: {
                    root.doUpdateModel();
                }
            }

            Component {
                id: typeSensorErrorComp;

                Text {
                    id: selectTypeText;

                    text: qsTr("Please select a type sensor");
                    color: Style.errorTextColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }
            }

            MacAddressElementView {
                id: macAddressInput;

                width: parent.width;

                readOnly: root.readOnly;

                onEditingFinished: {
                    root.doUpdateModel();
                }
            }
        }

        Column {
            width: parent.width;

            spacing: content.spacing;

            visible: !root.isNewDevice;

            TextElementView {
                id: deviceTypeText;

                width: parent.width;

                name: qsTr("Type");
            }

            TextElementView {
                id: macAddressText;

                width: parent.width;

                name: qsTr("MAC Address");
            }
        }
    }
}//Container


