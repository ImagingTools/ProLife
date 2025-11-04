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
	id: barChartElementView
	controlWidth: 130

	property int ySteps: 1

	property string gqlCommandId

	function updateModel(){
		loading.start()
		barChartRequest.send(timeFilter)
	}

	model:	TreeItemModel {
		id: dateModel
		Component.onCompleted: {
			let index = insertNewItem()
			setData("id", "Week", index)
			setData("name", qsTr("Last Week"), index)
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
		StackedBarChart {
			id: stackedBarChart
			height: Style.sizeHintM
			ySteps: barChartElementView.ySteps

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

				stackedBarChart.xLabel = barChart.m_axes.m_xLabel
				stackedBarChart.yLabel = barChart.m_axes.m_yLabel
				stackedBarChart.bars = bars
				loading.stop()
			}
		}
	}

	TimeFilter {
		id: timeFilter
		m_timeUnit: "Week"
		m_interpretationMode: "For"
	}

	GqlSdlRequestSender {
		id: barChartRequest
		gqlCommandId: barChartElementView.gqlCommandId
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
		color: Style.baseColor
		visible: false
	}
}
