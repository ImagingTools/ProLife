import QtQuick 2.0
import imtgui 1.0
import Acf 1.0

Rectangle {
    id: hardwareCard;

    height: hardwareCard.contentHeight;

    color: Style.baseColor;

    property string productId: model.ProductId ? model.ProductId : "";
    property bool isNewDevice: model.IsNewDevice ? model.IsNewDevice : false;

    property string macAddress: hardwareCard.isNewDevice ? qsTr("New Sensor") : model.MacAddress ? model.MacAddress : "";
    property string serialNumber: hardwareCard.isNewDevice ? qsTr("New Sensor") : model.SerialNumber ? model.SerialNumber : "";
    property string modelType: model.ModelTypeId ? model.ModelTypeId : "";

    property bool notExists: model.DeviceNotExists ? model.DeviceNotExists : false;

    property Item productCardRoot: null;

    property bool readOnly: false;
    property bool commmandsVisible: false;

    property int contentHeight: contentColumn.height + 20;

    signal clicked();
    signal edited();

    property bool checker: hardwareCard.productCardRoot != null && hardwareCard.notExists;
    onCheckerChanged: {
        if (checker){
            let message = qsTr("Sensor detection error. Please select a new sensor.");
            hardwareCard.productCardRoot.showErrorMessage(message);
        }
    }

    onMacAddressChanged: {
        updateElements();
    }

    onSerialNumberChanged: {
        updateElements();
    }

    onModelTypeChanged: {
        updateElements();
    }

    function updateElements(){
        elementsTableModel.Clear();

        let index = elementsTableModel.InsertNewItem();
        elementsTableModel.SetData("Key", "MAC Address", index)
        elementsTableModel.SetData("Value", hardwareCard.macAddress, index)

        index = elementsTableModel.InsertNewItem();
        elementsTableModel.SetData("Key", "Serial Number", index)
        elementsTableModel.SetData("Value", hardwareCard.serialNumber, index)

        index = elementsTableModel.InsertNewItem();
        elementsTableModel.SetData("Key", "Model Type", index)
        elementsTableModel.SetData("Value", hardwareCard.modelType, index)

        table.elements = elementsTableModel;
    }

    TreeItemModel {
        id: headersTableModel;

        Component.onCompleted: {
            let index = headersTableModel.InsertNewItem();

            headersTableModel.SetData("Id", "Key", index)
            headersTableModel.SetData("Name", "Key", index)

            index = headersTableModel.InsertNewItem();

            headersTableModel.SetData("Id", "Value", index)
            headersTableModel.SetData("Name", "Value", index)

            table.headers = headersTableModel;
        }
    }

    TreeItemModel {
        id: elementsTableModel;
    }

    Column {
        id: contentColumn;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;

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

    AuxButton {
        id: editButton;

        anchors.top: parent.top;
        anchors.topMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;

        width: 18;
        height: width;

        iconSource: enabled ? "../../../../Icons/Light/Edit_Off_Normal.svg" :
                              "../../../../Icons/Light/Edit_Off_Disabled.svg";
        visible: !hardwareCard.readOnly && hardwareCard.commmandsVisible;

        onClicked: {
            hardwareCard.edited();
        }
    }

    MouseArea {
        id: mouseArea;

        anchors.fill: parent;
        cursorShape: Qt.PointingHandCursor;

        hoverEnabled: true;

        visible: false;

        onClicked: {
            hardwareCard.clicked();
        }

        onEntered: {
            console.log("onEntered");

            hardwareCard.scale = 1.03;
        }

        onExited: {
            console.log("onExited");
            hardwareCard.scale = 1;
        }
    }

} //Card


