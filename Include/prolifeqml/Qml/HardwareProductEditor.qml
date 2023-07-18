import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Item {
    id: root;

    property int itemHeight: 23;
    property int margin: 10;

    property TreeItemModel productModel: TreeItemModel {}
    property var productLicensesModel: TreeItemModel{}
    property TreeItemModel devicesModel: TreeItemModel{}

    property alias deviceIndex: deviceCB.currentIndex;

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
            if (root.blockUpdatingModel){
                return;
            }

            if (deviceCB.currentIndex >= 0){
                let isNew = deviceCB.model.ContainsKey("IsNew", deviceCB.currentIndex);
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

        onCheckedItemsChanged: {
            if (root.blockUpdatingModel){
                return;
            }

            let checkedIndexes = modelsTable.getCheckedItems();
            if (checkedIndexes.length > 0){
                let index = checkedIndexes[0];
                let id = root.productLicensesModel.GetData("Id", index);
                root.productModel.SetData("ModelTypeId", id);
            }
            else{
                if (root.productModel.ContainsKey("ModelTypeId")){
                    root.productModel.RemoveData("ModelTypeId");
                }
            }
        }
    }

    property bool blockUpdatingModel: false;

    function updateModel(){
        if (root.blockUpdatingModel){
            return;
        }
    }

    function updateGui(){
        blockUpdatingModel = true;

        deviceCB.currentIndex = -1;
        if (root.productModel.ContainsKey("DeviceId")){
            let deviceId = root.productModel.GetData("DeviceId");
            let deviceModel = deviceCB.model;
            for (let i = 0; i < deviceModel.GetItemsCount(); i++){
                let id = deviceModel.GetData("Id", i);
                if (id === deviceId){
                    deviceCB.currentIndex = i;
                    break;
                }
            }
        }

        modelsTable.uncheckAll();

        if (root.productLicensesModel){
            if (root.productModel.ContainsKey("ModelTypeId")){
                let modelTypeId = root.productModel.GetData("ModelTypeId");

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
            let index = headersModel.InsertNewItem();
            headersModel.SetData("Id", "Name", index)
            headersModel.SetData("Name", "Model Name", index)

            index = headersModel.InsertNewItem();
            headersModel.SetData("Id", "Id", index)
            headersModel.SetData("Name", "Model-ID", index)

//            index = headersModel.InsertNewItem();
//            headersModel.SetData("Id", "Description", index)
//            headersModel.SetData("Name", "Description", index)

            modelsTable.headers = headersModel;
        }
    }
}//Container


