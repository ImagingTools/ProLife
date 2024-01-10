import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0

Dialog {
    id: productEditorDialog;

    width: root.width - 100;

//    buttonsModel: ListModel{
//        ListElement{Id: Enums.ButtonType.Ok; Name:qsTr("Save"); Enabled: true}
//        ListElement{Id: Enums.ButtonType.Cancel; Name:qsTr("Cancel"); Enabled: true}
//    }

    property var softwareIds: [];
    property string hardwareId: "";

//    notClosingButtons: "Save";

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
        productEditorDialog.buttons.addButton({"Id": Enums.ButtonType.Ok, "Name": qsTr("Apply"), "Enabled": false});
        productEditorDialog.buttons.addButton({"Id": Enums.ButtonType.Cancel, "Name": qsTr("Close"), "Enabled": true});
    }

    onFinished: {
        if (buttonId == Enums.ButtonType.Ok){
            modalDialogManager.openDialog(messageDialog, {});
        }
    }

    contentComp: Component {
        id: productPairEditor;

        HardwareProductBindingEditor {
            id: productBinding;

            width: productEditorDialog.width;
            height: contentHeight + 40;

            hardwareId: productEditorDialog.hardwareId

            onModelChanged: {
                productEditorDialog.buttons.setButtonState(Enums.ButtonType.Ok, true);
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
                if (buttonId == Enums.ButtonType.Yes){
                    if (productEditorDialog.contentItem.bindingModel.ContainsKey("Id")){
                        let onResult = function(id, name){}

                        documentController.updateData(
                                                      "HardwareProductBindingUpdate",
                                                      productEditorDialog.hardwareId,
                                                      productEditorDialog.contentItem.bindingModel,
                                                      {},
                                                      onResult
                                                     )
                    }
                    else{
                        productEditorDialog.contentItem.bindingModel.SetData("Id", productEditorDialog.hardwareId);

                        let onResult = function(id, name){}
                        documentController.setData(
                                                   "HardwareProductBindingAdd",
                                                   productEditorDialog.hardwareId,
                                                   productEditorDialog.contentItem.bindingModel,
                                                   {},
                                                   onResult
                                                   )
                    }

                  //  productEditorDialog.contentItem.includeIds = [];
                    productEditorDialog.contentItem.changesApplied = true;
                    productEditorDialog.buttons.setButtonState(Enums.ButtonType.Ok, false);
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


