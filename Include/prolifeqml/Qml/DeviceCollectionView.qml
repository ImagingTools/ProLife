import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

CollectionView {
    id: container;

    //    visibleMetaInfo: false;

    defaultSortHeaderIndex: 7;
    defaultOrderType: "DESC";

    property MainDocumentManager mainDocumentManager: null;

    Component.onCompleted: {
        // Events.subscribeEvent("OrdersCollectionUpdated", container.updateGui);
        Events.subscribeEvent("DevicesCollectionUpdated", container.collectionUpdated);
        container.commandsDelegatePath = "qrc:/qml/ProLife/DeviceCollectionViewCommandsDelegate.qml";

        baseCollectionView.commands.fieldsData.push("OrderUuid");
        baseCollectionView.commands.fieldsData.push("StatusId");
        baseCollectionView.commands.fieldsData.push("Licenses");
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("DevicesCollectionUpdated", container.collectionUpdated);
    }

    onVisibleChanged: {
        if (container.visible){
            container.updateGui();
        }
    }

    filterMenu: Component {
        Item {
            id: mainItem;

            width: parent.width;
            height: 40;

            onWidthChanged: {
                console.log("Filter onWidthChanged", width);
                if (width - filtermenu.width <= licenseFilterBlock.width){
                    licenseFilterBlock.visible = false;
                }
                else{
                    licenseFilterBlock.visible = true;
                }
            }

            TreeItemModel {
                id: modelCategogy;

                Component.onCompleted: {
                    let index = modelCategogy.InsertNewItem();
                    modelCategogy.SetData("Id", "None", index);
                    modelCategogy.SetData("Name", qsTr("Show All Sensors"), index);

                    index = modelCategogy.InsertNewItem();
                    modelCategogy.SetData("Id", "WithoutLicense", index);
                    modelCategogy.SetData("Name", qsTr("Sensors without a license"), index);

                    index = modelCategogy.InsertNewItem();
                    modelCategogy.SetData("Id", "WithLicense", index);
                    modelCategogy.SetData("Name", qsTr("Sensors with license"), index);

                    licenseComboBox.model = modelCategogy;
                }
            }

            Text {
                id: titleInstanceId;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: filtermenu.left;
                anchors.rightMargin: 10;

                visible: container.commandsDelegate ? container.commandsDelegate.filterByNewActive : false;

                text: qsTr("Only new sensors!");
                color: Style.errorTextColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
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

                    height: filtermenu.height;
                    width: 200;

                    backgroundColor: Style.baseColor;
                    currentIndex: 0;

                    radius: 3;

                    onCurrentIndexChanged: {
                        let objectFilter = container.modelFilter.GetData("LicenseFilter");
                        if (!objectFilter){
                            objectFilter = container.modelFilter.AddTreeModel("LicenseFilter")
                        }

                        if (licenseComboBox.currentIndex >= 0){
                            let value = licenseComboBox.model.GetData("Id", licenseComboBox.currentIndex);

                            objectFilter.SetData("Key", "Status");
                            objectFilter.SetData("Value", value);

                            container.updateGui();
                        }
                    }
                }
            }

            FilterMenu {
                id: filtermenu

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;

                decoratorSource: Style.filterPanelDecoratorPath;

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

//    filterMenu: Component {
//        FilterMenu {
//            decoratorSource: Style.filterPanelDecoratorPath;

//            onVisibleChanged: {
//                if (visible){
//                    if (container.commandsDelegate){
//                        let ok = container.commandsDelegate.filterByNewActive;
//                        prefixLoaderComp = ok ? textComp: null;
//                    }
//                }
//            }

//            Component {
//                id: textComp;

//                Text {
//                    id: titleInstanceId;
//                    text: qsTr("Only new sensors!");
//                    color: Style.errorTextColor;
//                    font.family: Style.fontFamily;
//                    font.pixelSize: Style.fontSize_common;
//                }
//            }

//            property bool isNewDevices: container.commandsDelegate ? container.commandsDelegate.filterByNewActive : false;
//            onIsNewDevicesChanged: {
//                prefixLoaderComp = isNewDevices ? textComp: null;
//            }
//        }
//    }

    function fillContextMenuModel(){
        contextMenuModel.clear();
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../Icons/Light/Edit_On_Normal.svg"});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../Icons/Light/Remove_On_Normal.svg"});
        contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
    }

    function collectionUpdated(){
        let notificationModel = container.baseCollectionView.commands.notificationModel;
        if (notificationModel){
            let counter = notificationModel.GetData("NewCount");
            if (counter > 0){
                if (counter > 99){
                    counter = '99+'
                }

                container.commandsProvider.setCommandNotification("ShowNew", counter);
            }
            else{
                container.commandsProvider.setCommandNotification("ShowNew", "");
            }
        }
    }

    function onCommandsModelChanged(){
        console.log("onCommandsModelChanged");
        let index = container.commandsProvider.getCommandIndex("ShowNew");
        if (index >= 0){
            container.commandsProvider.commandsModel.SetData("IsToggleable", true, index);
            container.commandsProvider.commandsModel.SetData("IsToggled", false, index);
        }

        index = container.commandsProvider.getCommandIndex("WithLicense");
        if (index >= 0){
            container.commandsProvider.commandsModel.SetData("IsToggleable", true, index);
            container.commandsProvider.commandsModel.SetData("IsToggled", false, index);
        }

        index = container.commandsProvider.getCommandIndex("WithoutLicense");
        if (index >= 0){
            container.commandsProvider.commandsModel.SetData("IsToggleable", true, index);
            container.commandsProvider.commandsModel.SetData("IsToggled", false, index);
        }

        container.commandsProvider.commandsModel.Refresh();
        container.commandsProvider.updateGui();
    }

    onHeadersChanged: {
        container.baseCollectionView.table.setColumnContentComponent(0, pairComp);
    }

    DeviceProductionStatus {
        id: deviceProductionStatus;
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

                //                source: "../../../../Icons/" + Style.theme + "/Key.svg";

                sourceSize.width: width;
                sourceSize.height: height;
            }

            Text {
                id: statusLable;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: image.right
                anchors.leftMargin: 10
                anchors.right: parent.right

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
                color: Style.textColor;

                elide: Text.ElideRight;
            }

            Component.onCompleted: {
                let loader = parent;
                let tableCellDelegate = loader.parent;

                let rowIndex = tableCellDelegate.rowIndex;
                if (rowIndex >= 0){
                    let statusId = container.table.elements.GetData("StatusId", rowIndex);
                    image.source = deviceProductionStatus.getIconPath(statusId);
                }

                statusLable.text = tableCellDelegate.getValue();
            }
        }
    }
}
