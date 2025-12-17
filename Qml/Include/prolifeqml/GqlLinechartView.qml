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
	id: graph2dElementView
	controlWidth: 130
	width: Style.sizeHintL
	contentSpacing: Style.marginM
	clip: true

	property string gqlCommandId
	property alias subscriptionCommandId: subscriptionClient.gqlCommandId

	property real chartHeight: Style.sizeHintS
	property string customerId: ""
	property TimeFilter timeFilter: TimeFilter {
		m_timeUnit: "Week"
		m_interpretationMode: "For"
	}

	function updateModel(){
		if (!visible){
			internal.updateRequested = true
			return
		}

		loading.start()
		licenseCreationInfoRequest.send()
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
			graph2dElementView.updateModel()
		}
	}

	QtObject {
		id: internal
		property bool updateRequested: false
	}

	bottomComp: Component {
		Graph2d {
			height: graph2dElementView.chartHeight
			hasData: true
			gridStepMajorX: 1
			gridStepMajorY: 1
			alwaysShowOrigin: true
			xScale: 2
			hasMinorGrid: false 
			hasTooltip: false
			fitToWidth: true
			legendFontSize: Style.fontSizeS
			labelFontSize: Style.fontSizeS
		}
	}

	Loading {
		id: loading
		z: parent.z + 1
		anchors.fill: parent
		background.color: Style.baseColor
		visible: false
	}

	GqlSdlRequestSender {
		id: licenseCreationInfoRequest
		gqlCommandId: graph2dElementView.gqlCommandId
		inputObjectComp: Component {
			ChartInput {
				m_timeFilter: graph2dElementView.timeFilter
				m_customerId: graph2dElementView.customerId
			}
		}
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
							let niceStep = graph2dElementView.niceStep(maxValue)
							if (niceStep <= 0){
								niceStep = 1
							}

							graph2dElementView.bottomItem.gridStepMajorY = niceStep
						}
					}
					graph2dElementView.bottomItem.requestPaint()
					loading.stop()
				}
			}
		}
	}
}
