import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import prolifeLicensesSdl 1.0
import prolifeAccountsSdl 1.0

Dialog {
	id: splitLicenseDialog

	title: qsTr("Split License")
	width: 800
	height: 500

	property string licenseId: ""
	property int maxAvailableCount: 0
	property int currentCount: 1
	property string errorMessage: ""
	property var childLicenses: []

	Component.onCompleted: {
		splitLicenseDialog.fillButtons()
		loadChildLicenses()
	}

	onLocalizationChanged: {
		splitLicenseDialog.fillButtons()
	}

	function fillButtons(){
		clearButtons()
		addButton(Enums.ok, qsTr("Save"), false)
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}

	property real spinBoxValue: 1
	property string selectedAccountId: ""
	property bool createNewMode: true
	property string selectedTargetLicenseId: ""

	function loadChildLicenses() {
		childLicensesInput.m_licenseId = splitLicenseDialog.licenseId
		childLicensesRequest.send(childLicensesInput)
	}

	onFinished: {
		if (buttonId === Enums.ok){
			// Validate inputs
			if (spinBoxValue <= 0 || spinBoxValue >= splitLicenseDialog.maxAvailableCount){
				return;
			}

			// Send the split request
			splitLicenseInput.m_licenseId = splitLicenseDialog.licenseId
			splitLicenseInput.m_licenseCount = spinBoxValue
			splitLicenseInput.m_accountId = selectedAccountId

			// Set targetLicenseId only if in transfer mode
			if (!createNewMode && selectedTargetLicenseId !== "") {
				splitLicenseInput.m_targetLicenseId = selectedTargetLicenseId
			}

			splitLicenseRequest.send(splitLicenseInput);
		}
	}

	contentComp: Component {
		Item {
			width: splitLicenseDialog.width
			height: splitLicenseDialog.height - 100

			Column {
				anchors.fill: parent
				anchors.margins: Style.marginL
				spacing: Style.marginL

				// Error message display
				BaseText {
					id: errorText
					width: parent.width
					visible: splitLicenseDialog.errorMessage !== ""
					text: splitLicenseDialog.errorMessage
					color: Style.errorTextColor
					wrapMode: Text.WordWrap
				}

				GroupElementView {
					width: parent.width

					// Mode selection
					RadioButtonsRowElementView {
						id: modeSelector
						width: parent.width
						name: qsTr("Split mode")
						labels: [qsTr("Create new license"), qsTr("Transfer to existing child")]
						controlWidth: 400

						onValueChanged: {
							splitLicenseDialog.createNewMode = (value === 0)
							updateButtonState()
						}

						Component.onCompleted: {
							value = 0  // Default to "Create new" mode
						}
					}

					SpinBoxElementView {
						id: licenseCountSpinBox
						width: parent.width
						name: qsTr("License count to split")
						from: startValue
						to: splitLicenseDialog.maxAvailableCount
						startValue: 1
						controlWidth: 150
						description: qsTr("Max available count: ") + splitLicenseDialog.maxAvailableCount
						onValueChanged: {
							splitLicenseDialog.spinBoxValue = value
							updateButtonState()
						}
					}

					ComboBoxElementView {
						id: accountComboBox
						width: parent.width
						name: qsTr("Target account")
						nameId: "name"
						model: CachedAccountCollection.collectionModel
						controlWidth: 250
						visible: splitLicenseDialog.createNewMode

						onCurrentIndexChanged: {
							if (currentIndex >= 0 && model){
								splitLicenseDialog.selectedAccountId = model.getData("id", currentIndex)
							}
							updateButtonState()
						}

						bottomComp: currentIndex >= 0 ? undefined : accountErrorComp

						Component {
							id: accountErrorComp
							BaseText {
								color: Style.errorTextColor;
								text: qsTr("Please select an account");
							}
						}
					}

					ComboBoxElementView {
						id: targetLicenseComboBox
						width: parent.width
						name: qsTr("Target child license")
						nameId: "serialNumber"
						model: childLicensesListModel
						controlWidth: 250
						visible: !splitLicenseDialog.createNewMode

						onCurrentIndexChanged: {
							if (currentIndex >= 0 && model){
								splitLicenseDialog.selectedTargetLicenseId = model.get(currentIndex).id
								// Auto-select the account of the target license
								splitLicenseDialog.selectedAccountId = model.get(currentIndex).accountId
							}
							updateButtonState()
						}

						bottomComp: currentIndex >= 0 ? undefined : targetErrorComp

						Component {
							id: targetErrorComp
							BaseText {
								color: Style.errorTextColor;
								text: qsTr("Please select a target license");
							}
						}
					}
				}

				function updateButtonState() {
					var isValid = licenseCountSpinBox.value > 0 && licenseCountSpinBox.value < splitLicenseDialog.maxAvailableCount;
					if (splitLicenseDialog.createNewMode) {
						isValid = isValid && accountComboBox.currentIndex >= 0;
					} else {
						isValid = isValid && targetLicenseComboBox.currentIndex >= 0;
					}
					splitLicenseDialog.setButtonEnabled(Enums.ok, isValid);
				}
			}
		}
	}

	SplitLicenseInput {
		id: splitLicenseInput;
	}

	GqlSdlRequestSender {
		id: splitLicenseRequest
		requestType: 1;
		gqlCommandId: ProlifeLicensesSdlCommandIds.s_splitLicense;

		sdlObjectComp: Component {
			SplitLicensePayload {
				onFinished: {
				}
			}
		}
	}

	// Child licenses request
	ChildLicensesListInput {
		id: childLicensesInput
	}

	GqlSdlRequestSender {
		id: childLicensesRequest
		requestType: 1
		gqlCommandId: ProlifeLicensesSdlCommandIds.s_childLicensesList

		sdlObjectComp: Component {
			ChildLicensesListPayload {
				onFinished: {
					if (m_ok && m_licenses) {
						childLicensesListModel.clear()
						for (var i = 0; i < m_licenses.length; i++) {
							var license = m_licenses[i]
							childLicensesListModel.append({
								id: license.m_id,
								serialNumber: license.m_serialNumber,
								accountId: license.m_accountId,
								accountName: license.m_accountName
							})
						}
					}
				}
			}
		}
	}

	ListModel {
		id: childLicensesListModel
	}

	Loading {
		id: loading
		anchors.fill: splitLicenseDialog
		visible: splitLicenseRequest.state === "Loading" || childLicensesRequest.state === "Loading"
		background.color: Style.backgroundColor2;
	}
}
