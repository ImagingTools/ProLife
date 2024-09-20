import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import prolifeOrdersSdl 1.0

Rectangle {
    id: hardwareCard;

    height: visible ? hardwareCard.contentHeight : 0;

    color: Style.baseColor;

    property OrderedProduct productItem: model.item ? model.item : null;

    property bool readOnly: false;
    property bool commmandsVisible: false;
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

    function onLocalizationChanged(language){
        hardwareCard.updateHeaders();

        hardwareCard.updateElements();
    }

    function updateElements(){
        if (!productItem){
            return;
        }

        elementsTableModel.clear();

        let index = elementsTableModel.insertNewItem();
        elementsTableModel.setData("Key", qsTr("MAC Address"), index)
        elementsTableModel.setData("Value", productItem.m_macAddress, index)

        index = elementsTableModel.insertNewItem();
        elementsTableModel.setData("Key", qsTr("Serial Number"), index)
        elementsTableModel.setData("Value", productItem.m_serialNumber, index)

        index = elementsTableModel.insertNewItem();
        elementsTableModel.setData("Key", qsTr("Model Type"), index)
        elementsTableModel.setData("Value", productItem.m_licenseName, index)

        index = elementsTableModel.insertNewItem();
        elementsTableModel.setData("Key", qsTr("Article"), index)
        elementsTableModel.setData("Value", productItem.m_licenseId, index)

        table.elements = elementsTableModel;
    }

    TreeItemModel {
        id: headersTableModel;

        Component.onCompleted: {
            hardwareCard.updateHeaders();
        }
    }

    function updateHeaders(){
        headersTableModel.clear();

        let index = headersTableModel.insertNewItem();

        headersTableModel.setData("Id", "Key", index)
        headersTableModel.setData("Name", qsTr("Key"), index)

        index = headersTableModel.insertNewItem();

        headersTableModel.setData("Id", "Value", index)
        headersTableModel.setData("Name", qsTr("Value"), index)

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
            var cellWidthModel = tableDecoratorModel.addTreeModel("CellWidth");

            let index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("Width", 150, index);

            index = cellWidthModel.insertNewItem();
            cellWidthModel.setData("Width", -1, index);
        }
    }
} //Card


