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
			anchors.fill: root
			boundsBehavior: Flickable.StopAtBounds;
	
			contentWidth: topFlow.width
			contentHeight: 2000//topFlow.height + flow.height
			clip: true;

			Component.onCompleted: {
				softwareProductStatsRequest.send()
				hardwareProductStatsRequest.send()
				licenseCreationInfoRequest.send()
				hardwareStatusInfoRequest.send()
				getTotalSummaryInfoRequest.send()
			}
	
			function createSegments(productStats){
				let segments = []
				for (let i = 0; i < productStats.count; ++i){
					let obj = {}
					let item = productStats.get(i).item
					obj.value = item.m_value
					obj.label = item.m_label
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
					PieChartData {
						onFinished: {
							let segments = flickable.createSegments(m_segments)
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
					PieChartData {
						onFinished: {
							let segments = flickable.createSegments(m_segments)
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

			GqlSdlRequestSender {
				id: hardwareStatusInfoRequest
				gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getHardwareStatusInfo
				sdlObjectComp: Component {
					PieChartData {
						onFinished: {
							let segments = flickable.createSegments(m_segments)
							hardwareStatusPiechartElementView.bottomItem.segments = segments
						}
					}
				}
			}

			GqlSdlRequestSender {
				id: getTotalSummaryInfoRequest
				gqlCommandId: ProlifeWorkspaceSdlCommandIds.s_getTotalSummaryInfo
				sdlObjectComp: Component {
					TotalSummaryInfo {
						onFinished: {
							collectionInfoRepeater.model = m_summaryInfos
						}
					}
				}
			}

			Flow {
				id: topFlow
				anchors.top: root.top
				anchors.left: root.left
				anchors.right: root.right
				anchors.margins: Style.marginXL
				spacing: Style.marginXL

				onHeightChanged: {
					console.log("topFlow onHeightChanged", height)
				}

				Repeater {
					id: collectionInfoRepeater
					delegate: Component {
						ElementView {
							width: Style.sizeHintM
							name: model.item.m_total
							titleFontSize: Style.fontSizeBXL
							controlComp: Component {
								Row {
									height: Style.controlHeightM
									spacing: Style.marginM
									Button {
										width: Style.sizeHintBXS
										height: Style.controlHeightM
										text: qsTr("Create New")
										onClicked: {
											let params = {}
											params.createNew = true
											let objectTypeId = model.item.m_objectTypeId
											let collectionId = model.item.m_collectionId
											NavigationController.navigate(collectionId + "/" + objectTypeId, params)
										}
									}

									Button {
										width: Style.sizeHintBXS
										height: Style.controlHeightM
										text: qsTr("View All")
										onClicked: {
											NavigationController.navigate(model.item.m_collectionId)
										}
									}
								}
							}

							StickerView {
								anchors.verticalCenter: parent.top
								anchors.left: parent.left
								anchors.leftMargin: Style.marginM
								color: Style.iconColorOnSelected
								text: model.item.m_title
							}
						}
					}
				}
			}

			Flow {
				id: flow
				anchors.top: topFlow.bottom
				anchors.bottom: root.bottom
				anchors.left: root.left
				anchors.right: root.right
				anchors.margins: Style.marginXL
				spacing: Style.marginXL

				onHeightChanged: {
					console.log("flow onHeightChanged", height)
				}
				
				ComboBoxElementView {
					id: graph2dElementView
					width: Style.sizeHintL
					name: qsTr("License Creation")
					currentIndex: 0
					controlWidth: 130
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

						licenseCreationInfoRequest.send(timeFilter)
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
						}
					}
				}

				ElementView {
					id: piechartElementView
					width: Style.sizeHintM
					name: qsTr("Software Used")

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
					name: qsTr("Hardware Used")
					bottomComp: Component {
						Piechart {
							id: piechart
							ring: false
						}
					}
				}
				
				ElementView {
					id: hardwareStatusPiechartElementView
					width: Style.sizeHintM
					name: qsTr("Hardware Status")
			
					bottomComp: Component {
						Piechart {
							id: piechart
							ring: false
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
