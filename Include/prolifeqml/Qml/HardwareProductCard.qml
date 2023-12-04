import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Rectangle {
    id: hardwareCard;

    height: hardwareCard.contentHeight;

    color: Style.baseColor;

    property bool isNewDevice: model.IsNewDevice ? model.IsNewDevice : false;
    property bool notExists: model.DeviceNotExists ? model.DeviceNotExists : false;
    property bool checker: hardwareCard.productCardRoot != null && hardwareCard.notExists;
    property bool readOnly: false;
    property bool commmandsVisible: false;

    property string productId: model.ProductUuid ? model.ProductUuid : "";
    property string macAddress: hardwareCard.isNewDevice ? qsTr("New Sensor") : model.MacAddress ? model.MacAddress : "";
    property string serialNumber: hardwareCard.isNewDevice ? qsTr("New Sensor") : model.SerialNumber ? model.SerialNumber : "";
    property string modelType: model.LicenseUuid ? model.LicenseUuid : "";

    property Item productCardRoot: null;

    property int contentHeight: contentColumn.height + 20

    signal clicked();
    signal edited();

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", hardwareCard.onLocalizationChanged);

        hardwareCard.updateElements();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", hardwareCard.onLocalizationChanged);
    }

    onCheckerChanged: {
        if (checker){
            let message = qsTr("Sensor detection error. Please select a new sensor.");
            hardwareCard.productCardRoot.showErrorMessage(message);
        }
    }

    function onLocalizationChanged(language){
        hardwareCard.updateHeaders();

        hardwareCard.updateElements();
    }

    function updateElements(){
        elementsTableModel.Clear();

        let index = elementsTableModel.InsertNewItem();
        elementsTableModel.SetData("Key", qsTr("MAC Address"), index)
        elementsTableModel.SetData("Value", hardwareCard.macAddress, index)

        index = elementsTableModel.InsertNewItem();
        elementsTableModel.SetData("Key", qsTr("Serial Number"), index)
        elementsTableModel.SetData("Value", hardwareCard.serialNumber, index)

        index = elementsTableModel.InsertNewItem();
        elementsTableModel.SetData("Key", qsTr("Model Type"), index)
        elementsTableModel.SetData("Value", model.LicenseName, index)

        table.elements = elementsTableModel;
    }

    TreeItemModel {
        id: headersTableModel;

        Component.onCompleted: {
            hardwareCard.updateHeaders();
        }
    }

    function updateHeaders(){
        headersTableModel.Clear();

        let index = headersTableModel.InsertNewItem();

        headersTableModel.SetData("Id", "Key", index)
        headersTableModel.SetData("Name", qsTr("Key"), index)

        index = headersTableModel.InsertNewItem();

        headersTableModel.SetData("Id", "Value", index)
        headersTableModel.SetData("Name", qsTr("Value"), index)

        table.headers = headersTableModel;
    }

    TreeItemModel {
        id: elementsTableModel;
    }

    Item {
        id: rightPanel;

        anchors.top: parent.top;
        anchors.right: parent.right;

        width: visible ? 30 : 0;
        height: parent.height;

        visible: editButton.visible;

        AuxButton {
            id: editButton;

            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.top: parent.top;
            anchors.topMargin: 10;

            width: 18;
            height: width;

            iconSource: enabled ? "../../../../" + Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal) :
                                  "../../../../" + Style.getIconPath("Icons/Edit", Icon.State.Off, Icon.Mode.Disabled);
            visible: !hardwareCard.readOnly && hardwareCard.commmandsVisible;

            onClicked: {
                hardwareCard.edited();
            }
        }
    }

    Column {
        id: contentColumn;

        anchors.top: parent.top;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: rightPanel.left;
        anchors.rightMargin: rightPanel.visible ? 0 : 10;

        AuxTable {
            id: table;

            width: contentColumn.width;
            height: contentHeight;

            radius: 0;

            selectable: false;
            separatorVisible: false;

            itemHeight: 25;
            headerHeight: 20;

            enableAlternating: false;

            clip: true;

            showHeaders: false;
        }
    }
} //Card


