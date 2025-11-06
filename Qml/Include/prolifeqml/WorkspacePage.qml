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
		
		Item {
			id: mainItem
			
			CustomScrollbar {
				id: scrollbar;
				
				z: parent.z + 1;
				
				anchors.right: parent.right;
				anchors.top: flickable.top;
				anchors.bottom: flickable.bottom;
				
				secondSize: 10;
				targetItem: flickable;
				
				radius: 2;
			}
			
			CustomScrollbar{
				id: scrollHoriz;
				
				z: parent.z + 1;
				
				anchors.left: flickable.left;
				anchors.right: flickable.right;
				anchors.bottom: flickable.bottom;
				
				secondSize: 10;
				
				vertical: false;
				targetItem: flickable;
			}
			
			Flickable {
				id: flickable
				width: mainItem.width
				height: mainItem.height
				boundsBehavior: Flickable.StopAtBounds;
				
				contentWidth: Math.max(topRow.width, softwareRow.width, hardwareRow.width)
				contentHeight: contentColumn.height + Style.sizeHintBXS
				clip: true
				
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
				
				Column {
					id: contentColumn
					anchors.top: flickable.top
					anchors.topMargin: Style.marginXL
					anchors.left: flickable.left
					anchors.leftMargin: Style.marginXL
					anchors.right: flickable.right
					anchors.rightMargin: Style.marginXL
					width: flickable.width
					spacing: Style.marginXL
					
					Row {
						id: topRow
						spacing: Style.marginXL
						visible: collectionInfoRepeater.count > 0
						Repeater {
							id: collectionInfoRepeater
							delegate: Component {
								ElementView {
									width: Style.sizeHintM
									name: model.item.m_total
									titleFontSize: Style.fontSizeBXL
									anchors.verticalCenter: parent.verticalCenter
									
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

					Row {
						id: softwareRow
						height: getSoftwareUsedPieChartRequest.height
						spacing: Style.marginXL
						visible: false
						clip: true
						Component.onCompleted: {
							let viewLicenses = PermissionsController.checkPermission("ViewLicenses")
							if (viewLicenses){
								softwareRow.visible = true
								getSoftwareUsedPieChartRequest.updateModel()
								getSoftwareUsedBarChart.updateModel()
								getLicenseCreationInfo.updateModel()
							}
						}
						
						GqlPiechartView {
							id: getSoftwareUsedPieChartRequest
							gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getSoftwareUsedPieChart
							name: qsTr("Software Used")
						}
						
						GqlBarchartView {
							id: getSoftwareUsedBarChart
							width: Style.sizeHintL
							name: qsTr("Software Used")
							currentIndex: 0
							gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getSoftwareUsedBarChart
							ySteps: 5
						}
						
						GqlLinechartView {
							id: getLicenseCreationInfo
							width: Style.sizeHintL
							gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getLicenseCreationInfo
							name: qsTr("License Creation")
							currentIndex: 0
						}
					}
					
					Row {
						id: hardwareRow
						height: getHardwareUsedPieChartRequest.height
						spacing: Style.marginXL
						visible: false
						clip: true
						Component.onCompleted: {
							let viewSensors = PermissionsController.checkPermission("ViewSensors")
							if (viewSensors){
								hardwareRow.visible = true
								getHardwareUsedPieChartRequest.updateModel()
								getHardwareUsedBarChart.updateModel()
								hardwareStatusInfoRequest.updateModel()
							}
						}
						
						GqlPiechartView {
							id: getHardwareUsedPieChartRequest
							gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareUsedPieChart
							name: qsTr("Hardware Used")
						}
						
						GqlBarchartView {
							id: getHardwareUsedBarChart
							width: Style.sizeHintL
							name: qsTr("Hardware Used")
							currentIndex: 0
							gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareUsedBarChart
						}
						
						GqlPiechartView {
							id: hardwareStatusInfoRequest
							gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareStatusInfo
							name: qsTr("Hardware Status")
						}
					}
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
