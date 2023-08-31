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
        productEditorDialog.buttons.addButton({"Id": "Cancel", "Name": qsTr("Cancel"), "Enabled": true});
    }

    onFinished: {
        if (buttonId === "Save"){
            if (productEditorDialog.contentItem.bindingModel.ContainsKey("Id")){
                documentController.updateData(productEditorDialog.hardwareId, productEditorDialog.contentItem.bindingModel)
            }
            else{
                productEditorDialog.contentItem.bindingModel.SetData("Id", productEditorDialog.hardwareId);

                documentController.setData(productEditorDialog.hardwareId, productEditorDialog.contentItem.bindingModel)
            }

            productEditorDialog.contentItem.includeIds = [];
            productEditorDialog.buttons.setButtonState("Save", false);
        }
    }

    contentComp: Component {
        id: productPairEditor;

//        onStatusChanged: {
//            console.log("DEBUG::31", status, productBinding.modelFilter.toJSON())
////            Component.Ready
//        }

        HardwareProductBindingEditor {
            id: productBinding;

            height: contentHeight + 40;

            hardwareId: productEditorDialog.hardwareId

            onModelChanged: {
                productEditorDialog.buttons.setButtonState("Save", true);
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


