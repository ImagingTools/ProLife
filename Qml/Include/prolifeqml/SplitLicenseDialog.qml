// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

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

	property real spinBoxValue: 1
	property string selectedAccountId: ""

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
							splitLicenseDialog.setButtonEnabled(Enums.ok, value > 0 && value < splitLicenseDialog.maxAvailableCount && accountComboBox.currentIndex >= 0);
						}
					}

					ComboBoxElementView {
						id: accountComboBox
						width: parent.width
						name: qsTr("Target account")
						nameId: "name"
						model: CachedAccountCollection.collectionModel
						controlWidth: 250

						onCurrentIndexChanged: {
							if (currentIndex >= 0 && model){
								splitLicenseDialog.selectedAccountId = model.getData("id", currentIndex)
							}

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
