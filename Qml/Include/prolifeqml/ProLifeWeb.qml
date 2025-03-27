import QtQuick 2.0
import Acf 1.0

Item {
	id: window;
	
	anchors.fill: parent;
	
	ProLifeMain {
		id: application;
		
		anchors.fill: parent;
		
		serverReady: true;
		
		Component.onCompleted: {
			designProvider.setDesignSchema("Light");
			context.application = ["ImtCore", "ProLife"];
		}
		
		function getServerUrl(){
			return context.location;
		}
	}
}
