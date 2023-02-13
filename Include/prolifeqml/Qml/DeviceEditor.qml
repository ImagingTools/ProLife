import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import imtlicgui 1.0
import Acf 1.0

DocumentBase {
    id: deviceEditorContainer;

    commandsDelegateSourceComp: deviceEditorCommandsDelegate;

    property TreeItemModel accountsModel: TreeItemModel {}
    property TreeItemModel productsModel: TreeItemModel {}

    property bool blockUpdatingModel: false;

    Component.onCompleted: {
        licensesProvider.updateModel();
    }

//    Component {
//        id: deviceEditorCommandsDelegate;
//        DeviceEditorCommandsDelegate {}
//    }

    onDocumentModelChanged: {
        updateGui();

        undoRedoManager.registerModel(documentModel)
    }

//    onAccountsModelChanged: {
//        console.log("onAccountsModelChanged", accountsModel);
//        customerCB.model = accountsModel;
//    }

//    onProductsModelChanged: {
//        console.log("onProductsModelChanged", productsModel);
//    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: deviceEditorContainer.commandsId;
        documentBase: deviceEditorContainer;

        onModelStateChanged: {
            deviceEditorContainer.updateGui();
        }
    }

    LicensesProvider {
        id: licensesProvider;
    }

    MouseArea {
        anchors.fill: parent;

        onClicked: {
            deviceEditorContainer.forceActiveFocus();
        }
    }

    function updateGui(){
        console.log("DeviceEditor begin updateGui");
        blockUpdatingModel = true;

        deviceNameInput.text = deviceEditorContainer.documentModel.GetData("DeviceName");

        if (deviceEditorContainer.documentModel.ContainsKey("SerialNumber")){
            serialNumberInput.text = deviceEditorContainer.documentModel.GetData("SerialNumber");
        }

        if (deviceEditorContainer.documentModel.ContainsKey("MacAddress")){
            macAddressInput.text = deviceEditorContainer.documentModel.GetData("MacAddress");
        }

        blockUpdatingModel = false;
        console.log("DeviceEditor end updateGui");
    }

    function updateModel(){
        console.log("DeviceEditor begin updateModel");
        undoRedoManager.beginChanges();

        let deviceName = deviceNameInput.text;
        deviceEditorContainer.documentModel.SetData("DeviceName", deviceName);

        let serialNumber = serialNumberInput.text;
        deviceEditorContainer.documentModel.SetData("SerialNumber", serialNumber);

        let macAddress = macAddressInput.text;
        deviceEditorContainer.documentModel.SetData("MacAddress", macAddress);

        undoRedoManager.endChanges();
    }

    Rectangle {
        anchors.fill: parent;

        color: Style.backgroundColor;
    }


    Column {
        id: bodyColumn;

        width: 500;
        height: childrenRect.height;

        spacing: 7;

        Text {
            id: titleDeviceName;
            text: qsTr("Device name");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextField {
            id: deviceNameInput;

            width: parent.width;
            height: 30;

            placeHolderText: qsTr("Enter device name");

            borderColor: Style.iconColorOnSelected;

            onEditingFinished: {
                let oldText = deviceEditorContainer.documentModel.GetData("DeviceName");
                if (currentId != deviceNameInput.text){
                    updateModel();
                }
            }

            KeyNavigation.tab: serialNumberInput;
        }

        Text {
            id: titleSerialNumberId;
            text: qsTr("Serial number");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextField {
            id: serialNumberInput;

            width: parent.width;
            height: 30;

            placeHolderText: qsTr("Enter serial number");

            borderColor: Style.iconColorOnSelected;

            onEditingFinished: {
                let oldText = deviceEditorContainer.documentModel.GetData("SerialNumber");
                if (oldText != serialNumberInput.text){
                    updateModel();
                }
            }

            KeyNavigation.tab: macAddressInput;
        }

        Text {
            id: titleMacAddressId;
            text: qsTr("MAC address");
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextField {
            id: macAddressInput;

            width: parent.width;
            height: 30;

            placeHolderText: qsTr("Enter MAC adress");

            borderColor: Style.iconColorOnSelected;

            onEditingFinished: {
                let oldText = documentModel.GetData("MacAddress");
                if (oldText != macAddressInput.text){
                    updateModel();
                }
            }

            KeyNavigation.tab: deviceNameInput;
        }
    }

} //Container


