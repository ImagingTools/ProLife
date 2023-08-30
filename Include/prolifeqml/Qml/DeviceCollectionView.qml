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
        FilterMenu {
            decoratorSource: Style.filterPanelDecoratorPath;

            onVisibleChanged: {
                if (visible){
                    if (container.commandsDelegate){
                        let ok = container.commandsDelegate.filterByNewActive;
                        prefixLoaderComp = ok ? textComp: null;
                    }
                }
            }

            Component {
                id: textComp;

                Text {
                    id: titleInstanceId;
                    text: qsTr("Only new sensors!");
                    color: Style.errorTextColor;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                }
            }

            property bool isNewDevices: container.commandsDelegate ? container.commandsDelegate.filterByNewActive : false;
            onIsNewDevicesChanged: {
                prefixLoaderComp = isNewDevices ? textComp: null;
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

                source: "../../../../Icons/" + Style.theme + "/Key.svg";

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
