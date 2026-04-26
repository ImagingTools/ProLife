import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import prolifeqml 1.0

ViewBase {
	id: procurementPageRoot
	anchors.fill: parent

	commandsControllerComp: Component {
		GqlBasedCommandsController {
			typeId: "Procurement"
		}
	}

	Component.onCompleted: {
		if (PermissionsController.checkPermission("ViewProcurement")) {
			stackView.addPage(procurementListPageComp)
		}

		if (PermissionsController.checkPermission("ManageIqcTemplates")) {
			stackView.addPage(iqcTemplatesPageComp)
		}

		stackView.setCurrentIndex(0)

		if (commandsController) {
			commandsController.setIsToggleable("ProcurementList", true)
			commandsController.setToggled("ProcurementList", true)
			commandsController.setIsToggleable("IqcTemplates", true)
			commandsController.setToggled("IqcTemplates", false)
		}
	}

	StackView {
		id: stackView
		anchors.fill: parent
	}

	Component {
		id: procurementListPageComp

		Item {
			anchors.fill: parent

			SplitView {
				anchors.fill: parent
				orientation: Qt.Horizontal

				// Left panel: list of procurement orders
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
							text: qsTr("Procurement Orders")
							font.bold: true
							font.pixelSize: 14
						}

						Rectangle {
							width: parent.width
							height: 1
							color: "#e0e0e0"
						}

						GqlCollectionView {
							id: procurementListView
							width: parent.width
							height: parent.height - 60
							typeId: "Procurement"
							onCurrentItemChanged: {
								if (currentItem) {
									procurementEditor.procurementId = currentItem.id
									procurementEditor.loadProcurementData()
								}
							}
						}

						Row {
							spacing: 8
							visible: PermissionsController.checkPermission("AddProcurement")

							Button {
								text: qsTr("New Order")
								onClicked: {
									procurementEditor.createNew()
								}
							}
						}
					}
				}

				// Right panel: detail/editor
				Rectangle {
					color: "transparent"

					ProcurementEditor {
						id: procurementEditor
						anchors.fill: parent
						anchors.margins: 8
					}
				}
			}
		}
	}

	Component {
		id: iqcTemplatesPageComp

		Item {
			anchors.fill: parent

			IqcTemplateEditor {
				id: iqcTemplateEditor
				anchors.fill: parent
				anchors.margins: 8
			}
		}
	}
}
