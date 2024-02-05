import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0

CollectionView {
    id: container;

    defaultSortHeaderIndex: 10;
    defaultOrderType: "DESC";
    visibleMetaInfo: false;
    filterMenuVisible: true;

    documentName: qsTr("Licenses");

    function fillContextMenuModel(){
        contextMenuModel.clear();
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../" +  Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal)});
    }

    Component.onCompleted: {
        container.commandsDelegatePath = "qrc:/qml/ProLife/SoftwareProductsCollectionViewCommandsDelegate.qml";

        baseCollectionView.commands.fieldsData.push("OrderUuid");
        baseCollectionView.commands.fieldsData.push("HardwareUuid");
        baseCollectionView.commands.fieldsData.push("InUse");

        container.commandId = "SoftwareProducts";
    }

    onHeadersChanged: {
        container.table.setColumnContentComponent(0, pairComp);

        let orderIndex = container.table.getHeaderIndex("OrderId");
        container.table.setColumnContentComponent(orderIndex, orderColumnContentComp);

        container.table.tableDecorator = tableDecoratorModel;
    }

    onDocumentManagerPtrChanged: {
        if (documentManagerPtr){
//            documentManagerPtr.setDocumentTitle(0, title);
            documentManagerPtr.registerDocument("SoftwareProduct", softwareEditorComp);
        }
    }

    Component {
        id: softwareEditorComp;

        SoftwareEditor {
        }
    }

    function onCommandsModelChanged(){
        let onlyPairedIndex = container.commandsProvider.getCommandIndex("OnlyPaired");

        if (onlyPairedIndex >= 0){
            container.commandsProvider.commandsModel.SetData("IsToggleable", true, onlyPairedIndex);
            container.commandsProvider.commandsModel.SetData("IsToggled", false, onlyPairedIndex);
        }

        let onlyUnpairedIndex = container.commandsProvider.getCommandIndex("OnlyUnpaired");
        if (onlyUnpairedIndex >= 0){
            container.commandsProvider.commandsModel.SetData("IsToggleable", true, onlyUnpairedIndex);
            container.commandsProvider.commandsModel.SetData("IsToggled", false, onlyUnpairedIndex);
        }

        container.commandsProvider.commandsModel.Refresh();
        container.commandsProvider.updateGui();
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

            onWidthChanged: {
                console.log("Filter onWidthChanged", width);
                if (width - filtermenu.width <= licenseFilterBlock.width + accountFilterBlock.width){
                    accountFilterBlock.visible = false;
                }
                else{
                    if (accountFilterBlock.canViewAccountFilter){
                        accountFilterBlock.visible = true;
                    }
                }
            }

            function onLocalizationChanged(language){
                mainItem.updateModel();
                accountFilterBlock.updateModel();
            }

            function updateModel(){
                modelCategogy.Clear();

                let index = modelCategogy.InsertNewItem();
                modelCategogy.SetData("Id", "All", index);
                modelCategogy.SetData("Name", qsTr("Show all licenses"), index);

                index = modelCategogy.InsertNewItem();
                modelCategogy.SetData("Id", "Paired", index);
                modelCategogy.SetData("Name", qsTr("Show only paired licenses"), index);

                index = modelCategogy.InsertNewItem();
                modelCategogy.SetData("Id", "NotPaired", index);
                modelCategogy.SetData("Name", qsTr("Show only not paired licenses"), index);

                index = modelCategogy.InsertNewItem();
                modelCategogy.SetData("Id", "InUse", index);
                modelCategogy.SetData("Name", qsTr("Show the licenses for which the file was created"), index);

                licenseComboBox.model = modelCategogy;
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

                    width: 300;
                    height: filtermenu.height;

                    currentIndex: 0;

                    radius: 3;

                    onCurrentIndexChanged: {
                        let objectFilter = container.modelFilter.GetData("ObjectFilter");
                        if (!objectFilter){
                            objectFilter = container.modelFilter.AddTreeModel("ObjectFilter")
                        }

                        if (licenseComboBox.currentIndex == 0){
                            objectFilter.SetData("LicenseFilter", "");
                        }
                        else if (licenseComboBox.currentIndex == 1){
                            objectFilter.SetData("LicenseFilter", "OnlyPaired");
                        }
                        else if (licenseComboBox.currentIndex == 2){
                            objectFilter.SetData("LicenseFilter", "OnlyUnpaired");
                        }
                        else if (licenseComboBox.currentIndex == 3){
                            objectFilter.SetData("LicenseFilter", "OnlyInUse");
                        }

                        container.updateGui();
                    }
                }
            }

            Item {
                id: accountFilterBlock;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: licenseFilterBlock.right;
                anchors.leftMargin: 10;

                width: accountComboBox.width;
                height: filtermenu.height;

                property bool canViewAccountFilter: false;

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ViewAllLicenses")
                    accountFilterBlock.canViewAccountFilter = ok;
                    accountFilterBlock.visible = ok;

                    if (ok){
                        accountsList.updateModel();
                    }
                }


                CollectionDataProvider {
                    id: accountsList;

                    commandId: "Accounts";

                    fields: ["Id", "Name"];

                    onCollectionModelChanged: {
                        accountsList.collectionModel.InsertNewItem(0);

                        accountFilterBlock.updateModel();
                    }
                }

                function updateModel(){
                    accountsList.collectionModel.SetData("Id", "All");
                    accountsList.collectionModel.SetData("Name", qsTr("All customers"))

                    accountComboBox.model = accountsList.collectionModel;
                }

                ComboBox {
                    id: accountComboBox;

                    anchors.bottom: parent.bottom;
                    anchors.left: parent.left;

                    height: filtermenu.height;
                    width: 200;

                    currentIndex: 0;

                    radius: 3;

                    shownItemsCount: 15;

                    onCurrentIndexChanged: {
                        let objectFilter = container.modelFilter.GetData("ObjectFilter");
                        if (!objectFilter){
                            objectFilter = container.modelFilter.AddTreeModel("ObjectFilter")
                        }

                        if (accountComboBox.currentIndex > 0){
                            let value = accountComboBox.model.GetData("Id", accountComboBox.currentIndex);
                            objectFilter.SetData("AccountFilter", value);
                        }
                        else{
                            objectFilter.SetData("AccountFilter", "");
                        }

                        container.updateGui();
                    }
                }
            }

            FilterMenu {
                id: filtermenu

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;

                decoratorSource: Style.filterPanelDecoratorPath;

                width: 325

                onTextFilterChanged: {
                    parent.textFilterChanged(index, text);
                }

                onClosed: {
                    licenseComboBox.currentIndex = 0;
                    accountComboBox.currentIndex = 0;

                    parent.closed();
                }
            }

            signal textFilterChanged(int index, string text);
            signal closed();
        }
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

                source: "../../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal);

                sourceSize.width: width;
                sourceSize.height: height;
            }

            Component.onCompleted: {
                let loader = parent;
                let tableCellDelegate = loader.cellDelegate;
                let value = tableCellDelegate.getValue();

                if (value === "NotPaired"){
                    image.source = "../../../../" + Style.getIconPath("Icons/Unlink", Icon.State.On, Icon.Mode.Normal);
                }
                else if (value === "IsPaired"){
                    image.source = "../../../../" + Style.getIconPath("Icons/Link", Icon.State.On, Icon.Mode.Normal);
                }
                else if (value === "InUse"){
                    image.source = "../../../../" + Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Normal);
                }
            }
        }
    }

    Component {
        id: orderColumnContentComp;
        Item {
            Image {
                id: image;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 5;

                width: 18;
                height: width;

                source: "../../../../" + Style.getIconPath("Icons/Alert", Icon.State.On, Icon.Mode.Normal);

                visible: false;

                sourceSize.width: width;
                sourceSize.height: height;
            }

            Text {
                id: lable;

                anchors.left: parent.left;
                anchors.right: parent.right;
                anchors.verticalCenter: parent.verticalCenter;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
                color: Style.textColor;

                elide: Text.ElideRight;
            }

            Component.onCompleted: {
                let loader = parent;
                let tableCellDelegate = loader.parent;

                let value = tableCellDelegate.getValue();
                let rowIndex = tableCellDelegate.rowIndex;

                if (rowIndex >= 0){
                    let orderUuid = container.table.elements.GetData("OrderUuid", rowIndex);
                    if (orderUuid === "undefined"){
                        image.visible = true;
                        lable.visible = false;
                    }
                    else{
                        lable.visible = true;
                        lable.text = value;
                    }
                }
            }
        }
    }

    TreeItemModel {
        id: tableDecoratorModel;

        Component.onCompleted: {
            var cellWidthModel = tableDecoratorModel.AddTreeModel("CellWidth");

            let index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", 40, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);

            index = cellWidthModel.InsertNewItem();
            cellWidthModel.SetData("Width", -1, index);
        }
    }
}


