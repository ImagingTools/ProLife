import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

Rectangle {
    id: container
    height: 85;
    width: 500;
    radius: 10;
    property text productName;
    property text productCategory;
    property text pairName;
    property text licenseName;
    property text licenseExpiration;
    property text macAddress;
    property text serialNumber;

    Text {
        id: productName;
        anchors.left: parent.left;
        anchors.leftMargin: 5;
        anchors.top: parent.top;
        anchors.topMargin: 5;
        text: model.ProductId;
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
        text: "(" + model.Category + ")";
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
        font.bold: true;
    }
    Text {
        id: productionStatus;
        anchors.right: parent.right;
        anchors.rightMargin: 5;
        anchors.top: parent.top;
        anchors.topMargin: 5;
        text: model.ProductionStatus;
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
    }
    Rectangle {
        anchors.top: productName.bottom;
        anchors.topMargin: 5;
        height: 1;
        width: parent.width;
        color: Style.textColor;
    }

    Text {
        id: licenseName;
        anchors.left: parent.left;
        anchors.leftMargin: 5;
        anchors.top: productName.bottom;
        anchors.topMargin: 10;
        text: qsTr("License: ") + model.LicenseName + qsTr("Data expired: 01.01.2024");
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
        visible: model.Category === "Software";
    }

    Text {
        id: linkedName;
        anchors.top: licenseName.bottom;
        anchors.topMargin: 5;
        anchors.left: parent.left;
        anchors.leftMargin: 5;
        text: qsTr("Associated:");
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
        //                font.underline: true;
    }
    Text {
        id: linked;
        anchors.top: licenseName.bottom;
        anchors.topMargin: 5;
        anchors.left: linkedName.right;
        anchors.leftMargin: 5;
        text:  model.LinkId;
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
        font.underline: true;
    }

    Text {
        id: macAddress;
        anchors.top: productName.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 5;
        text: qsTr("Mac address: ") + model.MacAddress;
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
        visible: model.Category === "Hardware";
    }

    Text {
        id: serialNumber;
        anchors.top: macAddress.top;
        anchors.right: parent.right;
        anchors.rightMargin: 5;
        text: qsTr("Serial number: ") + model.SerialNumber;
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;
        visible: macAddress.visible;
    }
}
