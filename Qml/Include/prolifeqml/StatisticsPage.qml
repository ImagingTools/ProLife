import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import prolifeWorkspaceSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

Item {
	id: root

	property int chartDefaultWidth: 600
	property int chartCountPerRow: 3
	property real chartWidth:
		(width >= chartDefaultWidth * chartCountPerRow + Style.marginL * (chartCountPerRow - 1))
		? chartDefaultWidth
		: (width - 2*Style.marginL * (chartCountPerRow - 1)) / chartCountPerRow

	property int spacing: Style.marginL
	property int commandsPanelHeight: Style.marginL

	property TimeFilter timeFilter: defaultTimeFilter
	property TimeFilter defaultTimeFilter: TimeFilter {
		m_timeUnit: "Year"
		m_interpretationMode: "This"
	}

	Item {
		x: row1.visible ? row1.x : row2.visible ? row2.x : Style.spacingM
		y: -height - ((root.commandsPanelHeight - height) / 2)
		z: parent.z + 1
		width: timeFilterDelegate.width
		height: timeFilterDelegate.height

		TimeFilterDelegate {
			id: timeFilterDelegate
			objectName: "TimeFilterDelegate"
			collectionFilter: CollectionFilter {}
			canTimeRangeEdit: false

			Component.onCompleted: {
				setTimeFilter(root.defaultTimeFilter, qsTr("This year"), true)
			}

			onAccepted: {
				root.timeFilter = timeFilter.copyMe()
			}

			onClearFilter: {
				root.timeFilter = null
			}
		}
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
		anchors.fill: parent
		contentWidth: width
		contentHeight: row1.height + row2.height + 2*Style.marginXXXL
		boundsBehavior: Flickable.StopAtBounds;

		clip: true

		Row {
			id: row1
			anchors.top: parent.top
			anchors.topMargin: root.spacing
			anchors.horizontalCenter: parent.horizontalCenter
			visible: false
			spacing: root.spacing
			height: visible ? getSoftwareCreationBarChart.height : 0
	
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
	
				let viewOrders = PermissionsController.checkPermission("ViewOrders")
				if (viewOrders){
					row1.visible = true
					getOrderCreationLineChart.updateModel()
					getOrderCreationLineChart.visible = true
				}
			}

			GqlBarchartView {
				id: getHardwareCreationBarChart
				objectName: "HardwareCreationBarChart"
				width: root.chartWidth
				chartHeight: 300
				name: qsTr("Hardware Instances by Period")
				gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareCreationBarChart
				subscriptionCommandId: "OnDevicesCollectionChanged"
				legendClickable: true
				visible: false
				timeFilter: root.timeFilter ? root.timeFilter : root.defaultTimeFilter
			}

			GqlBarchartView {
				id: getSoftwareCreationBarChart
				objectName: "SoftwareCreationBarChart"
				width: root.chartWidth
				chartHeight: 300
				name: qsTr("Software Instances by Period")
				gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getSoftwareCreationBarChart
				subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
				timeFilter: root.timeFilter ? root.timeFilter : root.defaultTimeFilter
				legendClickable: true
				visible: false
			}

			GqlLinechartView {
				id: getOrderCreationLineChart
				objectName: "OrderCreationLineChart"
				width: root.chartWidth
				chartHeight: 300
				name: qsTr("Order Creation By Period")
				gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getOrderCreationLineChart
				subscriptionCommandId: "OnOrdersCollectionChanged"
				visible: false
				timeFilter: root.timeFilter ? root.timeFilter : root.defaultTimeFilter
			}
		}
	
		Row {
			id: row2
			anchors.top: row1.bottom
			anchors.topMargin: root.spacing
			anchors.horizontalCenter: parent.horizontalCenter
			height: getSoftwareCustomerPieChart.height
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
				width: root.chartWidth
				chartHeight: 700
				name: qsTr("Hardware Instances by Customer")
				gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareCustomerPieChart
				subscriptionCommandId: "OnDevicesCollectionChanged"
				legendClickable: true
				visible: false
				timeFilter: root.timeFilter ? root.timeFilter : root.defaultTimeFilter
				Component.onCompleted: {
					updateModel()
				}
				onLegendClicked: {
					let navigationParams = {}
					navigationParams.customerId = id
					navigationParams.inUse = undefined
					navigationParams.internalUse = false
					NavigationController.navigate("Devices/<hardware-filter>", navigationParams)
				}
			}
	
			GqlPiechartView {
				id: getSoftwareCustomerPieChart
				objectName: "SoftwareCustomerPieChart"
				width: root.chartWidth
				chartHeight: 700
				name: qsTr("Software Instances by Customer")
				gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getSoftwareCustomerPieChart
				subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
				legendClickable: true
				visible: false
				timeFilter: root.timeFilter ? root.timeFilter : root.defaultTimeFilter
				Component.onCompleted: {
					updateModel()
				}
				onLegendClicked: {
					let navigationParams = {}
					navigationParams.customerId = id
					navigationParams.inUse = undefined
					navigationParams.internalUse = false
					NavigationController.navigate("SoftwareProducts/<software-filter>", navigationParams)
				}
			}
	
			GqlPiechartView {
				id: getHardwareConfigurationPieChart
				objectName: "HardwareConfigurationPieChart"
				width: root.chartWidth
				chartHeight: 700
				name: qsTr("Hardware Configuration")
				gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareConfigurationPieChart
				subscriptionCommandId: "OnDevicesCollectionChanged"
				legendClickable: true
				visible: false
				timeFilter: root.timeFilter ? root.timeFilter : root.defaultTimeFilter
				Component.onCompleted: {
					updateModel()
				}
				onLegendClicked: {
					let navigationParams = {}
					navigationParams.licenseId = id
					navigationParams.productId = CachedProductCollection.getProductIdByLicenseId(id)
					navigationParams.internalUse = false
					NavigationController.navigate("Devices/<hardware-filter>", navigationParams)
				}
			}
		}
	}
}
