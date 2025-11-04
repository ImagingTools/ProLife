import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtgui 1.0
import prolifeWorkspaceSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0
import com.imtcore.imtqml 1.0

ViewBase {
	id: root
	anchors.fill: parent
	commandsControllerComp: Component {
		GqlBasedCommandsController {
			typeId: "Workspace"
		}
	}

	Component.onCompleted: {
		stackView.addPage(workspacePageComp)

		let viewUserActions = PermissionsController.checkPermission("ViewUserActions")
		if (viewUserActions){
			stackView.addPage(userActionsPageComp)
		}
		stackView.setCurrentIndex(0)

		if (commandsController){
			commandsController.setIsToggleable("Workspace", true)
			commandsController.setToggled("Workspace", true)
			commandsController.setIsToggleable("UserActions", true)
			commandsController.setToggled("UserActions", false)
		}
	}

	onCommandActivated: {
		if (commandId === "Workspace"){
			stackView.setCurrentIndex(0)
		}
		else if (commandId === "UserActions"){
			stackView.setCurrentIndex(1)
		}

		commandsController.setToggled("Workspace", commandId === "Workspace")
		commandsController.setToggled("UserActions", commandId === "UserActions")
	}

	Component {
		id: workspacePageComp
		Flickable {
			id: flickable
			anchors.fill: root
			boundsBehavior: Flickable.StopAtBounds;
	
			contentWidth: topFlow.width
			contentHeight: 2000//topFlow.height + flow.height
			clip: true;

			GqlSdlRequestSender {
				id: getTotalSummaryInfoRequest
				gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getTotalSummaryInfo
				sdlObjectComp: Component {
					TotalSummaryInfo {
						onFinished: {
							collectionInfoRepeater.model = m_summaryInfos
						}
					}
				}

				Component.onCompleted: {
					send()
				}
			}

			Flow {
				id: topFlow
				anchors.top: root.top
				anchors.left: root.left
				anchors.right: root.right
				anchors.margins: Style.marginXL
				spacing: Style.marginXL

				Repeater {
					id: collectionInfoRepeater
					delegate: Component {
						ElementView {
							width: Style.sizeHintM
							name: model.item.m_total
							titleFontSize: Style.fontSizeBXL
							controlComp: Component {
								Row {
									height: Style.controlHeightM
									spacing: Style.marginM
									Button {
										width: Style.sizeHintBXS
										height: Style.controlHeightM
										text: qsTr("Create New")
										onClicked: {
											let params = {}
											params.createNew = true
											let objectTypeId = model.item.m_objectTypeId
											let collectionId = model.item.m_collectionId
											NavigationController.navigate(collectionId + "/" + objectTypeId, params)
										}
									}

									Button {
										width: Style.sizeHintBXS
										height: Style.controlHeightM
										text: qsTr("View All")
										onClicked: {
											NavigationController.navigate(model.item.m_collectionId)
										}
									}
								}
							}

							StickerView {
								anchors.verticalCenter: parent.top
								anchors.left: parent.left
								anchors.leftMargin: Style.marginM
								color: Style.iconColorOnSelected
								text: model.item.m_title
							}
						}
					}
				}
			}

			Flow {
				id: flow
				anchors.top: topFlow.bottom
				anchors.bottom: root.bottom
				anchors.left: root.left
				anchors.right: root.right
				anchors.margins: Style.marginXL
				spacing: Style.marginXL

				Component.onCompleted: {
					let viewLicenses = PermissionsController.checkPermission("ViewLicenses")
					if (viewLicenses){
						getSoftwareUsedPieChartRequest.visible = true
						getSoftwareUsedBarChart.visible = true
						getLicenseCreationInfo.visible = true
						getSoftwareUsedPieChartRequest.updateModel()
						getSoftwareUsedBarChart.updateModel()
						getLicenseCreationInfo.updateModel()
					}

					let viewSensors = PermissionsController.checkPermission("ViewSensors")
					if (viewSensors){
						getHardwareUsedPieChartRequest.visible = true
						getHardwareUsedBarChart.visible = true
						hardwareStatusInfoRequest.visible = true
						getHardwareUsedPieChartRequest.updateModel()
						getHardwareUsedBarChart.updateModel()
						hardwareStatusInfoRequest.updateModel()
					}
				}

				GqlPiechartView {
					id: getHardwareUsedPieChartRequest
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareUsedPieChart
					name: qsTr("Hardware Used")
					visible: false
				}

				GqlBarchartView {
					id: getHardwareUsedBarChart
					width: Style.sizeHintL
					name: qsTr("Hardware Used")
					currentIndex: 0
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareUsedBarChart
					visible: false
				}

				GqlPiechartView {
					id: hardwareStatusInfoRequest
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareStatusInfo
					name: qsTr("Hardware Status")
					visible: false
				}

				GqlPiechartView {
					id: getSoftwareUsedPieChartRequest
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getSoftwareUsedPieChart
					name: qsTr("Software Used")
					visible: false
				}

				GqlBarchartView {
					id: getSoftwareUsedBarChart
					width: Style.sizeHintL
					name: qsTr("Software Used")
					currentIndex: 0
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getSoftwareUsedBarChart
					ySteps: 5
					visible: false
				}

				GqlLinechartView {
					id: getLicenseCreationInfo
					width: Style.sizeHintL
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getLicenseCreationInfo
					name: qsTr("License Creation")
					currentIndex: 0
					visible: false
				}
			}
		}
	}

	Component {
		id: userActionsPageComp
		UserActionCollectionView {
			height: 500
		}
	}

	StackView {
		id: stackView
		anchors.fill: parent
	}
}
