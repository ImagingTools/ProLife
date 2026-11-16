import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/**
 * SectionHeaderView - title of a page section with its action links.
 *
 * The links stand to the right of the title while both fit on one line. On a narrow page they
 * go under the title and wrap, so they never run over the title or out of the page.
 */
Item {
	id: root

	property string title

	// The links, e.g. SectionLink items.
	default property alias links: linksFlow.data

	property int linkSpacing: Style.marginL

	// Width of the visible links in one line; updated with the width of the header.
	property real linksWidth: 0

	property var __trackedLinks: []

	readonly property bool compact: titleText.implicitWidth + Style.marginXL + root.linksWidth > root.width

	height: root.compact
		? titleText.height + (root.linksWidth > 0 ? Style.marginS + linksFlow.height : 0)
		: Math.max(titleText.height, linksFlow.height)

	function updateLinksWidth(){
		let total = 0
		let count = 0
		for (let i = 0; i < linksFlow.children.length; i++){
			let child = linksFlow.children[i]
			if (child.visible){
				total += child.implicitWidth
				count++
			}
		}

		root.linksWidth = count > 0 ? total + (count - 1) * root.linkSpacing : 0
	}

	onWidthChanged: {
		root.updateLinksWidth()
	}

	Component.onCompleted: {
		root.updateLinksWidth()
	}

	Text {
		id: titleText
		anchors.left: parent.left
		width: root.compact ? root.width : root.width - root.linksWidth - Style.marginXL
		color: Style.textColor
		font.family: Style.fontFamilyBold
		font.pixelSize: Style.fontSizeXL
		font.bold: true
		elide: Text.ElideRight
		text: root.title
	}

	Flow {
		id: linksFlow
		x: root.compact ? 0 : root.width - root.linksWidth
		y: root.compact ? titleText.height + Style.marginS : (root.height - height) / 2
		width: root.compact ? root.width : root.linksWidth + 1
		spacing: root.linkSpacing

		// The width of a link is known only once its text is laid out, and a link can be shown
		// or hidden later, so each link reports its changes.
		onChildrenChanged: {
			for (let i = 0; i < linksFlow.children.length; i++){
				let child = linksFlow.children[i]
				if (root.__trackedLinks.indexOf(child) < 0){
					root.__trackedLinks.push(child)
					child.implicitWidthChanged.connect(root.updateLinksWidth)
					child.visibleChanged.connect(root.updateLinksWidth)
				}
			}

			root.updateLinksWidth()
		}
	}
}
