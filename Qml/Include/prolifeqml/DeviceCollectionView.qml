import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import prolifeSensorsSdl 1.0

RemoteCollectionView {
    id: container;

    collectionId: "Devices";

    additionalFieldIds: ["OrderUuid", "StatusId", "Licenses"]

    collectionFilter: DeviceCollectionFilter {}

    commandsViewComp: Component {
        id: commandsDecoratorComp;

        CommandsPanel {
            commandId: container.viewId;

            onCommandActivated: {
                if (container.commandsDelegate){
                    container.commandsDelegate.commandHandle(commandId);
                }
            }
        }
    }

    commandsDelegateComp: Component {DeviceCollectionViewCommandsDelegate {
            collectionView: container;
        }
    }

    visibleMetaInfo: true;

    Component.onCompleted: {
        collectionFilter.setSortingOrder("DESC");
        collectionFilter.setSortingInfoId("LastModified");

        let documentManager = MainDocumentManager.getDocumentManager(container.collectionId);
        if (documentManager){
            container.commandsDelegate.documentManager = documentManager;

            documentManager.registerDocumentView("Device", "DeviceEditor", deviceEditorComp);
            documentManager.registerDocumentDataController("Device", dataControllerComp);
            documentManager.registerDocumentValidator("Device", deviceValidatorComp);
        }

        filterMenu.decorator = deviceCollectionFilterComp;
    }

    Component {
        id: deviceCollectionFilterComp;

        DeviceCollectionFilterDecorator {}
    }

    function onFilterChanged(filterId, filterValue){
        if (filterId == "AccountFilter"){
            container.collectionFilter.setAccountFilter(filterValue);
        }
        else if (filterId == "LicenseFilter"){
            container.collectionFilter.setLicenseFilter(filterValue);
        }
        else if (filterId == "StatusFilter"){
            container.collectionFilter.setDeviceStatusFilter(filterValue);
        }

        container.doUpdateGui();
    }

    onHeadersChanged: {
        container.table.setColumnContentComponent(0, pairComp);
    }

    Component {
        id: deviceEditorComp;

        DeviceEditor {
            id: deviceEditor;

            commandsControllerComp:
                Component {CommandsPanelController {
                    commandId: "Device";
                    uuid: deviceEditor.viewId;
                }}

            commandsDelegateComp: Component {ViewCommandsDelegateBase {
                    view: deviceEditor;
                    onCommandActivated: {
                        if (commandId == "Bind"){
                            let documentManager = MainDocumentManager.getDocumentManager(container.collectionId);
                            if (documentManager){
                                let documentData = documentManager.getDocumentDataByView(deviceEditor);
                                if (!documentData){
                                    ModalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Unknown error")});

                                    return;
                                }

                                let documentIndex = documentData.documentIndex;
                                if (documentIndex < 0){
                                    ModalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Unknown error")});

                                    return;
                                }

                                let isDirty = documentData.isDirty;
                                let isNew = documentManager.documentsModel.get(documentIndex).IsNew;
                                if (isNew || isDirty){
                                    ModalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Please save the document first"), "title": qsTr("Save document")});

                                    return;
                                }

                                let documentModel = documentData.documentDataController.documentModel;
                                if (!documentModel){
                                    ModalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Unknown error")});

                                    return;
                                }

                                let macAddress = "";

                                if (documentModel.containsKey("MacAddress")){
                                    macAddress = documentModel.getData("MacAddress");
                                }

                                if (!macAddressValidator.isValid(macAddress)){
                                    ModalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Please enter a valid MAC-Address")});

                                    return;
                                }

                                let title = qsTr("Add license to sensor '%1'");
                                title = title.replace("%1", macAddress);

                                let documentId = documentData.documentId;
                                if (documentId === ""){
                                    ModalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Unknown error")});

                                    return;
                                }

                                ModalDialogManager.openDialog(productPairEditorDialog, {"hardwareId": documentId, "title": title});
                            }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: saveDialogComp;

        ErrorDialog {
            width: 300;

            title: qsTr("Warning message");
        }
    }

    Component {
        id: productPairEditorDialog;

        HardwareProductBindingDialog {
        }
    }

    MacAddressValidator {
        id: macAddressValidator;
    }

    MetaInfoProvider {
        id: metaInfoProvider;

        onMetaInfoModelChanged: {
            container.setMetaInfoModel(metaInfoModel);
        }
    }

    Component {
        id: dataControllerComp;

        GqlRequestDocumentDataController {
            id: requestDocumentDataController

            gqlGetCommandId: "DeviceItem";
            gqlUpdateCommandId: "DeviceUpdate";
            gqlAddCommandId: "DeviceAdd";

            documentModelComp: Component {
                DeviceData {}
            }

            payloadModel: DeviceDataPayload {
                onFinished: {
                    requestDocumentDataController.documentModel = m_deviceData
                }
            }
        }
    }

    Component {
        id: deviceValidatorComp;

        DeviceValidator {
        }
    }

    DeviceProductionStatus {
        id: deviceProductionStatus;
    }

    Component {
        id: pairComp;
        TableCellDelegateBase {
            id: cellDelegate
            Image {
                id: image;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 5;

                width: 20;
                height: width;

                sourceSize.width: width;
                sourceSize.height: height;
            }

            Text {
                id: statusLable;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: image.right
                anchors.leftMargin: Style.size_mainMargin;
                anchors.right: parent.right

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
                color: Style.textColor;

                elide: Text.ElideRight;
            }

            onRowIndexChanged: {
                if (!rowDelegate){
                    return
                }

                if (rowIndex >= 0){
                    let statusId = cellDelegate.rowDelegate.tableItem.elements.getData("StatusId", rowIndex);
                    image.source = deviceProductionStatus.getIconPath(statusId);
                    statusLable.text = cellDelegate.getValue();
                }
            }
        }
    }
}
