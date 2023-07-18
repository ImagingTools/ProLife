import QtQuick 2.0
import imtgui 1.0
import Acf 1.0
import imtlicgui 1.0

Rectangle {
    id: softwareCard;

    width: 500;
    height: softwareCard.contentHeight + 20;

    radius: 3;
    color: Style.backgroundColor;

    property string productId: model.ProductId ? model.ProductId : "";
    property string serialNumber: model.SerialNumber ? model.SerialNumber : "";
    property string licenseName;
    property int margin: 10;

    property var licensesModel: model.ActiveLicenses ? model.ActiveLicenses : null;

    property bool readOnly: false;
    property bool commmandsVisible: false;

    property Item productCardRoot: null;

    property LicensesProvider licensesProvider: null;

//    property int contentHeight: Math.max(header.height + licensesView.height + 30, 108);

    property int contentHeight: contentColumn.height + 20;

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

    Column {
        id: contentColumn;

        anchors.top: parent.top;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;
//        anchors.verticalCenter: parent.verticalCenter;

        spacing: 10;

        Rectangle {
            id: header;

            width: parent.width;
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

                //            tooltipText: qsTr("Edit");

                visible: !softwareCard.readOnly && softwareCard.commmandsVisible;

                onClicked: {
                    softwareCard.edited();
                }
            }
        }

        Item {
            width: parent.width;
            height: serialNumberTitle.height;

            Text {
                id: serialNumberTitle;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 8;

                text: qsTr("License Number:")
                color: Style.textColor;
                font.family: Style.fontFamilyBold;
                font.pixelSize: Style.fontSize_common;
            }

            Text {
                id: serialNumber;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: serialNumberTitle.right;
                anchors.leftMargin: 10;

                width: parent.width - serialNumberTitle.width;

                elide: Text.ElideRight;
                wrapMode: Text.NoWrap;

                text: softwareCard.serialNumber != "" ? softwareCard.serialNumber : qsTr("Not set");

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }
        }

        Item {
            width: parent.width;
            height: licenceText.height;

            visible: licenceText.visible;

            Text {
                id: licenceText;

                anchors.left: parent.left;
                anchors.leftMargin: 8;

                width: parent.width;

                text: qsTr("No Licenses");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                elide: Text.ElideRight;
                wrapMode: Text.NoWrap;

                visible: licensesView.elementsList.count === 0;
            }
        }

        AuxTable {
            id: licensesView;

            width: parent.width;
            height: contentHeight + itemHeight;

            enableAlternating: false;

            radius: softwareCard.radius;
            backgroundElementsColor: Style.backgroundColor;
            backgroundHeadersColor: Style.backgroundColor;

            visible: licensesView.elementsList.count !== 0;
            selectable: false;
            separatorVisible: false;

            itemHeight: 25;
            headerHeight: 20;

            clip: true;
        }
    } // Column

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


