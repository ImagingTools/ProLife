import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0

Dialog {
    id: productEditorDialog;

    property int rootWidth: root ? root.width - 100 : 0;

    onRootWidthChanged: {
        width = rootWidth;
    }

    property var softwareIds: [];
    property string hardwareId: "";

    notClosingButtons: Enums.ok;

    signal saved();

    onHardwareIdChanged: {
        console.log("onHardwareIdChanged", hardwareId);
        documentController.documentId = hardwareId;
        documentController.updateDocumentModel();
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
        productEditorDialog.buttonsModel.append({"Id": Enums.ok, "Name": qsTr("Apply"), "Enabled": false});
        productEditorDialog.buttonsModel.append({"Id": Enums.cancel, "Name": qsTr("Close"), "Enabled": true});
    }

    onFinished: {
        if (buttonId == Enums.ok){
            modalDialogManager.openDialog(messageDialog, {});
        }
    }

    contentComp: Component {
        id: productPairEditor;

        HardwareProductBindingEditor {
            id: productBinding;

            width: productEditorDialog.width;
            height: contentHeight + 40;

            onModelChanged: {
                productEditorDialog.buttons.setButtonState(Enums.ok, true);
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
                if (buttonId == Enums.yes){
                    let bindingModel = productEditorDialog.contentItem.bindingModel;

                    bindingModel.SetData("Id", productEditorDialog.hardwareId);
                    documentController.documentModel = bindingModel;

                    documentController.saveDocument();

                    productEditorDialog.buttons.setButtonState(Enums.ok, false);
                    productEditorDialog.buttonsModel.setProperty(1, "Name", qsTr("Close"));

                    productEditorDialog.saved();
                    productEditorDialog.finished(Enums.cancel);
                }
            }
        }
    }

    GqlDocumentDataController {
        id: documentController;

        gqlGetCommandId: "HardwareProductBindingItem";
        gqlUpdateCommandId: "HardwareProductBindingUpdate";
        gqlAddCommandId: "HardwareProductBindingAdd";

        onDocumentModelChanged: {
            console.log("documentController onDocumentModelChanged", documentModel.toJSON());
            productEditorDialog.contentItem.bindingModel = documentModel;
        }
    }
}//Container


