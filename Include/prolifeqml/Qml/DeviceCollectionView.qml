import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

CollectionView {
    id: container;

    defaultSortHeaderIndex: 7;
    defaultOrderType: "DESC";
    filterMenuVisible: true;

    property MainDocumentManager mainDocumentManager: null;

    Component.onCompleted: {
        Events.subscribeEvent("DevicesCollectionUpdated", container.collectionUpdated);
        container.commandsDelegatePath = "qrc:/qml/ProLife/DeviceCollectionViewCommandsDelegate.qml";

        baseCollectionView.commands.fieldsData.push("OrderUuid");
        baseCollectionView.commands.fieldsData.push("StatusId");
        baseCollectionView.commands.fieldsData.push("Licenses");
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("DevicesCollectionUpdated", container.collectionUpdated);
    }

    onVisibleChanged: {
        if (container.visible){
            container.updateGui();
        }
    }

    filterMenu: Component {
        Item {
            id: mainItem;

            width: parent.width;
            height: 40;

            Component.onCompleted: {
                Events.subscribeEvent("OnLocalizationChanged", onLocalizationChanged);
            }

            Component.onDestruction: {
                Events.unSubscribeEvent("OnLocalizationChanged", onLocalizationChanged);
            }

            function onLocalizationChanged(language){
                mainItem.updateModel();
            }

            function updateModel(){
                console.log("updateModel");
                modelCategogy.Clear();

                let index = modelCategogy.InsertNewItem();
                modelCategogy.SetData("Id", "None", index);
                modelCategogy.SetData("Name", qsTr("Show All Sensors"), index);

                console.log("text", qsTr("Show All Sensors"));

                index = modelCategogy.InsertNewItem();
                modelCategogy.SetData("Id", "WithoutLicense", index);
                modelCategogy.SetData("Name", qsTr("Sensors without a license"), index);

                index = modelCategogy.InsertNewItem();
                modelCategogy.SetData("Id", "WithLicense", index);
                modelCategogy.SetData("Name", qsTr("Sensors with license"), index);

                licenseComboBox.model = modelCategogy;
            }

            onWidthChanged: {
                console.log("Filter onWidthChanged", width);
                if (width - filtermenu.width <= licenseFilterBlock.width){
                    licenseFilterBlock.visible = false;
                }
                else{
                    licenseFilterBlock.visible = true;
                }
            }

            TreeItemModel {
                id: modelCategogy;

                Component.onCompleted: {
                    mainItem.updateModel();
                }
            }

            Item {
                id: licenseFilterBlock;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 10;

                width: licenseComboBox.width;
                height: filtermenu.height;

                ComboBox {
                    id: licenseComboBox;

                    anchors.bottom: parent.bottom;
                    anchors.left: parent.left;

                    height: filtermenu.height;
                    width: 200;

                    backgroundColor: Style.baseColor;
                    currentIndex: 0;

                    radius: 3;

                    onCurrentIndexChanged: {
                        let objectFilter = container.modelFilter.GetData("LicenseFilter");
                        if (!objectFilter){
                            objectFilter = container.modelFilter.AddTreeModel("LicenseFilter")
                        }

                        if (licenseComboBox.currentIndex >= 0){
                            let value = licenseComboBox.model.GetData("Id", licenseComboBox.currentIndex);

                            objectFilter.SetData("Key", "Status");
                            objectFilter.SetData("Value", value);

                            container.updateGui();
                        }
                    }
                }
            }

            Text {
                id: titleInstanceId;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: filtermenu.left;
                anchors.rightMargin: 10;

                visible: container.commandsDelegate ? container.commandsDelegate.filterByNewActive : false;

                text: qsTr("Only new sensors!");

                color: Style.errorTextColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            FilterMenu {
                id: filtermenu

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;

                width: 325;

                decoratorSource: Style.filterPanelDecoratorPath;

                onTextFilterChanged: {
                    parent.textFilterChanged(index, text);
                }

                onClosed: {
                    licenseComboBox.currentIndex = 0;

                    parent.closed();
                }
            }

            signal textFilterChanged(int index, string text);
            signal closed();
        }
    }

    function fillContextMenuModel(){
        contextMenuModel.clear();
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../" + Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal)});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../"  + Style.getIconPath("Icons/Remove", Icon.State.On, Icon.Mode.Normal)});
        contextMenuModel.append({"Id": "SetDescription", "Name": qsTr("Set Description"), "IconSource": ""});
    }

    function collectionUpdated(){
        let notificationModel = container.baseCollectionView.commands.notificationModel;
        if (notificationModel){
            let counter = notificationModel.GetData("NewCount");
            if (counter > 0){
                if (counter > 99){
                    counter = '99+'
                }

                container.commandsProvider.setCommandNotification("ShowNew", counter);
            }
            else{
                container.commandsProvider.setCommandNotification("ShowNew", "");
            }
        }
    }

    function onCommandsModelChanged(){
        console.log("onCommandsModelChanged");
        let index = container.commandsProvider.getCommandIndex("ShowNew");
        if (index >= 0){
            container.commandsProvider.commandsModel.SetData("IsToggleable", true, index);
            container.commandsProvider.commandsModel.SetData("IsToggled", false, index);
        }

        index = container.commandsProvider.getCommandIndex("WithLicense");
        if (index >= 0){
            container.commandsProvider.commandsModel.SetData("IsToggleable", true, index);
            container.commandsProvider.commandsModel.SetData("IsToggled", false, index);
        }

        index = container.commandsProvider.getCommandIndex("WithoutLicense");
        if (index >= 0){
            container.commandsProvider.commandsModel.SetData("IsToggleable", true, index);
            container.commandsProvider.commandsModel.SetData("IsToggled", false, index);
        }

        container.commandsProvider.commandsModel.Refresh();
        container.commandsProvider.updateGui();
    }

    onHeadersChanged: {
        container.baseCollectionView.table.setColumnContentComponent(0, pairComp);
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
