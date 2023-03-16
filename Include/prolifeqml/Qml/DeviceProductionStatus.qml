import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

QtObject {
    id: root;

    property TreeItemModel statusModel: TreeItemModel {}
    property TreeItemModel availableModel: TreeItemModel {}

    property var availableStates: ({})

    Component.onCompleted: {
        let index = root.statusModel.InsertNewItem();

        // 0
        root.statusModel.SetData("Id", "None", index);
        root.statusModel.SetData("Name", qsTr("None"), index);

        index = root.statusModel.InsertNewItem();

        // 1
        root.statusModel.SetData("Id", "Accepted", index);
        root.statusModel.SetData("Name", qsTr("Accepted"), index);

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

//        root.availableStates["None"] = ["None", "Accepted"]
//        root.availableStates["Accepted"] = ["Accepted", "InProgress", "Canceled", "OnHold"]
//        root.availableStates["InProgress"] = ["InProgress", "Finished"]
//        root.availableStates["Canceled"] = ["Canceled", "None"]
//        root.availableStates["OnHold"] = ["OnHold", "Accepted", "InProgress"]
//        root.availableStates["Finished"] = ["Finished"]
    }

//    function getStatusIndex(statusId){
//        for (let i = 0; i < root.statusModel.GetItemsCount(); i++){
//            let id = root.statusModel.GetData("Id", i);
//            if (id === statusId){
//                return i;
//            }
//        }

//        return -1;
//    }

//    function getStatusName(statusId){
//        for (let i = 0; i < root.statusModel.GetItemsCount(); i++){
//            let id = root.statusModel.GetData("Id", i);
//            if (id === statusId){
//                let name = root.statusModel.GetData("Name", i);
//                return name;
//            }
//        }

//        return "";
//    }

//    function getAvailableModel(statusId){
//        console.log("getAvailableModel", statusId);
//        let index = -1;
//        for (let i = 0; i < root.statusModel.GetItemsCount(); i++){
//            let id = root.statusModel.GetData("Id", i);
//            if (id === statusId){
//                index = i;
//                break;
//            }
//        }

//        console.log("index", index);

//        if (index >= 0){
//            root.availableModel.Clear();

//            let data = root.availableStates[statusId];

//            for (let i = 0; i < data.length; i++){
//                let id = data[i];
//                let name = root.getStatusName(id);

//                let j = root.availableModel.InsertNewItem();

//                root.availableModel.SetData("Id", id, j);
//                root.availableModel.SetData("Name", name, j);
//            }

//            return root.availableModel;
//        }

//        return null;
//    }
}
