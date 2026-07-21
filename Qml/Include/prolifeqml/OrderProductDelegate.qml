import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import prolifeOrdersSdl 1.0

/**
 * OrderProductDelegate — one product line in the Order editor list.
 *
 * Title row layout (left → right):
 *   [Icon] [Product name elided] … [tags] [Edit] [Remove]
 *
 * Tags sit next to the action buttons; the name shrinks and elides so it
 * never overlaps tags or commands.
 */
ElementView {
	id: root

	property OrderedProduct productItem: model.item ? model.item : null
	// Title is rendered in topComp (icon + name + tags + commands).
	name: ""

	clip: false

	property TreeItemModel activeCommandsModel: TreeItemModel {}
	property bool expanded: true
	property bool readOnly: false
	property bool inUse: productItem ? productItem.m_inUse : false
	property bool isNew: productItem ? productItem.m_isNew : false
	property string categoryId: productItem ? productItem.m_categoryId : ""
	property bool productInUse: softwareCommandsModel.completed && root.inUse
	property bool isMultiple: productItem ? productItem.m_isMultiple : false
	property int productCount: productItem ? productItem.m_productCount : 0

	readonly property bool isSoftware: root.categoryId === "Software"
	readonly property string categoryLabel: root.isSoftware ? qsTr("Software") : qsTr("Hardware")
	readonly property string categoryIcon: root.isSoftware ? "Icons/Key" : "Icons/Sensor"
	readonly property string productName: productItem ? productItem.m_productName : ""
	readonly property string categoryBadgeColor: root.isSoftware
		? Style.secondColor
		: Style.iconColorOnSelected
	readonly property string newBadgeColor: Style.selectedLinkToColor
	readonly property string multiBadgeColor: Style.subtitleColor

	signal removed()
	signal edited()

	topComp: Component {
		Item {
			id: headerRoot
			width: parent ? parent.width : 0
			height: Math.max(Style.iconSizeM, Style.controlHeightM, nameText.height, tagsRow.height)

			// Left: icon + eliding name (takes remaining space before tags).
			Row {
				id: nameRow
				anchors.left: parent.left
				anchors.right: tagsRow.left
				anchors.rightMargin: Style.marginM
				anchors.verticalCenter: parent.verticalCenter
				spacing: Style.marginM
				clip: true

				Image {
					anchors.verticalCenter: parent.verticalCenter
					width: Style.iconSizeM
					height: Style.iconSizeM
					source: "../../../../" + Style.getIconPath(root.categoryIcon, Icon.State.On, Icon.Mode.Normal)
					sourceSize.width: width
					sourceSize.height: height
				}

				Text {
					id: nameText
					anchors.verticalCenter: parent.verticalCenter
					// Bound width so long names elide before the tags.
					width: Math.max(0, nameRow.width - Style.iconSizeM - nameRow.spacing)
					text: root.productName
					color: Style.textColor
					font.family: Style.fontFamilyBold
					font.pixelSize: Style.fontSizeXL
					font.bold: true
					elide: Text.ElideRight
					wrapMode: Text.NoWrap
				}
			}

			// Tags immediately before Edit/Remove.
			Row {
				id: tagsRow
				anchors.right: commands.left
				anchors.rightMargin: Style.marginM
				anchors.verticalCenter: parent.verticalCenter
				spacing: Style.marginS

				Rectangle {
					anchors.verticalCenter: parent.verticalCenter
					height: Style.controlHeightS
					width: categoryTagText.implicitWidth + Style.marginS * 2
					radius: Style.radiusS
					color: root.categoryBadgeColor

					Text {
						id: categoryTagText
						anchors.centerIn: parent
						text: root.categoryLabel
						color: Style.baseColor
						font.family: Style.fontFamily
						font.pixelSize: Style.fontSizeS
					}
				}

				Rectangle {
					anchors.verticalCenter: parent.verticalCenter
					height: Style.controlHeightS
					width: newTagText.implicitWidth + Style.marginS * 2
					radius: Style.radiusS
					color: root.newBadgeColor
					visible: root.isNew

					Text {
						id: newTagText
						anchors.centerIn: parent
						text: qsTr("New")
						color: Style.baseColor
						font.family: Style.fontFamily
						font.pixelSize: Style.fontSizeS
					}
				}

				Rectangle {
					anchors.verticalCenter: parent.verticalCenter
					height: Style.controlHeightS
					width: multiTagText.implicitWidth + Style.marginS * 2
					radius: Style.radiusS
					color: root.multiBadgeColor
					visible: root.isMultiple && root.productCount > 0

					Text {
						id: multiTagText
						anchors.centerIn: parent
						text: qsTr("Multi") + " × " + root.productCount
						color: Style.baseColor
						font.family: Style.fontFamily
						font.pixelSize: Style.fontSizeS
					}
				}
			}

			SimpleCommandsDecorator {
				id: commands
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				height: Style.controlHeightM
				commandModel: root.activeCommandsModel

				onCommandActivated: {
					if (commandId == "Remove") {
						root.removed()
					}
					else if (commandId == "Edit") {
						root.edited()
					}
				}
			}
		}
	}

	bottomComp: root.isSoftware ? softwareProductCard : hardwareProductCard

	onProductInUseChanged: {
		if (productInUse) {
			root.setCommandValue(softwareCommandsModel, "Edit", "IsEnabled", !root.inUse)
		}
	}

	onReadOnlyChanged: {
		root.setIsEnabledCommand(softwareCommandsModel, "Edit", !root.readOnly)
		root.setIsEnabledCommand(softwareCommandsModel, "Remove", !root.readOnly)
	}

	function setIsEnabledCommand(commandsModel, commandId, isEnabled) {
		for (let i = 0; i < commandsModel.getItemsCount(); i++) {
			let id = commandsModel.getData("id", i)
			if (id === commandId) {
				commandsModel.setData("isEnabled", isEnabled, i)
				break
			}
		}
	}

	function setCommandValue(commandsModel, commandId, commandKey, commandValue) {
		for (let i = 0; i < commandsModel.getItemsCount(); i++) {
			let id = commandsModel.getData("id", i)
			if (id === commandId) {
				commandsModel.setData(commandKey, commandValue, i)
				break
			}
		}
	}

	Component {
		id: softwareProductCard
		SoftwareProductCard {
			visible: root.expanded
		}
	}

	Component {
		id: hardwareProductCard
		HardwareProductCard {
			visible: root.expanded
		}
	}

	TreeItemModel {
		id: softwareCommandsModel
		property bool completed: false

		Component.onCompleted: {
			let index = softwareCommandsModel.insertNewItem()
			softwareCommandsModel.setData("id", "Edit", index)
			softwareCommandsModel.setData("name", "Edit", index)
			softwareCommandsModel.setData("icon", "Icons/Edit", index)
			softwareCommandsModel.setData("isEnabled", !root.readOnly, index)
			softwareCommandsModel.setData("visible", true, index)

			index = softwareCommandsModel.insertNewItem()
			softwareCommandsModel.setData("id", "Remove", index)
			softwareCommandsModel.setData("name", "Remove", index)
			softwareCommandsModel.setData("icon", "Icons/Delete", index)
			softwareCommandsModel.setData("isEnabled", !root.readOnly, index)
			softwareCommandsModel.setData("visible", true, index)

			root.activeCommandsModel = softwareCommandsModel
			softwareCommandsModel.completed = true
		}
	}
}
