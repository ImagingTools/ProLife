import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtgui 1.0
import prolifeWorkspaceSdl 1.0
import com.imtcore.imtqml 1.0
import imtbaseComplexCollectionFilterSdl 1.0

ElementView {
	id: barChartElementView
	controlWidth: 130
	width: Style.sizeHintM
	contentSpacing: Style.marginM
	clip: true

	property int ySteps: 1

	property string gqlCommandId
	property alias subscriptionCommandId: subscriptionClient.gqlCommandId
	property string customerId: ""

	property real chartHeight: Style.sizeHintS
	
	property TimeFilter timeFilter: null

	property bool legendClickable: false
	signal legendClicked(string id, string  label, string color, int value)

	function updateModel(){
		if (!visible){
			internal.updateRequested = true
			return
		}

		loading.start()
		barChartRequest.send()
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

	onCustomerIdChanged: {
		updateModel()
	}

	onTimeFilterChanged: {
		updateModel()
	}

	onVisibleChanged: {
		requestUpdateModel()
	}

	function requestUpdateModel(){
		if (visible && internal.updateRequested){
			updateModel()
			internal.updateRequested = false
		}
	}

	SubscriptionClient {
		id: subscriptionClient
		onMessageReceived: {
			barChartElementView.updateModel()
		}
	}

	QtObject {
		id: internal
		property bool updateRequested: false
	}

	bottomComp: Component {
		StackedBarChart {
			id: stackedBarChart
			height: barChartElementView.chartHeight
			ySteps: barChartElementView.ySteps
			legendClickable: barChartElementView.legendClickable
			onLegendClicked: {
				barChartElementView.legendClicked(id, label, color, value)
			}

			function createFromObject(barChart){
				let bars = []
				for (let i = 0; i < barChart.m_bars.count; ++i){
					let barItem = barChart.m_bars.get(i).item
					let barObj = {}
					
					let segments = []
					for (let j = 0; j < barItem.m_segments.count; ++j){
						let segmentItem = barItem.m_segments.get(j).item
						let segmentObj = {}
						segmentObj.label = segmentItem.m_label
						segmentObj.value = segmentItem.m_value
						segmentObj.color = segmentItem.m_color
						segments.push(segmentObj)
					}

					barObj.segments = segments
					barObj.label = barItem.m_label
					
					bars.push(barObj)
				}

				if (barChart.m_summary){
					let maxItem = barChart.m_summary.m_maxItem
					if (maxItem){
						let maxValue = maxItem.m_value
						let niceStep = barChartElementView.niceStep(maxValue)
						if (niceStep <= 0){
							niceStep = 1
						}

						stackedBarChart.ySteps = niceStep
					}
				}

				stackedBarChart.xLabel = barChart.m_axes.m_xLabel
				stackedBarChart.yLabel = barChart.m_axes.m_yLabel
				stackedBarChart.bars = bars
				loading.stop()
			}
		}
	}

	GqlSdlRequestSender {
		id: barChartRequest
		gqlCommandId: barChartElementView.gqlCommandId
		inputObjectComp: Component {
			ChartInput {
				m_timeFilter: barChartElementView.timeFilter
				m_customerId: barChartElementView.customerId
			}
		}

		sdlObjectComp: Component {
			BarChartData {
				onFinished: {
					barChartElementView.bottomItem.createFromObject(this)
				}
			}
		}
	}

	Loading {
		id: loading
		z: parent.z + 1
		anchors.fill: parent
		background.color: Style.baseColor
		visible: false
	}
}
