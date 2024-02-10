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

    commandsDelegate: DeviceCollectionViewCommandsDelegate {
        collectionView: container;
    }

    filterMenu.decorator: deviceCollectionFilterComp;

    Component.onCompleted: {
        collectionFilter.setSortingOrder("DESC");
        collectionFilter.setSortingInfoId("LastModified");

        let documentManager = MainDocumentManager.getDocumentManager(container.collectionId);
        if (documentManager){
            container.commandsDelegate.documentManager = documentManager;

            documentManager.registerDocumentView("Device", "DeviceEditor", deviceEditorComp);
//            documentManager.registerDocumentDataController("Device", dataControllerComp);
//            documentManager.registerDocumentValidator("Device", deviceValidatorComp);
        }
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
    }

    function fillContextMenuModel(){
        contextMenuModel.clear();

        if (container.commandsProvider.commandExists("Edit")){
            contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../" + Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal)});
        }

        let canRemoveSensor = PermissionsController.checkPermission("RemoveSensor");
        if (canRemoveSensor){
            contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../"  + Style.getIconPath("Icons/Remove", Icon.State.On, Icon.Mode.Normal)});
        }

        let ok = PermissionsController.checkPermission("ChangeSensor");
        if (!ok){
            ok = PermissionsController.checkPermission("ChangeSensorDescription");
        }

        if (ok){
            contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
        }
    }

//    function collectionUpdated(){
//        let notificationModel = container.baseCollectionView.commands.notificationModel;
//        if (notificationModel){
//            let counter = notificationModel.GetData("NewCount");
//            if (counter > 0){
//                if (counter > 99){
//                    counter = '99+'
//                }

//                container.commandsProvider.setCommandNotification("ShowNew", counter);
//            }
//            else{
//                container.commandsProvider.setCommandNotification("ShowNew", "");
//            }
//        }
//    }

    onHeadersChanged: {
        container.table.setColumnContentComponent(0, pairComp);
    }

    Component {
        id: deviceEditorComp;

        SoftwareProductCollectionView {

        }
    }

//    Component {
//        id: deviceEditorComp;

//        DeviceEditor {
//            id: deviceEditor;

//            commandsController: CommandsRepresentationProvider {
//                commandId: "Device";
//                uuid: deviceEditor.viewId;
//            }
//        }
//    }

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
        Item {
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

            Component.onCompleted: {
                let loader = parent;
                let tableCellDelegate = loader.parent;

                let rowIndex = tableCellDelegate.rowIndex;
                if (rowIndex >= 0){
                    let statusId = container.table.elements.GetData("StatusId", rowIndex);
                    image.source = deviceProductionStatus.getIconPath(statusId);
                }

                statusLable.text = tableCellDelegate.getValue();
            }
        }
    }
}
