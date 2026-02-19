// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtgui 1.0
import prolifeWorkspaceSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

ElementView {
	id: piechartElementView
	width: Style.sizeHintM
	contentSpacing: Style.marginM

	property string gqlCommandId
	property alias subscriptionCommandId: subscriptionClient.gqlCommandId

	clip: true
	property bool legendClickable: false

	property real chartHeight: Style.sizeHintS
	property string customerId: ""
	property TimeFilter timeFilter: TimeFilter {
		m_timeUnit: "Week"
		m_interpretationMode: "For"
	}

	signal legendClicked(string id, string  label, string color, int value)

	bottomComp: Component {
		Piechart {
			id: piechart
			ring: false
			height: piechartElementView.chartHeight
			legendClickable: piechartElementView.legendClickable
			onLegendClicked: {
				piechartElementView.legendClicked(id, label, color, value)
			}
		}
	}

	function updateModel(){
		if (!visible){
			internal.updateRequested = true
			return
		}

		loading.start()
		piechartInfoRequest.send()
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
			piechartElementView.updateModel()
		}
	}

	QtObject {
		id: internal
		property bool updateRequested: false
	}

	GqlSdlRequestSender {
		id: piechartInfoRequest
		gqlCommandId: piechartElementView.gqlCommandId
		inputObjectComp: Component {
			ChartInput {
				m_customerId: piechartElementView.customerId
				m_timeFilter: piechartElementView.timeFilter
			}
		}
		sdlObjectComp: Component {
			PieChartData {
				onFinished: {
					let segments = []
					for (let i = 0; i < m_segments.count; ++i){
						let obj = {}
						let item = m_segments.get(i).item
						obj.id = item.m_id
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
		background.color: Style.baseColor
		visible: false
	}
}
