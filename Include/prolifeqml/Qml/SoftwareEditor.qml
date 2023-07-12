import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

DocumentBase {
    id: root;

    onDocumentModelChanged: {
        serialNumberInput.focus = true;

        root.updateGui();
    }

    function updateGui(){
        root.blockUpdatingModel = true;

        if (root.documentModel.ContainsKey("SerialNumber")){
            serialNumberInput.text = root.documentModel.GetData("SerialNumber");
        }

        root.blockUpdatingModel = false;
    }

    function updateModel(){
        if (root.blockUpdatingModel){
            return;
        }

        root.documentModel.SetData("SerialNumber", serialNumberInput.text)
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }

    Flickable {
        id: flickable;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
        anchors.left: parent.left;
        anchors.leftMargin: 20;

        width: 450;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: bodyColumn;

            width: flickable.width;

            spacing: 7;

            Text {
                id: titleSerialNumber;

                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;

                text: qsTr("Serial Number");
            }

            CustomTextField {
                id: serialNumberInput;

                height: 30;
                width: bodyColumn.width;

                placeHolderText: qsTr("Enter the serial number");

                onEditingFinished: {
                    let oldText = root.documentModel.GetData("SerialNumber");
                    if (oldText && oldText !== serialNumberInput.text || !oldText && serialNumberInput.text !== ""){
                        root.updateModel();
                    }
                }
            }
        }
    }
}//Container


