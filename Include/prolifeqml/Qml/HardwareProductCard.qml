import QtQuick 2.0
import imtgui 1.0
import Acf 1.0

Rectangle {
    id: hardwareCard;

    height: hardwareCard.contentHeight + 20;

    radius: 3;
    color: Style.backgroundColor;

    property string productId: model.ProductId ? model.ProductId : "";
    property bool isNewDevice: model.IsNewDevice ? model.IsNewDevice : false;

    property string macAddress: hardwareCard.isNewDevice ? qsTr("New Sensor") : model.MacAddress ? model.MacAddress : "";
    property string serialNumber: hardwareCard.isNewDevice ? qsTr("New Sensor") : model.SerialNumber ? model.SerialNumber : "";
    property string modelType: model.ModelTypeId ? model.ModelTypeId : "";

    property bool notExists: model.DeviceNotExists ? model.DeviceNotExists : false;

    property Item productCardRoot: null;

    property bool readOnly: false;
    property bool commmandsVisible: false;

//    property int contentHeight: header.height + macAddressBlock.height + serialNumberBlock.height + modelTypeBlock.height + 50;
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

    Column {
        id: contentColumn;

        anchors.top: parent.top;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;

        spacing: 10;

        property int firstWidthPercent: 45;
        property int secondWidthPercent: 55;

        Rectangle {
            id: header;

            width: parent.width;
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

    //            tooltipText: qsTr("Edit");

                iconSource: enabled ? "../../../../Icons/Light/Edit_Off_Normal.svg" :
                                      "../../../../Icons/Light/Edit_Off_Disabled.svg";

                visible: !hardwareCard.readOnly && hardwareCard.commmandsVisible;

                onClicked: {
                    hardwareCard.edited();
                }
            }
        } // header

        Item {
            id: macAddressBlock;

            width: parent.width;
            height: macAddressTitle.height;

            Item {
                anchors.left: parent.left;

                width: parent.width * (contentColumn.firstWidthPercent / 100);
                height: parent.height;

                Text {
                    id: macAddressTitle;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;

                    width: parent.width;

                    text: qsTr("MAC Address:")
                    color: Style.textColor;
                    font.family: Style.fontFamilyBold;
                    font.pixelSize: Style.fontSize_common;
                }
            }

            Item {
                anchors.right: parent.right;

                width: parent.width * (contentColumn.secondWidthPercent / 100);
                height: parent.height;

                Text {
                    id: macAddress;

                    anchors.verticalCenter: parent.verticalCenter;

                    width: parent.width;

                    elide: Text.ElideRight;
                    wrapMode: Text.NoWrap;

                    text: hardwareCard.macAddress;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }
            }
        } // Mac Address

        Item {
            id: serialNumberBlock;

            width: parent.width;
            height: serialNumberTitle.height;

            Item {
                anchors.left: parent.left;

                width: parent.width * (contentColumn.firstWidthPercent / 100);
                height: parent.height;

                Text {
                    id: serialNumberTitle;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;

                    width: parent.width;

                    text: qsTr("Serial Number:")
                    color: Style.textColor;
                    font.family: Style.fontFamilyBold;
                    font.pixelSize: Style.fontSize_common;
                }
            }

            Item {
                anchors.right: parent.right;

                width: parent.width * (contentColumn.secondWidthPercent / 100);
                height: parent.height;

                Text {
                    id: serialNumber;

                    anchors.verticalCenter: parent.verticalCenter;

                    width: parent.width;

                    elide: Text.ElideRight;
                    wrapMode: Text.NoWrap;

                    text: hardwareCard.serialNumber;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }
            }
        } // Serial Number

        Item {
            id: modelTypeBlock;
//            anchors.top: serialNumberBlock.bottom;
//            anchors.topMargin: visible ? 10 : 0;
//            anchors.left: parent.left;
//            anchors.leftMargin: 10;
//            anchors.right: parent.right;
//            anchors.rightMargin: 10;

            width: parent.width;
            height: visible ? modelTypeTitle.height : -10;

            visible: hardwareCard.modelType !== "";

            Item {
                anchors.left: parent.left;

                width: parent.width * (contentColumn.firstWidthPercent / 100);
                height: parent.height;

                Text {
                    id: modelTypeTitle;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;

                    width: parent.width;

                    text: qsTr("Model Type:")
                    color: Style.textColor;
                    font.family: Style.fontFamilyBold;
                    font.pixelSize: Style.fontSize_common;
                }
            }

            Item {
                anchors.right: parent.right;

                width: parent.width * (contentColumn.secondWidthPercent / 100);
                height: parent.height;

                Text {
                    id: modelType;

                    anchors.verticalCenter: parent.verticalCenter;

                    width: parent.width;

                    elide: Text.ElideRight;
                    wrapMode: Text.NoWrap;

                    text: hardwareCard.modelType;

                    color: Style.textColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }
            }
        } // Model Type
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


