import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtqml 1.0

Dialog {
    id: productEditorDialog;

    width: root.width - 100;

    property var softwareIds: [];
    property string hardwareId: "";

    notClosingButtons: "Save";

    signal saved();

    onHardwareIdChanged: {
        documentController.getData(hardwareId, {}, "HardwareProductBinding")
    }

    Component.onCompleted: {
        productEditorDialog.fillButtons();

        productEditorDialog.title = qsTr("Add License to Sensor");
    }

    onLocalizationChanged: {
        productEditorDialog.fillButtons();
    }

    function fillButtons(){
        productEditorDialog.buttonsModel.clear();
        productEditorDialog.buttons.addButton({"Id": "Save", "Name": qsTr("Apply"), "Enabled": false});
        productEditorDialog.buttons.addButton({"Id": "Cancel", "Name": qsTr("Close"), "Enabled": true});
    }

    onFinished: {
        if (buttonId === "Save"){
//            let newLicensesInfo = productEditorDialog.contentItem.newLicensesInfo;
//            let keys = Object.keys(newLicensesInfo);
//            for (let key of keys){
//                newLicensesInfo[key];
//            }

            modalDialogManager.openDialog(messageDialog, {});
        }
    }

    contentComp: Component {
        id: productPairEditor;

        HardwareProductBindingEditor {
            id: productBinding;

            height: contentHeight + 40;

            hardwareId: productEditorDialog.hardwareId

            onModelChanged: {
                productEditorDialog.buttons.setButtonState("Save", true);
                productEditorDialog.buttonsModel.setProperty(1, "Name", qsTr("Cancel"));
            }
        }
    }

    Component {
        id: messageDialog;

        MessageDialog {
            title: qsTr("Apply Changes");
            message: qsTr("Please check the data before saving. Save changes ?")
            onFinished: {
                if (buttonId == "Yes"){
                    if (productEditorDialog.contentItem.bindingModel.ContainsKey("Id")){
                        documentController.updateData(productEditorDialog.hardwareId, productEditorDialog.contentItem.bindingModel)
                    }
                    else{
                        productEditorDialog.contentItem.bindingModel.SetData("Id", productEditorDialog.hardwareId);

                        documentController.setData(productEditorDialog.hardwareId, productEditorDialog.contentItem.bindingModel)
                    }

                  //  productEditorDialog.contentItem.includeIds = [];
                    productEditorDialog.contentItem.changesApplied = true;
                    productEditorDialog.buttons.setButtonState("Save", false);
                    productEditorDialog.buttonsModel.setProperty(1, "Name", qsTr("Close"));

                    productEditorDialog.contentItem.beginBindingModel.Copy(productEditorDialog.contentItem.bindingModel);

                    productEditorDialog.saved();

                    productEditorDialog.finished("Cancel");
                }
                else if (buttonId == "No"){
                }
            }
        }
    }

    GqlDocumentDataController {
        id: documentController;

        documentTypeId: "HardwareProductBinding";

        onDocumentModelChanged: {
            productEditorDialog.contentItem.bindingModel = documentModel;
        }

        onError: {
            productEditorDialog.contentItem.bindingModelReady = true;
        }
    }
}//Container


