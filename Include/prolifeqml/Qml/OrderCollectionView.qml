import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtqml 1.0

CollectionView {
    id: container;

    visibleMetaInfo: false;

    defaultSortHeaderIndex: 6;
    defaultOrderType: "DESC";
    filterMenuVisible: true;

    onVisibleChanged: {
        if (container.visible){
            container.updateGui();
        }
    }

    function fillContextMenuModel(){
        contextMenuModel.clear();
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../" + Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal)});

        let canRemoveOrder = PermissionsController.checkPermission("RemoveOrder");
        if (canRemoveOrder){
            contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../" + Style.getIconPath("Icons/Remove", Icon.State.On, Icon.Mode.Normal)});
        }

        let canChangeOrder = PermissionsController.checkPermission("ChangeOrder");
        if (canChangeOrder){
            contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
        }
    }

    Component {
        id: statusComp;
        Item {
            Image {
                id: image;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 5;

                width: 18;
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
                statusLable.text = tableCellDelegate.getValue();

                image.source = deviceProductionStatus.getIconPath(statusLable.text);
            }
        }
    }
}
