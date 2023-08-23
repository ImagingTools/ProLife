import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

CollectionView {
    id: container;

//    visibleMetaInfo: false;

    defaultSortHeaderIndex: 6;
    defaultOrderType: "DESC";

    property MainDocumentManager mainDocumentManager: null;

    Component.onCompleted: {
        // Events.subscribeEvent("OrdersCollectionUpdated", container.updateGui);
        Events.subscribeEvent("DevicesCollectionUpdated", container.collectionUpdated);
        container.commandsDelegatePath = "qrc:/qml/ProLife/DeviceCollectionViewCommandsDelegate.qml";

        baseCollectionView.commands.fieldsData.push("OrderUuid");
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
            height: 30;

            TreeItemModel {
                id: modelCategogy;

                Component.onCompleted: {
                    let index = modelCategogy.InsertNewItem();
                    modelCategogy.SetData("Id", "AllLicense", index);
                    modelCategogy.SetData("Name", qsTr("All"), index);

                    index = modelCategogy.InsertNewItem();
                    modelCategogy.SetData("Id", "WithoutLicense", index);
                    modelCategogy.SetData("Name", qsTr("Without a license"), index);

                    index = modelCategogy.InsertNewItem();
                    modelCategogy.SetData("Id", "WithLicense", index);
                    modelCategogy.SetData("Name", qsTr("With license"), index);

                    licenseComboBox.model = modelCategogy;
                }
            }

            Text {
                id: titleInstanceId;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: licenseTitle.left;
                anchors.rightMargin: 20
                visible: container.commandsDelegate ? container.commandsDelegate.filterByNewActive : false;
                text: qsTr("Only new sensors!");
                color: Style.errorTextColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Text {
                id: licenseTitle
                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: filtermenu.left;
                anchors.rightMargin: 20
//                text: qsTr("Availability of a license")
                text: container.commandsDelegate.filterLicense
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

//            ComboBox {
//                 id: licenseComboBox;

//                 anchors.verticalCenter: parent.verticalCenter;
//                 anchors.right: filtermenu.left;
//                 anchors.rightMargin: 20

//                 height: 25;
//                 width: 140;

//                 backgroundColor: Style.baseColor;
//                 currentIndex: 0;

//                 radius: 0;

//                 onCurrentIndexChanged: {
//                     let objectFilter = filterModel.GetData("LicenseFilter");
//                     if (!objectFilter){
//                         objectFilter = filterModel.AddTreeModel("LicenseFilter")
//                     }

//                     objectFilter.SetData("Key", "State");
//                     objectFilter.SetData("Value", currentIndex);
//                 }
//            }

        FilterMenu {
            id: filtermenu
            anchors.right: parent.right
            decoratorSource: Style.filterPanelDecoratorPath;

            width: 325

            onVisibleChanged: {
                if (visible){
                    if (container.commandsDelegate){
                        let ok = container.commandsDelegate.filterByNewActive;
                        prefixLoaderComp = ok ? textComp: null;
                    }
                }
            }

            onTextFilterChanged: {
                container.modelFilter.SetData("TextFilter", text);
                container.baseCollectionView.commands.updateModels()
            }

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
        }
        }
    }

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
//        let rolesIndex = container.baseCollectionView.table.getHeaderIndex("Roles");
//        let groupsIndex = container.baseCollectionView.table.getHeaderIndex("Groups");
        console.log("onHeadersChanged")
        container.baseCollectionView.table.setColumnContentComponent(0, pairComp);
        console.log("onHeadersChanged2")
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

                source: "../../../../Icons/Light/Key.svg";

                sourceSize.width: width;
                sourceSize.height: height;
            }

            Text {
                id: statusLable;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: image.right
                anchors.leftMargin: 10

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
                color: Style.textColor;

                elide: Text.ElideRight;
            }

            Component.onCompleted: {
                let loader = parent;
                let tableCellDelegate = loader.parent;
                statusLable.text = tableCellDelegate.getValue();
                let softwareLinksCount = container.table.elements.GetData("SoftwareLinksCount", tableCellDelegate.rowIndex);
                if (softwareLinksCount === 0){
                    console.log("softwareLinksCount",softwareLinksCount)
                    image.visible = false
                }
            }
        }
    }

}
