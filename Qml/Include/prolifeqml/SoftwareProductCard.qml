import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0

Rectangle {
    id: softwareCard;

    width: 500;
    height: softwareCard.contentHeight;

    color: Style.baseColor;

    property int margin: 10;
    property int contentHeight: contentColumn.height;

    property string licenseUuid: model.LicenseUuid ? model.LicenseUuid : "";
    property string licenseId: model.LicenseId;
    property string licenseName: model.LicenseName;
    property string expiration: model.Expiration ? model.Expiration : "";
    property string productId: model.ProductUuid ? model.ProductUuid : "";
    property string serialNumber: model.SerialNumber ? model.SerialNumber : "";

    property bool readOnly: false;
    property bool commmandsVisible: false;
    property bool inUse: model.InUse ? model.InUse : "";

    property Item productCardRoot: null;

    signal clicked();
    signal edited();

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", softwareCard.onLocalizationChanged);

        softwareCard.updateElements();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", softwareCard.onLocalizationChanged);
    }

    function onLocalizationChanged(language){
        softwareCard.updateElements();
    }

    Item {
        id: rightPanel;

        anchors.top: parent.top;
        anchors.right: parent.right;

        width: visible ? 30 : 0;
        height: parent.height;

        visible: editButton.visible;

        Button {
            id: editButton;

            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.top: parent.top;
            anchors.topMargin: 10;

            width: 18;
            height: width;

            iconSource: enabled ? "../../../../" + Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal) :
                                  "../../../../" + Style.getIconPath("Icons/Edit", Icon.State.Off, Icon.Mode.Disabled);
            visible: !softwareCard.readOnly && softwareCard.commmandsVisible;

            decorator: Component {
                ButtonDecorator {
                    color: parent.hovered ? Style.buttonHoverColor : "transparent";
                    border.width: 0;
                }
            }

            onClicked: {
                softwareCard.edited();
            }
        }
    }

    Column {
        id: contentColumn;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: rightPanel.left;

        Table {
            id: licensesView;

            width: contentColumn.width;
            height: contentHeight;

            enableAlternating: false;

            radius: 0;
            backgroundElementsColor: Style.baseColor;
            backgroundHeadersColor: Style.alternateBaseColor;

            selectable: false;
            separatorVisible: false;

            onElementsCountChanged: {
                licensesView.visible = elementsCount !== 0;
            }

            itemHeight: 25;

            clip: true;
            showHeaders: false;

            onHeadersChanged: {
                licensesView.tableDecorator = tableDecoratorModel;
            }
        }
    } // Column

    TreeItemModel {
        id: headersLicensesTable;

        Component.onCompleted: {
            softwareCard.updateHeaders();
        }
    }

    TreeItemModel {
        id: elementsTableModel;
    }

    function updateElements(){
        console.log("updateElements");

        elementsTableModel.Clear();

        let index = elementsTableModel.InsertNewItem();
        elementsTableModel.SetData("Key", qsTr("Serial Number"), index)
        elementsTableModel.SetData("Value", softwareCard.serialNumber, index)

        let licenseId = softwareCard.licenseId;
        let licenseName = softwareCard.licenseName;

        let name = licenseName;
        if (licenseId !== ""){
            name += "(" + licenseId + ")";
        }

        index = elementsTableModel.InsertNewItem();
        elementsTableModel.SetData("Key", qsTr("License"), index)
        elementsTableModel.SetData("Value", name, index);

        let expiration = qsTr("Unlimited");

        if (softwareCard.expiration !== ""){
            expiration = softwareCard.expiration;
        }

        index = elementsTableModel.InsertNewItem();
        elementsTableModel.SetData("Key", qsTr("Expiration"), index)
        elementsTableModel.SetData("Value", expiration, index)

        licensesView.elements = elementsTableModel;
    }

    function updateHeaders(){
        headersLicensesTable.Clear();

        let index = headersLicensesTable.InsertNewItem();

        headersLicensesTable.SetData("Id", "Key", index)
        headersLicensesTable.SetData("Name", qsTr("Key"), index)

        index = headersLicensesTable.InsertNewItem();

        headersLicensesTable.SetData("Id", "Value", index)
        headersLicensesTable.SetData("Name", qsTr("Value"), index)

        licensesView.headers = headersLicensesTable;
    }

    TreeItemModel {
        id: tableDecoratorModel;

        Component.onCompleted: {
            var cellWidthModel = tableDecoratorModel.AddTreeModel("CellWidth");

            let index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", 150, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);
        }
    }
} //Card


