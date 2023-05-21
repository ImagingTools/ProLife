import QtQuick 2.0
import imtgui 1.0
import Acf 1.0

Rectangle {
    id: hardwareCard;

    height: header.height + macAddressBlock.height + serialNumberBlock.height + 40;

    radius: 10;
    color: Style.backgroundColor;

    property string productId: model.ProductId ? model.ProductId : "";

    property string deviceId: model.DeviceId ? model.DeviceId : "";

    property string macAddress;
    property string serialNumber;

    property TreeItemModel devicesModel: TreeItemModel {};

    property bool readOnly: false;
    property bool commmandsVisible: false;

    signal clicked();
    signal edited();

    onDevicesModelChanged: {
        console.log("onDevicesModelChanged", devicesModel.toJSON());

        if (hardwareCard.devicesModel != null){
            for (let i = 0; i < hardwareCard.devicesModel.GetItemsCount(); i++){
                let id = hardwareCard.devicesModel.GetData("Id", i);
                if (id === hardwareCard.deviceId){
                    let macAddress = hardwareCard.devicesModel.GetData("MacAddress", i);
                    hardwareCard.macAddress = macAddress;

                    let serialNumber = hardwareCard.devicesModel.GetData("SerialNumber", i);
                    hardwareCard.serialNumber = serialNumber;
                }
            }
        }
    }

    Rectangle {
        id: header;

        anchors.top: parent.top;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;

        height: 30;

        radius: hardwareCard.radius;
        color: Style.imagingToolsGradient2;

        Text {
            id: productTitle;

            anchors.left: parent.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: editButton.left;
            anchors.rightMargin: 10;

            text: hardwareCard.productId;
            color: Style.textColor;
            font.family: Style.fontFamilyBold;
            font.pixelSize: Style.fontSize_common;
            elide: Text.ElideRight;
            wrapMode: Text.NoWrap;
        }

        AuxButton {
            id: editButton;

            anchors.verticalCenter: parent.verticalCenter;
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
    }

    Item {
        id: macAddressBlock;

        anchors.top: header.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;

        height: macAddressTitle.height;

        Text {
            id: macAddressTitle;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;

            text: qsTr("Mac address:")
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            font.bold: true;
        }

        Text {
            id: macAddress;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: macAddressTitle.right;
            anchors.leftMargin: 10;
            anchors.right: parent.right;

            elide: Text.ElideRight;
            wrapMode: Text.NoWrap;

            text: hardwareCard.macAddress;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }
    }

    Item {
        id: serialNumberBlock;

        anchors.top: macAddressBlock.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;

        height: serialNumberTitle.height;

        Text {
            id: serialNumberTitle;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;

            text: qsTr("Serial Number:")
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            font.bold: true;
        }

        Text {
            id: serialNumber;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: serialNumberTitle.right;
            anchors.leftMargin: 10;
            anchors.right: parent.right;

            elide: Text.ElideRight;
            wrapMode: Text.NoWrap;

            text: hardwareCard.serialNumber;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
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


