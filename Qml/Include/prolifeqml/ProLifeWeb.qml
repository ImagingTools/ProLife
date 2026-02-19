// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0

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
