import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import prolifeOrdersSdl 1.0
import prolifeSensorsSdl 1.0

ViewBase {
    id: root;

    height: content.height;

    property var productLicensesModel: TreeItemModel{}
    property TreeItemModel devicesModel: TreeItemModel{}
    property alias deviceIndex: deviceCB.currentIndex;
    property bool isNewDevice: switchNewSensor.checked;
    property int productIndex: -1;
    property OrderedProduct productItem: model ? model : null;

    function updateGui(){
        let isNew = productItem.m_isNew;
        if (isNew){
            switchNewSensor.checked = true;

            macAddressInput.text = productItem.m_macAddress;
            serialNumberInput.text = productItem.m_serialNumber;

            typesCB.currentIndex = -1;

            let licenseUuid = productItem.m_licenseUuid;
			if (typesCB.sourceModel){
				for (let i = 0; i < typesCB.sourceModel.getItemsCount(); i++){
					let id = typesCB.sourceModel.getData(DeviceItemTypeMetaInfo.s_id, i);
                    if (id === licenseUuid){
                        typesCB.currentIndex = i;
                        break;
                    }
                }
            }
        }
        else{
            switchNewSensor.checked = false;

            deviceCB.currentIndex = -1;
            let deviceId = productItem.m_id;

			if (deviceCB.sourceModel){
				for (let i = 0; i < deviceCB.sourceModel.getItemsCount(); i++){
					let id = deviceCB.sourceModel.getData(DeviceItemTypeMetaInfo.s_id, i);

                    if (id === deviceId){
                        deviceCB.currentIndex = i;
                        break;
                    }
                }
            }
        }
    }

    function updateModel(){
        productItem.m_isNew = isNewDevice;

        if (isNewDevice){
            if (typesCB.currentIndex >= 0){
                productItem.m_licenseUuid = root.productLicensesModel.getData(DeviceItemTypeMetaInfo.s_id, typesCB.currentIndex);
                productItem.m_licenseId = root.productLicensesModel.getData(DeviceItemTypeMetaInfo.s_licenseId, typesCB.currentIndex);
                productItem.m_licenseName = root.productLicensesModel.getData(DeviceItemTypeMetaInfo.s_licenseName, typesCB.currentIndex);
            }
            else{
                productItem.m_licenseUuid = "";
                productItem.m_licenseId = "";
                productItem.m_licenseName = "";
            }

            productItem.m_macAddress = macAddressInput.text;
            productItem.m_serialNumber = serialNumberInput.text;
        }
        else{
            if (deviceCB.currentIndex >= 0){
				let deviceId = deviceCB.sourceModel.getData(DeviceItemTypeMetaInfo.s_id, deviceCB.currentIndex);

                productItem.m_id = deviceId;

				if (deviceCB.sourceModel.containsKey(DeviceItemTypeMetaInfo.s_licenseUuid, deviceCB.currentIndex)){
					let configurationType = deviceCB.sourceModel.getData(DeviceItemTypeMetaInfo.s_licenseUuid, deviceCB.currentIndex);
                    productItem.m_licenseUuid = configurationType;
                }
                else{
                    productItem.m_licenseUuid = "";
                }

				if (deviceCB.sourceModel.containsKey(DeviceItemTypeMetaInfo.s_licenseId, deviceCB.currentIndex)){
					let licenseId = deviceCB.sourceModel.getData(DeviceItemTypeMetaInfo.s_licenseId, deviceCB.currentIndex);
                    productItem.m_licenseId = licenseId;
                }
                else{
                    productItem.m_licenseId = "";
                }

				if (deviceCB.sourceModel.containsKey(DeviceItemTypeMetaInfo.s_licenseName, deviceCB.currentIndex)){
					let licenseName = deviceCB.sourceModel.getData(DeviceItemTypeMetaInfo.s_licenseName, deviceCB.currentIndex);
                    productItem.m_licenseName = licenseName;
                }
                else{
                    productItem.m_licenseName = "";
                }

				if (deviceCB.sourceModel.containsKey(DeviceItemTypeMetaInfo.s_macAddress, deviceCB.currentIndex)){
					let macAddress = deviceCB.sourceModel.getData(DeviceItemTypeMetaInfo.s_macAddress, deviceCB.currentIndex);
                    productItem.m_macAddress = macAddress;
                }
                else{
                    productItem.m_macAddress = "";
                }

				if (deviceCB.sourceModel.containsKey(DeviceItemTypeMetaInfo.s_serialNumber, deviceCB.currentIndex)){
					let serialNumber = deviceCB.sourceModel.getData(DeviceItemTypeMetaInfo.s_serialNumber, deviceCB.currentIndex);
                    productItem.m_serialNumber = serialNumber;
                }
                else{
                    productItem.m_serialNumber = "";
                }
            }
            else{
                productItem.m_licenseUuid = "";
                productItem.m_licenseId = "";
                productItem.m_licenseName = "";
                productItem.m_macAddress = "";
                productItem.m_serialNumber = "";
            }
        }
    }

    Column {
        id: content;
        width: parent.width;
        spacing: Style.sizeMainMargin;

        SwitchElementView {
            id: switchNewSensor;
            width: parent.width;
            name: qsTr("New Sensor");
            visible: root.productIndex == -1 || root.isNewDevice;

            onCheckedChanged: {
                deviceCB.visible = !checked;

                root.doUpdateModel();
            }

            Component.onCompleted: {
                let canAddSensor = PermissionsController.checkPermission("AddSensor");
                if (!canAddSensor){
                    switchNewSensor.visible = false;
                }
            }
        }

        FilterableComboBoxElementView {
            id: deviceCB;
            width: parent.width;
            controlWidth: 500;
			sourceModel: root.devicesModel;
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
					text: model.DeviceType;
                }
            }

            onCurrentIndexChanged: {
				if (deviceCB.currentIndex >= 0 && deviceCB.sourceModel){
					if (deviceCB.sourceModel.containsKey(DeviceItemTypeMetaInfo.s_licenseName, deviceCB.currentIndex)){
						let licenseName = deviceCB.sourceModel.getData(DeviceItemTypeMetaInfo.s_licenseName, deviceCB.currentIndex)

                        deviceTypeText.text = licenseName;
                    }

					if (deviceCB.sourceModel.containsKey(DeviceItemTypeMetaInfo.s_macAddress, deviceCB.currentIndex)){
						let macAddress = deviceCB.sourceModel.getData(DeviceItemTypeMetaInfo.s_macAddress, deviceCB.currentIndex)

                        macAddressText.text = macAddress;
                    }

					if (deviceCB.sourceModel.containsKey(DeviceItemTypeMetaInfo.s_serialNumber, deviceCB.currentIndex)){
						let serialNumber = deviceCB.sourceModel.getData(DeviceItemTypeMetaInfo.s_serialNumber, deviceCB.currentIndex)

                        serialNumberText.text = serialNumber;
                    }

					if (deviceCB.sourceModel.containsKey(DeviceItemTypeMetaInfo.s_licenseId, deviceCB.currentIndex)){
						let licenseId = deviceCB.sourceModel.getData(DeviceItemTypeMetaInfo.s_licenseId, deviceCB.currentIndex)

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
                font.pixelSize: Style.fontSizeXSmall;
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
				sourceModel: root.productLicensesModel;
                name: qsTr("Types");
                nameId: DeviceItemTypeMetaInfo.s_licenseName;
                bottomComp: currentIndex < 0 ? typeSensorErrorComp : undefined;
                filteringFields: [DeviceItemTypeMetaInfo.s_licenseName, DeviceItemTypeMetaInfo.s_licenseId];

                onCurrentIndexChanged: {
                    if (currentIndex >= 0){
						if (typesCB.sourceModel.containsKey(DeviceItemTypeMetaInfo.s_licenseId, currentIndex)){
							let licenseId = typesCB.sourceModel.getData(DeviceItemTypeMetaInfo.s_licenseId, currentIndex)
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
                    font.pixelSize: Style.fontSizeXSmall;
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


