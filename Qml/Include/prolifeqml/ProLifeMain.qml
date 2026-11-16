import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtnotifygui 1.0
import prolifeqml 1.0

ApplicationMain {
	id: window;
	
	useWebSocketSubscription: true;
	// canRecoveryPassword: false;
	authConnectionState: pumaConnectionChecker.status

	property GqlBasedNotificationApiClient notificationApiClient: GqlBasedNotificationApiClient {}

	Component.onCompleted: {
		NotificationTypeRegistry.registerRoute("SupportTicket", "Tickets/Ticket/{id}")
		NotificationTypeRegistry.registerRoute("Tenant", "Tenants/Tenant/{id}")
		NotificationCenter.pageRoute = "Notifications"
		NotificationCenter.apiClient = window.notificationApiClient
	}

	WebSocketConnectionChecker {
		id: pumaConnectionChecker;
		gqlCommandId: "PumaWsConnection";
		subscriptionManager: window.subscriptionManager;
	}

	Connections {
		target: AuthorizationController;

		function onLoggedIn(){
			CachedFeatureCollection.updateModel();
			CachedProductCollection.updateModel();
			CachedLicenseCollection.updateModel();
		}

		function onLoggedOut(){
			CachedFeatureCollection.clearModel();
			CachedProductCollection.clearModel();
			CachedLicenseCollection.clearModel();
		}
	}
}

