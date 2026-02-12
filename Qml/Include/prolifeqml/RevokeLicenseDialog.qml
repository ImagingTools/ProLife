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
				Item {
					width: parent.width
					height: parent.height - 150

					BaseTable {
						id: childLicensesTable
						anchors.fill: parent
						model: childLicensesModel
						selectionMode: Enums.singleSelection
						
						onCurrentIndexChanged: {
							if (currentIndex >= 0){
								revokeLicenseDialog.selectedChildLicenseId = model.getData("id", currentIndex)
								revokeLicenseDialog.selectedChildCount = model.getData("productCount", currentIndex)
								let isBound = model.getData("isBound", currentIndex)
								
								if (isBound){
									revokeLicenseDialog.errorMessage = qsTr("This license is bound to hardware and cannot be revoked")
									revokeLicenseDialog.setButtonEnabled(Enums.ok, false)
								} else {
									revokeLicenseDialog.errorMessage = ""
									// Set revoke count to max available
									revokeLicenseDialog.revokeCount = revokeLicenseDialog.selectedChildCount
									revokeCountSpinBox.setValue(revokeLicenseDialog.selectedChildCount)
									revokeLicenseDialog.setButtonEnabled(Enums.ok, true)
								}
							} else {
								revokeLicenseDialog.setButtonEnabled(Enums.ok, false)
							}
						}

						columns: [
							{
								"id": "accountName",
								"name": qsTr("Account"),
								"width": 200
							},
							{
								"id": "productCount",
								"name": qsTr("License Count"),
								"width": 120
							},
							{
								"id": "hardwareId",
								"name": qsTr("Hardware ID"),
								"width": 200
							},
							{
								"id": "isBound",
								"name": qsTr("Bound"),
								"width": 80,
								"decorator": function(value) {
									return value ? qsTr("Yes") : qsTr("No")
								}
							}
						]
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
						startValue: revokeLicenseDialog.selectedChildCount
						controlWidth: 150
						description: qsTr("Max available: ") + revokeLicenseDialog.selectedChildCount
						
						onValueChanged: {
							revokeLicenseDialog.revokeCount = value
						}
					}
				}
			}
		}
	}

	// Model for child licenses
	BaseListModel {
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
								let index = childLicensesModel.insertNewItem()
								
								childLicensesModel.setData("id", item.m_id, index)
								childLicensesModel.setData("accountId", item.m_accountId || "", index)
								childLicensesModel.setData("accountName", item.m_accountName || qsTr("Unknown"), index)
								childLicensesModel.setData("productCount", item.m_productCount || 0, index)
								childLicensesModel.setData("hardwareId", item.m_hardwareId || "", index)
								childLicensesModel.setData("isBound", item.m_isBound || false, index)
							}
						}
						
						childLicensesModel.refresh()
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
