import QtQuick 2.12
import imtgui 1.0

CollectionView {
    id: container;

    Component.onCompleted: {
        container.commandUpdateGui = "ProductCollectionUpdateGui";
       // container.commandsDelegatePath = "../../imtauthgui/AccountCollectionViewCommandsDelegate.qml";
        console.log("Product collection view complete");
    }
}
