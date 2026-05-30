import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0

Item {
    id: root

    // Current license status
    property string licenseStatus: "inactive"  // "inactive", "active", "grace", "expired"
    property string activationToken: ""
    property string expiresAt: ""
    property var activatedFeatures: []

    // Signals
    signal licenseActivated(string token, string expiresAt, var features)
    signal licenseDeactivated()
    signal licenseValidationFailed(string message)
    signal heartbeatCompleted(bool valid, string expiresAt)

    // Configuration
    property int heartbeatIntervalMs: 24 * 60 * 60 * 1000  // 24 hours default

    // Activate license with an activation key
    function activateLicense(activationKey, machineFingerprint) {
        if (activationKey === "" || machineFingerprint === "") {
            licenseValidationFailed(qsTr("Activation key and machine fingerprint are required"));
            return;
        }

        var input = {
            "activationKey": activationKey,
            "machineFingerprint": machineFingerprint,
            "machineName": Qt.platform.os
        };

        gqlActivate.execute(input);
    }

    // Validate current license token
    function validateLicense(machineFingerprint) {
        if (root.activationToken === "") {
            licenseValidationFailed(qsTr("No active license token"));
            return;
        }

        var input = {
            "token": root.activationToken,
            "machineFingerprint": machineFingerprint
        };

        gqlValidate.execute(input);
    }

    // Deactivate current license
    function deactivateLicense() {
        if (root.activationToken === "") {
            return;
        }

        var input = {
            "token": root.activationToken
        };

        gqlDeactivate.execute(input);
    }

    // Send heartbeat
    function sendHeartbeat(machineFingerprint) {
        if (root.activationToken === "") {
            return;
        }

        var input = {
            "token": root.activationToken,
            "machineFingerprint": machineFingerprint
        };

        gqlHeartbeat.execute(input);
    }

    // Internal: Heartbeat timer
    Timer {
        id: heartbeatTimer
        interval: root.heartbeatIntervalMs
        repeat: true
        running: root.licenseStatus === "active"

        onTriggered: {
            root.sendHeartbeat(_private.machineFingerprint);
        }
    }

    Item {
        id: _private
        property string machineFingerprint: ""
    }

    // GraphQL mutation handlers
    GqlMutationController {
        id: gqlActivate
        mutationName: "ActivateLicense"

        onCompleted: {
            if (result && result.success) {
                root.activationToken = result.token || "";
                root.expiresAt = result.expiresAt || "";
                root.activatedFeatures = result.features || [];
                root.licenseStatus = "active";
                root.licenseActivated(root.activationToken, root.expiresAt, root.activatedFeatures);
            } else {
                var message = result ? result.message : qsTr("Activation failed");
                root.licenseValidationFailed(message);
            }
        }

        onFailed: {
            root.licenseValidationFailed(qsTr("Network error during activation"));
        }
    }

    GqlMutationController {
        id: gqlValidate
        mutationName: "ValidateLicense"

        onCompleted: {
            if (result && result.valid) {
                root.expiresAt = result.expiresAt || "";
                root.activatedFeatures = result.features || [];
                root.licenseStatus = "active";
            } else {
                root.licenseStatus = "grace";
                var message = result ? result.message : qsTr("Validation failed");
                root.licenseValidationFailed(message);
            }
        }

        onFailed: {
            // Network error - enter grace period
            root.licenseStatus = "grace";
        }
    }

    GqlMutationController {
        id: gqlDeactivate
        mutationName: "DeactivateLicense"

        onCompleted: {
            if (result && result.success) {
                root.activationToken = "";
                root.expiresAt = "";
                root.activatedFeatures = [];
                root.licenseStatus = "inactive";
                root.licenseDeactivated();
            }
        }

        onFailed: {
            // Still deactivate locally on network error
            root.activationToken = "";
            root.licenseStatus = "inactive";
            root.licenseDeactivated();
        }
    }

    GqlMutationController {
        id: gqlHeartbeat
        mutationName: "Heartbeat"

        onCompleted: {
            if (result && result.valid) {
                root.expiresAt = result.expiresAt || "";
                root.licenseStatus = "active";
                root.heartbeatCompleted(true, root.expiresAt);
            } else {
                root.licenseStatus = "grace";
                root.heartbeatCompleted(false, "");
            }
        }

        onFailed: {
            // Network error - stay in current status, will retry
            root.licenseStatus = "grace";
            root.heartbeatCompleted(false, "");
        }
    }
}


