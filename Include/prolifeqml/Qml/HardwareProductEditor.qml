import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Item {
    id: root;

    property int itemHeight: 23;
    property int margin: 10;

    property TreeItemModel productModel: TreeItemModel {}
    property var productLicensesModel: TreeItemModel{}
    property TreeItemModel devicesModel: TreeItemModel{}

    property alias deviceIndex: deviceCB.currentIndex;

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", root.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", root.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        root.updateHeaders();
    }

    Text {
        id: deviceText;

        anchors.top: parent.top;
        text: qsTr("Device");
        color: Style.textColor;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSize_common;
    }

    ComboBox {
        id: deviceCB;

        anchors.top: deviceText.bottom;
        anchors.topMargin: root.margin;

        height: root.itemHeight;
        width: parent.width;

        radius: 3;

        model: root.devicesModel;

        onCurrentIndexChanged: {
            console.log("deviceCB onCurrentIndexChanged", deviceCB.currentIndex)

            if (root.blockUpdatingModel){
                return;
            }

            if (deviceCB.currentIndex >= 0){
                let isNew = deviceCB.currentIndex === 0;
                if (isNew){
                    root.productModel.SetData("IsNewDevice", true);
                }
                else{
                    root.productModel.RemoveData("IsNewDevice");
                }

                if (root.productModel.ContainsKey("DeviceNotExists")){
                    root.productModel.RemoveData("DeviceNotExists");
                }

                let deviceId = deviceCB.model.GetData("Id", deviceCB.currentIndex);
                root.productModel.SetData("DeviceId", deviceId);

                if (deviceCB.model.ContainsKey("LicenseUuid", deviceCB.currentIndex)){
                    let configurationType = deviceCB.model.GetData("LicenseUuid", deviceCB.currentIndex);
                    root.productModel.SetData("LicenseUuid", configurationType);
                }

                if (deviceCB.model.ContainsKey("LicenseId", deviceCB.currentIndex)){
                    let licenseId = deviceCB.model.GetData("LicenseId", deviceCB.currentIndex);
                    root.productModel.SetData("LicenseId", licenseId);
                }

                if (deviceCB.model.ContainsKey("LicenseName", deviceCB.currentIndex)){
                    let licenseName = deviceCB.model.GetData("LicenseName", deviceCB.currentIndex);
                    root.productModel.SetData("LicenseName", licenseName);
                }

                if (deviceCB.model.ContainsKey("MacAddress", deviceCB.currentIndex)){
                    let macAddress = deviceCB.model.GetData("MacAddress", deviceCB.currentIndex);
                    root.productModel.SetData("MacAddress", macAddress);
                }

                if (deviceCB.model.ContainsKey("SerialNumber", deviceCB.currentIndex)){
                    let serialNumber = deviceCB.model.GetData("SerialNumber", deviceCB.currentIndex);
                    root.productModel.SetData("SerialNumber", serialNumber);
                }

                root.updateModelsGui();
            }
        }
    }

    Text {
        id: selectSensorText;

        anchors.top: deviceCB.bottom;
        anchors.topMargin: root.margin;

        text: qsTr("Please select a sensor");
        color: Style.errorTextColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;

        visible: deviceCB.currentIndex < 0;
    }

    Text {
        id: licensesText;

        anchors.top: selectSensorText.visible ? selectSensorText.bottom : deviceCB.bottom;
        anchors.topMargin: root.margin;

        text: qsTr("Models");
        color: Style.textColor;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSize_common;
    }

    AuxTable {
        id: modelsTable;

        anchors.top: licensesText.bottom;
        anchors.topMargin: root.margin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: root.margin;

        width: parent.width;

        radius: 0;

        checkable: true;
        canSelectAll: false;
        isMultiCheckable: false;

        elements: root.productLicensesModel;

        readOnly: deviceCB.currentIndex !== 0;

        onReadOnlyChanged: {
            console.log("onReadOnlyChanged", modelsTable.readOnly);
        }

        onCheckedItemsChanged: {
            if (root.blockUpdatingModel){
                return;
            }

            let uuid = "";
            let licenseId = "";
            let licenseName = "";

            let checkedIndexes = modelsTable.getCheckedItems();
            if (checkedIndexes.length > 0){
                let index = checkedIndexes[0];

                uuid = root.productLicensesModel.GetData("Id", index);
                licenseId = root.productLicensesModel.GetData("LicenseId", index);
                licenseName = root.productLicensesModel.GetData("LicenseName", index);
            }

            root.productModel.SetData("LicenseUuid", uuid);
            root.productModel.SetData("LicenseId", licenseId);
            root.productModel.SetData("LicenseName", licenseName);
        }
    }

    property bool blockUpdatingModel: false;

    function updateModel(){
        if (root.blockUpdatingModel){
            return;
        }
    }

    function updateGui(){
        console.log("HardwareProductEditor updateGui");

        blockUpdatingModel = true;

        deviceCB.currentIndex = -1;
        if (root.productModel.ContainsKey("DeviceId")){
            let deviceId = root.productModel.GetData("DeviceId");
            let deviceModel = deviceCB.model;
            for (let i = 0; i < deviceModel.GetItemsCount(); i++){
                let id = deviceModel.GetData("Id", i);
                if (id === deviceId){
                    if (deviceModel.ContainsKey("LicenseUuid", i)){
                        let configurationType = deviceModel.GetData("LicenseUuid", i);
                        if (configurationType && configurationType != ""){
                            root.productModel.SetData("LicenseUuid", configurationType);
                        }
                    }

                    deviceCB.currentIndex = i;
                    break;
                }
            }
        }

        updateModelsGui();

        blockUpdatingModel = false;
    }

    function updateModelsGui(){
        blockUpdatingModel = true;

        modelsTable.uncheckAll();

        if (root.productLicensesModel){
            if (root.productModel.ContainsKey("LicenseUuid")){
                let modelTypeId = root.productModel.GetData("LicenseUuid");

                for (let i = 0; i < root.productLicensesModel.GetItemsCount(); i++){
                    let id = root.productLicensesModel.GetData("Id", i);
                    if (id === modelTypeId){
                        modelsTable.checkItem(i);
                        break;
                    }
                }
            }
        }

        blockUpdatingModel = false;
    }

    TreeItemModel {
        id: headersModel;
        Component.onCompleted: {
            root.updateHeaders();
        }
    }

    function updateHeaders(){
        headersModel.Clear();

        let index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "LicenseName", index)
        headersModel.SetData("Name", qsTr("Model Name"), index)

        index = headersModel.InsertNewItem();
        headersModel.SetData("Id", "LicenseId", index)
        headersModel.SetData("Name", qsTr("Model-ID"), index)

        modelsTable.headers = headersModel;
    }
}//Container


