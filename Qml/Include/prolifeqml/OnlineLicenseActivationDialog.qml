import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0

Dialog {
    id: activationDialog

    title: qsTr("Online License Activation")
    width: 600
    height: 400

    property string activationKey: ""
    property string statusMessage: ""
    property bool isActivating: false
    property string currentStatus: "inactive"  // "inactive", "active", "grace", "expired"
    property string expiresAt: ""

    signal activated(string token, string expiresAt)
    signal deactivated()

    Component.onCompleted: {
        activationDialog.fillButtons();
    }

    onLocalizationChanged: {
        activationDialog.fillButtons();
    }

    function fillButtons() {
        clearButtons();
        if (currentStatus === "active" || currentStatus === "grace") {
            addButton(Enums.cancel, qsTr("Close"), false);
            addButton(Enums.custom1, qsTr("Deactivate"), false);
        } else {
            addButton(Enums.cancel, qsTr("Cancel"), false);
            addButton(Enums.ok, qsTr("Activate"), false);
        }
    }

    onCurrentStatusChanged: {
        fillButtons();
    }

    onButtonClicked: {
        if (buttonId === Enums.ok) {
            activate();
        } else if (buttonId === Enums.custom1) {
            deactivate();
        } else {
            close();
        }
    }

    function activate() {
        if (activationKeyInput.text === "") {
            statusMessage = qsTr("Please enter an activation key");
            return;
        }

        isActivating = true;
        statusMessage = qsTr("Activating...");
        onlineLicenseController.activateLicense(activationKeyInput.text, machineFingerprint());
    }

    function deactivate() {
        isActivating = true;
        statusMessage = qsTr("Deactivating...");
        onlineLicenseController.deactivateLicense();
    }

    function machineFingerprint() {
        // Generate machine fingerprint from available system info
        return Qt.md5(Qt.platform.os + "_" + Qt.application.name);
    }

    OnlineLicenseController {
        id: onlineLicenseController

        onLicenseActivated: {
            activationDialog.isActivating = false;
            activationDialog.currentStatus = "active";
            activationDialog.expiresAt = expiresAt;
            activationDialog.statusMessage = qsTr("License activated successfully. Expires: %1").arg(expiresAt);
            activationDialog.activated(token, expiresAt);
        }

        onLicenseDeactivated: {
            activationDialog.isActivating = false;
            activationDialog.currentStatus = "inactive";
            activationDialog.expiresAt = "";
            activationDialog.statusMessage = qsTr("License deactivated");
            activationDialog.deactivated();
        }

        onLicenseValidationFailed: {
            activationDialog.isActivating = false;
            activationDialog.statusMessage = message;
        }
    }

    // UI Layout
    Column {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 16

        // Status indicator
        Row {
            spacing: 8
            width: parent.width

            Rectangle {
                width: 12
                height: 12
                radius: 6
                anchors.verticalCenter: parent.verticalCenter
                color: {
                    switch (activationDialog.currentStatus) {
                        case "active": return "#4CAF50";   // green
                        case "grace": return "#FF9800";    // orange
                        case "expired": return "#F44336";  // red
                        default: return "#9E9E9E";         // grey
                    }
                }
            }

            Text {
                text: {
                    switch (activationDialog.currentStatus) {
                        case "active": return qsTr("Active");
                        case "grace": return qsTr("Grace Period (Offline)");
                        case "expired": return qsTr("Expired");
                        default: return qsTr("Not Activated");
                    }
                }
                font.pixelSize: 14
                font.bold: true
            }
        }

        // Expiration info
        Text {
            visible: activationDialog.expiresAt !== ""
            text: qsTr("Expires: %1").arg(activationDialog.expiresAt)
            font.pixelSize: 12
            color: "#666666"
        }

        // Activation key input (only shown when inactive)
        Column {
            visible: activationDialog.currentStatus === "inactive" || activationDialog.currentStatus === "expired"
            width: parent.width
            spacing: 8

            Text {
                text: qsTr("Enter your activation key:")
                font.pixelSize: 13
            }

            TextInput {
                id: activationKeyInput
                width: parent.width
                height: 40
                font.pixelSize: 16
                font.family: "monospace"
                maximumLength: 19  // XXXX-XXXX-XXXX-XXXX

                property string placeholderText: "XXXX-XXXX-XXXX-XXXX"

                Text {
                    text: activationKeyInput.placeholderText
                    color: "#999999"
                    visible: !activationKeyInput.text
                    font: activationKeyInput.font
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        // Status message
        Text {
            visible: activationDialog.statusMessage !== ""
            text: activationDialog.statusMessage
            font.pixelSize: 12
            color: activationDialog.currentStatus === "active" ? "#4CAF50" : "#F44336"
            wrapMode: Text.WordWrap
            width: parent.width
        }
    }
}


