import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Dialog {
    id: root;

    width: 500;

    property TreeItemModel documentModel: TreeItemModel{}
    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}
    property TreeItemModel orderProductsModel: TreeItemModel{}
    property int activeProductIndex: -1;

    property string orderId;
    property string orderUuid;

    onDocumentModelChanged: {
//        root.contentItem.documentModel = root.documentModel;
    }

    Component.onCompleted: {
        root.buttons.addButton({"Id": "Save", "Name": "Save", "Enabled": false});
        root.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        root.title = qsTr("Product editor");
    }

    contentComp: Component {
        id: installationEditor;
        InstallationEditor {

            licensesModel: root.licensesModel;
            productsModel: root.productsModel;
            orderProductsModel: root.orderProductsModel;
            activeProductIndex: root.activeProductIndex;
            orderId: root.orderId;
            orderUuid: root.orderUuid;
            width: root.width - 100;
            height: 350;

            rootItem: root;

            onActiveProductIndexChanged: {
                root.activeProductIndex = activeProductIndex;
            }
        }
    }
}//Container


