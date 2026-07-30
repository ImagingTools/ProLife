import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtcolgui 1.0

Item {
    id: root

    property BaseModel activationKeysModel: BaseModel {}

    signal activationKeyCreated(string key)
    signal activationKeyRevoked(string keyId)
    signal activationDeactivated(string activationId)

    // Create a new activation key
    function createActivationKey(productId, licenseId, maxActivations, expiresAt) {
        var input = {
            "productId": productId,
            "licenseId": licenseId
        };

        if (maxActivations > 0) {
            input.maxActivations = maxActivations;
        }

        if (expiresAt !== "") {
            input.expiresAt = expiresAt;
        }

        gqlCreateKey.execute(input);
    }

    // Revoke an activation key (disables all associated activations)
    function revokeActivationKey(activationKeyId) {
        var input = {
            "activationKeyId": activationKeyId
        };

        gqlRevokeKey.execute(input);
    }

    // Admin force-deactivate a specific online activation
    function deactivateOnlineActivation(activationId) {
        var input = {
            "activationId": activationId
        };

        gqlDeactivateActivation.execute(input);
    }

    // GraphQL handlers
    GqlMutationController {
        id: gqlCreateKey
        mutationName: "CreateActivationKey"

        onCompleted: {
            if (result && result.success) {
                root.activationKeyCreated(result.activationKey);
            } else {
                var message = result ? result.message : qsTr("Failed to create activation key");
                ModalDialogManager.openDialog(errorDialogComp, {"message": message});
            }
        }

        onFailed: {
            ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("Network error while creating activation key")});
        }
    }

    GqlMutationController {
        id: gqlRevokeKey
        mutationName: "RevokeActivationKey"

        onCompleted: {
            if (result && result.success) {
                root.activationKeyRevoked(result.activationKeyId || "");
            } else {
                var message = result ? result.message : qsTr("Failed to revoke activation key");
                ModalDialogManager.openDialog(errorDialogComp, {"message": message});
            }
        }

        onFailed: {
            ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("Network error while revoking activation key")});
        }
    }

    GqlMutationController {
        id: gqlDeactivateActivation
        mutationName: "DeactivateOnlineActivation"

        onCompleted: {
            if (result && result.success) {
                root.activationDeactivated(result.activationId || "");
            } else {
                var message = result ? result.message : qsTr("Failed to deactivate online activation");
                ModalDialogManager.openDialog(errorDialogComp, {"message": message});
            }
        }

        onFailed: {
            ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("Network error while deactivating activation")});
        }
    }

    Component {
        id: errorDialogComp

        ErrorDialog {
            title: qsTr("Activation Key Error")
        }
    }
}


