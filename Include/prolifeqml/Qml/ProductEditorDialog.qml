import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Dialog {
    id: productEditorDialog;

    width: 500;

    property TreeItemModel productModel: TreeItemModel {}

    property TreeItemModel documentModel: TreeItemModel{}
    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}
    property TreeItemModel orderProductsModel: TreeItemModel{}
    property int activeProductIndex: -1;

    property string orderId;
    property string orderUuid;

    property bool isPairEditing: false;

    Component.onCompleted: {
        productEditorDialog.buttons.addButton({"Id": "Save", "Name": "OK", "Enabled": false});
        productEditorDialog.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        productEditorDialog.title = qsTr("Product editor");
    }

    contentComp: Component {
        id: installationEditor;

        ProductEditor {

            width: productEditorDialog.width - 100;
            height: 400;

            rootItem: productEditorDialog;
        }
    }
}//Container


