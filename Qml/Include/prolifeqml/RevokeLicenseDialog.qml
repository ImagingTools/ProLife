import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import prolifeLicensesSdl 1.0

Dialog {
	id: revokeLicenseDialog

	title: qsTr("Revoke License")
	width: 900
	height: 600

	property string parentLicenseId: ""
	property string errorMessage: ""
	property string selectedChildLicenseId: ""
	property int selectedChildCount: 0
	property int revokeCount: 1

	Component.onCompleted: {
		revokeLicenseDialog.fillButtons()
		loadChildLicenses()
	}

	onLocalizationChanged: {
		revokeLicenseDialog.fillButtons()
	}

	function fillButtons(){
		clearButtons()
		addButton(Enums.ok, qsTr("Revoke"), false)
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}

	function loadChildLicenses() {
		childLicensesInput.m_parentLicenseId = revokeLicenseDialog.parentLicenseId
		childLicensesRequest.send(childLicensesInput)
	}

	onFinished: {
		if (buttonId === Enums.ok){
			// Validate selection
			if (selectedChildLicenseId === ""){
				revokeLicenseDialog.errorMessage = qsTr("Please select a license to revoke")
				return;
			}

			if (revokeCount <= 0 || revokeCount > selectedChildCount){
				revokeLicenseDialog.errorMessage = qsTr("Invalid revoke count")
				return;
			}

			// Send the revoke request
			revokeLicenseInput.m_childLicenseId = selectedChildLicenseId
			revokeLicenseInput.m_revokeCount = revokeCount

			revokeLicenseRequest.send(revokeLicenseInput);
		}
	}

	contentComp: Component {
		Item {
			width: revokeLicenseDialog.width
			height: revokeLicenseDialog.height - 100

			Column {
				anchors.fill: parent
				anchors.margins: Style.marginL
				spacing: Style.marginL

				// Error message display
				BaseText {
					id: errorText
					width: parent.width
					visible: revokeLicenseDialog.errorMessage !== ""
					text: revokeLicenseDialog.errorMessage
					color: Style.errorTextColor
					wrapMode: Text.WordWrap
				}

				BaseText {
					width: parent.width
					text: qsTr("Child licenses (received through Split):")
					font.bold: true
				}

				// Table with child licenses
				Rectangle {
					width: parent.width
					height: parent.height - 200
					border.color: Style.borderColor
					border.width: 1
					color: Style.backgroundColor

					ListView {
						id: childLicensesListView
						anchors.fill: parent
						anchors.margins: 1
						model: childLicensesModel
						clip: true
						
						header: Rectangle {
							width: parent.width
							height: 40
							color: Style.tableHeaderBackgroundColor
							
							Row {
								anchors.fill: parent
								
								BaseText {
									width: 200
									height: parent.height
									text: qsTr("Account")
									font.bold: true
									verticalAlignment: Text.AlignVCenter
									horizontalAlignment: Text.AlignHCenter
								}
								
								BaseText {
									width: 120
									height: parent.height
									text: qsTr("License Count")
									font.bold: true
									verticalAlignment: Text.AlignVCenter
									horizontalAlignment: Text.AlignHCenter
								}
								
								BaseText {
									width: 200
									height: parent.height
									text: qsTr("Hardware ID")
									font.bold: true
									verticalAlignment: Text.AlignVCenter
									horizontalAlignment: Text.AlignHCenter
								}
								
								BaseText {
									width: 80
									height: parent.height
									text: qsTr("Bound")
									font.bold: true
									verticalAlignment: Text.AlignVCenter
									horizontalAlignment: Text.AlignHCenter
								}
							}
						}
						
						delegate: Rectangle {
							width: parent ? parent.width : 0
							height: 40
							color: index % 2 === 0 ? Style.tableRowBackgroundColor1 : Style.tableRowBackgroundColor2
							border.color: childLicensesListView.currentIndex === index ? Style.selectionColor : "transparent"
							border.width: 2
							
							MouseArea {
								anchors.fill: parent
								onClicked: {
									childLicensesListView.currentIndex = index
									revokeLicenseDialog.selectedChildLicenseId = model.id
									revokeLicenseDialog.selectedChildCount = model.productCount
									
									if (model.isBound){
										revokeLicenseDialog.errorMessage = qsTr("This license is bound to hardware and cannot be revoked")
										revokeLicenseDialog.setButtonEnabled(Enums.ok, false)
									} else {
										revokeLicenseDialog.errorMessage = ""
										// Set revoke count to max available
										revokeLicenseDialog.revokeCount = revokeLicenseDialog.selectedChildCount
										revokeCountSpinBox.setValue(revokeLicenseDialog.selectedChildCount)
										revokeLicenseDialog.setButtonEnabled(Enums.ok, true)
									}
								}
							}
							
							Row {
								anchors.fill: parent
								
								BaseText {
									width: 200
									height: parent.height
									text: model.accountName || qsTr("Unknown")
									verticalAlignment: Text.AlignVCenter
									horizontalAlignment: Text.AlignLeft
									leftPadding: Style.marginM
								}
								
								BaseText {
									width: 120
									height: parent.height
									text: model.productCount || 0
									verticalAlignment: Text.AlignVCenter
									horizontalAlignment: Text.AlignHCenter
								}
								
								BaseText {
									width: 200
									height: parent.height
									text: model.hardwareId || ""
									verticalAlignment: Text.AlignVCenter
									horizontalAlignment: Text.AlignLeft
									leftPadding: Style.marginM
								}
								
								BaseText {
									width: 80
									height: parent.height
									text: model.isBound ? qsTr("Yes") : qsTr("No")
									verticalAlignment: Text.AlignVCenter
									horizontalAlignment: Text.AlignHCenter
									color: model.isBound ? Style.errorTextColor : Style.textColor
								}
							}
						}
					}
				}

				GroupElementView {
					width: parent.width

					SpinBoxElementView {
						id: revokeCountSpinBox
						width: parent.width
						name: qsTr("Number of licenses to revoke")
						from: 1
						to: revokeLicenseDialog.selectedChildCount > 0 ? revokeLicenseDialog.selectedChildCount : 1
						startValue: revokeLicenseDialog.selectedChildCount > 0 ? revokeLicenseDialog.selectedChildCount : 1
						controlWidth: 150
						description: qsTr("Max available: ") + revokeLicenseDialog.selectedChildCount
						enabled: revokeLicenseDialog.selectedChildCount > 0
						
						onValueChanged: {
							if (revokeLicenseDialog.selectedChildCount > 0) {
								revokeLicenseDialog.revokeCount = value
							}
						}
					}
				}
			}
		}
	}

	// Model for child licenses
	ListModel {
		id: childLicensesModel
	}

	// GraphQL request to get child licenses
	ChildLicensesListInput {
		id: childLicensesInput
	}

	GqlSdlRequestSender {
		id: childLicensesRequest
		requestType: 0; // Query
		gqlCommandId: ProlifeLicensesSdlCommandIds.s_childLicensesList;

		sdlObjectComp: Component {
			ChildLicensesListPayload {
				onFinished: {
					if (m_ok){
						// Populate the model
						childLicensesModel.clear()
						
						if (m_items){
							for (let i = 0; i < m_items.length; i++){
								let item = m_items[i]
								childLicensesModel.append({
									"id": item.m_id || "",
									"accountId": item.m_accountId || "",
									"accountName": item.m_accountName || qsTr("Unknown"),
									"productCount": item.m_productCount || 0,
									"hardwareId": item.m_hardwareId || "",
									"isBound": item.m_isBound || false
								})
							}
						}
					} else {
						revokeLicenseDialog.errorMessage = m_message || qsTr("Failed to load child licenses")
					}
				}
			}
		}
	}

	// GraphQL mutation to revoke license
	RevokeLicenseInput {
		id: revokeLicenseInput
	}

	GqlSdlRequestSender {
		id: revokeLicenseRequest
		requestType: 1; // Mutation
		gqlCommandId: ProlifeLicensesSdlCommandIds.s_revokeLicense;

		sdlObjectComp: Component {
			RevokeLicensePayload {
				onFinished: {
					if (!m_ok){
						revokeLicenseDialog.errorMessage = m_message || qsTr("Failed to revoke license")
					} else {
						revokeLicenseDialog.close()
					}
				}
			}
		}
	}

	Loading {
		id: loading
		anchors.fill: revokeLicenseDialog
		visible: childLicensesRequest.state === "Loading" || revokeLicenseRequest.state === "Loading"
		background.color: Style.backgroundColor2;
	}
}
