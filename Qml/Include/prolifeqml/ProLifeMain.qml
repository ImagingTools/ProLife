import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import prolifeqml 1.0

ApplicationMain{
    id: window;

    useWebSocketSubscription: true;
    loadPageByClick: false;
    canRecoveryPassword: false;

    systemStatus: "UNKNOWN";

    Component.onCompleted: {
        context.application = 'ProLife';

        Events.subscribeEvent("Login", loginSuccesful);
    }

    Component.onDestruction: {
         Events.unSubscribeEvent("Login", loginSuccesful);
    }

    ModalDialogManager {
        id: modalDialogManager;

        z: 30;

        anchors.fill: parent;
    }

    function loginSuccesful(){
        CachedProductCollection.updateModel();
        CachedLicenseCollection.updateModel();
        CachedAccountCollection.updateModel();
        CachedOrderCollection.updateModel();
        CachedDeviceCollection.updateModel();
//        CachedGroupCollection.updateModel();
//        CachedUserCollection.updateModel();
//        CachedRoleCollection.updateModel();
    }
}

