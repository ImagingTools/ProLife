import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import Qt.labs.platform 1.1

Item {
    id: controller;

    property string defaultName: "License.lic";
    property string fileName: controller.defaultName;

    Item {
        id: _private;

        property string dataId: ""
    }

    function createLicenseFile(dataId){
        _private.dataId = dataId;

        fileDialogSave.currentFile = 'file:///' + controller.fileName;

        fileDialogSave.open();
    }

    RemoteFileController {
        id: remoteFileController;

        prefix: "/files";

        onFileDownloadFailed: {
            modalDialogManager.openDialog(messageErrorDialog, {"message": qsTr("Error when trying to create a license file.")});
        }

        onStateChanged: {
            console.log("onStateChanged", state);

            if (state == "Loading"){
                Events.sendEvent("StartLoading");
            }
            else if (state == "Ready"){
                Events.sendEvent("StopLoading");
            }
            else{
                Events.sendEvent("StopLoading");
            }
        }
    }

    Component {
        id: messageErrorDialog;

        ErrorDialog {
            title: qsTr("License creation error");
        }
    }

    FileDialog {
        id: fileDialogSave;

        title: qsTr("Save file");

        nameFilters: ["License files (*.lic)", "All files (*)"];

        fileMode: FileDialog.SaveFile;

        onAccepted: {
            var pathDir = fileDialogSave.folder.toString();
            remoteFileController.downloadedFileLocation = pathDir.replace('file:///', '');
            var fileName = fileDialogSave.file.toString().replace(pathDir + "/", '');

            let id = _private.dataId;

            if (fileName == ""){
                fileName= controller.fileName;
            }

            remoteFileController.GetFile(id, fileName);
        }
    }

}//Container


