import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

CollectionView {
    id: container;

    visibleMetaInfo: false;

    defaultSortHeaderIndex: 6;
    defaultOrderType: "DESC";

    property MainDocumentManager mainDocumentManager: null;

    Component.onCompleted: {
        // Events.subscribeEvent("OrdersCollectionUpdated", container.updateGui);
        Events.subscribeEvent("DevicesCollectionUpdated", container.collectionUpdated);
        container.commandsDelegatePath = "qrc:/qml/ProLife/DeviceCollectionViewCommandsDelegate.qml";

        baseCollectionView.commands.fieldsData.push("OrderUuid");
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
                    let ok = container.commandsDelegate.filterByNewActive;
                    prefixLoaderComp = ok ? textComp: null;
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

            property bool isNewDevices: container.commandsDelegate.filterByNewActive;
            onIsNewDevicesChanged: {
                prefixLoaderComp = isNewDevices ? textComp: null;
            }
        }
    }

    function fillContextMenuModel(){
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../Icons/Light/Edit_On_Normal.svg"});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../Icons/Light/Remove_On_Normal.svg"});
        contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
    }

    function collectionUpdated(){
        let notificationModel = container.baseCollectionView.commands.notificationModel;
        if (notificationModel){
            let counter = notificationModel.GetData("NewCount");
            if (counter > 0){
                container.commandsProvider.setCommandNotification("ShowNew", counter);
            }
            else{
                container.commandsProvider.setCommandNotification("ShowNew", "");
            }
        }

        if (container.commandsDelegate && container.commandsDelegate.filterByNewActive){
            container.commandsProvider.setCommandNotification("ShowNew", "");
        }
    }
}
