import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtgui 1.0
import prolifeWorkspaceSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0
import com.imtcore.imtqml 1.0

ViewBase {
	id: root
	anchors.fill: parent
	commandsControllerComp: Component {
		GqlBasedCommandsController {
			typeId: "Workspace"
		}
	}

	property var palette: [
		"#4CAF50", "#FFC107", "#2196F3", "#F44336",
		"#9C27B0", "#00BCD4", "#8BC34A", "#FF9800"
	]

	Component.onCompleted: {
		stackView.addPage(workspacePageComp)
		stackView.addPage(userActionsPageComp)
		stackView.setCurrentIndex(0)
	
		if (commandsController){
			commandsController.setIsToggleable("Workspace", true)
			commandsController.setToggled("Workspace", true)
			commandsController.setIsToggleable("UserActions", true)
			commandsController.setToggled("UserActions", false)
		}
	}

	onCommandActivated: {
		if (commandId === "Workspace"){
			stackView.setCurrentIndex(0)
		}
		else if (commandId === "UserActions"){
			stackView.setCurrentIndex(1)
		}

		commandsController.setToggled("Workspace", commandId === "Workspace")
		commandsController.setToggled("UserActions", commandId === "UserActions")
	}

	Component {
		id: workspacePageComp
		Flickable {
			id: flickable
			anchors.fill: parent
			boundsBehavior: Flickable.StopAtBounds;
	
			contentWidth: flow.width
			contentHeight: flow.height

			Component.onCompleted: {
				softwareProductStatsRequest.send()
				hardwareProductStatsRequest.send()
				licenseCreationInfoRequest.send()
			}
	
			function createSegments(productStats){
				let segments = []
				for (let i = 0; i < productStats.count; ++i){
					let obj = {}
					let item = productStats.get(i).item
					obj.value = item.m_totalLicenses
					obj.label = item.m_productName
					obj.color = root.palette[i % root.palette.length]
					segments.push(obj)
				}
		
				return segments
			}
		
			GqlSdlRequestSender {
				id: softwareProductStatsRequest
				gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getLicenseProductStats
				inputObjectComp: Component {
					LicenseProductInfo {
						m_productInfo: "Software"
					}
				}
				sdlObjectComp: Component {
					LicenseProductStats {
						onFinished: {
							let segments = flickable.createSegments(m_productStats)
							piechartElementView.bottomItem.segments = segments
						}
					}
				}
			}
		
			GqlSdlRequestSender {
				id: hardwareProductStatsRequest
				gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getLicenseProductStats
				inputObjectComp: Component {
					LicenseProductInfo {
						m_productInfo: "Hardware"
					}
				}
		
				sdlObjectComp: Component {
					LicenseProductStats {
						onFinished: {
							let segments = flickable.createSegments(m_productStats)
							hardwarePiechartElementView.bottomItem.segments = segments
						}
					}
				}
			}
		
			TimeFilter {
				id: timeFilter
				m_interpretationMode: "For"
			}
		
			GqlSdlRequestSender {
				id: licenseCreationInfoRequest
				gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getLicenseCreationInfo
				inputObjectComp: Component {
					TimeFilter {
						m_timeUnit: "Week"
						m_interpretationMode: "For"
					}
				}
		
				sdlObjectComp: Component {
					LicenseCreationInfo {
						onFinished: {
							graph2dElementView.bottomItem.legendX = m_chart.m_axes.m_xLabel
							graph2dElementView.bottomItem.legendY = m_chart.m_axes.m_yLabel
							graph2dElementView.bottomItem.title = ""
							
							let linePoints = []
							for (let i = 0; i < m_chart.m_points.count; ++i){
								let point = m_chart.m_points.get(i).item
								linePoints.push(Qt.point(point.m_x, point.m_y))
							}
		
							graph2dElementView.bottomItem.linePoints = linePoints
							graph2dElementView.bottomItem.requestPaint()
						}
					}
				}
			}

			Flow {
				id: flow
				anchors.fill: root
				anchors.margins: Style.marginXL
				spacing: Style.marginXL
		
				ElementView {
					id: piechartElementView
					width: Style.sizeHintM
					name: qsTr("Number of software product used")
			
					bottomComp: Component {
						Piechart {
							id: piechart
							ring: false
							showLegend: true
						}
					}
				}
			
				ElementView {
					id: hardwarePiechartElementView
					width: Style.sizeHintM
					name: qsTr("Number of hardware product used")
			
					bottomComp: Component {
						Piechart {
							id: piechart
							ring: false
							showLegend: true
						}
					}
				}
			
				ComboBoxElementView {
					id: graph2dElementView
					width: Style.sizeHintL
					name: qsTr("Number of license creation")
					currentIndex: 0
					controlWidth: 130
					model: TreeItemModel {
						Component.onCompleted: {
							let index = insertNewItem()
							setData("id", "week", index)
							setData("name", qsTr("Last 7 days"), index)
							
							index = insertNewItem()
							setData("id", "month", index)
							setData("name", qsTr("Last 30 days"), index)
						}
					}
	
					onCurrentIndexChanged: {
						if (currentIndex < 0){
							return
						}
	
						if (currentIndex === 0){
							timeFilter.m_timeUnit = "Week"
						}
						else if (currentIndex === 1){
							timeFilter.m_timeUnit = "Month"
						}
	
						licenseCreationInfoRequest.send(timeFilter)
					}
			
					bottomComp: Component {
						Graph2d {
							height: Style.sizeHintS
							hasData: true
						}
					}
				}
			}
		}
	}

	Component {
		id: userActionsPageComp
		UserActionCollectionView {
			height: 500
		}
	}

	StackView {
		id: stackView
		anchors.fill: parent
	}
}
