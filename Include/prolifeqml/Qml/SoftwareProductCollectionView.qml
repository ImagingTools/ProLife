import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

CollectionView {
    id: container;

    defaultSortHeaderIndex: 1;

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

    filterMenu: Component {
        Item {
            id: mainItem;

            width: parent.width;
            height: 40;

            TreeItemModel {
                id: modelCategogy;

                Component.onCompleted: {
                    let index = modelCategogy.InsertNewItem();
                    modelCategogy.SetData("Id", "All", index);
                    modelCategogy.SetData("Name", qsTr("Show all licenses"), index);

                    index = modelCategogy.InsertNewItem();
                    modelCategogy.SetData("Id", "Paired", index);
                    modelCategogy.SetData("Name", qsTr("Show only paired licenses"), index);

                    index = modelCategogy.InsertNewItem();
                    modelCategogy.SetData("Id", "NotPaired", index);
                    modelCategogy.SetData("Name", qsTr("Show only not paired licenses"), index);

//                    index = modelCategogy.InsertNewItem();
//                    modelCategogy.SetData("Id", "InUse", index);
//                    modelCategogy.SetData("Name", qsTr("In-Use"), index);

                    licenseComboBox.model = modelCategogy;
                }
            }

            Item {
                id: licenseFilterBlock;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 10;

                width: licenseComboBox.width;
                height: filtermenu.height;

                ComboBox {
                    id: licenseComboBox;

                    anchors.bottom: parent.bottom;
                    anchors.left: parent.left;

                    width: 200;
                    height: filtermenu.height;

                    backgroundColor: Style.baseColor;
                    currentIndex: 0;

                    radius: 3;

                    onCurrentIndexChanged: {
                        let objectFilter = container.modelFilter.GetData("ObjectFilter");
                        if (!objectFilter){
                            objectFilter = container.modelFilter.AddTreeModel("ObjectFilter")
                        }

                        if (licenseComboBox.currentIndex == 0){
                            container.modelFilter.RemoveData("ObjectFilter");
                        }
                        else if (licenseComboBox.currentIndex == 1){
                            objectFilter.SetData("Key", "DeviceId");
                            objectFilter.SetData("Value", "");
                            objectFilter.SetData("IsEqual", false);
                        }
                        else if (licenseComboBox.currentIndex == 2){
                            objectFilter.SetData("Key", "DeviceId");
                            objectFilter.SetData("Value", "");
                            objectFilter.SetData("IsEqual", true);
                        }
//                        else if (licenseComboBox.currentIndex == 3){
//                            objectFilter.SetData("Key", "InUse");
//                            objectFilter.SetData("Value", "true");
//                            objectFilter.SetData("IsEqual", true);
//                        }

                        container.updateGui();
                    }
                }
            }

            FilterMenu {
                id: filtermenu

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;

                decoratorSource: Style.filterPanelDecoratorPath;

                width: 325

                onTextFilterChanged: {
                    parent.textFilterChanged(index, text);
                }

                onClosed: {
                    licenseComboBox.currentIndex = 0;

                    parent.closed();
                }
            }

            signal textFilterChanged(int index, string text);
            signal closed();
        }
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


