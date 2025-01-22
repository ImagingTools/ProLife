//import QtQuick 2.0
import QtQml
import Acf 1.0
import imtgui 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import prolifeqml 1.0

ApplicationMain {
    id: window;

    useWebSocketSubscription: true;
	// canRecoveryPassword: false;
    authorizationServerConnected: pumaConnectionChecker.status === 1;

    Connections {
        target: AuthorizationController;

        function onLoginSuccessful(){
            CachedProductCollection.updateModel();
            CachedLicenseCollection.updateModel();
            CachedAccountCollection.updateModel();
            CachedOrderCollection.updateModel();
            CachedDeviceCollection.updateModel();
            CachedSoftwareCollection.updateModel();
        }

        function onLogoutSignal(){
            CachedProductCollection.clearModel();
			CachedLicenseCollection.clearModel();
            CachedAccountCollection.clearModel();
            CachedOrderCollection.clearModel();
            CachedDeviceCollection.clearModel();
            CachedSoftwareCollection.clearModel();
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
				PopupManager.addWarningMessage(qsTr("Lost connection to Lisa server"), false);
            }
        }
    }
}

