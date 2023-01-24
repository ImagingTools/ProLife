import QtQuick 2.0
import Acf 1.0
import QtQuick.Dialogs 1.3

Item {
    id: controller;

    Item {
        id: _private;

        property string dataId: ""
    }

    function createLicenseFile(dataId){
        _private.dataId = dataId;

        fileDialogSave.open();
    }

    RemoteFileController {
        id: remoteFileController;

        prefix: "/files";

    }

    FileDialog {
        id: fileDialogSave;

        title: qsTr("Save file");
        selectExisting: false;
        folder: shortcuts.home;

        nameFilters: ["License files (*.lic)", "All files (*)"];

        onAccepted: {
            var pathDir = fileDialogSave.folder.toString();
            remoteFileController.downloadedFileLocation = pathDir.replace('file:///', '');
            var fileName = fileDialogSave.fileUrl.toString().replace(pathDir + "/", '');

            let id = _private.dataId;

            if (fileName == ""){
                fileName = {};
                fileName["name"] = id + ".lic";
            }

            remoteFileController.GetFile(id, fileName);
        }
    }

}//Container


