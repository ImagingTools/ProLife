import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import prolifeOrdersSdl 1.0

Rectangle {
	id: softwareCard

	height: visible ? softwareCard.contentHeight : 0
	color: Style.baseColor

	property int contentHeight: contentColumn.height
	property OrderedProduct productItem: model.item
	property bool readOnly: false

	readonly property int labelWidth: Style.sizeHintXXXS
	property string articleText: productItem && productItem.m_licenseId !== ""
		? productItem.m_licenseName + " (" + productItem.m_licenseId + ")"
		: (productItem ? productItem.m_licenseName : "")
	property string expirationText: productItem && productItem.m_expiration !== ""
		? productItem.m_expiration
		: qsTr("Unlimited")

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
				width: softwareCard.labelWidth
				text: qsTr("Software-ID")
				color: Style.subtitleColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
			}

			Text {
				width: parent.width - softwareCard.labelWidth - parent.spacing
				text: softwareCard.productItem ? softwareCard.productItem.m_serialNumber : ""
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
				width: softwareCard.labelWidth
				text: qsTr("Article")
				color: Style.subtitleColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
			}

			Text {
				width: parent.width - softwareCard.labelWidth - parent.spacing
				text: softwareCard.articleText
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
