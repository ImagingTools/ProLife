import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

QtObject {
    id: root;

    property TreeItemModel statusModel: TreeItemModel {}
    property TreeItemModel availableModel: TreeItemModel {}

    property var availableStates: ({})

    Component.onCompleted: {
        let index = root.statusModel.insertNewItem();

        // 0
        root.statusModel.setData("Id", "None", index);
        root.statusModel.setData("Name", qsTr("None"), index);

        index = root.statusModel.insertNewItem();

        // 1
        root.statusModel.setData("Id", "Accepted", index);
        root.statusModel.setData("Name", qsTr("Accepted"), index);

        index = root.statusModel.insertNewItem();

        // 2
        root.statusModel.setData("Id", "InProgress", index);
        root.statusModel.setData("Name", qsTr("In Progress"), index);

        index = root.statusModel.insertNewItem();

        // 3
        root.statusModel.setData("Id", "Canceled", index);
        root.statusModel.setData("Name", qsTr("Canceled"), index);

        index = root.statusModel.insertNewItem();

        // 4
        root.statusModel.setData("Id", "OnHold", index);
        root.statusModel.setData("Name", qsTr("On Hold"), index);

        index = root.statusModel.insertNewItem();

        // 5
        root.statusModel.setData("Id", "Finished", index);
        root.statusModel.setData("Name", qsTr("Finished"), index);
    }

    function getIconPath(statusId){
        if (statusId === "None"){
            return "../../../../" + Style.getIconPath("Icons/StateUnknown", Icon.State.On, Icon.Mode.Active);
        }
        else if (statusId === "Canceled"){
            return "../../../../" + Style.getIconPath("Icons/Cancel", Icon.State.On, Icon.Mode.Active);
        }
        else if (statusId === "Accepted" || statusId === "InProgress"){
            return "../../../../" + Style.getIconPath("Icons/Timeline", Icon.State.On, Icon.Mode.Active);
        }
        else if (statusId === "OnHold"){
            return "../../../../" + Style.getIconPath("Icons/Pause", Icon.State.On, Icon.Mode.Active);
        }
        else{
            return "../../../../" + Style.getIconPath("Icons/StateOk", Icon.State.On, Icon.Mode.Active);
        }
    }
}
