import QtQuick 2.12
import Acf 1.0
import Qt.labs.platform 1.1

Item {
    id: controller;

    property Item productProvider: null;

    Item {
        id: _private;

        property string dataId: ""
    }

    function createLicenseFile(dataId){

        console.log("createLicenseFile", dataId);
        _private.dataId = dataId;

        let defaultName = "License.lic";
        if (controller.productProvider != null){
            let macAddress = controller.productProvider.getMacAddressFromCurrentPair();
            if (macAddress !== null && macAddress !== ""){
                let splitData = macAddress.split(':');
                defaultName = splitData.join('_') + '_' + defaultName;
            }
        }

        fileDialogSave.currentFile = 'file:///' + defaultName;

        fileDialogSave.open();
    }

    RemoteFileController {
        id: remoteFileController;

        prefix: "/files";
    }

    FileDialog {
        id: fileDialogSave;

        title: qsTr("Save file");
       // folder: shortcuts.home;

        nameFilters: ["License files (*.lic)", "All files (*)"];

        fileMode: FileDialog.SaveFile;

        onAccepted: {
            var pathDir = fileDialogSave.folder.toString();
            remoteFileController.downloadedFileLocation = pathDir.replace('file:///', '');
            var fileName = fileDialogSave.file.toString().replace(pathDir + "/", '');

            let id = _private.dataId;

            if (fileName == ""){
                fileName = {};
                fileName["name"] = id + ".lic";
            }

            remoteFileController.GetFile(id, fileName);
        }
    }

}//Container


