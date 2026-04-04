import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import prolifeqml 1.0
import prolifeProcurementSdl 1.0

ViewBase {
	id: procurementEditorContainer

	property var procurementData: model ? model : null
	property bool isNew: false
	property string procurementId: ""

	onProcurementDataChanged: {
		checkPermissions()
		if (procurementData) {
			updateGui()
		}
	}

	function loadProcurementData() {
		if (procurementId) {
			// Load procurement data via data provider
			dataProvider.loadItem(procurementId)
		}
	}

	function createNew() {
		isNew = true
		dataProvider.createNew()
		updateGui()
		checkPermissions()
	}

	function setReadOnly(readOnly) {
		orderNumberInput.readOnly = readOnly
		supplierIdInput.readOnly = readOnly
		supplierNameInput.readOnly = readOnly
		descriptionInput.readOnly = readOnly
		expectedDeliveryDateInput.readOnly = readOnly
		statusCB.changeable = !readOnly
	}

	function checkPermissions() {
		let canAdd = PermissionsController.checkPermission("AddProcurement")
		let canChange = PermissionsController.checkPermission("ChangeProcurement")

		if (isNew && canAdd) {
			setReadOnly(false)
		} else {
			setReadOnly(!canChange)
		}

		let ok = canAdd || canChange
		if (commandsController) {
			commandsController.setCommandVisible("Undo", ok)
			commandsController.setCommandVisible("Redo", ok)
			commandsController.setCommandVisible("Save", ok)
		}
	}

	function updateGui() {
		if (!procurementData) {
			return
		}
		orderNumberInput.text = procurementData.m_procurementOrderNumber || ""
		supplierIdInput.text = procurementData.m_supplierId || ""
		supplierNameInput.text = procurementData.m_supplierName || ""
		descriptionInput.text = procurementData.m_description || ""
		expectedDeliveryDateInput.text = procurementData.m_expectedDeliveryDate || ""
		linkedOrderInput.text = procurementData.m_linkedOrderUuid || ""

		statusCB.currentIndex = -1
		let statusVal = procurementData.m_procurementStatus
		if (statusCB.model) {
			for (let i = 0; i < statusCB.model.count; i++) {
				if (statusCB.model.get(i).id === statusVal) {
					statusCB.currentIndex = i
					break
				}
			}
		}
	}

	function updateModel() {
		if (!procurementData) {
			return
		}
		procurementData.m_procurementOrderNumber = orderNumberInput.text
		procurementData.m_supplierId = supplierIdInput.text
		procurementData.m_supplierName = supplierNameInput.text
		procurementData.m_description = descriptionInput.text
		procurementData.m_expectedDeliveryDate = expectedDeliveryDateInput.text
		procurementData.m_linkedOrderUuid = linkedOrderInput.text

		if (statusCB.currentIndex >= 0 && statusCB.model) {
			procurementData.m_procurementStatus = statusCB.model.get(statusCB.currentIndex).id
		}
	}

	Rectangle {
		anchors.fill: parent
		color: Style.backgroundColor2
	}

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
			width: 700
			spacing: Style.marginXL

			// --- Procurement Order Information ---
			GroupHeaderView {
				title: qsTr("Procurement Order")
				width: content.width
				groupView: orderGroup
			}

			GroupElementView {
				id: orderGroup
				width: content.width

				TextInputElementView {
					id: orderNumberInput
					width: parent.width
					labelText: qsTr("Order Number")
					readOnly: true
				}

				TextInputElementView {
					id: supplierIdInput
					width: parent.width
					labelText: qsTr("Supplier ID")
					readOnly: true
				}

				TextInputElementView {
					id: supplierNameInput
					width: parent.width
					labelText: qsTr("Supplier Name")
					readOnly: true
				}

				ComboBoxElementView {
					id: statusCB
					width: parent.width
					labelText: qsTr("Status")
					changeable: false
					model: ListModel {
						ListElement { id: "PS_CREATED";            name: qsTr("Created") }
						ListElement { id: "PS_ORDERED";            name: qsTr("Ordered") }
						ListElement { id: "PS_PARTIALLY_RECEIVED"; name: qsTr("Partially Received") }
						ListElement { id: "PS_RECEIVED";           name: qsTr("Received") }
						ListElement { id: "PS_IN_QC";              name: qsTr("In QC") }
						ListElement { id: "PS_ON_HOLD";            name: qsTr("On Hold") }
						ListElement { id: "PS_CLOSED";             name: qsTr("Closed") }
						ListElement { id: "PS_CANCELED";           name: qsTr("Canceled") }
					}
					displayRole: "name"
					valueRole: "id"
				}

				TextInputElementView {
					id: expectedDeliveryDateInput
					width: parent.width
					labelText: qsTr("Expected Delivery Date")
					readOnly: true
				}

				TextInputElementView {
					id: descriptionInput
					width: parent.width
					labelText: qsTr("Description / Notes")
					readOnly: true
					multiline: true
				}
			}

			// --- Linked Order (optional) ---
			GroupHeaderView {
				title: qsTr("Linked Production Order (optional)")
				width: content.width
				groupView: linkedOrderGroup
			}

			GroupElementView {
				id: linkedOrderGroup
				width: content.width

				TextInputElementView {
					id: linkedOrderInput
					width: parent.width
					labelText: qsTr("Linked Order UUID")
					readOnly: true
					placeholderText: qsTr("Leave empty for stock procurement")
				}
			}

			// --- Deliveries ---
			GroupHeaderView {
				title: qsTr("Deliveries")
				width: content.width
				groupView: deliveriesGroup
			}

			GroupElementView {
				id: deliveriesGroup
				width: content.width

				Item {
					width: parent.width
					height: deliveriesView.height + (addDeliveryButton.visible ? addDeliveryButton.height + 8 : 0)

					ListView {
						id: deliveriesView
						width: parent.width
						height: Math.min(contentHeight, 200)
						clip: true
						model: procurementData ? procurementData.m_deliveries : null

						delegate: Rectangle {
							width: deliveriesView.width
							height: 48
							color: index % 2 === 0 ? Style.backgroundColor1 : Style.backgroundColor2
							border.color: "#e0e0e0"
							border.width: 1

							Row {
								anchors.fill: parent
								anchors.margins: 8
								spacing: 8

								Text {
									text: model.deliveryNoteNumber || ""
									width: 150
									elide: Text.ElideRight
								}
								Text {
									text: model.deliveryDate || ""
									width: 100
								}
								Text {
									text: model.deliveryStatus || ""
									width: 100
								}
								Text {
									text: model.receivedBy || ""
									width: 120
									elide: Text.ElideRight
								}
							}
						}
					}

					Button {
						id: addDeliveryButton
						anchors.top: deliveriesView.bottom
						anchors.topMargin: 8
						text: qsTr("Add Delivery")
						visible: PermissionsController.checkPermission("AddProcurementDelivery")
						onClicked: {
							procurementDeliveryDialog.open()
						}
					}
				}
			}

			// --- IQC Runs ---
			GroupHeaderView {
				title: qsTr("IQC Runs")
				width: content.width
				groupView: iqcGroup
				visible: PermissionsController.checkPermission("ViewIncomingQualityControl")
			}

			GroupElementView {
				id: iqcGroup
				width: content.width
				visible: PermissionsController.checkPermission("ViewIncomingQualityControl")

				Item {
					width: parent.width
					height: iqcRunsView.height + (startIqcButton.visible ? startIqcButton.height + 8 : 0)

					ListView {
						id: iqcRunsView
						width: parent.width
						height: Math.min(contentHeight, 200)
						clip: true
						model: null

						delegate: Rectangle {
							width: iqcRunsView.width
							height: 40
							color: index % 2 === 0 ? Style.backgroundColor1 : Style.backgroundColor2
							border.color: "#e0e0e0"
							border.width: 1

							Row {
								anchors.fill: parent
								anchors.margins: 8
								spacing: 8

								Text {
									text: model.runMode || ""
									width: 80
								}
								Text {
									text: model.runStatus || ""
									width: 100
								}
								Text {
									text: model.runResult || ""
									width: 100
								}
								Text {
									text: model.systemId || ""
									width: 120
									elide: Text.ElideRight
								}
							}
						}
					}

					Button {
						id: startIqcButton
						anchors.top: iqcRunsView.bottom
						anchors.topMargin: 8
						text: qsTr("Start IQC Run")
						visible: PermissionsController.checkPermission("EditIncomingQualityControl")
						onClicked: {
							iqcRunDialog.open()
						}
					}
				}
			}
		}
	}

	// Placeholder dialogs - would be implemented as separate QML components
	Dialog {
		id: procurementDeliveryDialog
		title: qsTr("Add Delivery")
		width: 500
		height: 350
		modal: true
		standardButtons: Dialog.Ok | Dialog.Cancel

		Column {
			anchors.fill: parent
			anchors.margins: 16
			spacing: 8

			TextInputElementView {
				id: deliveryNoteInput
				width: parent.width
				labelText: qsTr("Delivery Note Number")
			}

			TextInputElementView {
				id: deliveryDateInput
				width: parent.width
				labelText: qsTr("Delivery Date (ISO8601)")
			}

			TextInputElementView {
				id: receivedByInput
				width: parent.width
				labelText: qsTr("Received By")
			}
		}
	}

	Dialog {
		id: iqcRunDialog
		title: qsTr("Start IQC Run")
		width: 400
		height: 250
		modal: true
		standardButtons: Dialog.Ok | Dialog.Cancel

		Column {
			anchors.fill: parent
			anchors.margins: 16
			spacing: 8

			ComboBoxElementView {
				id: runModeSelect
				width: parent.width
				labelText: qsTr("Run Mode")
				model: ListModel {
					ListElement { id: "RM_MANUAL";    name: qsTr("Manual") }
					ListElement { id: "RM_AUTOMATED"; name: qsTr("Automated") }
					ListElement { id: "RM_HYBRID";    name: qsTr("Hybrid") }
				}
				displayRole: "name"
				valueRole: "id"
			}
		}
	}
}
