import QtQuick 2.12
import imtgui 1.0

CollectionView {
    id: container;

    visibleMetaInfo: false;

    Component.onCompleted: {
        container.commandUpdateGui = "DeviceCollectionUpdateGui";
        console.log("Device collection view complete");
    }

    onVisibleChanged: {
        if (container.visible){
            container.updateGui();
        }
    }
}
