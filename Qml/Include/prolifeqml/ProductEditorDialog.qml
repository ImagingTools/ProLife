import QtQuick 2.0
import imtgui 1.0
import imtcontrols 1.0
import Acf 1.0

Dialog {
    id: productEditorDialog;

//    width: rootWidth - 100 < 800 ? rootWidth - 100 : 800;
//    height: rootHeight - 100 < 700 ? rootHeight - 100 : 700;

//    width: 800;
//    height: 500;

    backgroundColor: Style.backgroundColor2;

    property int rootWidth: root ? root.width: 0;
    property int rootHeight: root ? root.height: 0;

    property TreeItemModel productModel: TreeItemModel {}

    property TreeItemModel documentModel: TreeItemModel{}
    property TreeItemModel licensesModel: TreeItemModel{}
    property TreeItemModel productsModel: TreeItemModel{}
    property int activeProductIndex: -1;

    property string orderId;
    property string orderUuid;

    property bool serialNumberEdit: true;

    Component.onCompleted: {
        productEditorDialog.buttonsModel.append({"Id": Enums.ok, "Name": qsTr("OK"), "Enabled": false});
        productEditorDialog.buttonsModel.append({"Id": Enums.cancel, "Name": qsTr("Cancel"), "Enabled": true});

        productEditorDialog.title = qsTr("Product editor");
    }

    contentComp: Component {
        ProductEditor {
            width: 800;
            height: 700;

            rootItem: productEditorDialog;

            serialNumberEdit: productEditorDialog.serialNumberEdit;
        }
    }
}//Container


