import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

Item {
    id: productInfo;

    height: column.height + 5;
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

    property string deviceId;

    property bool selected: false;

    property TreeItemModel commandsModel: null;

    onCommandsModelChanged: {
        if (productInfo.commandsModel != null){
            commands.commandModel = productInfo.commandsModel;
        }
    }

    signal removed();
    signal edited();
    signal createLicenseFile();

    signal clicked();
    signal pairClicked();

    MouseArea {
        anchors.fill: parent;

        onClicked: {
            console.log("onClicked");
            productInfo.clicked();
        }
    }

    Rectangle {
        id: background;

        anchors.fill: parent;

        color: Style.baseColor;
        radius: 10;

        border.color: productInfo.selected ? Style.textSelected : "transparent";
        border.width: 2;
    }

    Column {
        id: column;

        anchors.left: parent.left;
        anchors.leftMargin: 8;
        anchors.right: parent.right;
        anchors.rightMargin: anchors.leftMargin;
        anchors.verticalCenter: parent.verticalCenter;

        spacing: 5;

        width: parent.width;

        SimpleCommandsDecorator {
            id: commands;

            anchors.right: parent.right;
            anchors.rightMargin: 7;
            anchors.top: parent.top;

            width: 60;

            radius: 10;
//            color: background.color;

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
            height: 25;

            Text {
                id: productName;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
//                anchors.leftMargin: 5;

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

//        Text {
//            id: productionStatus;

//            anchors.left: parent.left;
//            anchors.leftMargin: 5;

//            text: qsTr("Status: ") + productInfo.productStatus;
//            color: Style.textColor;
//            font.family: Style.fontFamily;
//            font.pixelSize: Style.fontSize_common;
//            visible:  productInfo.productCategory === "Hardware";
//        }

        Text {
            id: licenseName;

            anchors.left: parent.left;
//            anchors.leftMargin: 5;

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
            height: linkedName.height;

            Text {
                id: linkedName;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
//                anchors.leftMargin: 5;
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

                MouseArea {
                    id: ma;
                    anchors.fill: parent;
                    cursorShape: Qt.PointingHandCursor;
                    onClicked: {
                        productInfo.pairClicked()
                    }
                }
            }
        }

        Item {
            width: parent.width;
            height: macAddressTitle.height;

            visible:  productInfo.productCategory === "Hardware";

            Text {
                id: macAddressTitle;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
//                anchors.leftMargin: 5;

                text: qsTr("Mac address:")
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                visible: productInfo.productCategory === "Hardware";
                font.bold: true;
            }

            Text {
                id: macAddress;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: macAddressTitle.right;
                anchors.leftMargin: 5;
                text: productInfo.macAddress;
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }
        }

        Item {
            width: parent.width;
            height: serialNumberTitle.height;

            visible:  productInfo.productCategory === "Hardware";

            Text {
                id: serialNumberTitle;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
//                anchors.leftMargin: 5;

                text: qsTr("Serial Number:")
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                visible: productInfo.productCategory === "Hardware";
                font.bold: true;
            }

            Text {
                id: serialNumber;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: serialNumberTitle.right;
                anchors.leftMargin: 5;
                text: productInfo.serialNumber;
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }
        }
    }
}

