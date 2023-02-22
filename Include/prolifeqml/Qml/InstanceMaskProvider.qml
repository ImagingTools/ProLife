import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

Item {
    id: root;

    property SettingsProvider settingsProvider: null;

    function getInstanceMask(){
        let localModel = root.settingsProvider.serverModel;

        if (localModel == null){
            localModel = root.settingsProvider.localModel;
        }

        if (localModel == null){
            return null;
        }

        for (let i = 0; i < localModel.GetItemsCount(); i++){
            let pageModel = localModel.GetModelFromItem(i);

            if (pageModel){
                let pageId = pageModel.GetData("Id");
                if (pageId === "Server"){
                    let elements = pageModel.GetData("Parameters");

                    for (let j = 0; j < elements.GetItemsCount(); j++){
                        let elementId = elements.GetData("Id", j);
                        if (elementId === "InstanceMask"){
                            let elementValue = elements.GetData("Value", j);
                            return elementValue;
                        }
                    }
                }
            }
        }
    }
}
