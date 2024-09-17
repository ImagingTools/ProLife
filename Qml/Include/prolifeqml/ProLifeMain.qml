import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import prolifeqml 1.0

ApplicationMain{
    id: window;

    useWebSocketSubscription: true;
    loadPageByClick: false;
    canRecoveryPassword: false;
    authorizationServerConnected: pumaConnectionChecker.status === 1;

    Component.onCompleted: {
        context.appName = 'ProLife';
        AuthorizationController.productId = context.appName;
    }

    Connections {
        target: AuthorizationController;

        function onLoginSuccessful(){
            CachedProductCollection.updateModel();
            CachedLicenseCollection.updateModel();
            CachedAccountCollection.updateModel();
            CachedOrderCollection.updateModel();
            CachedDeviceCollection.updateModel();
            CachedSoftwareCollection.updateModel();

            CachedGroupCollection.updateModel();
            CachedUserCollection.updateModel();

            CachedRoleCollection.productId = context.appName;
            CachedRoleCollection.updateModel();
        }

        function onLogoutSignal(){
            CachedProductCollection.clearModel();
            CachedLicenseCollection.clearModel();
            CachedAccountCollection.clearModel();
            CachedOrderCollection.clearModel();
            CachedDeviceCollection.clearModel();
            CachedSoftwareCollection.clearModel();

            CachedGroupCollection.clearModel();
            CachedUserCollection.clearModel();
            CachedRoleCollection.clearModel();
        }
    }

    WebSocketConnectionChecker {
        id: pumaConnectionChecker;
        subscriptionManager: window.subscriptionManager;
        subscriptionRequestId: "PumaWsConnection";
    }

    WebSocketConnectionChecker {
        id: lisaConnectionChecker;
        subscriptionManager: window.subscriptionManager;
        subscriptionRequestId: "LisaWsConnection";

        onStatusChanged: {
            if (status === 2){
                if (!window.wasError){
                    ModalDialogManager.openDialog(errorDialog, {});
                    window.wasError = true;
                }
            }
        }
    }

    property bool wasError: false;
    Component {
        id: errorDialog;

        ErrorDialog {
            title: qsTr("Warning Message");
            message: qsTr("Lisa server connection error");

            onFinished: {}
        }
    }
}

