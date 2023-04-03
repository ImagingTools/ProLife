import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

CollectionView {
    id: container;

    visibleMetaInfo: false;

    property MainDocumentManager mainDocumentManager: null;

    Component.onCompleted: {
        Events.subscribeEvent("OrdersCollectionUpdated", container.updateGui);
        Events.subscribeEvent("DevicesCollectionUpdated", container.collectionUpdated);
        container.commandsDelegatePath = "qrc:/qml/ProLife/DeviceCollectionViewCommandsDelegate.qml";

        baseCollectionView.commands.fieldsData.push("OrderUuid");
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OrdersCollectionUpdated", container.updateGui);
        Events.unSubscribeEvent("DevicesCollectionUpdated", container.collectionUpdated);
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
    }
}
