import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import Acf 1.0

Dialog {
    id: root;

    width: 500;

    property TreeItemModel documentModel: TreeItemModel{}
    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}

    onDocumentModelChanged: {
        root.contentItem.documentModel = root.documentModel;
    }

    Component.onCompleted: {
        root.buttons.addButton({"Id": "Save", "Name": "Save", "Enabled": true});
        root.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        root.title = qsTr("Product editor");
    }

    onFinished: {
    }

    contentComp: InstallationEditor {
        id: installationEditor;
        licensesModel: root.licensesModel;
        productsModel: root.productsModel;
        width: root.width - 100;
        height: 350;
    }
}//Container


