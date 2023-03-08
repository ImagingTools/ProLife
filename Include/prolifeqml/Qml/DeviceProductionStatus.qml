import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

QtObject {
    id: root;

    property TreeItemModel statusModel: TreeItemModel {}
    property TreeItemModel availableModel: TreeItemModel {}

    property var machineStates: ({})

    Component.onCompleted: {
        let index = statusModel.InsertNewItem();

        // 0
        statusModel.SetData("Id", "None", index);
        statusModel.SetData("Name", qsTr("None"), index);

        index = statusModel.InsertNewItem();

        // 1
        statusModel.SetData("Id", "Accepted", index);
        statusModel.SetData("Name", qsTr("Accepted"), index);

        index = statusModel.InsertNewItem();

        // 2
        statusModel.SetData("Id", "InProgress", index);
        statusModel.SetData("Name", qsTr("In Progress"), index);

        index = statusModel.InsertNewItem();

        // 3
        statusModel.SetData("Id", "Canceled", index);
        statusModel.SetData("Name", qsTr("Canceled"), index);

        index = statusModel.InsertNewItem();

        // 4
        statusModel.SetData("Id", "OnHold", index);
        statusModel.SetData("Name", qsTr("On Hold"), index);

        index = statusModel.InsertNewItem();

        // 5
        statusModel.SetData("Id", "Finished", index);
        statusModel.SetData("Name", qsTr("Finished"), index);

        machineStates[0] = [1]
        machineStates[1] = [2, 3, 4]
        machineStates[2] = [5]
        machineStates[3] = [0]
        machineStates[4] = [0]
        machineStates[5] = []
    }

    function getAvailableModel(state){
        if (state < 0 || state >= statusModel.GetItemsCount()){
            return null;
        }

        availableModel.Clear();

        let data = machineStates[state];

        for (let i = 0; i < data.length; i++){
            let index = availableModel.InsertNewItem();

            availableModel.CopyItemDataFromModel(index, statusModel, i);
        }

        return availableModel;
    }
}
