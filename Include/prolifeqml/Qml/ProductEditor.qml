import QtQuick 2.0
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

DocumentBase {
    id: productEditorContainer;

    anchors.fill: parent;


    property int textInputHeight: 30;

    Component.onCompleted: {
        productNameInput.focus = true;
    }

    onDocumentModelChanged: {
        updateGui();
        undoRedoManager.registerModel(documentModel);
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: productEditorContainer.commandsId;

        onModelStateChanged: {
            updateGui();
        }
    }

    function updateGui(){
        console.log("ProductEditor updateGui");
        productNameInput.text = documentModel.GetData("Name");

        productDescriptionInput.text = documentModel.GetData("Description");

        productManufacturerInput.text = documentModel.GetData("Manufacturer");
    }

    function updateModel(){
        console.log("updateModel");

        undoRedoManager.beginChanges();

        let name = productNameInput.text;
        documentModel.SetData("Name", name)

        let description = productDescriptionInput.text;
        documentModel.SetData("Description", description)

        let manufacturer = productManufacturerInput.text;
        documentModel.SetData("Manufacturer", manufacturer)

        console.log("test" + documentModel.toJSON());
        undoRedoManager.endChanges();
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Flickable {
        anchors.fill: parent;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height + 50;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: bodyColumn;

            anchors.left: parent.left;
            anchors.leftMargin: 20;

            width: 450;

            spacing: 7;

            Text {
                id: titleAccountName;
                text: qsTr("Product name");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: productNameInput;

                height: productEditorContainer.textInputHeight;
                width: bodyColumn.width;

                onEditingFinished: {
                    let oldText = documentModel.GetData("Name");
                    if (oldText != productNameInput.text){
                        updateModel();
                    }
                }

                KeyNavigation.tab: productDescriptionInput;
            }

            Text {
                id: titleproductDescription;
                text: qsTr("Product Description");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: productDescriptionInput;

                height: productEditorContainer.textInputHeight;
                width: bodyColumn.width;

                onEditingFinished: {
                    let oldText = documentModel.GetData("Description");
                    if (oldText != productDescriptionInput.text){
                        updateModel();
                    }
                }

                KeyNavigation.tab: productManufacturerInput;
            }

            Text {
                id: titleproductManufacturer;
                text: qsTr("Product manufacturer");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: productManufacturerInput;

                height: productEditorContainer.textInputHeight;
                width: bodyColumn.width;

                onEditingFinished: {
                    let oldText = documentModel.GetData("Manufacturer");
                    if (oldText != productManufacturerInput.text){
                        updateModel();
                    }
                }

                // KeyNavigation.tab: productNameInput;
            }
        }//Body column
    }//Flickable
}// product Editor container
