import QtQuick 2.0
import Acf 1.0
import imtlicgui 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: window;

    anchors.fill: parent;

    property alias localSettings: settingsProvider.localModel;

    signal settingsUpdate(string pageId);

    onSettingsUpdate: {
        console.log("window onSettingsUpdate");
    }

    function updateModels(){
        console.log("window updateModels");
        thumbnailDecorator.userManagementProvider.updateModel();
    }

    function updateAllModels(){
        settingsProvider.updateModel();
        thumbnailDecorator.updateModels();
    }



    SettingsProvider {
        id: settingsProvider;

        root: window;
    }

    ThumbnailDecorator {
        id: thumbnailDecorator;

        root: window;

        anchors.fill: parent;
    }
}
