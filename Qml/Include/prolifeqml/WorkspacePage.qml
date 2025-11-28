import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtgui 1.0
import prolifeWorkspaceSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0
import imtlicgui 1.0
import imtcolgui 1.0
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

		if (PermissionsController.checkPermission("ViewAnalytics")){
			stackView.addPage(statisticsPageComp)
		}

		stackView.setCurrentIndex(0)
		
		if (commandsController){
			commandsController.setIsToggleable("Dashboard", true)
			commandsController.setToggled("Dashboard", true)
			commandsController.setIsToggleable("UserActions", true)
			commandsController.setToggled("UserActions", false)
			commandsController.setIsToggleable("Analytics", true)
			commandsController.setToggled("Analytics", false)
		}
	}

	property string customerId
	property TimeFilter timeFilter: null
	property TimeFilter defaultTimeFilter: TimeFilter {
		m_timeUnit: "Week"
		m_interpretationMode: "For"
	}

	onCommandActivated: {
		NavigationController.push("Workspace/" + commandId)

		checkCurrentPage(commandId)
	}

	function checkCurrentPage(commandId){
		if (commandId === "Dashboard"){
			stackView.setCurrentIndex(0)
		}
		else if (commandId === "UserActions"){
			stackView.setCurrentIndex(1)
		}
		else if (commandId === "Analytics"){
			stackView.setCurrentIndex(2)
		}

		commandsController.setToggled("Dashboard", commandId === "Dashboard")
		commandsController.setToggled("UserActions", commandId === "UserActions")
		commandsController.setToggled("Analytics", commandId === "Analytics")
	}

	NavigableItem {
		id: navigableItem
		parentSegment: "Workspace"
		paths: ["Dashboard", "UserActions", "Analytics"]
		onActivated: {
			root.checkCurrentPage(matchedPath)
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

			function navigateToHardware(productName, inUse, statusId){
				let productId = CachedProductCollection.getProductIdByName(productName)
				let params = {}
				params.productId = productId
				params.customerId = root.customerId
				params.inUse = inUse
				params.internalUse = false

				if (statusId){
					params.statusId = statusId
				}
		
				if (root.timeFilter){
					let timeFilterObj = {}
					timeFilterObj.name = timeFilterDelegate.mainButtonText
					timeFilterObj.data = root.timeFilter
					params.timeFilter = timeFilterObj
				}
		
				NavigationController.navigate("Devices/<hardware-filter>", params)
			}
		
			function navigateToSoftware(productName){
				let productId = CachedProductCollection.getProductIdByName(productName)
				
				let params = {}
				params.productId = productId
				params.customerId = root.customerId
				params.inUse = true
				params.internalUse = false
		
				if (root.timeFilter){
					let timeFilterObj = {}
					timeFilterObj.name = timeFilterDelegate.mainButtonText
					timeFilterObj.data = root.timeFilter
					params.timeFilter = timeFilterObj
				}
		
				NavigationController.navigate("SoftwareProducts/<software-filter>", params)
			}

			Item {
				x: row1.visible ? row1.x : row2.visible ? row2.x : Style.spacingM
				y: -height - ((root.commandsPanelHeight - height) / 2)
				z: parent.z + 1
				width: filterRow.width
				height: filterRow.height
				
				visible: stackView.currentIndex == 0

				Row {
					id: filterRow
					height: customerFilterDelegate.height
					spacing: Style.spacingM
					TimeFilterDelegate {
						id: timeFilterDelegate
						objectName: "TimeFilterDelegate"
						collectionFilter: CollectionFilter {}
						canTimeRangeEdit:false
						timeFilter: TimeFilter {
							m_timeUnit: "Week"
							m_interpretationMode: "For"
						}
						
						onAccepted: {
							root.timeFilter = timeFilter.copyMe()
						}

						onClearFilter: {
							root.timeFilter = null
						}
					}
					
					CustomerFilterDelegate {
						id: customerFilterDelegate
						objectName: "CustomerFilterDelegate"
						collectionFilter: CollectionFilter {}

						onOptionSelectionChanged: {
							if (optionIds.length > 0){
								root.customerId = optionIds[0]
							}
							else{
								root.customerId = ""
							}
						}
					}
				}
			}

			Row {
				id: topRow
				anchors.top: parent.top
				anchors.topMargin: chartsBlock.spacing
				anchors.horizontalCenter: parent.horizontalCenter
				spacing: chartsBlock.spacing
				visible: collectionInfoRepeater.count > 0
				height: 110

				property string customerId: root.customerId
				property TimeFilter timeFilter: root.timeFilter
				property bool updateRequested: false

				signal loadingStart()
				signal loadingStop()
				
				onCustomerIdChanged: {
					updateModel()
				}

				onTimeFilterChanged: {
					updateModel()
				}

				onVisibleChanged: {
					if (visible && updateRequested){
						updateModel()
						updateRequested = false
					}
				}
				
				function updateModel(){
					if (!visible){
						updateRequested = true
						return
					}

					topRow.loadingStart()
					getTotalSummaryInfoRequest.send()
				}

				GqlSdlRequestSender {
					id: getTotalSummaryInfoRequest
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getTotalSummaryInfo
					inputObjectComp: Component {
						ChartInput {
							m_customerId: root.customerId
							m_timeFilter: root.timeFilter
						}
					}

					sdlObjectComp: Component {
						TotalSummaryInfo {
							onFinished: {
								collectionInfoRepeater.model = m_summaryInfos
								topRow.loadingStop()
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
						topRow.updateModel()
					}
				}

				SubscriptionClient {
					gqlCommandId: "OnSoftwareProductsCollectionChanged"
					onMessageReceived: {
						topRow.updateModel()
					}
				}

				SubscriptionClient {
					gqlCommandId: "OnOrdersCollectionChanged"
					onMessageReceived: {
						topRow.updateModel()
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
							objectName: model.item.m_collectionId +  "Info"

							Component.onCompleted: {
								checkWidth()
							}

							onWidthChanged: {
								checkWidth()
							}
							
							function checkWidth(){
								if (!controlItem){
									controlItem.width = 0
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
										objectName: "CreateNewButton"
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
										objectName: "ViewAllButton"
										onClicked: {
											let objectTypeId = model.item.m_objectTypeId
											let collectionId = model.item.m_collectionId
											NavigationController.navigate(collectionId + "/" + objectTypeId)
										}
									}
								}
							}

							Loading {
								id: loading
								anchors.fill: parent
								color: Style.baseColor
								visible: false
							}

							StickerView {
								anchors.verticalCenter: parent.top
								anchors.left: parent.left
								anchors.leftMargin: Style.marginM
								color: Style.iconColorOnSelected
								text: model.item.m_title
							}

							Connections {
								target: topRow
								function onLoadingStart(){
									loading.start()
								}
								function onLoadingStop(){
									loading.stop()
								}
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
				property real chartHeight: row1.height - 85

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
					objectName: "SoftwareUsedPieChart"
					width: chartsBlock.chartWidth
					chartHeight: row1.chartHeight
					name: qsTr("Active Software Instances by Product")
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getSoftwareUsedPieChart
					subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
					legendClickable: true
					customerId: root.customerId
					timeFilter: root.timeFilter
					onLegendClicked: {
						chartsBlock.navigateToSoftware(label)
					}
				}
		
				GqlBarchartView {
					id: softwareUsedBarChart
					objectName: "SoftwareUsedBarChart"
					width: chartsBlock.chartWidth
					chartHeight: row1.chartHeight
					name: qsTr("Active Software Instances by Period")
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getSoftwareUsedBarChart
					subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
					customerId: root.customerId
					timeFilter: root.timeFilter ? root.timeFilter : root.defaultTimeFilter
					legendClickable: true
					onLegendClicked: {
						chartsBlock.navigateToSoftware(label)
					}
				}
		
				GqlLinechartView {
					id: licenseCreationInfo
					objectName: "LicenseCreationInfo"
					width: chartsBlock.chartWidth
					chartHeight: row1.chartHeight
					name: qsTr("License Creation Activity")
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getLicenseCreationInfo
					subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
					customerId: root.customerId
					timeFilter: root.timeFilter ? root.timeFilter : root.defaultTimeFilter
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

				property real chartHeight: row2.height - 85

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
					objectName: "HardwareUsedPieChart"
					width: chartsBlock.chartWidth
					chartHeight: row2.chartHeight
					name: qsTr("Active Hardware Instances by Product")
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareUsedPieChart
					subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
					legendClickable: true
					customerId: root.customerId
					timeFilter: root.timeFilter
					onLegendClicked: {
						chartsBlock.navigateToHardware(label, true)
					}
				}
		
				GqlBarchartView {
					id: hardwareUsedBarChart
					objectName: "HardwareUsedBarChart"
					width: chartsBlock.chartWidth
					chartHeight: row2.chartHeight
					name: qsTr("Active Hardware Instances by Period")
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareUsedBarChart
					subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
					customerId: root.customerId
					timeFilter: root.timeFilter ? root.timeFilter : root.defaultTimeFilter
					legendClickable: true
					onLegendClicked: {
						chartsBlock.navigateToHardware(label, true)
					}
				}
		
				GqlPiechartView {
					id: hardwareStatusInfo
					objectName: "HardwareStatusInfo"
					width: chartsBlock.chartWidth
					chartHeight: row2.chartHeight
					name: qsTr("Hardware Status")
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareStatusInfo
					subscriptionCommandId: "OnDevicesCollectionChanged"
					customerId: root.customerId
					timeFilter: root.timeFilter
					legendClickable: true
					onLegendClicked: {
						let statusId = deviceProductionStatus.getStatusIdByName(label)
						chartsBlock.navigateToHardware(label, false, String(deviceProductionStatus.getStatusIndex(statusId)))
					}
					
					DeviceProductionStatus {
						id: deviceProductionStatus
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

	Component {
		id: statisticsPageComp
		StatisticsPage {
		}
	}

	StackView {
		id: stackView
		anchors.fill: parent
	}
}
