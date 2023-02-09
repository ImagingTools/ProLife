import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

Item {
    id: productInfo;

    height: column.height;
    width: 500;

    property string productName;
    property string productCategory;
    property string productStatus: "None";
    property string pairName;
    property string licenseName;
    property string licenseExpiration;
    property string macAddress;
    property string serialNumber;
    property string productionStatus;

    property TreeItemModel commandsModel: null;

    onCommandsModelChanged: {
        if (productInfo.commandsModel != null){
            commands.commandModel = productInfo.commandsModel;
        }
    }

    signal removed();
    signal edited();
    signal createLicenseFile();

    Rectangle {
        id: background;

        anchors.fill: column;

        color: Style.baseColor;
        radius: 10;
    }

    Column {
        id: column;

        width: parent.width;

        SimpleCommandsDecorator {
            id: commands;

            anchors.right: parent.right;
            anchors.rightMargin: 7;
            anchors.top: parent.top;

            width: 60;

            radius: 10;
            color: productInfo.color;

            onCommandActivated: {
                if (commandId == "Remove"){
                    productInfo.removed();
                }
                else if (commandId == "Edit"){
                    productInfo.edited();
                }
                else if (commandId == "CreateLicenseFile"){
                    productInfo.createLicenseFile();
                }
            }
        }

        Item {
            width: parent.width;
            height: 30;

            Text {
                id: productName;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 5;

                text: productInfo.productName;
                color: Style.textColor;
                font.family: Style.fontFamilyBold;
                font.pixelSize: Style.fontSize_common;
                font.bold: true;
            }

            Text {
                id: productCategory;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: productName.right;
                anchors.leftMargin: 5;
                text: "(" + productInfo.productCategory + ")";
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                font.bold: true;
            }
        }

        Rectangle {
            height: 1;
            width: parent.width;

            color: Style.textColor;
        }

        Text {
            id: productionStatus;

            anchors.left: parent.left;
            anchors.leftMargin: 5;

            text: qsTr("Status: ") + productInfo.productStatus;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            visible:  productInfo.productCategory === "Hardware";
        }

        Text {
            id: licenseName;

            anchors.left: parent.left;
            anchors.leftMargin: 5;

            width: parent.width - 10;

            text: qsTr("License: ") + productInfo.licenseName + " " + productInfo.licenseExpiration;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            visible: productInfo.productCategory === "Software";

            wrapMode: Text.WrapAnywhere;

            font.bold: true;
        }

        Item {
            width: parent.width;
            height: 35;

            Text {
                id: linkedName;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 5;
                text: qsTr("Pair:");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                //                font.underline: true;
            }
            Text {
                id: linked;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: linkedName.right;
                anchors.leftMargin: 5;
                text:  productInfo.pairName;
                color: Style.textSelected;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                font.bold: true;
            }
        }

        Text {
            id: macAddress;

            anchors.left: parent.left;
            anchors.leftMargin: 5;
            text: qsTr("Mac address: ") + productInfo.macAddress;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            visible: productInfo.productCategory === "Hardware";
        }

        Text {
            id: serialNumber;

            anchors.left: parent.left;
            anchors.leftMargin: 5;

            width: parent.width - 10;

            text: qsTr("Serial number: ") + productInfo.serialNumber;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            visible: macAddress.visible;

            wrapMode: Text.WrapAnywhere;
        }
    }
}

