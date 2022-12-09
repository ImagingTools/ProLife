import QtQuick 2.0
import QtWebSockets 1.15
import Acf 1.0
import imtlicgui 1.0
import imtgui 1.0
import imtqml 1.0
//import Qt.labs.platform 1.1

Rectangle {
    id: window;

    anchors.fill: parent;

    width: 350;
    height: 550;

    color: "#5d5d5d";

    Style {
        id: Style;

        Component.onCompleted: {
            Style.mainFontSource = "../Fonts/Ubuntu-Light.ttf";
            Style.boldFontSource = "../Fonts/Ubuntu-Bold.ttf";
        }
    }
    Item {
        id: localSettingsProvider;
        property alias localSettings: settingsProvider.localModel;

        Component.onCompleted: {
            localSettings.toJSON("{\"ComponentType\":\"UNKNOWN\",\"Elements\":[{\"ComponentType\":\"ComboBox\",\"Id\":\"Mode\",\"Name\":\"\",\"Parameters\":[{\"Id\":\"Light\",\"Name\":\"Light\"},{\"Id\":\"Dark\",\"Name\":\"Dark\"}],\"Value\":1},{\"ComponentType\":\"ComboBox\",\"Id\":\"Language\",\"Name\":\"\",\"Parameters\":[{\"Id\":\"en_US\",\"Name\":\"English\"},{\"Id\":\"de_DE\",\"Name\":\"German\"},{\"Id\":\"ru_RU\",\"Name\":\"Russian\"},{\"Id\":\"pl_PL\",\"Name\":\"Polish\"}],\"Value\":2},{\"ComponentType\":\"TextInput\",\"Id\":\"InstanceMask\",\"Name\":\"\",\"Value\":\"^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$\"}],\"Id\":\"General\",\"Name\":\"\"}")
        }
    }

    function updateAllModels(){
        settingsProvider.updateModel();

        thumbnailDecorator.updateModels();

        treeViewModel.updateModel();
    }

    TreeViewModel {
        id: treeViewModel;
    }

    SettingsProvider {
        id: settingsProvider;

        root: window;
    }

    ThumbnailDecorator {
        id: thumbnailDecorator;
        anchors.fill: parent;

        Component.onCompleted: {
          //  console.log("ThumbnailDecorator onCompleted", MeterEnum.ID);
               thumbnailDecorator.userManagementProvider.updateModel();
//            thumbnailDecorator.updateModels();
        }
    }

}


