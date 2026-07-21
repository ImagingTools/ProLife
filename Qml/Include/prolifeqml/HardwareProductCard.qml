import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import prolifeOrdersSdl 1.0

Rectangle {
	id: hardwareCard

	height: visible ? hardwareCard.contentHeight : 0
	color: Style.baseColor

	property OrderedProduct productItem: model.item ? model.item : null
	property bool readOnly: false
	property int contentHeight: contentColumn.height
	readonly property int labelWidth: Style.sizeHintXXXS

	Column {
		id: contentColumn
		anchors.top: parent.top
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
				text: hardwareCard.productItem ? hardwareCard.productItem.m_macAddress : ""
				color: Style.textColor
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
				text: hardwareCard.productItem ? hardwareCard.productItem.m_serialNumber : ""
				color: Style.textColor
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
				text: hardwareCard.productItem ? hardwareCard.productItem.m_licenseName : ""
				color: Style.textColor
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
				text: hardwareCard.productItem ? hardwareCard.productItem.m_licenseId : ""
				color: Style.textColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				elide: Text.ElideRight
			}
		}
	}
}
