import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

CollectionView {
    id: container;

    defaultSortHeaderIndex: 2;

    visibleMetaInfo: false;

    property MainDocumentManager mainDocumentManager: null;

    function fillContextMenuModel(){
        contextMenuModel.clear();
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../Icons/Light/Edit_On_Normal.svg"});
    }

    Component.onCompleted: {
        container.commandsDelegatePath = "qrc:/qml/ProLife/SoftwareProductsCollectionViewCommandsDelegate.qml";

        baseCollectionView.commands.fieldsData.push("OrderUuid");
        baseCollectionView.commands.fieldsData.push("HardwareUuid");
        baseCollectionView.commands.fieldsData.push("InUse");
    }

    onVisibleChanged: {
        if (container.visible){
            container.updateGui();
        }
    }

    onHeadersChanged: {
        container.table.setColumnContentComponent(0, pairComp);

        let orderIndex = container.table.getHeaderIndex("OrderId");
        container.table.setColumnContentComponent(orderIndex, orderColumnContentComp);

        container.table.tableDecorator = tableDecoratorModel;
    }

    function onCommandsModelChanged(){
        let onlyPairedIndex = container.commandsProvider.getCommandIndex("OnlyPaired");

        if (onlyPairedIndex >= 0){
            container.commandsProvider.commandsModel.SetData("IsToggleable", true, onlyPairedIndex);
            container.commandsProvider.commandsModel.SetData("IsToggled", false, onlyPairedIndex);
        }

        let onlyUnpairedIndex = container.commandsProvider.getCommandIndex("OnlyUnpaired");
        if (onlyUnpairedIndex >= 0){
            container.commandsProvider.commandsModel.SetData("IsToggleable", true, onlyUnpairedIndex);
            container.commandsProvider.commandsModel.SetData("IsToggled", false, onlyUnpairedIndex);
        }

        container.commandsProvider.commandsModel.Refresh();
        container.commandsProvider.updateGui();
    }

    Component {
        id: pairComp;
        Item {
            Image {
                id: image;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 5;

                width: 18;
                height: width;

                source: "../../../../Icons/Light/Ok_Off_Normal.svg";

                sourceSize.width: width;
                sourceSize.height: height;
            }

            Component.onCompleted: {
                let loader = parent;
                let tableCellDelegate = loader.cellDelegate;
                let value = tableCellDelegate.getValue();

                if (value === "NotPaired"){
                    image.source = "../../../../Icons/Light/Unlink_On_Normal.svg";
                }
                else if (value === "IsPaired"){
                    image.source = "../../../../Icons/Light/Link_On_Normal.svg";
                }
                else if (value === "InUse"){
                    image.source = "../../../../Icons/Light/Lock_On_Normal.svg";
                }
            }
        }
    }

    Component {
        id: orderColumnContentComp;
        Item {
            Image {
                id: image;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 5;

                width: 18;
                height: width;

                source: "../../../../Icons/Light/Alert_On_Normal.svg";

                visible: false;

                sourceSize.width: width;
                sourceSize.height: height;
            }

            Text {
                id: lable;

                anchors.left: parent.left;
                anchors.right: parent.right;
                anchors.verticalCenter: parent.verticalCenter;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
                color: Style.textColor;

                elide: Text.ElideRight;
            }

            Component.onCompleted: {
                let loader = parent;
                let tableCellDelegate = loader.parent;

                let value = tableCellDelegate.getValue();
                let rowIndex = tableCellDelegate.rowIndex;

                if (rowIndex >= 0){
                    let orderUuid = container.table.elements.GetData("OrderUuid", rowIndex);
                    if (orderUuid === "undefined"){
                        image.visible = true;
                        lable.visible = false;
                    }
                    else{
                        lable.visible = true;
                        lable.text = value;
                    }
                }
            }
        }
    }

    TreeItemModel {
        id: tableDecoratorModel;

        Component.onCompleted: {
            var cellWidthModel = tableDecoratorModel.AddTreeModel("CellWidth");

            let index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", 40, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);
        }
    }
}


