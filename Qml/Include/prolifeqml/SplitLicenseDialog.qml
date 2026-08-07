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
	backgroundColor: Style.baseColor
	width: 800
	height: 500

	property string licenseId: ""
	property int maxAvailableCount: 0
	property string errorMessage: ""

	property BaseModel itemsModel: null

	Component.onCompleted: {
		splitLicenseDialog.fillButtons()
	}

	onLocalizationChanged: {
		splitLicenseDialog.fillButtons()
	}

	onLicenseIdChanged: {
		if (licenseId !== ""){
			loadChildLicenses()
		}
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
		childLicensesInput.m_parentLicenseId = splitLicenseDialog.licenseId
		childLicensesRequest.send(childLicensesInput)
	}

	onFinished: {
		if (buttonId === Enums.ok){
			// Validate inputs
			if (spinBoxValue <= 0 || spinBoxValue > splitLicenseDialog.maxAvailableCount){
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

			property BaseModel itemsModel: splitLicenseDialog.itemsModel
			onItemsModelChanged: {
				if (itemsModel){
					targetLicenseComboBox.model = itemsModel
				}
			}

			Column {
				id: column
				anchors.fill: parent
				anchors.margins: Style.marginL
				spacing: Style.marginL

				GroupElementView {
					width: parent.width

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
							column.updateButtonState()
						}
					}

					SwitchElementView {
						id: switchElementView
						width: parent.width
						name: qsTr("New License")
						checked: true
						onCheckedChanged: {
							splitLicenseDialog.createNewMode = checked
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
							column.updateButtonState()
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
						nameId: "softwareId"
						controlWidth: 250
						visible: !splitLicenseDialog.createNewMode

						onCurrentIndexChanged: {
							if (currentIndex >= 0 && model){
								splitLicenseDialog.selectedTargetLicenseId = model.get(currentIndex).item.m_id
								// Auto-select the account of the target license
								splitLicenseDialog.selectedAccountId = model.get(currentIndex).item.m_accountId
							}
							column.updateButtonState()
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
					var isValid = licenseCountSpinBox.value > 0 && licenseCountSpinBox.value <= splitLicenseDialog.maxAvailableCount;
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
					if (m_ok && m_items) {
						splitLicenseDialog.itemsModel = m_items
					}
				}
			}
		}
	}

	Loading {
		id: loading
		anchors.fill: splitLicenseDialog
		visible: splitLicenseRequest.state === "Loading" || childLicensesRequest.state === "Loading"
		background.color: Style.baseColor;
	}
}
