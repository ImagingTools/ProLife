import QtQuick 2.12
import imtgui 1.0
import Acf 1.0

CollectionView {
    id: container;

    visibleMetaInfo: false;

    defaultSortHeaderIndex: 6;
    defaultOrderType: "DESC";

    onVisibleChanged: {
        if (container.visible){
            container.updateGui();
        }
    }

    function fillContextMenuModel(){
        contextMenuModel.clear();
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../Icons/Light/Edit_On_Normal.svg"});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../Icons/Light/Remove_On_Normal.svg"});
        contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
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
