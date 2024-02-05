import QtQuick 2.15
import Acf 1.0
import imtdocgui 1.0

DocumentWorkspaceCommandsDelegateBase {
    documentPtr: deviceEditorContainer;

    onCommandActivated: {
        if (commandId == "Bind"){
            if (deviceEditorContainer.isDirty){
                modalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Please save the document first.")});

                return;
            }

            let hardwareUuid = deviceEditorContainer.documentId;
            let macAddress = "";

            if (deviceEditorContainer.model.ContainsKey("MacAddress")){
                macAddress = deviceEditorContainer.model.GetData("MacAddress");
            }

            if (hardwareUuid === "" || macAddress === ""){
                modalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Please enter the MAC-Address then save the document.")});

                return;
            }

            modalDialogManager.openDialog(productPairEditorDialog, {"hardwareId": hardwareUuid});
        }
    }

//    Component {
//        id: saveDialogComp;

//        ErrorDialog {
//            width: 300;

//            title: qsTr("Save document");
//        }
//    }

//    Component {
//        id: productPairEditorDialog;

//        HardwareProductBindingDialog {
//        }
//    }
}

