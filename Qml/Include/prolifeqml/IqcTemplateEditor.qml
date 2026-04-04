import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import prolifeqml 1.0
import prolifeIQCSdl 1.0

ViewBase {
	id: iqcTemplateEditorContainer

	property var templateData: model ? model : null
	property bool isNew: false

	onTemplateDataChanged: {
		checkPermissions()
		if (templateData) {
			updateGui()
		}
	}

	function setReadOnly(readOnly) {
		templateNameInput.readOnly = readOnly
		supplierIdInput.readOnly = readOnly
		componentIdInput.readOnly = readOnly
		versionInput.readOnly = readOnly
		activeCheck.enabled = !readOnly
		completionRuleCB.changeable = !readOnly
		passRuleInput.readOnly = readOnly
	}

	function checkPermissions() {
		let canManage = PermissionsController.checkPermission("ManageIqcTemplates")
		setReadOnly(!canManage)

		addItemButton.visible = canManage
		if (commandsController) {
			commandsController.setCommandVisible("Undo", canManage)
			commandsController.setCommandVisible("Redo", canManage)
			commandsController.setCommandVisible("Save", canManage)
		}
	}

	function updateGui() {
		if (!templateData) {
			return
		}
		templateNameInput.text = templateData.m_name || ""
		supplierIdInput.text = templateData.m_supplierId || ""
		componentIdInput.text = templateData.m_componentId || ""
		versionInput.text = templateData.m_version ? String(templateData.m_version) : "1"
		activeCheck.checked = templateData.m_active !== false
		passRuleInput.text = templateData.m_passRuleDescription || ""

		setComboValue(completionRuleCB, templateData.m_completionRule)

		checklistItemsView.model = templateData.m_checklistItems
	}

	function setComboValue(combo, value) {
		combo.currentIndex = -1
		if (combo.model) {
			for (let i = 0; i < combo.model.count; i++) {
				if (combo.model.get(i).id === value) {
					combo.currentIndex = i
					return
				}
			}
		}
	}

	function updateModel() {
		if (!templateData) {
			return
		}
		templateData.m_name = templateNameInput.text
		templateData.m_supplierId = supplierIdInput.text
		templateData.m_componentId = componentIdInput.text
		templateData.m_version = parseInt(versionInput.text) || 1
		templateData.m_active = activeCheck.checked
		templateData.m_passRuleDescription = passRuleInput.text

		if (completionRuleCB.currentIndex >= 0 && completionRuleCB.model) {
			templateData.m_completionRule = completionRuleCB.model.get(completionRuleCB.currentIndex).id
		}
	}

	Rectangle {
		anchors.fill: parent
		color: Style.backgroundColor2
	}

	SplitView {
		anchors.fill: parent
		orientation: Qt.Horizontal

		// Left: template list
		Rectangle {
			width: parent.width * 0.35
			color: "transparent"
			border.color: "#e0e0e0"
			border.width: 1

			Column {
				anchors.fill: parent
				anchors.margins: 8
				spacing: 4

				Text {
					text: qsTr("IQC Templates")
					font.bold: true
					font.pixelSize: 14
				}

				Rectangle {
					width: parent.width
					height: 1
					color: "#e0e0e0"
				}

				GqlCollectionView {
					id: templateListView
					width: parent.width
					height: parent.height - 60
					typeId: "IqcTemplate"
					onCurrentItemChanged: {
						if (currentItem) {
							isNew = false
							dataProvider.loadItem(currentItem.id)
						}
					}
				}

				Row {
					spacing: 8
					visible: PermissionsController.checkPermission("ManageIqcTemplates")

					Button {
						text: qsTr("New Template")
						onClicked: {
							isNew = true
							dataProvider.createNew()
							updateGui()
							checkPermissions()
						}
					}
				}
			}
		}

		// Right: template editor
		Rectangle {
			color: "transparent"

			CustomScrollbar {
				id: scrollbar
				z: parent.z + 1
				anchors.right: parent.right
				anchors.top: flickable.top
				anchors.bottom: flickable.bottom
				secondSize: 10
				targetItem: flickable
				radius: 2
			}

			Flickable {
				id: flickable
				anchors.left: parent.left
				anchors.leftMargin: Style.marginXL
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				anchors.right: scrollbar.left
				anchors.rightMargin: Style.marginXL

				contentWidth: content.width
				contentHeight: content.height + 2 * Style.marginXL
				boundsBehavior: Flickable.StopAtBounds
				clip: true

				Column {
					id: content
					width: 600
					spacing: Style.marginXL

					// --- Template Definition ---
					GroupHeaderView {
						title: qsTr("Template Definition")
						width: content.width
						groupView: templateDefinitionGroup
					}

					GroupElementView {
						id: templateDefinitionGroup
						width: content.width

						TextInputElementView {
							id: templateNameInput
							width: parent.width
							labelText: qsTr("Template Name")
							readOnly: true
						}

						TextInputElementView {
							id: supplierIdInput
							width: parent.width
							labelText: qsTr("Supplier ID")
							readOnly: true
							placeholderText: qsTr("Leave empty for any supplier")
						}

						TextInputElementView {
							id: componentIdInput
							width: parent.width
							labelText: qsTr("Component ID / Part Number")
							readOnly: true
							placeholderText: qsTr("Leave empty for any component")
						}

						TextInputElementView {
							id: versionInput
							width: parent.width
							labelText: qsTr("Version")
							readOnly: true
						}

						Row {
							width: parent.width
							spacing: 8

							Text {
								anchors.verticalCenter: parent.verticalCenter
								text: qsTr("Active")
							}

							CheckBox {
								id: activeCheck
								enabled: false
							}
						}
					}

					// --- Policy ---
					GroupHeaderView {
						title: qsTr("Completion Policy")
						width: content.width
						groupView: policyGroup
					}

					GroupElementView {
						id: policyGroup
						width: content.width

						ComboBoxElementView {
							id: completionRuleCB
							width: parent.width
							labelText: qsTr("Completion Rule")
							changeable: false
							model: ListModel {
								ListElement { id: "CR_ALL_REQUIRED_DONE";  name: qsTr("All Required Items Done") }
								ListElement { id: "CR_ALL_ITEMS_DONE";     name: qsTr("All Items Done") }
								ListElement { id: "CR_AT_LEAST_ONE_DONE"; name: qsTr("At Least One Done") }
							}
							displayRole: "name"
							valueRole: "id"
						}

						TextInputElementView {
							id: passRuleInput
							width: parent.width
							labelText: qsTr("Pass Rule Description")
							readOnly: true
							multiline: true
							placeholderText: qsTr("e.g. No critical fails AND fail rate <= 5%")
						}
					}

					// --- Checklist Items ---
					GroupHeaderView {
						title: qsTr("Checklist Items")
						width: content.width
						groupView: checklistGroup
					}

					GroupElementView {
						id: checklistGroup
						width: content.width

						ListView {
							id: checklistItemsView
							width: parent.width
							height: Math.min(contentHeight, 300)
							clip: true
							model: null

							header: Rectangle {
								width: checklistItemsView.width
								height: 28
								color: Style.headerColor

								Row {
									anchors.fill: parent
									anchors.margins: 4
									spacing: 4
									Text { text: qsTr("Title");    width: 180; font.bold: true }
									Text { text: qsTr("Severity"); width: 80;  font.bold: true }
									Text { text: qsTr("Required"); width: 70;  font.bold: true }
									Text { text: qsTr("Type");     width: 80;  font.bold: true }
									Text { text: qsTr("Min");      width: 50;  font.bold: true }
									Text { text: qsTr("Max");      width: 50;  font.bold: true }
									Text { text: qsTr("Unit");     width: 60;  font.bold: true }
								}
							}

							delegate: Rectangle {
								width: checklistItemsView.width
								height: 36
								color: index % 2 === 0 ? Style.backgroundColor1 : Style.backgroundColor2
								border.color: "#e0e0e0"
								border.width: 1

								Row {
									anchors.fill: parent
									anchors.margins: 4
									spacing: 4
									Text { text: model.title || "";    width: 180; elide: Text.ElideRight }
									Text { text: model.severity || ""; width: 80 }
									Text { text: model.required ? qsTr("Yes") : qsTr("No"); width: 70 }
									Text { text: model.expectedValueType || ""; width: 80 }
									Text { text: model.minValue || ""; width: 50 }
									Text { text: model.maxValue || ""; width: 50 }
									Text { text: model.unit || "";     width: 60 }
								}
							}
						}

						Button {
							id: addItemButton
							text: qsTr("Add Checklist Item")
							visible: false
							onClicked: {
								checklistItemDialog.open()
							}
						}
					}
				}
			}
		}
	}

	Dialog {
		id: checklistItemDialog
		title: qsTr("Add Checklist Item")
		width: 500
		height: 420
		modal: true
		standardButtons: Dialog.Ok | Dialog.Cancel

		Column {
			anchors.fill: parent
			anchors.margins: 16
			spacing: 8

			TextInputElementView {
				id: itemTitleInput
				width: parent.width
				labelText: qsTr("Title")
			}

			TextInputElementView {
				id: itemDescInput
				width: parent.width
				labelText: qsTr("Description")
			}

			ComboBoxElementView {
				id: itemSeverityCB
				width: parent.width
				labelText: qsTr("Severity")
				model: ListModel {
					ListElement { id: "IS_INFO";     name: qsTr("Info") }
					ListElement { id: "IS_MAJOR";    name: qsTr("Major") }
					ListElement { id: "IS_CRITICAL"; name: qsTr("Critical") }
				}
				displayRole: "name"
				valueRole: "id"
			}

			Row {
				spacing: 8
				Text {
					anchors.verticalCenter: parent.verticalCenter
					text: qsTr("Required")
				}
				CheckBox {
					id: itemRequiredCheck
					checked: true
				}
			}

			ComboBoxElementView {
				id: itemValueTypeCB
				width: parent.width
				labelText: qsTr("Expected Value Type")
				model: ListModel {
					ListElement { id: "EVT_BOOLEAN"; name: qsTr("Boolean") }
					ListElement { id: "EVT_NUMERIC"; name: qsTr("Numeric") }
					ListElement { id: "EVT_TEXT";    name: qsTr("Text") }
					ListElement { id: "EVT_ENUM";    name: qsTr("Enum") }
				}
				displayRole: "name"
				valueRole: "id"
			}
		}
	}
}
