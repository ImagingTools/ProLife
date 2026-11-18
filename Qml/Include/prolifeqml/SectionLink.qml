import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/**
 * SectionLink - action link of a section header, e.g. "+ Add Component".
 */
Text {
	id: root

	// An inactive link is greyed out and does not react.
	property bool active: true

	signal clicked()

	font.pixelSize: Style.fontSizeM
	font.bold: true
	color: root.active ? Style.linkColor : Style.inactiveTextColor

	MouseArea {
		objectName: "MouseArea"
		anchors.fill: parent
		enabled: root.active
		hoverEnabled: true
		cursorShape: Qt.PointingHandCursor
		onClicked: {
			root.clicked()
		}
	}
}
