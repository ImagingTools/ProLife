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
    authorizationServerConnected: pumaConnected;

    Component.onCompleted: {
        context.appName = 'ProLife';
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

    property bool pumaConnected: false;
    SubscriptionClient {
        id: pumaSub;

        property bool ok: window.subscriptionManager.status === 1;
        onOkChanged: {
            let subscriptionRequestId = "PumaWsConnection"
            var query = Gql.GqlRequest("subscription", subscriptionRequestId);
            var queryFields = Gql.GqlObject("notification");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            window.subscriptionManager.registerSubscription(query, pumaSub)
        }

        onStateChanged: {
            if (state === "Ready"){
                if (pumaSub.containsKey("data")){
                    let localModel = pumaSub.getData("data")

                    if (localModel.containsKey("PumaWsConnection")){
                        localModel = localModel.getData("PumaWsConnection")

                        if (localModel.containsKey("status")){
                            let status = localModel.getData("status")
                            if (status === "Disconnected"){
                                window.pumaConnected = false;

                            }
                            else if (status === "Connected"){
                                window.pumaConnected = true;
                            }
                        }
                    }
                }
            }
        }
    }

    property bool lisaConnected: false;
    SubscriptionClient {
        id: lisaSub;

        property bool ok: window.subscriptionManager.status === 1;
        onOkChanged: {
            let subscriptionRequestId = "LisaWsConnection"
            var query = Gql.GqlRequest("subscription", subscriptionRequestId);
            var queryFields = Gql.GqlObject("notification");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            window.subscriptionManager.registerSubscription(query, lisaSub)
        }

        onStateChanged: {
            if (state === "Ready"){
                if (lisaSub.containsKey("data")){
                    let localModel = lisaSub.getData("data")

                    if (localModel.containsKey("LisaWsConnection")){
                        localModel = localModel.getData("LisaWsConnection")

                        if (localModel.containsKey("status")){
                            let status = localModel.getData("status")
                            if (status === "Disconnected"){
                                window.lisaConnected = false;

                                if (!window.wasError){
                                    ModalDialogManager.openDialog(errorDialog, {});

                                    window.wasError = true;
                                }
                            }
                            else if (status === "Connected"){
                                window.lisaConnected = true;
                            }
                        }
                    }
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

