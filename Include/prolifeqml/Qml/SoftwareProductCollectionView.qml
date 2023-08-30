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
                let tableCellDelegate = loader.parent;

                let value = tableCellDelegate.getValue();
                if (value){
                    image.source = "../../../../Icons/Light/Ok_On_Normal.svg";
                }
                else{
                    image.source = "../../../../Icons/Light/Close_On_Normal.svg";
                }
            }
        }
    }

    TreeItemModel {
        id: tableDecoratorModel;

        Component.onCompleted: {
            var cellWidthModel = tableDecoratorModel.AddTreeModel("CellWidth");

            let index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", 80, index);

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


