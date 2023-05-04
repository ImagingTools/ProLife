import QtQuick 2.0
import Acf 1.0
import imtlicgui 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: window;

    anchors.fill: parent;

    property alias localSettings: settingsProviderLocal.localModel;
    property alias applicationInfo: applicationInfoProviderLocal.clientApplicationInfo;
    property alias settingsProvider: settingsProviderLocal;
    property alias designSchemaProviderAlias: designSchemaProvider;

    signal settingsUpdate();

    signal localSettingsUpdated();

    onSettingsUpdate: {
        console.log("window onSettingsUpdate");
    }

    onLocalSettingsUpdated: {
        updateAllModels();
    }

    function updateModels(){
        console.log("window updateModels");
        thumbnailDecorator.userManagementProvider.updateModel();
    }

    function updateAllModels(){
        settingsProviderLocal.updateModel();

        console.log("thumbnailDecorator.updateModels");
        thumbnailDecorator.updateModels();

        applicationInfoProviderLocal.updateModel();
    }

    function updateServerSettings(){
        settingsProviderLocal.updateModel();
    }

    SettingsProvider {
        id: settingsProviderLocal;

        root: window;

        onServerModelChanged: {
            settingsObserver.registerModel(settingsProviderLocal.serverModel);
            designSchemaProvider.applyDesignSchema();
        }

        onLocalModelChanged: {
            console.log("onLocalModelChanged");

            localSettingsModelObserver.registerModel(settingsProviderLocal.localModel);
            timer.start();
        }
    }

    // Timer for updating design schema when start application, without this timer request does not come
    Timer {
        id: timer;

        interval: 100;

        onTriggered: {
            designSchemaProvider.applyDesignSchema();
        }
    }

    ApplicationInfoProvider {
        id: applicationInfoProviderLocal;
    }

    ServerSettingsModelObserver {
        id: settingsObserver;

        designProvider: designSchemaProvider;
        languageProvider: langProvider;

        root: window;
    }

    LocalSettingsModelObserver {
        id: localSettingsModelObserver;

        designProvider: designSchemaProvider;
        languageProvider: langProvider;
    }

    DesignSchemaProvider {
        id: designSchemaProvider;

        settingsProvider: settingsProviderLocal;
    }

    LanguageProvider {
        id: langProvider;

        settingsProvider: settingsProviderLocal;
    }

    InstanceMaskProvider {
        id: instanceMaskProvider;

        settingsProvider: settingsProviderLocal;
    }

    ThumbnailDecorator {
        id: thumbnailDecorator;
        anchors.fill: parent;
        root: window;
        settingsProvider: settingsProviderLocal;

        applicationInfoProvider: applicationInfoProviderLocal;
        Component.onCompleted: {
            thumbnailDecorator.userManagementProvider.updateModel();
        }
    }
}
