import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

Rectangle {
    id: productInfo;

    height: 85;
    width: 500;

    color: Style.baseColor;

    radius: 10;
    property string productName;
    property string productCategory;
    property string productStatus: "Status";
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

    Text {
        id: productName;
        anchors.left: parent.left;
        anchors.leftMargin: 5;
        anchors.top: parent.top;
        anchors.topMargin: 5;
        text: productInfo.productName;
        color: Style.textColor;
        font.family: Style.fontFamilyBold;
        font.pixelSize: Style.fontSize_common;
        font.bold: true;
    }
    Text {
        id: productCategory;
        anchors.top: parent.top;
        anchors.topMargin: 5;
        anchors.left: productName.right;
        anchors.leftMargin: 5;
        text: "(" + productInfo.productCategory + ")";
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
        font.bold: true;
    }

    Rectangle {
        anchors.top: productName.bottom;
        anchors.topMargin: 5;
        height: 1;
        width: parent.width;
        color: Style.textColor;
    }

    Text {
        id: productionStatus;
        anchors.right: parent.right;
        anchors.rightMargin: 5;
        anchors.top: productName.bottom;
        anchors.topMargin: 10;
        text: productInfo.productStatus;
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
        visible: false;
    }

    Text {
        id: licenseName;
        anchors.left: parent.left;
        anchors.leftMargin: 5;
        anchors.top: productionStatus.bottom;
        anchors.topMargin: 5;
        text: qsTr("License: ") + productInfo.licenseName + " " + productInfo.licenseExpiration;
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
        visible: productInfo.productCategory === "Software";
    }

    Text {
        id: linkedName;
        anchors.top: productName.bottom;
        anchors.topMargin: 10;
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
        anchors.top: productName.bottom;
        anchors.topMargin: 10;
        anchors.left: linkedName.right;
        anchors.leftMargin: 5;
        text:  productInfo.pairName;
        color: Style.textSelected;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
        font.bold: true;
    }

    Text {
        id: macAddress;
        anchors.top: linkedName.bottom;
        anchors.topMargin: 5;
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
        anchors.top: macAddress.top;
        anchors.right: parent.right;
        anchors.rightMargin: 5;
        text: qsTr("Serial number: ") + productInfo.serialNumber;
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
        visible: macAddress.visible;
    }
}
