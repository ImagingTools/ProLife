import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Dialog {
    id: root;

    width: 500;

    property TreeItemModel documentModel: TreeItemModel {}

    Component.onCompleted: {
        root.buttons.addButton({"Id": "Save", "Name": "Save", "Enabled": true});
        root.buttons.addButton({"Id": "Cancel", "Name": "Cancel", "Enabled": true});

        root.title = qsTr("Device editor");
    }

    contentComp: Component {
        id: deviceEditor;

        DeviceEditor {
            width: root.width - 100;
            height: 500;

            documentModel: root.documentModel;

            orderComboBoxEnabled: false;
            deviceTypeComboBoxEnabled: false;

            itemLoad: false;
        }
    }
}//Container


