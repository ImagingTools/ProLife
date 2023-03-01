import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

CollectionView {
    id: container;

    visibleMetaInfo: false;

    Component.onCompleted: {
        container.commandUpdateGui = "DeviceCollectionUpdateGui";
        console.log("Device collection view complete");

        Events.subscribeEvent("OrdersCollectionUpdated", container.updateGui);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OrdersCollectionUpdated", container.updateGui);
    }

    function fillContextMenuModel(){
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../Icons/Light/Edit_On_Normal.svg"});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../Icons/Light/Remove_On_Normal.svg"});
    }
}
