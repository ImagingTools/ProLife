import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

ApplicationMain{
    id: window;

    property InstanceMaskProvider instanceMaskProvider :InstanceMaskProvider {
        settingsProvider: window.settingsProvider;
    }

//    SubscriptionManager {
//        id: subscriptionManager
//        Component.onCompleted: {
//            url = "ws://127.0.0.1:7778"
//        }
//    }
}

