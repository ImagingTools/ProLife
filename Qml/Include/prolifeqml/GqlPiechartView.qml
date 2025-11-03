import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtgui 1.0
import prolifeWorkspaceSdl 1.0

ElementView {
	id: piechartElementView
	width: Style.sizeHintM

	Component.onCompleted: {
		updateModel()
	}

	property string gqlCommandId

	property var palette: [
		"#4CAF50", // Green
		"#FFC107", // Amber
		"#2196F3", // Blue
		"#F44336", // Red
		"#9C27B0", // Purple
		"#00BCD4", // Cyan
		"#8BC34A", // Light Green
		"#FF9800", // Orange
		"#E91E63", // Pink
		"#607D8B"  // Blue Grey
	]

	bottomComp: Component {
		Piechart {
			id: piechart
			ring: false
		}
	}

	function updateModel(){
		piechartInfoRequest.send()
	}

	GqlSdlRequestSender {
		id: piechartInfoRequest
		gqlCommandId: piechartElementView.gqlCommandId
		sdlObjectComp: Component {
			PieChartData {
				onFinished: {
					let segments = []
					for (let i = 0; i < m_segments.count; ++i){
						let obj = {}
						let item = m_segments.get(i).item
						obj.value = item.m_value
						obj.label = item.m_label
						obj.color = item.m_color || piechartElementView.palette[i % piechartElementView.palette.length]
						segments.push(obj)
					}
					piechartElementView.bottomItem.segments = segments
				}
			}
		}
	}
}
