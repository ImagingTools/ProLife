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

	property string gqlCommandId

	function updateModel(){
		licenseCreationInfoRequest.send(timeFilter)
	}

	model: TreeItemModel {
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
		Graph2d {
			width: Style.sizeHintL
			height: Style.sizeHintM
			hasData: true
			gridStepMajorX: 1
			gridStepMajorY: 1
			alwaysShowOrigin: true
			xScale: 2
			hasMinorGrid: false 
			hasTooltip: false
			fitToWidth: true
		}
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
					graph2dElementView.bottomItem.requestPaint()
				}
			}
		}
	}
}
