import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

QtObject {
    id: root;

    property TreeItemModel statusModel: TreeItemModel {}

    Component.onCompleted: {
        let index = root.statusModel.InsertNewItem();

        // 0
        root.statusModel.SetData("Id", "None", index);
        root.statusModel.SetData("Name", qsTr("None"), index);

        index = root.statusModel.InsertNewItem();

        // 1
        root.statusModel.SetData("Id", "Created", index);
        root.statusModel.SetData("Name", qsTr("Created"), index);

        index = root.statusModel.InsertNewItem();

        // 2
        root.statusModel.SetData("Id", "InProgress", index);
        root.statusModel.SetData("Name", qsTr("In Progress"), index);

        index = root.statusModel.InsertNewItem();

        // 3
        root.statusModel.SetData("Id", "Canceled", index);
        root.statusModel.SetData("Name", qsTr("Canceled"), index);

        index = root.statusModel.InsertNewItem();

        // 4
        root.statusModel.SetData("Id", "OnHold", index);
        root.statusModel.SetData("Name", qsTr("On Hold"), index);

        index = root.statusModel.InsertNewItem();

        // 5
        root.statusModel.SetData("Id", "Finished", index);
        root.statusModel.SetData("Name", qsTr("Finished"), index);

        index = root.statusModel.InsertNewItem();

        // 6
        root.statusModel.SetData("Id", "Closed", index);
        root.statusModel.SetData("Name", qsTr("Closed"), index);
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
