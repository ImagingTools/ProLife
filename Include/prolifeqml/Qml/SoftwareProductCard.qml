import QtQuick 2.0
import imtgui 1.0
import Acf 1.0
import imtlicgui 1.0

Rectangle {
    id: softwareCard;

    width: 500;

//    height: header.height + licensesView.height + 30;

    height: Math.max(header.height + licensesView.height + 30, 108);

    radius: 10;
    color: Style.backgroundColor;

    property string productId: model.ProductId;
    property string licenseName;

    property var licensesModel: model.ActiveLicenses;

    property bool readOnly: false;
    property bool commmandsVisible: false;

    property LicensesProvider licensesProvider: null;
    property ListView productsView: null;

    property bool listContainsMouse: mouseArea.containsMouse;

    signal clicked();
    signal edited();

    onListContainsMouseChanged: {
        if (softwareCard.productsView){
//            if (scrollbar.visible){
//                softwareCard.productsView.interactive = !softwareCard.listContainsMouse;
//            }
        }
    }

    onLicensesProviderChanged: {
        if (softwareCard.licensesProvider != null){
            licensesView.model = 0;
            licensesView.model = softwareCard.licensesModel;
        }
    }

    onLicensesModelChanged: {
        if (softwareCard.licensesModel){
            licensesView.model = softwareCard.licensesModel;

//            if (softwareCard.productsView){
//                softwareCard.productsView.interactive = !scrollbar.visible;
//            }
        }
    }


    MouseArea {
        id: mouseArea;

//        anchors.top: licensesView.top;
//        anchors.bottom: licensesView.bottom;
//        anchors.left: licensesView.left;
//        anchors.right: scrollbar.right;
//        hoverEnabled: true;
    }

//    Column {
//        id: body;

//        width: parent.width;

//        spacing: 10;

//        Rectangle {
//            id: header;

//            width: parent.width;

////            anchors.top: parent.top;
////            anchors.topMargin: 10;
////            anchors.left: parent.left;
////            anchors.leftMargin: 10;
////            anchors.right: parent.right;
////            anchors.rightMargin: 10;

//            height: 30;

//            radius: softwareCard.radius;
//            color: Style.imagingToolsGradient2;

//            Text {
//                id: productTitle;

//                anchors.left: parent.left;
//                anchors.leftMargin: 10;
//                anchors.verticalCenter: parent.verticalCenter;
//                anchors.right: editButton.left;
//                anchors.rightMargin: 10;

//                text: softwareCard.productId;
//                color: Style.textColor;
//                font.family: Style.fontFamilyBold;
//                font.pixelSize: Style.fontSize_common;
//                elide: Text.ElideRight;
//                wrapMode: Text.NoWrap;
//            }

//            AuxButton {
//                id: editButton;

//                anchors.verticalCenter: parent.verticalCenter;
//                anchors.right: parent.right;
//                anchors.rightMargin: 10;

//                width: 18;
//                height: width;

//                iconSource: enabled ? "../../../../Icons/Light/Edit_Off_Normal.svg" :
//                                      "../../../../Icons/Light/Edit_Off_Disabled.svg";

//                visible: !softwareCard.readOnly && softwareCard.commmandsVisible;

//                onClicked: {
//                    softwareCard.edited();
//                }
//            }
//        }

//        ListView {
//            id: licensesView;

//             width: parent.width;

////            anchors.top: header.bottom;
////            anchors.topMargin: 10;
////            anchors.left: parent.left;
////            anchors.leftMargin: 10;
////            anchors.right: parent.right;
////            anchors.rightMargin: 10;
////            anchors.bottom: parent.bottom;
////            anchors.bottomMargin: 10;

//            height: contentHeight;

//            boundsBehavior: Flickable.StopAtBounds;

//            interactive: false;

//            clip: true;

//            delegate: Item {
//                id: licenseDelegate;

//                width: licensesView.width;
//                height: licenceText.height;

//                Text {
//                    anchors.left: parent.left;
//                    anchors.right: parent.right;

//                    text: softwareCard.licensesProvider ? model.Expiration === "" ? softwareCard.licensesProvider.getLicenseName(softwareCard.productId, model.Id) + " (Unlimited)" : softwareCard.licensesProvider.getLicenseName(softwareCard.productId, model.Id) + " (" + model.Expiration+ ")" : "";
//                    color: Style.textColor;
//                    font.family: Style.fontFamily;
//                    font.pixelSize: Style.fontSize_common;

//                    elide: Text.ElideRight;
//                    wrapMode: Text.NoWrap;
//                }
//            }
//        }

//    }

    Rectangle {
        id: header;

        anchors.top: parent.top;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;

        height: 30;

        radius: softwareCard.radius;
        color: Style.imagingToolsGradient2;

        Text {
            id: productTitle;

            anchors.left: parent.left;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: editButton.left;
            anchors.rightMargin: 10;

            text: softwareCard.productId;
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

            visible: !softwareCard.readOnly && softwareCard.commmandsVisible;

            onClicked: {
                softwareCard.edited();
            }
        }
    }

//    Item {
//        id: licensesItem;

//        anchors.top: header.bottom;
//        anchors.topMargin: 10;
//        anchors.left: parent.left;
//        anchors.leftMargin: 10;
//        anchors.right: parent.right;
//        anchors.rightMargin: 10;

//        height: licencesTitle.height;

//        clip: true;

//        visible: false;

//        Text {
//            id: licencesTitle;

//            anchors.verticalCenter: parent.verticalCenter;
//            anchors.left: parent.left;

//            text: qsTr("Licenses:")
//            color: Style.textColor;
//            font.family: Style.fontFamily;
//            font.pixelSize: Style.fontSize_common;
//            font.bold: true;
//        }

//        Text {
//            id: licenses;

//            anchors.verticalCenter: parent.verticalCenter;
//            anchors.left: licencesTitle.right;
//            anchors.leftMargin: 10;
//            anchors.right: parent.right;

//            elide: Text.ElideRight;
//            wrapMode: Text.NoWrap;

//            text: softwareCard.licenseName;
//            color: Style.textColor;
//            font.family: Style.fontFamily;
//            font.pixelSize: Style.fontSize_common;
//        }
//    }

//    CustomScrollbar {
//        id: scrollbar;
//        z: 100;

//        anchors.right: licensesView.right;
//        anchors.top: licensesView.top;
//        anchors.bottom: licensesView.bottom;

//        backgroundColor: Style.baseColor;

//        secondSize: 7;
//        targetItem: licensesView;
//    }

    Text {
        id: licenceText;

        anchors.top: header.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;

        text: qsTr("No Licenses");
        color: Style.textColor;
        font.family: Style.fontFamily;
        font.pixelSize: Style.fontSize_common;

        elide: Text.ElideRight;
        wrapMode: Text.NoWrap;

        visible: licensesView.count === 0;
    }

    ListView {
        id: licensesView;

        anchors.top: header.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;
//        anchors.bottom: parent.bottom;
//        anchors.bottomMargin: 10;

        boundsBehavior: Flickable.StopAtBounds;

        interactive: false;

        height: contentHeight;

        clip: true;

        delegate: Item {
            id: licenseDelegate;

            width: licensesView.width;
            height: licenceText.height;

            Text {
                anchors.left: parent.left;
                anchors.right: parent.right;

                text: softwareCard.licensesProvider ? model.Expiration === "" ? softwareCard.licensesProvider.getLicenseName(softwareCard.productId, model.Id) + " (Unlimited)" : softwareCard.licensesProvider.getLicenseName(softwareCard.productId, model.Id) + " (" + model.Expiration+ ")" : "";
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                elide: Text.ElideRight;
                wrapMode: Text.NoWrap;
            }
        }
    }

//    AuxButton {
//        id: arrowButton;

//        anchors.top: licensesView.bottom;
//        anchors.horizontalCenter: licensesView.horizontalCenter;

//        width: 15;
//        height: width;

//        visible: licensesView.contentHeight > licensesView.height;

//        iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";

//        onClicked: {
//            licensesView.height = licensesView.contentHeight;
//        }
//    }
} //Card


