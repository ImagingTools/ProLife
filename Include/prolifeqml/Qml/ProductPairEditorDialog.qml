import QtQuick 2.0
import imtgui 1.0
import Acf 1.0

Dialog {
    id: productEditorDialog;

    width: 500;

    property string orderId;
    property string softwareId;

    Component.onCompleted: {
        productEditorDialog.buttons.addButton({"Id": "Link", "Name": "Link", "Enabled": false});
        productEditorDialog.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        productEditorDialog.title = qsTr("Product pair editor");
    }

    contentComp: Component {
        id: productPairEditor;

        ProductPairEditor {
            width: 300;
            height: contentHeight + 40;

            orderId: productEditorDialog.orderId;

            onSelectionChanged: {
                let indexes = table.getSelectedIndexes();

                productEditorDialog.buttons.setButtonState("Link", indexes.length > 0 && selectedProductId !== "")
            }
        }
    }
}//Container


