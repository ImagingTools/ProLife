import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0

RemoteCollectionView {
    id: container;

    collectionId: "Devices";

    additionalFieldIds: ["OrderUuid", "StatusId", "Licenses"]

    collectionFilter: DeviceCollectionFilter {}

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
        if (filterId == "TextFilter"){
            container.collectionFilter.setTextFilter(filterValue);
        }
        else if (filterId == "AccountFilter"){
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
                Component {CommandsRepresentationProvider {
                commandId: "Device";
                uuid: deviceEditor.viewId;
            }}
        }
    }

    MetaInfoProvider {
        id: metaInfoProvider;

        onMetaInfoModelChanged: {
            container.setMetaInfoModel(metaInfoModel);
        }
    }

    Component {
        id: dataControllerComp;

        GqlDocumentDataController {
            gqlGetCommandId: "DeviceItem";
            gqlUpdateCommandId: "DeviceUpdate";
            gqlAddCommandId: "DeviceAdd";
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
                anchors.leftMargin: 10
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
                    let statusId = cellDelegate.rowDelegate.tableItem.elements.GetData("StatusId", rowIndex);
                    image.source = deviceProductionStatus.getIconPath(statusId);
                    statusLable.text = cellDelegate.getValue();
                }
            }
        }
    }
}
