// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
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
	authConnectionState: pumaConnectionChecker.status
	
	Connections {
		target: AuthorizationController;
		
		function onLoggedIn(){
			CachedOrderCollection.updateModel();
			CachedAccountCollection.updateModel();
		}
		
		function onLoggedOut(){
			CachedOrderCollection.clearModel();
			CachedAccountCollection.clearModel();
		}
	}
	
	WebSocketConnectionChecker {
		id: pumaConnectionChecker;
		gqlCommandId: "PumaWsConnection";
		subscriptionManager: window.subscriptionManager;
	}
	
	WebSocketConnectionChecker {
		id: lisaConnectionChecker;
		gqlCommandId: "LisaWsConnection";
		subscriptionManager: window.subscriptionManager;
		onStatusChanged: {
			if (status === 1){
				if (!CachedProductCollection.completed){
					CachedProductCollection.updateModel();
				}
				
				if (!CachedLicenseCollection.completed){
					CachedLicenseCollection.updateModel();
				}
				
				if (PopupManager.messageIsOpened(gqlCommandId)){
					PopupManager.closeMessage(gqlCommandId);
				}
			}
			else{
				if (!PopupManager.messageIsOpened(gqlCommandId)){
					PopupManager.addWarningMessage(qsTr("Lost connection to Lisa server"), false, gqlCommandId);
				}
			}
		}
	}
}

