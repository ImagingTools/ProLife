import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtgui 1.0
import prolifeWorkspaceSdl 1.0
import com.imtcore.imtqml 1.0
import imtbaseComplexCollectionFilterSdl 1.0

ComboBoxElementView {
	id: graph2dElementView
	name: qsTr("License Creation")
	controlWidth: 130
	width: Style.sizeHintL

	clip: true

	property string gqlCommandId
	property alias subscriptionCommandId: subscriptionClient.gqlCommandId

	property real chartHeight: Style.sizeHintS

	function updateModel(){
		if (!visible){
			internal.updateRequested = true
			return
		}

		loading.start()
		licenseCreationInfoRequest.send(timeFilter)
	}

	function niceStep(maxValue){
		if (maxValue <= 0)
			return 1
	
		let rough = maxValue / 5
		let magnitude = Math.pow(10, Math.floor(Math.log10(rough)))
		let normalized = rough / magnitude
	
		let nice
		if (normalized <= 1)      nice = 1
		else if (normalized <= 2) nice = 2
		else if (normalized <= 5) nice = 5
		else                      nice = 10
	
		let result = nice * magnitude
		return Math.round(result)
	}

	onVisibleChanged: {
		if (visible && internal.updateRequested){
			updateModel()
			internal.updateRequested = false
		}
	}

	SubscriptionClient {
		id: subscriptionClient
		onMessageReceived: {
			graph2dElementView.updateModel()
		}
	}

	QtObject {
		id: internal
		property bool updateRequested: false
	}

	model: TreeItemModel {
		Component.onCompleted: {
			let index = insertNewItem()
			setData("id", "Week", index)
			setData("name", qsTr("Last 7 Days"), index)
			setData("mode", "For", index)

			index = insertNewItem()
			setData("id", "Month", index)
			setData("name", qsTr("This Month"), index)
			setData("mode", "Current", index)

			index = insertNewItem()
			setData("id", "Month", index)
			setData("name", qsTr("Last Month"), index)
			setData("mode", "Last", index)

			index = insertNewItem()
			setData("id", "Year", index)
			setData("name", qsTr("This Year"), index)
			setData("mode", "Current", index)

			index = insertNewItem()
			setData("id", "Year", index)
			setData("name", qsTr("Last Year"), index)
			setData("mode", "Last", index)
		}
	}

	onCurrentIndexChanged: {
		if (currentIndex < 0){
			return
		}

		let unit = model.getData("id", currentIndex)
		let mode = model.getData("mode", currentIndex)

		timeFilter.m_timeUnit = unit
		timeFilter.m_interpretationMode = mode

		updateModel()
	}

	bottomComp: Component {
		Graph2d {
			height: graph2dElementView.chartHeight
			hasData: true
			gridStepMajorX: 1
			// gridStepMajorY: 3
			alwaysShowOrigin: true
			xScale: 2
			hasMinorGrid: false 
			hasTooltip: false
			fitToWidth: true
		}
	}

	Loading {
		id: loading
		z: parent.z + 1
		anchors.fill: parent
		color: Style.baseColor
		visible: false
	}

	TimeFilter {
		id: timeFilter
		m_timeUnit: "Week"
		m_interpretationMode: "For"
	}

	GqlSdlRequestSender {
		id: licenseCreationInfoRequest
		gqlCommandId: graph2dElementView.gqlCommandId
		sdlObjectComp: Component {
			LineChartData {
				onFinished: {
					graph2dElementView.bottomItem.legendX = m_axes.m_xLabel
					graph2dElementView.bottomItem.legendY = m_axes.m_yLabel
					graph2dElementView.bottomItem.title = ""
					
					let linePoints = []
					for (let i = 0; i < m_points.count; ++i){
						let point = m_points.get(i).item
						linePoints.push(Qt.point(point.m_x, point.m_y))
					}

					graph2dElementView.bottomItem.labelXValues = m_labels
					graph2dElementView.bottomItem.linePoints = linePoints
					
					if (m_summary){
						let maxItem = m_summary.m_maxItem
						if (maxItem){
							let maxValue = maxItem.m_value
							graph2dElementView.bottomItem.gridStepMajorY = graph2dElementView.niceStep(maxValue)
						}
					}
					graph2dElementView.bottomItem.requestPaint()
					loading.stop()
				}
			}
		}
	}
}
