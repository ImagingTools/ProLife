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
	property int selectedChildCount: 1
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

			if (revokeCount <= 0 || revokeCount > selectedChildCount){
				console.error(qsTr("Invalid revoke count"))
				
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
							addHeader("productCount", qsTr("Product Count"))
							addHeader("isBound", qsTr("Is Bound"))
						}
					}

					Connections {
						target: tableElementView.table
						function onSelectionChanged(selectedIndexes){
							revokeLicenseDialog.setButtonEnabled(Enums.ok, selectedIndexes.length === 1)

							if (selectedIndexes.length !== 1){
								revokeLicenseDialog.selectedChildCount = 1
								revokeLicenseDialog.selectedChildLicenseId = ""
								return
							}

							revokeLicenseDialog.selectedChildLicenseId = target.elements.get(selectedIndexes[0]).item.m_id
							revokeLicenseDialog.selectedChildCount = target.elements.get(selectedIndexes[0]).item.m_productCount
						}
						function onHeadersChanged(){
							target.setColumnContentById("isBound", isBoundColumnDelegateComp)
						}
					}
					
					Component {
						id: isBoundColumnDelegateComp;
						TableCellDelegateBase {
							id: cellDelegate

							Image {
								id: image;
								anchors.verticalCenter: parent.verticalCenter;
								anchors.left: parent.left;
								anchors.leftMargin: Style.marginM;
								width: Style.iconSizeM;
								height: width;
								source: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal);
								sourceSize.width: width;
								sourceSize.height: height;
							}

							onReused: {
								if (!rowDelegate){
									return
								}

								if (rowIndex >= 0){
									let isBound = cellDelegate.getValue();
									image.visible = isBound;
								}
							}
						}
					}
				}

				SpinBoxElementView {
					id: revokeCountSpinBox
					width: parent.width
					name: qsTr("Number of licenses to revoke")
					from: 1
					to: revokeLicenseDialog.selectedChildCount
					startValue: 1
					controlWidth: 150
					description: qsTr("Max available: ") + revokeLicenseDialog.selectedChildCount
					readOnly: revokeLicenseDialog.selectedChildLicenseId === ""
					
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
