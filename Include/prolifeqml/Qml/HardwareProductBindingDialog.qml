import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtqml 1.0

Dialog {
    id: productEditorDialog;

    width: 800;

    property var softwareIds: [];
    property string hardwareId: "";

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
        productEditorDialog.buttons.addButton({"Id": "Save", "Name": qsTr("Save"), "Enabled": false});
        productEditorDialog.buttons.addButton({"Id": "Cancel", "Name": qsTr("Close"), "Enabled": true});
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
        }
    }

    contentComp: Component {
        id: productPairEditor;

        HardwareProductBindingEditor {
            id: productBinding;

            height: contentHeight + 40;

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


