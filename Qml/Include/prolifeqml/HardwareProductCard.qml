import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
    id: hardwareCard;

    height: visible ? hardwareCard.contentHeight : 0;

    color: Style.baseColor;

    property bool isNewDevice: false//model.IsNewDevice ? model.IsNewDevice : false;
    property bool notExists: model.DeviceNotExists ? model.DeviceNotExists : false;
    property bool checker: hardwareCard.productCardRoot != null && hardwareCard.notExists;
    property bool readOnly: false;
    property bool commmandsVisible: false;

    property string productId: model.ProductUuid ? model.ProductUuid : "";
    property string macAddress: hardwareCard.isNewDevice ? qsTr("New Sensor") : model.MacAddress ? model.MacAddress : "";
    property string serialNumber: hardwareCard.isNewDevice ? qsTr("New Sensor") : model.SerialNumber ? model.SerialNumber : "";

    property Item productCardRoot: null;

    property int contentHeight: contentColumn.height;

    signal clicked();
    signal edited();

    Component.onCompleted: {
        Events.subscribeEvent("OnLocalizationChanged", hardwareCard.onLocalizationChanged);

        hardwareCard.updateElements();
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", hardwareCard.onLocalizationChanged);
    }

    onCheckerChanged: {
        if (checker){
            let message = qsTr("Sensor detection error. Please select a new sensor.");
            hardwareCard.productCardRoot.showErrorMessage(message);
        }
    }

    function onLocalizationChanged(language){
        hardwareCard.updateHeaders();

        hardwareCard.updateElements();
    }

    function updateElements(){
        elementsTableModel.Clear();

        let index = elementsTableModel.InsertNewItem();
        elementsTableModel.SetData("Key", qsTr("MAC Address"), index)
        elementsTableModel.SetData("Value", hardwareCard.macAddress, index)

        index = elementsTableModel.InsertNewItem();
        elementsTableModel.SetData("Key", qsTr("Serial Number"), index)
        elementsTableModel.SetData("Value", hardwareCard.serialNumber, index)

        index = elementsTableModel.InsertNewItem();
        elementsTableModel.SetData("Key", qsTr("Model Type"), index)
        elementsTableModel.SetData("Value", model.LicenseName, index)

        index = elementsTableModel.InsertNewItem();
        elementsTableModel.SetData("Key", qsTr("Article"), index)
        elementsTableModel.SetData("Value", model.LicenseId, index)

        table.elements = elementsTableModel;
    }

    TreeItemModel {
        id: headersTableModel;

        Component.onCompleted: {
            hardwareCard.updateHeaders();
        }
    }

    function updateHeaders(){
        headersTableModel.Clear();

        let index = headersTableModel.InsertNewItem();

        headersTableModel.SetData("Id", "Key", index)
        headersTableModel.SetData("Name", qsTr("Key"), index)

        index = headersTableModel.InsertNewItem();

        headersTableModel.SetData("Id", "Value", index)
        headersTableModel.SetData("Name", qsTr("Value"), index)

        table.headers = headersTableModel;
    }

    TreeItemModel {
        id: elementsTableModel;
    }

    Column {
        id: contentColumn;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.left: parent.left;
        anchors.right: parent.right;

        Table {
            id: table;

            width: contentColumn.width;
            height: contentHeight;

            radius: 0;

            selectable: false;
            separatorVisible: false;

            itemHeight: 25;
            enableAlternating: false;

            clip: true;
            showHeaders: false;

            onHeadersChanged: {
                table.tableDecorator = tableDecoratorModel;
            }
        }
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


