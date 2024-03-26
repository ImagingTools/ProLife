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
            console.log("PumaWsConnection onStateChanged", state);

            if (state === "Ready"){
                if (pumaSub.ContainsKey("data")){
                    let localModel = pumaSub.GetData("data")

                    if (localModel.ContainsKey("PumaWsConnection")){
                        localModel = localModel.GetData("PumaWsConnection")

                        if (localModel.ContainsKey("status")){
                            let status = localModel.GetData("status")
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
            console.log("LisaWsConnection onStateChanged", state);

            if (state === "Ready"){
                console.log("data", lisaSub.toJSON());

                if (lisaSub.ContainsKey("data")){
                    let localModel = lisaSub.GetData("data")

                    if (localModel.ContainsKey("LisaWsConnection")){
                        localModel = localModel.GetData("LisaWsConnection")

                        if (localModel.ContainsKey("status")){
                            let status = localModel.GetData("status")
                            if (status === "Disconnected"){
                                window.lisaConnected = false;

                                if (!window.errorVisible){
                                    modalDialogManager.openDialog(errorDialog, {});
                                }
                            }
                            else if (status === "Connected"){
                                window.lisaConnected = true;

                                window.errorVisible = false;
                            }
                        }
                    }
                }
            }
        }
    }

    property bool errorVisible: false;

    Component {
        id: errorDialog;

        ErrorDialog {
            title: qsTr("Warning Message");
            message: qsTr("Lisa server connection error");

            Component.onCompleted: {
                window.errorVisible = true;
            }

            onFinished: {}
        }
    }
}

