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
        let onResult = function(documentModel){
            productEditorDialog.contentItem.bindingModel = documentModel;
        }

        documentController.getData("HardwareProductBinding", hardwareId, {}, onResult)
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
                        let onResult = function(id, name){}

                        documentController.updateData(
                                                      "HardwareProductBinding",
                                                      productEditorDialog.hardwareId,
                                                      productEditorDialog.contentItem.bindingModel,
                                                      onResult
                                                     )
                    }
                    else{
                        productEditorDialog.contentItem.bindingModel.SetData("Id", productEditorDialog.hardwareId);

                        let onResult = function(id, name){}
                        documentController.setData(
                                                   "HardwareProductBinding",
                                                   productEditorDialog.hardwareId,
                                                   productEditorDialog.contentItem.bindingModel,
                                                   onResult
                                                   )
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

        onError: {
            productEditorDialog.contentItem.bindingModelReady = true;
        }
    }
}//Container


