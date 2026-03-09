// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import prolifeqml 1.0

Dialog {
	id: licenseTreeDialog

	title: qsTr("License Hierarchy - Full View")
	width: parent ? parent.width * 0.95 : 1200
	height: parent ? parent.height * 0.95 : 800

	property var treeData: null
	property string currentLicenseId: ""

	Component.onCompleted: {
		licenseTreeDialog.fillButtons()
	}

	onLocalizationChanged: {
		licenseTreeDialog.fillButtons()
	}

	function fillButtons(){
		clearButtons()
		addButton(Enums.close, qsTr("Close"), true)
	}

	contentComp: Component {
		Item {
			width: licenseTreeDialog.width
			height: licenseTreeDialog.height - 100

			CustomScrollbar {
				id: scrollbar
				z: parent.z + 1
				anchors.right: parent.right
				anchors.top: canvasFlickable.top
				anchors.bottom: canvasFlickable.bottom
				secondSize: 10
				targetItem: canvasFlickable
			}
			
			CustomScrollbar{
				id: scrollHoriz
				z: parent.z + 1
				anchors.left: canvasFlickable.left
				anchors.right: canvasFlickable.right
				anchors.bottom: canvasFlickable.bottom
				secondSize: 10
				vertical: false
				targetItem: canvasFlickable
			}

			Flickable {
				id: canvasFlickable
				anchors.fill: parent
				anchors.margins: Style.marginL
				contentWidth: licenseTreeCanvas.treeWidth
				contentHeight: licenseTreeCanvas.implicitHeight
				clip: true

				LicenseTreeCanvas {
					id: licenseTreeCanvas
					width: canvasFlickable.width
					treeData: licenseTreeDialog.treeData
					currentLicenseId: licenseTreeDialog.currentLicenseId
				}
			}
		}
	}
}
