import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import prolifeOrdersSdl 1.0

Rectangle {
	id: hardwareCard

	height: hardwareCard.expanded ? hardwareCard.contentHeight : 0
	opacity: hardwareCard.expanded ? 1 : 0
	visible: hardwareCard.height > 0
	clip: true
	color: Style.baseColor

	property bool expanded: true
	property OrderedProduct productItem: model.item ? model.item : null
	property bool readOnly: false
	property int contentHeight: contentColumn.height + Style.marginL

	readonly property int labelWidth: Style.sizeHintXXXS
	readonly property string emptyValueText: "—"
	readonly property string macAddressText: productItem ? productItem.m_macAddress : ""
	readonly property string serialNumberText: productItem ? productItem.m_serialNumber : ""
	readonly property string modelTypeText: productItem ? productItem.m_licenseName : ""
	readonly property string articleText: productItem ? productItem.m_licenseId : ""

	Behavior on height {
		NumberAnimation {
			duration: 160
			easing.type: Easing.OutCubic
		}
	}

	Behavior on opacity {
		NumberAnimation {
			duration: 160
		}
	}

	Column {
		id: contentColumn
		anchors.top: parent.top
		anchors.topMargin: Style.marginL
		anchors.left: parent.left
		anchors.right: parent.right
		spacing: Style.marginS

		Row {
			width: parent.width
			spacing: Style.marginM

			Text {
				width: hardwareCard.labelWidth
				text: qsTr("MAC Address")
				color: Style.subtitleColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
			}

			Text {
				width: parent.width - hardwareCard.labelWidth - parent.spacing
				text: hardwareCard.macAddressText !== "" ? hardwareCard.macAddressText : hardwareCard.emptyValueText
				color: hardwareCard.macAddressText !== "" ? Style.textColor : Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				elide: Text.ElideRight
			}
		}

		Row {
			width: parent.width
			spacing: Style.marginM

			Text {
				width: hardwareCard.labelWidth
				text: qsTr("Serial Number")
				color: Style.subtitleColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
			}

			Text {
				width: parent.width - hardwareCard.labelWidth - parent.spacing
				text: hardwareCard.serialNumberText !== "" ? hardwareCard.serialNumberText : hardwareCard.emptyValueText
				color: hardwareCard.serialNumberText !== "" ? Style.textColor : Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				elide: Text.ElideRight
			}
		}

		Row {
			width: parent.width
			spacing: Style.marginM

			Text {
				width: hardwareCard.labelWidth
				text: qsTr("Model Type")
				color: Style.subtitleColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
			}

			Text {
				width: parent.width - hardwareCard.labelWidth - parent.spacing
				text: hardwareCard.modelTypeText !== "" ? hardwareCard.modelTypeText : hardwareCard.emptyValueText
				color: hardwareCard.modelTypeText !== "" ? Style.textColor : Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				elide: Text.ElideRight
			}
		}

		Row {
			width: parent.width
			spacing: Style.marginM

			Text {
				width: hardwareCard.labelWidth
				text: qsTr("Article")
				color: Style.subtitleColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
			}

			Text {
				width: parent.width - hardwareCard.labelWidth - parent.spacing
				text: hardwareCard.articleText !== "" ? hardwareCard.articleText : hardwareCard.emptyValueText
				color: hardwareCard.articleText !== "" ? Style.textColor : Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				elide: Text.ElideRight
			}
		}
	}
}
