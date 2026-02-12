import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import prolifeLicensesSdl 1.0
import prolifeAccountsSdl 1.0

Dialog {
	id: splitLicenseDialog

	title: qsTr("Split License")
	width: 500
	height: 300
	canMove: false

	property string licenseId: ""
	property int maxAvailableCount: 0
	property int currentCount: 1

	notClosingButtons: Enums.ok

	Component.onCompleted: {
		splitLicenseDialog.fillButtons()
	}

	onLocalizationChanged: {
		splitLicenseDialog.fillButtons()
	}

	function fillButtons(){
		clearButtons()
		addButton(Enums.ok, qsTr("Save"), false)
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}

	onFinished: {
		if (buttonId == Enums.ok){
			// Validate inputs
			if (licenseCountSpinBox.value <= 0 || licenseCountSpinBox.value >= maxAvailableCount){
				return;
			}

			if (accountComboBox.currentIndex < 0){
				return;
			}

			// Get account ID
			let accountId = accountComboBox.model.getData("id", accountComboBox.currentIndex);

			// Send the split request
			splitLicenseInput.m_licenseId = licenseId;
			splitLicenseInput.m_licenseCount = licenseCountSpinBox.value;
			splitLicenseInput.m_accountId = accountId;

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

				GroupElementView {
					width: parent.width

					SpinBoxElementView {
						id: licenseCountSpinBox
						width: parent.width
						name: qsTr("License Count to Split")
						from: 1
						to: splitLicenseDialog.maxAvailableCount - 1
						value: 1
						controlWidth: 150

						onValueChanged: {
							splitLicenseDialog.setButtonEnabled(Enums.ok, value > 0 && value < splitLicenseDialog.maxAvailableCount && accountComboBox.currentIndex >= 0);
						}
					}

					BaseText {
						width: parent.width
						text: qsTr("Maximum available: %1").arg(splitLicenseDialog.maxAvailableCount)
						color: Style.hintTextColor
					}

					ComboBoxElementView {
						id: accountComboBox
						width: parent.width
						name: qsTr("Target Account")
						nameId: "name"
						model: CachedAccountCollection.accountsModel
						controlWidth: 300

						onCurrentIndexChanged: {
							splitLicenseDialog.setButtonEnabled(Enums.ok, licenseCountSpinBox.value > 0 && licenseCountSpinBox.value < splitLicenseDialog.maxAvailableCount && currentIndex >= 0);
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
					if (m_ok){
						splitLicenseDialog.finished(Enums.ok);
					} else {
						// Show error message
						console.error("Split License Error:", m_message);
						splitLicenseDialog.finished(Enums.cancel);
					}
				}
			}
		}
	}

	Loading {
		id: loading
		anchors.fill: splitLicenseDialog
		visible: splitLicenseRequest.state === "Loading"
		background.color: Style.backgroundColor2;
	}
}
