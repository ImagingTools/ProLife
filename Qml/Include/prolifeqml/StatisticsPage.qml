import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtlicgui 1.0
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

	Row {
		anchors.top: parent.top
		anchors.topMargin: Style.marginL
		anchors.horizontalCenter: parent.horizontalCenter
		height: parent.height
		spacing: Style.spacingL

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
			chartHeight: 500
			name: qsTr("Hardware Instances by Customer")
			gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareCustomerPieChart
			subscriptionCommandId: "OnDevicesCollectionChanged"
			legendClickable: true
			visible: false
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
			chartHeight: 500
			name: qsTr("Software Instances by Customer")
			gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getSoftwareCustomerPieChart
			subscriptionCommandId: "OnSoftwareProductsCollectionChanged"
			legendClickable: true
			visible: false
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
			chartHeight: 500
			name: qsTr("Hardware Configuration")
			gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareConfigurationPieChart
			subscriptionCommandId: "OnDevicesCollectionChanged"
			legendClickable: true
			visible: false
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
