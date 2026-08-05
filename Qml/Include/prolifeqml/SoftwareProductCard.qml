import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import prolifeOrdersSdl 1.0

Rectangle {
	id: softwareCard

	height: softwareCard.expanded ? softwareCard.contentHeight : 0
	opacity: softwareCard.expanded ? 1 : 0
	visible: softwareCard.height > 0
	clip: true
	color: Style.baseColor

	property bool expanded: true
	property int contentHeight: contentColumn.height + Style.marginL
	property OrderedProduct productItem: model.item
	property bool readOnly: false

	readonly property int labelWidth: Style.sizeHintXXXS
	readonly property string emptyValueText: "—"
	readonly property string softwareIdText: productItem ? productItem.m_serialNumber : ""
	property string articleText: productItem && productItem.m_licenseId !== ""
		? productItem.m_licenseName + " (" + productItem.m_licenseId + ")"
		: (productItem ? productItem.m_licenseName : "")
	property string expirationText: productItem && productItem.m_expiration !== ""
		? productItem.m_expiration
		: qsTr("Unlimited")

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
				width: softwareCard.labelWidth
				text: qsTr("Software-ID")
				color: Style.subtitleColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
			}

			Text {
				width: parent.width - softwareCard.labelWidth - parent.spacing
				text: softwareCard.softwareIdText !== "" ? softwareCard.softwareIdText : softwareCard.emptyValueText
				color: softwareCard.softwareIdText !== "" ? Style.textColor : Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				elide: Text.ElideRight
			}
		}

		Row {
			width: parent.width
			spacing: Style.marginM

			Text {
				width: softwareCard.labelWidth
				text: qsTr("Article")
				color: Style.subtitleColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
			}

			Text {
				width: parent.width - softwareCard.labelWidth - parent.spacing
				text: softwareCard.articleText !== "" ? softwareCard.articleText : softwareCard.emptyValueText
				color: softwareCard.articleText !== "" ? Style.textColor : Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				elide: Text.ElideRight
			}
		}

		Row {
			width: parent.width
			spacing: Style.marginM

			Text {
				width: softwareCard.labelWidth
				text: qsTr("Expiration")
				color: Style.subtitleColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
			}

			Text {
				width: parent.width - softwareCard.labelWidth - parent.spacing
				text: softwareCard.expirationText
				color: Style.textColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				elide: Text.ElideRight
			}
		}
	}
}
