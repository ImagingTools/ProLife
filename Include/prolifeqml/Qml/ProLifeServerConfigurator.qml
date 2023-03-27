import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Rectangle {
    id: window;

    anchors.fill: parent;

    color: Style.backgroundColor;

    property alias localSettings: preferenceDialog.settingsModel;
    signal settingsUpdate();

    onLocalSettingsChanged: {
        if (window.localSettings){
            console.log("window onLocalSettingsChanged", window.localSettings.toJSON());
        }
    }

    MouseArea{
        anchors.fill: parent;

        onClicked: {
            preferenceDialog.visible = true;
        }
    }

    Preference {
        id: preferenceDialog;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: buttons.top;

        onSettingsModelChanged: {
            if (preferenceDialog.settingsModel != null){
                console.log("typeof settingsModel", typeof settingsModel);

//                settingsModelObserver.registerModel(settingsModel);
            }
        }

        onModelChanged: {
            console.log("onModelChanged");
            buttons.setButtonState("Apply", true);
        }
    }

    TreeItemModelObserver {
        id: settingsModelObserver;

        onModelChanged: {
            buttons.setButtonState("Apply", true);
        }
    }

    ButtonsDialog {
        id: buttons;

        anchors.bottom: parent.bottom;
        anchors.right: parent.right;
        anchors.rightMargin: 20;
        anchors.bottomMargin: 10;

        Component.onCompleted: {
            buttons.addButton({"Id":"Apply", "Name": qsTr("Apply"), "Enabled": false});
        }

        onButtonClicked: {
            if (buttonId === "Apply"){
                window.settingsUpdate();

                buttons.setButtonState("Apply", false);
            }
        }
    }

    ModalDialogManager {
        id: modalDialogManager;

        z: 30;

        anchors.fill: parent;
    }
}


