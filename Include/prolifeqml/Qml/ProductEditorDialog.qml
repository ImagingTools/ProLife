import QtQuick 2.0
import imtgui 1.0
import imtcontrols 1.0
import Acf 1.0

Dialog {
    id: productEditorDialog;

    width: 650;

    property TreeItemModel productModel: TreeItemModel {}

    property TreeItemModel documentModel: TreeItemModel{}
    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}
    property int activeProductIndex: -1;

    property string orderId;
    property string orderUuid;

    property bool serialNumberEdit: true;

    Component.onCompleted: {
        productEditorDialog.buttons.addButton({"Id": Enums.ButtonType.Ok, "Name": "OK", "Enabled": false});
        productEditorDialog.buttons.addButton({"Id": Enums.ButtonType.Cancel, "Name": "Cancel", "Enabled": true});

        productEditorDialog.title = qsTr("Product editor");
    }

    contentComp: Component {
        ProductEditor {
            width: productEditorDialog.width - 100;
            height: 400;

            rootItem: productEditorDialog;

            serialNumberEdit: productEditorDialog.serialNumberEdit;
        }
    }
}//Container


