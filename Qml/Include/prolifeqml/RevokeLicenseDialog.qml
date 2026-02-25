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

	property string selectedChildLicenseId: ""
	property int selectedAvailableCount: 0
	property int selectedBoundCount: 0
	property int revokeCount: 1

	property BaseModel tableModel: null

	Component.onCompleted: {
		revokeLicenseDialog.fillButtons()
	}

	onLocalizationChanged: {
		revokeLicenseDialog.fillButtons()
	}

	onParentLicenseIdChanged: {
		loadChildLicenses()
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
				console.error(qsTr("Please select a license to revoke"))
				return;
			}

			if (revokeCount <= 0 || revokeCount > selectedAvailableCount){
				console.error(qsTr("Invalid revoke count. You can only revoke available (not bound) licenses."))
				
				return
			}

			// Send the revoke request
			revokeLicenseInput.m_childLicenseId = selectedChildLicenseId
			revokeLicenseInput.m_revokeCount = revokeCount

			revokeLicenseRequest.send(revokeLicenseInput)
		}
	}

	contentComp: Component {
		Item {
			width: revokeLicenseDialog.width
			height: revokeLicenseDialog.height - 100

			property BaseModel tableModel: revokeLicenseDialog.tableModel
			onTableModelChanged: {
				if (tableModel && tableElementView.table){
					tableElementView.table.isMultiSelect = false
					tableElementView.table.headers = tableHeaders
					tableElementView.table.elements = tableModel
				}
			}

			GroupElementView {
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.top: parent.top
				anchors.topMargin: Style.marginL
				width: parent.width - 2 * Style.marginL

				TableElementView {
					id: tableElementView
					name: qsTr("Child Licenses")

					TableHeaders {
						id: tableHeaders
						Component.onCompleted: {
							addHeader("softwareId", qsTr("Software-ID"))
							addHeader("accountName", qsTr("Account Name"))
							addHeader("productCount", qsTr("Total Count"))
							addHeader("availableCount", qsTr("Available Count"))
							addHeader("boundCount", qsTr("Bound Count"))
						}
					}

					Connections {
						target: tableElementView.table
						function onSelectionChanged(selectedIndexes){
							if (selectedIndexes.length !== 1){
								revokeLicenseDialog.selectedAvailableCount = 0
								revokeLicenseDialog.selectedBoundCount = 0
								revokeLicenseDialog.selectedChildLicenseId = ""
								revokeLicenseDialog.setButtonEnabled(Enums.ok, false)
								return
							}

							revokeLicenseDialog.selectedChildLicenseId = target.elements.get(selectedIndexes[0]).item.m_id
							revokeLicenseDialog.selectedAvailableCount = target.elements.get(selectedIndexes[0]).item.m_availableCount
							revokeLicenseDialog.selectedBoundCount = target.elements.get(selectedIndexes[0]).item.m_boundCount

							revokeLicenseDialog.setButtonEnabled(Enums.ok, revokeLicenseDialog.selectedAvailableCount > 0)
						}
					}
				}

				SpinBoxElementView {
					id: revokeCountSpinBox
					width: parent.width
					name: qsTr("Number of licenses to revoke")
					from: 1
					to: revokeLicenseDialog.selectedAvailableCount > 0 ? revokeLicenseDialog.selectedAvailableCount : 1
					startValue: 1
					controlWidth: 150
					description: qsTr("Available to revoke: ") + revokeLicenseDialog.selectedAvailableCount + 
					             (revokeLicenseDialog.selectedBoundCount > 0 ? qsTr(" (Bound: ") + revokeLicenseDialog.selectedBoundCount + ")" : "")
					readOnly: revokeLicenseDialog.selectedChildLicenseId === "" || revokeLicenseDialog.selectedAvailableCount === 0
					
					onValueChanged: {
						revokeLicenseDialog.revokeCount = value
					}
				}
			}
		}
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
						revokeLicenseDialog.tableModel = m_items
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
