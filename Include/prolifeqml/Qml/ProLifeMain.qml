import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

ApplicationMain{
    id: window;

    useWebSocketSubscription: true;

    property InstanceMaskProvider instanceMaskProvider :InstanceMaskProvider {
        settingsProvider: window.settingsProvider;
    }
}

