import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import prolifeqml 1.0
import prolifeIQCSdl 1.0

ViewBase {
	id: iqcRunEditorContainer

	property var iqcRunData: model ? model : null
	property bool isNew: false
	property string batchUuid: ""

	onIqcRunDataChanged: {
		checkPermissions()
		if (iqcRunData) {
			updateGui()
		}
	}

	function setReadOnly(readOnly) {
		annotationsInput.readOnly = readOnly
		defectCodesInput.readOnly = readOnly
		runModeCB.changeable = !readOnly
		runResultCB.changeable = !readOnly
		dispositionCB.changeable = !readOnly
	}

	function checkPermissions() {
		let canEdit = PermissionsController.checkPermission("EditIncomingQualityControl")
		setReadOnly(!canEdit)

		if (commandsController) {
			commandsController.setCommandVisible("Undo", canEdit)
			commandsController.setCommandVisible("Redo", canEdit)
			commandsController.setCommandVisible("Save", canEdit)
		}
	}

	function updateGui() {
		if (!iqcRunData) {
			return
		}

		batchUuidLabel.text = iqcRunData.m_batchUuid || ""
		systemIdLabel.text = iqcRunData.m_systemId || ""
		externalRunIdLabel.text = iqcRunData.m_externalRunId || ""
		startedAtLabel.text = iqcRunData.m_startedAt || ""
		completedAtLabel.text = iqcRunData.m_completedAt || ""
		annotationsInput.text = iqcRunData.m_annotations || ""
		defectCodesInput.text = iqcRunData.m_defectCodes || ""

		setComboValue(runModeCB, iqcRunData.m_runMode)
		setComboValue(runResultCB, iqcRunData.m_runResult)
		setComboValue(dispositionCB, iqcRunData.m_disposition)

		resultItemsView.model = iqcRunData.m_resultItems
	}

	function setComboValue(combo, value) {
		combo.currentIndex = -1
		if (combo.model) {
			for (let i = 0; i < combo.model.getItemsCount(); i++) {
				if (combo.model.getData("id", i) === value) {
					combo.currentIndex = i
					return
				}
			}
		}
	}

	function updateModel() {
		if (!iqcRunData) {
			return
		}
		iqcRunData.m_annotations = annotationsInput.text
		iqcRunData.m_defectCodes = defectCodesInput.text

		if (runModeCB.currentIndex >= 0 && runModeCB.model) {
			iqcRunData.m_runMode = runModeCB.model.getData("id", runModeCB.currentIndex)
		}
		if (runResultCB.currentIndex >= 0 && runResultCB.model) {
			iqcRunData.m_runResult = runResultCB.model.getData("id", runResultCB.currentIndex)
		}
		if (dispositionCB.currentIndex >= 0 && dispositionCB.model) {
			iqcRunData.m_disposition = dispositionCB.model.getData("id", dispositionCB.currentIndex)
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

			// --- Run information ---
			GroupHeaderView {
				title: qsTr("IQC Run")
				width: content.width
				groupView: runInfoGroup
			}

			GroupElementView {
				id: runInfoGroup
				width: content.width

				LabelElementView {
					id: batchUuidLabel
					width: parent.width
					labelText: qsTr("Batch UUID")
				}

				LabelElementView {
					id: systemIdLabel
					width: parent.width
					labelText: qsTr("System ID")
				}

				LabelElementView {
					id: externalRunIdLabel
					width: parent.width
					labelText: qsTr("External Run ID")
				}

				ComboBoxElementView {
					id: runModeCB
					width: parent.width
					labelText: qsTr("Run Mode")
					changeable: false
					model: ListModel {
						ListElement { id: "RM_MANUAL";    name: qsTr("Manual") }
						ListElement { id: "RM_AUTOMATED"; name: qsTr("Automated") }
						ListElement { id: "RM_HYBRID";    name: qsTr("Hybrid") }
					}
					displayRole: "name"
					valueRole: "id"
				}

				ComboBoxElementView {
					id: runResultCB
					width: parent.width
					labelText: qsTr("Result")
					changeable: false
					model: ListModel {
						ListElement { id: "RR_NONE";         name: qsTr("None") }
						ListElement { id: "RR_PASS";         name: qsTr("Pass") }
						ListElement { id: "RR_FAIL";         name: qsTr("Fail") }
						ListElement { id: "RR_HOLD";         name: qsTr("Hold") }
						ListElement { id: "RR_PARTIAL_PASS"; name: qsTr("Partial Pass") }
					}
					displayRole: "name"
					valueRole: "id"
				}

				ComboBoxElementView {
					id: dispositionCB
					width: parent.width
					labelText: qsTr("Disposition")
					changeable: false
					model: ListModel {
						ListElement { id: "DISP_NONE";               name: qsTr("None") }
						ListElement { id: "DISP_RELEASED";           name: qsTr("Released") }
						ListElement { id: "DISP_QUARANTINED";        name: qsTr("Quarantined") }
						ListElement { id: "DISP_RETURN_TO_SUPPLIER"; name: qsTr("Return to Supplier") }
						ListElement { id: "DISP_SCRAPPED";           name: qsTr("Scrapped") }
						ListElement { id: "DISP_USE_AS_IS";          name: qsTr("Use As Is") }
					}
					displayRole: "name"
					valueRole: "id"
				}

				LabelElementView {
					id: startedAtLabel
					width: parent.width
					labelText: qsTr("Started At")
				}

				LabelElementView {
					id: completedAtLabel
					width: parent.width
					labelText: qsTr("Completed At")
				}

				TextInputElementView {
					id: annotationsInput
					width: parent.width
					labelText: qsTr("Annotations")
					readOnly: true
					multiline: true
				}

				TextInputElementView {
					id: defectCodesInput
					width: parent.width
					labelText: qsTr("Defect Codes (comma-separated)")
					readOnly: true
				}
			}

			// --- Result Items ---
			GroupHeaderView {
				title: qsTr("Result Items / Measurements")
				width: content.width
				groupView: resultItemsGroup
			}

			GroupElementView {
				id: resultItemsGroup
				width: content.width

				ListView {
					id: resultItemsView
					width: parent.width
					height: Math.min(contentHeight, 300)
					clip: true
					model: null

					header: Rectangle {
						width: resultItemsView.width
						height: 28
						color: Style.headerColor

						Row {
							anchors.fill: parent
							anchors.margins: 4
							spacing: 4
							Text { text: qsTr("Name");       width: 160; font.bold: true }
							Text { text: qsTr("Type");       width: 70;  font.bold: true }
							Text { text: qsTr("Value");      width: 120; font.bold: true }
							Text { text: qsTr("Pass");       width: 60;  font.bold: true }
							Text { text: qsTr("Unit");       width: 60;  font.bold: true }
							Text { text: qsTr("Evidence");   width: 180; font.bold: true }
						}
					}

					delegate: Rectangle {
						width: resultItemsView.width
						height: 32
						color: index % 2 === 0 ? Style.backgroundColor1 : Style.backgroundColor2
						border.color: "#e0e0e0"
						border.width: 1

						Row {
							anchors.fill: parent
							anchors.margins: 4
							spacing: 4
							Text { text: model.name || "";         width: 160; elide: Text.ElideRight }
							Text { text: model.valueType || "";    width: 70 }
							Text { text: model.valueText || "";    width: 120; elide: Text.ElideRight }
							Text {
								width: 60
								text: model.passResult === 1 ? qsTr("✓ Pass") : (model.passResult === 0 ? qsTr("✗ Fail") : qsTr("—"))
								color: model.passResult === 1 ? "#27ae60" : (model.passResult === 0 ? "#e74c3c" : "#888")
							}
							Text { text: model.unit || "";         width: 60 }
							Text { text: model.evidenceRefs || ""; width: 180; elide: Text.ElideRight }
						}
					}
				}
			}
		}
	}
}
