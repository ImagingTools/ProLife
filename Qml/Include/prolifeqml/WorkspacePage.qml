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
			commandsController.setIsToggleable("Dashboard", true)
			commandsController.setToggled("Dashboard", true)
			commandsController.setIsToggleable("UserActions", true)
			commandsController.setToggled("UserActions", false)
		}
	}

	onCommandActivated: {
		if (commandId === "UserActions"){
			NavigationController.push("Workspace/UserActions")
		}
		
		checkCurrentPage(commandId)
	}

	function checkCurrentPage(commandId){
		if (commandId === "Dashboard"){
			stackView.setCurrentIndex(0)
		}
		else if (commandId === "UserActions"){
			stackView.setCurrentIndex(1)
		}
		
		commandsController.setToggled("Dashboard", commandId === "Dashboard")
		commandsController.setToggled("UserActions", commandId === "UserActions")
	}

	NavigableItem {
		id: navigableItem
		parentSegment: "Workspace"
		paths: ["Dashboard", "UserActions"]
		onActivated: {
			if (matchedPath === paths[0]){
				root.checkCurrentPage("Dashboard")
			}
			else if (matchedPath === paths[1]){
				root.checkCurrentPage("UserActions")
			}
		}
		
		onParentActivated: {
			root.checkCurrentPage("Dashboard")
		}
	}
	
	Component {
		id: workspacePageComp

		Item {
			id: chartsBlock
			anchors.fill: parent

			property int spacing: Style.marginL
			property int chartDefaultWidth: 600
			property int chartCountPerRow: 3
			property real chartWidth:
				(width >= chartDefaultWidth * chartCountPerRow + spacing * (chartCountPerRow - 1))
				? chartDefaultWidth
				: (width - 2*spacing * (chartCountPerRow - 1)) / chartCountPerRow
			property real rowHeight:
				(!row1.visible && !row2.visible)
					? height - topRow.height - 2*spacing
				: (row1.visible && row2.visible)
					? (height - topRow.height - 4*spacing) / 2
				: height - topRow.height - 3*spacing

			Row {
				id: topRow
				anchors.top: parent.top
				anchors.topMargin: chartsBlock.spacing
				anchors.horizontalCenter: parent.horizontalCenter
				spacing: chartsBlock.spacing
				visible: collectionInfoRepeater.count > 0
				height: 110

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

				SubscriptionClient {
					gqlCommandId: "OnDevicesCollectionChanged"
					onMessageReceived: {
						getTotalSummaryInfoRequest.send()
					}
				}

				SubscriptionClient {
					gqlCommandId: "OnSoftwareProductsCollectionChanged"
					onMessageReceived: {
						getTotalSummaryInfoRequest.send()
					}
				}

				SubscriptionClient {
					gqlCommandId: "OnOrdersCollectionChanged"
					onMessageReceived: {
						getTotalSummaryInfoRequest.send()
					}
				}

				Repeater {
					id: collectionInfoRepeater
					delegate: Component {
						ElementView {
							anchors.verticalCenter: parent.verticalCenter
							width: chartsBlock.chartWidth
							name: model.item.m_total
							titleFontSize: Style.fontSizeBXL
							contentMargin: Style.marginM
							
							onWidthChanged: {
								if (!controlItem){
									return
								}

								if (contentWidth > (controlItem.contentWidth + 2 * contentMargin)){
									controlItem.width = controlItem.contentWidth
								}
								else{
									controlItem.width = 0
								}
							}
							bottomComp: Component {
								Row {
									height: Style.marginL
									spacing: Style.marginXL
									visible: model.item.m_collectionId === "SoftwareProducts" || model.item.m_collectionId === "Devices"
									Row {
										id: inUseRow
										anchors.verticalCenter: parent.verticalCenter
										spacing: Style.marginS
										height: parent.height

										Rectangle {
											id: inUseRect
											anchors.verticalCenter: parent.verticalCenter
											width: Style.buttonWidthXXS
											height: width
											radius: width
											color: "green"
										}

										BaseText {
											anchors.verticalCenter: parent.verticalCenter
											text: qsTr("Running: ") + model.item.m_inUseCount
										}
									}
									Row {
										id: notInUseRow
										anchors.verticalCenter: parent.verticalCenter
										spacing: Style.marginS
										height: parent.height

										Rectangle {
											id: notInUseRect
											anchors.verticalCenter: parent.verticalCenter
											width: Style.buttonWidthXXS
											height: width
											radius: width
											color: "gray"
										}

										BaseText {
											anchors.verticalCenter: parent.verticalCenter
											text: qsTr("In Stock: ") + model.item.m_notInUseCount
										}
									}
									Row {
										id: internaUseRow
										anchors.verticalCenter: parent.verticalCenter
										spacing: Style.marginS
										height: parent.height

										Rectangle {
											anchors.verticalCenter: parent.verticalCenter
											width: Style.buttonWidthXXS
											height: width
											radius: width
											color: "blue"
										}

										BaseText {
											anchors.verticalCenter: parent.verticalCenter
											text: qsTr("Internal: ") + model.item.m_internalUseCount
										}
									}
								}
							}

							controlComp: Component {
								Item {
									width: contentWidth
									height: Style.controlHeightM
									clip: true
									visible: width > 0
									property int contentWidth: createNewButton.width + viewAllButton.width + 3*Style.spacingM
									Button {
										id: createNewButton
										anchors.left: parent.left
										anchors.leftMargin: Style.spacingM
										height: Style.controlHeightM
										text: qsTr("Create New")
										widthFromDecorator: true
										onClicked: {
											let params = {}
											params.createNew = true
											let objectTypeId = model.item.m_objectTypeId
											let collectionId = model.item.m_collectionId
											NavigationController.navigate(collectionId + "/" + objectTypeId, params)
										}
									}
									
									Button {
										id: viewAllButton
										anchors.left: createNewButton.right
										anchors.leftMargin: Style.spacingM
										height: Style.controlHeightM
										text: qsTr("View All")
										widthFromDecorator: true
										onClicked: {
											let objectTypeId = model.item.m_objectTypeId
											let collectionId = model.item.m_collectionId
											NavigationController.navigate(collectionId + "/" + objectTypeId)
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

			/* ROW 1 */
			Row {
				id: row1
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.top: topRow.bottom
				anchors.topMargin: chartsBlock.spacing
				height: chartsBlock.rowHeight
				spacing: chartsBlock.spacing

				visible: false
				property real chartHeight: row1.height - 100

				Component.onCompleted: {
					let viewLicenses = PermissionsController.checkPermission("ViewLicenses")
					if (viewLicenses){
						softwareUsedPieChart.updateModel()
						softwareUsedBarChart.updateModel()
						licenseCreationInfo.updateModel()
						row1.visible = true
					}
				}
		
				GqlPiechartView {
					id: softwareUsedPieChart
					width: chartsBlock.chartWidth
					chartHeight: row1.chartHeight
					name: qsTr("Software In Use")
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getSoftwareUsedPieChart
					subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
				}
		
				GqlBarchartView {
					id: softwareUsedBarChart
					width: chartsBlock.chartWidth
					chartHeight: row1.chartHeight
					name: qsTr("Software In Use")
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getSoftwareUsedBarChart
					subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
					currentIndex: 0
				}
		
				GqlLinechartView {
					id: licenseCreationInfo
					width: chartsBlock.chartWidth
					chartHeight: row1.chartHeight
					name: qsTr("License Creation")
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getLicenseCreationInfo
					subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
					currentIndex: 0
				}
			}
		
			/* ROW 2 */
			Row {
				id: row2
				anchors.top: row1.bottom
				anchors.topMargin: chartsBlock.spacing
				anchors.horizontalCenter: parent.horizontalCenter
				height: chartsBlock.rowHeight
				spacing: chartsBlock.spacing
				visible: false

				property real chartHeight: row2.height - 100

				Component.onCompleted: {
					let viewHardware = PermissionsController.checkPermission("ViewSensors")
					if (viewHardware){
						hardwareUsedPieChart.updateModel()
						hardwareUsedBarChart.updateModel()
						hardwareStatusInfo.updateModel()
						row2.visible = true
					}
				}
		
				GqlPiechartView {
					id: hardwareUsedPieChart
					width: chartsBlock.chartWidth
					chartHeight: row2.chartHeight
					name: qsTr("Hardware In Use")
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareUsedPieChart
					subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
				}
		
				GqlBarchartView {
					id: hardwareUsedBarChart
					width: chartsBlock.chartWidth
					chartHeight: row2.chartHeight
					name: qsTr("Hardware In Use")
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareUsedBarChart
					subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
					currentIndex: 0
				}
		
				GqlPiechartView {
					id: hardwareStatusInfo
					width: chartsBlock.chartWidth
					chartHeight: row2.chartHeight
					name: qsTr("Hardware Status")
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareStatusInfo
					subscriptionCommandId: "OnDevicesCollectionChanged"
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
