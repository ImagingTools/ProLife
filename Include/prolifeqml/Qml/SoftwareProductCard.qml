import QtQuick 2.0
import imtgui 1.0
import Acf 1.0
import imtlicgui 1.0

Rectangle {
    id: softwareCard;

    width: 500;
    height: contentHeight;

    radius: 10;
    color: Style.backgroundColor;

    property string productId: model.ProductId ? model.ProductId : "";
    property string licenseName;

    property var licensesModel: model.ActiveLicenses ? model.ActiveLicenses : null;

    property bool readOnly: false;
    property bool commmandsVisible: false;

    property Item productCardRoot: null;

    property LicensesProvider licensesProvider: null;
    property ListView productsView: null;

    property int contentHeight: Math.max(header.height + licensesView.height + 30, 108);

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

            //            tooltipText: qsTr("Edit");

            visible: !softwareCard.readOnly && softwareCard.commmandsVisible;

            onClicked: {
                softwareCard.edited();
            }
        }
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

        visible: licensesView.elementsList.count === 0;
    }

    AuxTable {
        id: licensesView;

        anchors.top: header.bottom;
        anchors.topMargin: 10;
        anchors.left: parent.left;
        anchors.leftMargin: 10;
        anchors.right: parent.right;
        anchors.rightMargin: 10;

        radius: softwareCard.radius;
        backgroundElementsColor: Style.backgroundColor;
        //        backgroundHeadersColor: Style.alternateBaseColor;
        backgroundHeadersColor: Style.backgroundColor;

        visible: licensesView.elementsList.count !== 0;
        selectable: false;
        separatorVisible: false;

        itemHeight: 25;
        headerHeight: 20;

        height: contentHeight + itemHeight;
        clip: true;
    }

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


