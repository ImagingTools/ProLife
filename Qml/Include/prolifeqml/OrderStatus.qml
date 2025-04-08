import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

QtObject {
    id: root;

    property TreeItemModel statusModel: TreeItemModel {}

    Component.onCompleted: {
        let index = root.statusModel.insertNewItem();

        // 0
        root.statusModel.setData("id", "Created", index);
        root.statusModel.setData("name", qsTr("Created"), index);

        index = root.statusModel.insertNewItem();

        // 1
        root.statusModel.setData("id", "InProgress", index);
        root.statusModel.setData("name", qsTr("In Progress"), index);

        index = root.statusModel.insertNewItem();

        // 2
        root.statusModel.setData("id", "Canceled", index);
        root.statusModel.setData("name", qsTr("Canceled"), index);

        index = root.statusModel.insertNewItem();

        // 3
        root.statusModel.setData("id", "Closed", index);
        root.statusModel.setData("name", qsTr("Closed"), index);
    }

    function getIconPath(statusId){
        if (statusId === "None"){
            return "qrc:/Light/Icons/StateUnknown_On_Active";
        }
        else if (statusId === "Canceled"){
            return "qrc:/Light/Icons/Cancel_On_Active";
        }
        else if (statusId === "Accepted" || statusId === "InProgress"){
            return "qrc:/Light/Icons/Timeline_On_Active";
        }
        else if (statusId === "OnHold"){
            return "qrc:/Light/Icons/Pause_On_Active";
        }
        else{
            return "qrc:/Light/Icons/StateOk_On_Active";
        }
    }
}
