import QtQuick 2.0
import imtgui 1.0
import Acf 1.0
import imtlicgui 1.0

Rectangle {
    id: softwareCard;

    radius: 10;
    color: Style.backgroundColor;

    property string productId: model.ProductId;
    property string licenseName;

    property var licensesModel: model.ActiveLicenses;

    property bool commandsVisible: false;

    property LicensesProvider licensesProvider: null;

    signal clicked();

    signal edited();

    onLicensesProviderChanged: {
        console.log("SoftwareCard onLicensesProviderChanged", softwareCard.licensesProvider);
        if (softwareCard.licensesProvider != null){
            licensesView.model = 0;
            licensesView.model = softwareCard.licensesModel;
        }
    }

    onLicensesModelChanged: {
        console.log("SoftwareCard onLicensesModelChanged", softwareCard.licensesProvider);
        if (softwareCard.licensesModel){
            licensesView.model = softwareCard.licensesModel;
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

            visible: softwareCard.commandsVisible;

            onClicked: {
                softwareCard.edited();
            }
        }
    }

    Item {
        id: licensesItem;

        anchors.top: header.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;

        height: licencesTitle.height;

        clip: true;

        visible: false;

        Text {
            id: licencesTitle;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;

            text: qsTr("Licenses:")
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            font.bold: true;
        }

        Text {
            id: licenses;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: licencesTitle.right;
            anchors.leftMargin: 10;
            anchors.right: parent.right;

            elide: Text.ElideRight;
            wrapMode: Text.NoWrap;

            text: softwareCard.licenseName;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }
    }

    CustomScrollbar {
        id: scrollbar;
        z: 100;

        anchors.right: licensesView.right;
//        anchors.leftMargin: 5;
        anchors.top: licensesView.top;
        anchors.bottom: licensesView.bottom;

        backgroundColor: Style.baseColor;

        secondSize: 5;
        targetItem: licensesView;
    }

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
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;

        boundsBehavior: Flickable.StopAtBounds;

        clip: true;

        delegate: Item {
            id: licenseDelegate;

            width: licensesView.width;
            height: licenceText.height;

            Text {
                id: licenceText;

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

    MouseArea {
        id: mouseArea;

        anchors.fill: parent;
        cursorShape: Qt.PointingHandCursor;

        hoverEnabled: true;

        visible: false;

        onClicked: {
            softwareCard.clicked();
        }

        onEntered: {
            console.log("onEntered");

            softwareCard.scale = 1.03;
        }

        onExited: {
            console.log("onExited");
            softwareCard.scale = 1;
        }
    }

} //Card


