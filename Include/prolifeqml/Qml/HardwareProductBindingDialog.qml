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
        console.log("onHardwareIdChanged", hardwareId);
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

//            onCheckedItemsChanged: {
//                productEditorDialog.buttons.setButtonState("Save", true);
//            }

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

            productEditorDialog.contentItem.updateGui();
        }
    }
}//Container


