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
        documentController.documentId = hardwareId;
        documentController.updateDocumentModel();
    }

    Component.onCompleted: {
        productEditorDialog.fillButtons();

        productEditorDialog.title = qsTr("Add license to sensor");
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
            let project = ""
            if (productEditorDialog.contentItem.bindingModel.ContainsKey("Project")){
                project = productEditorDialog.contentItem.bindingModel.GetData("Project")
            }

            modalDialogManager.openDialog(messageDialog, {"placeHolderText":project});
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

        InputDialog {
            title: qsTr("Apply changes");
            message: qsTr("Please check the data before saving. Save changes ?")
            placeHolderText: qsTr("Please enter the project");
            onFinished: {
                if (buttonId == Enums.ok){
                    let bindingModel = productEditorDialog.contentItem.bindingModel;

                    bindingModel.SetData("Id", productEditorDialog.hardwareId);
                    bindingModel.SetData("Project", inputValue);

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
            productEditorDialog.contentItem.bindingModel = documentModel;
        }
    }
}//Container


