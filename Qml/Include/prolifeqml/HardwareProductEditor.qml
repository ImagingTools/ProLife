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

    property bool isNewProduct: root.model.getData("IsNew") ? root.model.getData("IsNew") : false;

    function updateGui(){
        let isNew = model.getData("IsNew")
        if (isNew){
            switchNewSensor.checked = true;

            macAddressInput.text = "";
            serialNumberInput.text = "";

            if (root.model.containsKey("MacAddress")){
                let macAddress = root.model.getData("MacAddress");
                macAddressInput.text = macAddress;
            }

            if (root.model.containsKey("SerialNumber")){
                let serialNumber = root.model.getData("SerialNumber");
                serialNumberInput.text = serialNumber;
            }

            typesCB.currentIndex = -1;

            if (root.model.containsKey("LicenseUuid")){
                let licenseUuid = root.model.getData("LicenseUuid");

                if (typesCB.model){
                    for (let i = 0; i < typesCB.model.getItemsCount(); i++){
                        let id = typesCB.model.getData("Id", i);
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
            if (root.model.containsKey("Id")){
                let deviceId = root.model.getData("Id")
                if (deviceCB.model){
                    for (let i = 0; i < deviceCB.model.getItemsCount(); i++){
                        let id = deviceCB.model.getData("Id", i);
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
        root.model.setData("IsNew", isNewDevice);

        if (isNewDevice){
            if (typesCB.currentIndex >= 0){
                let index = typesCB.currentIndex;

                let uuid = root.productLicensesModel.getData("Id", index);
                let licenseId = root.productLicensesModel.getData("LicenseId", index);
                let licenseName = root.productLicensesModel.getData("LicenseName", index);

                root.model.setData("LicenseUuid", uuid);
                root.model.setData("LicenseId", licenseId);
                root.model.setData("LicenseName", licenseName);
            }
            else{
                root.model.setData("LicenseUuid", "");
                root.model.setData("LicenseId", "");
                root.model.setData("LicenseName", "");
            }

            root.model.setData("MacAddress", macAddressInput.text);
            root.model.setData("SerialNumber", serialNumberInput.text);
        }
        else{
            if (deviceCB.currentIndex >= 0){
                let deviceId = deviceCB.model.getData("Id", deviceCB.currentIndex);
                root.model.setData("Id", deviceId);

                if (deviceCB.model.containsKey("LicenseUuid", deviceCB.currentIndex)){
                    let configurationType = deviceCB.model.getData("LicenseUuid", deviceCB.currentIndex);
                    root.model.setData("LicenseUuid", configurationType);
                }
                else{
                    root.model.setData("LicenseUuid", "");
                }

                if (deviceCB.model.containsKey("LicenseId", deviceCB.currentIndex)){
                    let licenseId = deviceCB.model.getData("LicenseId", deviceCB.currentIndex);
                    root.model.setData("LicenseId", licenseId);
                }
                else{
                    root.model.setData("LicenseId", "");
                }

                if (deviceCB.model.containsKey("LicenseName", deviceCB.currentIndex)){
                    let licenseName = deviceCB.model.getData("LicenseName", deviceCB.currentIndex);
                    root.model.setData("LicenseName", licenseName);
                }
                else{
                    root.model.setData("LicenseName", "");
                }

                if (deviceCB.model.containsKey("MacAddress", deviceCB.currentIndex)){
                    let macAddress = deviceCB.model.getData("MacAddress", deviceCB.currentIndex);
                    root.model.setData("MacAddress", macAddress);
                }
                else{
                    root.model.setData("MacAddress", "");
                }

                if (deviceCB.model.containsKey("SerialNumber", deviceCB.currentIndex)){
                    let serialNumber = deviceCB.model.getData("SerialNumber", deviceCB.currentIndex);
                    root.model.setData("SerialNumber", serialNumber);
                }
                else{
                    root.model.setData("SerialNumber", "");
                }
            }
            else{
                root.model.setData("LicenseUuid", "");
                root.model.setData("LicenseId", "");
                root.model.setData("LicenseName", "");
                root.model.setData("MacAddress", "");
                root.model.setData("SerialNumber", "");
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

            visible: root.productIndex == -1 || root.model.getData("IsNew");

            onCheckedChanged: {
                deviceCB.visible = !checked;

                root.doUpdateModel();
            }
        }

        FilterableComboBoxElementView {
            id: deviceCB;

            width: parent.width;
            controlWidth: 500;

            model: root.devicesModel;

            name: qsTr("Hardware-ID");
            nameId: "DeviceType";

            bottomComp: currentIndex < 0 ? sensorErrorComp : undefined;

            // SMacAddress1 - sxxxxxxxxxxxx
            // SMacAddress2 - s:xxxxxxxxxxxx
            // SMacAddress3 - s:xx:xx:xx:xx:xx:xx
            // SMacAddress4 - sxx:xx:xx:xx:xx:xx

            filteringFields: ["SMacAddress1", "SMacAddress2", "SMacAddress3", "SMacAddress4", "DeviceType", "MacAddress"];

            delegate: Component {
                FilterableComboBoxDelegate {
                    width: comboBoxRef ? comboBoxRef.width : 0;
                    comboBoxRef: deviceCB.cbRef;

                    description: model.MacAddress === "" ? qsTr("MAC Address") + ": " + qsTr("not specified"): qsTr("MAC Address") + ": " + model.MacAddress;
                }
            }

            onCurrentIndexChanged: {
                if (deviceCB.currentIndex >= 0 && deviceCB.model){
                    if (deviceCB.model.containsKey("LicenseName", deviceCB.currentIndex)){
                        let licenseName = deviceCB.model.getData("LicenseName", deviceCB.currentIndex)

                        deviceTypeText.text = licenseName;
                    }

                    if (deviceCB.model.containsKey("MacAddress", deviceCB.currentIndex)){
                        let macAddress = deviceCB.model.getData("MacAddress", deviceCB.currentIndex)

                        macAddressText.text = macAddress;
                    }

                    if (deviceCB.model.containsKey("SerialNumber", deviceCB.currentIndex)){
                        let serialNumber = deviceCB.model.getData("SerialNumber", deviceCB.currentIndex)

                        serialNumberText.text = serialNumber;
                    }

                    if (deviceCB.model.containsKey("LicenseId", deviceCB.currentIndex)){
                        let licenseId = deviceCB.model.getData("LicenseId", deviceCB.currentIndex)

                        articulText.text = licenseId;
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

            spacing: parent.spacing;

            visible: root.isNewDevice;

            FilterableComboBoxElementView {
                id: typesCB;

                width: parent.width;
                controlWidth: 500;

                model: root.productLicensesModel;

                name: qsTr("Types");
                nameId: "LicenseName";

                bottomComp: currentIndex < 0 ? typeSensorErrorComp : undefined;
                filteringFields: ["LicenseName", "LicenseId"];

                onCurrentIndexChanged: {
                    if (currentIndex >= 0){
                        if (typesCB.model.containsKey("LicenseId", currentIndex)){
                            let licenseId = typesCB.model.getData("LicenseId", currentIndex)

                            newArticulText.text = licenseId;
                        }
                    }

                    root.doUpdateModel();
                }

                delegate: Component {
                    FilterableComboBoxDelegate {
                        width: comboBoxRef ? comboBoxRef.width : 0;
                        comboBoxRef: typesCB.cbRef;

                        text: model.LicenseName;
                        description: model.LicenseId;
                    }
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
                controlWidth: 500;

                readOnly: root.readOnly;

                visible: parent.visible && typesCB.currentIndex >= 0;

                onEditingFinished: {
                    root.doUpdateModel();
                }
            }

            TextInputElementView {
                id: serialNumberInput;

                width: parent.width;
                controlWidth: 500;

                name: qsTr("Serial Number");
                placeHolderText: qsTr("Enter the serial number");

                readOnly: root.readOnly;

                visible: parent.visible && typesCB.currentIndex >= 0;

                onEditingFinished: {
                    root.doUpdateModel();
                }
            }

            TextElementView {
                id: newArticulText;

                width: parent.width;

                visible: parent.visible && typesCB.currentIndex >= 0;

                name: qsTr("Article Number");
            }
        }

        Column {
            width: parent.width;

            spacing: parent.spacing;

            visible: !root.isNewDevice;

            TextElementView {
                id: deviceTypeText;

                width: parent.width;

                name: qsTr("Type");

                visible: parent.visible && deviceCB.currentIndex >= 0;
            }

            TextElementView {
                id: macAddressText;

                width: parent.width;

                name: qsTr("MAC Address");

                visible: parent.visible && deviceCB.currentIndex >= 0;
            }

            TextElementView {
                id: serialNumberText;

                width: parent.width;

                name: qsTr("Serial Number");

                visible: parent.visible && deviceCB.currentIndex >= 0;
            }

            TextElementView {
                id: articulText;

                width: parent.width;

                name: qsTr("Article Number");

                visible: parent.visible && deviceCB.currentIndex >= 0;
            }
        }
    }
}//Container


