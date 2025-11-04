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

	property string gqlCommandId

	bottomComp: Component {
		Piechart {
			id: piechart
			ring: false
		}
	}

	function updateModel(){
		loading.start()
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
						obj.color = item.m_color
						segments.push(obj)
					}
					piechartElementView.bottomItem.segments = segments
					loading.stop()
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
