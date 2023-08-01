import QtQuick 2.0
import imtgui 1.0
import Acf 1.0
import imtlicgui 1.0

Rectangle {
    id: softwareCard;

    width: 500;
    height: softwareCard.contentHeight;

    color: Style.baseColor;

    property string productId: model.ProductId ? model.ProductId : "";
    property string serialNumber: model.SerialNumber ? model.SerialNumber : "";
    property string licenseName;
    property int margin: 10;

    property var licensesModel: model.ActiveLicenses ? model.ActiveLicenses : null;

    property bool readOnly: false;
    property bool commmandsVisible: false;

    property Item productCardRoot: null;

    property LicensesProvider licensesProvider: null;

    property int contentHeight: noLicensesView.visible ? noLicensesView.height + 20 : contentColumn.height + 20;
//    property int contentHeight: contentColumn.height;

    signal clicked();
    signal edited();

    onLicensesProviderChanged: {
        if (softwareCard.licensesProvider != null){
            licensesView.elements = 0;
            licensesView.elements = softwareCard.licensesModel;
        }
    }

    property bool ok: softwareCard.licensesProvider != null && productId && licensesModel !== null;

    onOkChanged: {
        if (ok){
            productCardRoot.orderEditorPtr.blockUpdatingModel = true
            for (let i = 0; i < softwareCard.licensesModel.GetItemsCount(); i++){
                let licenseId = softwareCard.licensesModel.GetData("Id", i);
                let expiration = softwareCard.licensesModel.GetData("Expiration", i);

                if (expiration === ""){
                    softwareCard.licensesModel.SetData("Expiration", "Unlimited", i);
                }

                let licenseName = softwareCard.licensesProvider.getLicenseName(softwareCard.productId, licenseId);
                softwareCard.licensesModel.SetData("Name", licenseName, i);
            }

            productCardRoot.orderEditorPtr.blockUpdatingModel = false

            licensesView.elements = softwareCard.licensesModel;
        }
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

            iconSource: enabled ? "../../../../Icons/Light/Edit_Off_Normal.svg" :
                                  "../../../../Icons/Light/Edit_Off_Disabled.svg";
            visible: !softwareCard.readOnly && softwareCard.commmandsVisible;

            onClicked: {
                softwareCard.edited();
            }
        }
    }

    Item {
        id: noLicensesView;

        width: parent.width;
        height: visible ? 40 : 0;

        visible: licenceText.visible;

        Text {
            id: licenceText;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: parent.left;
            anchors.leftMargin: 20;
            anchors.right: parent.right;
            anchors.rightMargin: 20;

//            width: parent.width;

            text: qsTr("No Licenses");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            elide: Text.ElideRight;
            wrapMode: Text.NoWrap;

            visible: licensesView.elementsList.count === 0;
        }
    }

    Column {
        id: contentColumn;

//        anchors.verticalCenter: parent.verticalCenter;
        anchors.top: parent.top;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: rightPanel.left;
        anchors.rightMargin: rightPanel.visible ? 0 : 10;

        AuxTable {
            id: licensesView;

            width: contentColumn.width;
            height: contentHeight;

            enableAlternating: false;

            radius: 0;
            backgroundElementsColor: Style.baseColor;
            backgroundHeadersColor: Style.alternateBaseColor;

            visible: licensesView.elementsList.count !== 0;
            selectable: false;
            separatorVisible: false;

            itemHeight: 25;
            headerHeight: 20;

            clip: true;
            showHeaders: false;
        }
    } // Column

//    AuxButton {
//        id: editButton;

//        anchors.top: parent.top;
//        anchors.topMargin: 10;
//        anchors.right: parent.right;
//        anchors.rightMargin: 10;

//        width: 18;
//        height: width;

//        iconSource: enabled ? "../../../../Icons/Light/Edit_Off_Normal.svg" :
//                              "../../../../Icons/Light/Edit_Off_Disabled.svg";
//        visible: !softwareCard.readOnly && softwareCard.commmandsVisible;

//        onClicked: {
//            softwareCard.edited();
//        }
//    }

    TreeItemModel {
        id: headersLicensesTable;

        Component.onCompleted: {
            let index = headersLicensesTable.InsertNewItem();

            headersLicensesTable.SetData("Id", "Name", index)
            headersLicensesTable.SetData("Name", "License Name", index)

            index = headersLicensesTable.InsertNewItem();

            headersLicensesTable.SetData("Id", "Expiration", index)
            headersLicensesTable.SetData("Name", "Expiration", index)

            licensesView.headers = headersLicensesTable;
        }
    }
} //Card


