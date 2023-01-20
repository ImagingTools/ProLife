import QtQuick 2.12
import imtgui 1.0

CollectionView {
    id: container;

    Component.onCompleted: {
        container.commandUpdateGui = "OrderCollectionUpdateGui";
       // container.commandsDelegatePath = "../../imtauthgui/OrderCollectionViewCommandsDelegate.qml";
        console.log("Order collection view complete");
    }
}
