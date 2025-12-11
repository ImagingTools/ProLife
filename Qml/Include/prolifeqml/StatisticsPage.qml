import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import prolifeWorkspaceSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0
import imtbaseCommandsSdl 1.0
import imtguigql 1.0

ViewBase {
	id: root

	commandsSeparatorVisible: false
	property int spacing: Style.marginL
	property int commandsPanelHeight: Style.marginL

	property TimeFilter timeFilter: defaultTimeFilter
	property TimeFilter defaultTimeFilter: TimeFilter {
		m_timeUnit: "Year"
		m_interpretationMode: "Current"
	}

	onCommandActivated: {
		checkCurrentPage(commandId)
	}

	commandsControllerComp: Component {
		GqlBasedCommandsController {
			typeId: "Analytics"
		}
	}

	Component.onCompleted: {
		let viewLicenses = PermissionsController.checkPermission("ViewLicenses")
		let viewSensors = PermissionsController.checkPermission("ViewSensors")
		if (viewLicenses && viewSensors){
			stackView.addPage(softwareHadrwarePageComp)
		}

		if (PermissionsController.checkPermission("ViewOrders")){
			stackView.addPage(ordersPageComp)
		}

		stackView.setCurrentIndex(0)
		
		if (commandsController){
			commandsController.setIsToggleable("Software_Hardware", true)
			commandsController.setToggled("Software_Hardware", true)
			commandsController.setIsToggleable("Orders", true)
			commandsController.setToggled("Orders", false)
		}
	}

	function checkCurrentPage(commandId){
		if (commandId === "Software_Hardware"){
			stackView.setCurrentIndex(0)
		}
		else if (commandId === "Orders"){
			stackView.setCurrentIndex(1)
		}

		commandsController.setToggled("Software_Hardware", commandId === "Software_Hardware")
		commandsController.setToggled("Orders", commandId === "Orders")
	}

	StackView {
		id: stackView
		anchors.fill: parent
	}

	Item {
		x: root.spacing
		y: -height - ((root.commandsPanelHeight - height) / 2)
		z: parent.z + 1
		width: timeFilterDelegate.width
		height: timeFilterDelegate.height

		TimeFilterDelegate {
			id: timeFilterDelegate
			objectName: "TimeFilterDelegate"
			canTimeRangeEdit: false
			showFilterDetails: true
			name: qsTr("Creation Date")

			Component.onCompleted: {
				setTimeUnit("Current", "Year", true)
			}

			onAccepted: {
				root.timeFilter = timeFilter.copyMe()
			}

			onCleared: {
				root.timeFilter = null
			}
		}
	}

	Component {
		id: softwareHadrwarePageComp
		
		Item {
			anchors.fill: parent
			
			Row {
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.leftMargin: root.spacing
				anchors.right: parent.right
				anchors.rightMargin: root.spacing
				anchors.bottom: parent.bottom
				anchors.bottomMargin: root.spacing
				spacing: root.spacing

				Column {
					width: parent.width - getHardwareConfigurationPieChart.width - parent.spacing
					height: parent.height
					spacing: root.spacing

					Row {
						id: row1
						width: parent.width
						height: parent.height / 2 - parent.spacing / 2
						spacing: root.spacing
						visible: false
						
						Component.onCompleted: {
							let viewLicenses = PermissionsController.checkPermission("ViewLicenses")
							if (viewLicenses){
								row1.visible = true
								getSoftwareCreationBarChart.updateModel()
								getSoftwareCreationBarChart.visible = true
							}
							
							let viewHardware = PermissionsController.checkPermission("ViewSensors")
							if (viewHardware){
								row1.visible = true
								getHardwareCreationBarChart.updateModel()
								getHardwareCreationBarChart.visible = true
							}
						}
						
						GqlBarchartView {
							id: getHardwareCreationBarChart
							objectName: "HardwareCreationBarChart"
							width: parent.width / 2 - parent.spacing / 2
							chartHeight: parent.height - 85
							name: qsTr("Hardware Instances by Period")
							gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareCreationBarChart
							subscriptionCommandId: "OnDevicesCollectionChanged"
							legendClickable: true
							visible: false
							timeFilter: root.timeFilter
							onLegendClicked: {
								let productId = CachedProductCollection.getProductIdByName(label)
								let params = {}
								params.productId = productId
								params.internalUse = false
								if (root.timeFilter){
									let timeFilterObj = {}
									timeFilterObj.unit = root.timeFilter.m_timeUnit
									timeFilterObj.mode = root.timeFilter.m_interpretationMode
									params.timeFilter = timeFilterObj
								}
								
								NavigationController.navigate("Devices/<hardware-filter>", params)
							}
						}
						
						GqlBarchartView {
							id: getSoftwareCreationBarChart
							objectName: "SoftwareCreationBarChart"
							width: parent.width / 2 - parent.spacing / 2
							chartHeight: parent.height - 85
							name: qsTr("Software Instances by Period")
							gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getSoftwareCreationBarChart
							subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
							timeFilter: root.timeFilter
							legendClickable: true
							visible: false
							onLegendClicked: {
								let productId = CachedProductCollection.getProductIdByName(label)
								let params = {}
								params.productId = productId
								params.internalUse = false
								if (root.timeFilter){
									let timeFilterObj = {}
									timeFilterObj.unit = root.timeFilter.m_timeUnit
									timeFilterObj.mode = root.timeFilter.m_interpretationMode
									params.timeFilter = timeFilterObj
								}
								
								NavigationController.navigate("SoftwareProducts/<software-filter>", params)
							}
						}
					}

					Row {
						id: row2
						width: parent.width
						height: parent.height / 2 - parent.spacing / 2
						spacing: root.spacing
						
						Component.onCompleted: {
							let viewLicenses = PermissionsController.checkPermission("ViewLicenses")
							if (viewLicenses){
								getSoftwareCustomerPieChart.visible = true
							}
							
							let viewHardware = PermissionsController.checkPermission("ViewSensors")
							if (viewHardware){
								getHardwareCustomerPieChart.visible = true
								getHardwareConfigurationPieChart.visible = true
							}
						}
						
						GqlPiechartView {
							id: getHardwareCustomerPieChart
							objectName: "HardwareCustomerPieChart"
							width: parent.width / 2 - parent.spacing / 2
							chartHeight: parent.height - 85
							name: qsTr("Hardware Instances by Customer")
							gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareCustomerPieChart
							subscriptionCommandId: "OnDevicesCollectionChanged"
							legendClickable: true
							visible: false
							timeFilter: root.timeFilter
							Component.onCompleted: {
								updateModel()
							}
							onLegendClicked: {
								let navigationParams = {}
								navigationParams.customerId = id
								navigationParams.inUse = undefined
								navigationParams.internalUse = false

								if (root.timeFilter){
									let timeFilterObj = {}
									timeFilterObj.unit = root.timeFilter.m_timeUnit
									timeFilterObj.mode = root.timeFilter.m_interpretationMode
									navigationParams.timeFilter = timeFilterObj
								}

								NavigationController.navigate("Devices/<hardware-filter>", navigationParams)
							}
						}
						
						GqlPiechartView {
							id: getSoftwareCustomerPieChart
							objectName: "SoftwareCustomerPieChart"
							width: parent.width / 2 - parent.spacing / 2
							chartHeight: parent.height - 85
							name: qsTr("Software Instances by Customer")
							gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getSoftwareCustomerPieChart
							subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
							legendClickable: true
							visible: false
							timeFilter: root.timeFilter
							Component.onCompleted: {
								updateModel()
							}
							onLegendClicked: {
								let navigationParams = {}
								navigationParams.customerId = id
								navigationParams.inUse = undefined
								navigationParams.internalUse = false
								
								if (root.timeFilter){
									let timeFilterObj = {}
									timeFilterObj.unit = root.timeFilter.m_timeUnit
									timeFilterObj.mode = root.timeFilter.m_interpretationMode
									navigationParams.timeFilter = timeFilterObj
								}
								
								NavigationController.navigate("SoftwareProducts/<software-filter>", navigationParams)
							}
						}
					}
				}

				GqlPiechartView {
					id: getHardwareConfigurationPieChart
					objectName: "HardwareConfigurationPieChart"
					width: root.width / 3 - 4+root.spacing
					chartHeight: parent.height - 85
					name: qsTr("Hardware Configuration")
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareConfigurationPieChart
					subscriptionCommandId: "OnDevicesCollectionChanged"
					legendClickable: true
					visible: false
					timeFilter: root.timeFilter
					Component.onCompleted: {
						updateModel()
					}
					onLegendClicked: {
						let navigationParams = {}
						navigationParams.licenseId = id
						navigationParams.productId = CachedProductCollection.getProductIdByLicenseId(id)
						navigationParams.internalUse = false

						if (root.timeFilter){
							let timeFilterObj = {}
							timeFilterObj.unit = root.timeFilter.m_timeUnit
							timeFilterObj.mode = root.timeFilter.m_interpretationMode
							navigationParams.timeFilter = timeFilterObj
						}

						NavigationController.navigate("Devices/<hardware-filter>", navigationParams)
					}
				}
			}
		}
	}

	Component {
		id: ordersPageComp
		Item {
			Row {
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.leftMargin: root.spacing
				anchors.right: parent.right
				anchors.rightMargin: root.spacing
				anchors.bottom: parent.bottom
				anchors.bottomMargin: root.spacing
				
				GqlLinechartView {
					id: getOrderCreationLineChart
					objectName: "OrderCreationLineChart"
					width: parent.width
					chartHeight: parent.height - 85
					name: qsTr("Order Creation By Period")
					gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getOrderCreationLineChart
					subscriptionCommandId: "OnOrdersCollectionChanged"
					timeFilter: root.timeFilter
				}
			}
		}
	}
}
