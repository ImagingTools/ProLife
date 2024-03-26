import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtdocgui 1.0

RemoteCollectionView {
    id: container;

    collectionId: "SoftwareProducts";
    collectionFilter: LicenseCollectionFilter {}

    filterMenu.decorator: licenseCollectionFilterComp;

    dataControllerComp:
        Component {
        CollectionRepresentation {
            collectionId: "SoftwareProducts";

            Component.onCompleted: {
                additionalFieldIds.push("OrderUuid");
                additionalFieldIds.push("HardwareUuid");
                additionalFieldIds.push("InUse");
                additionalFieldIds.push("ProductUuid");
                additionalFieldIds.push("CustomerUuid");
            }
        }
    }

    commandsControllerComp: Component {CommandsRepresentationProvider {
            commandId: "SoftwareProducts";
            uuid: container.viewId;
        }
    }

    commandsDelegateComp: Component {SoftwareProductsCollectionViewCommandsDelegate {
            collectionView: container;
        }
    }

    visibleMetaInfo: false;

    onHeadersChanged: {
        if (container.table.headers.GetItemsCount() > 0){
            container.table.setColumnContentComponent(0, pairComp);

            let orderIndex = container.table.getHeaderIndex("OrderId");
            container.table.setColumnContentComponent(orderIndex, orderColumnContentComp);
            container.table.tableDecorator = tableDecoratorModel;
        }
    }

    Component.onCompleted: {
        collectionFilter.setSortingOrder("DESC");
        collectionFilter.setSortingInfoId("LastModified");

        registerDocumentInfo();
    }

    function registerDocumentInfo(){
        let documentManager = MainDocumentManager.getDocumentManager("SoftwareProducts");
        if (documentManager){
            container.commandsDelegate.documentManager = documentManager;
            documentManager.registerDocumentView("SoftwareProduct", "SoftwareProductEditor", softwareEditorComp);
            documentManager.registerDocumentDataController("SoftwareProduct", dataControllerComp);
            documentManager.registerDocumentValidator("SoftwareProduct", licenseValidatorComp);
        }
    }

    Component {
        id: softwareEditorComp;

        SoftwareEditor {
            id: softwareEditor;

            commandsDelegateComp: Component {ViewCommandsDelegateBase {
                view: softwareEditor;
            }
            }

            commandsControllerComp:
                Component {CommandsRepresentationProvider {
                    commandId: "SoftwareProduct";
                    uuid: softwareEditor.viewId;
                }
            }
        }
    }

    Component {
        id: dataControllerComp;

        GqlDocumentDataController {
            gqlGetCommandId: "SoftwareProductItem";
            gqlUpdateCommandId: "SoftwareProductUpdate";
            gqlAddCommandId: "SoftwareProductAdd";
        }
    }

    Component {
        id: licenseValidatorComp;

        LicenseValidator {
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
        id: licenseCollectionFilterComp;

        DecoratorBase {
            id: mainItem;

            width: baseElement ? baseElement.width: 0;
            height: 40;

            Component.onCompleted: {
                Events.subscribeEvent("OnLocalizationChanged", onLocalizationChanged);
                checkWidth();
            }

            Component.onDestruction: {
                Events.unSubscribeEvent("OnLocalizationChanged", onLocalizationChanged);
            }

            onWidthChanged: {
                checkWidth();
            }

            CollectionDataProvider {
                id: accountsList;

                commandId: "Accounts";

                fields: ["Id", "Name"];

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ViewAllLicenses")

                    accountComboBox.visible = ok;

                    if (ok){
                        accountsList.updateModel();
                    }
                }

                onCollectionModelChanged: {
                    accountsList.collectionModel.InsertNewItem(0);

                    accountsList.updateComboBoxModel();
                }

                function updateComboBoxModel(){
                    accountsList.collectionModel.SetData("Id", "All");
                    accountsList.collectionModel.SetData("Name", qsTr("All customers"))

                    accountComboBox.model = accountsList.collectionModel;
                }
            }

            function onLocalizationChanged(language){
                mainItem.updateModel();

                accountsList.updateComboBoxModel();
            }

            function checkWidth(){
                if (width - filtermenu.width <= content.width + 2 * content.spacing){
                    content.visible = false;
                }
                else{
                    content.visible = true;
                }
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

            Row {
                id: content;

                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;

                spacing: Style.size_mainMargin;

                ComboBox {
                    id: licenseComboBox;

                    width: 300;
                    height: filtermenu.height;

                    currentIndex: 0;

                    radius: 3;

                    onCurrentIndexChanged: {
                        if (licenseComboBox.currentIndex == 0){
                            container.collectionFilter.setLicenseFilter("");
                        }
                        else if (licenseComboBox.currentIndex == 1){
                            container.collectionFilter.setLicenseFilter("OnlyPaired");
                        }
                        else if (licenseComboBox.currentIndex == 2){
                            container.collectionFilter.setLicenseFilter("OnlyUnpaired");
                        }
                        else if (licenseComboBox.currentIndex == 3){
                            container.collectionFilter.setLicenseFilter("OnlyInUse");
                        }
                    }
                }

                ComboBox {
                    id: accountComboBox;

                    width: 200;
                    height: filtermenu.height;

                    currentIndex: 0;

                    radius: 3;

                    shownItemsCount: 15;

                    onCurrentIndexChanged: {
                        if (accountComboBox.currentIndex > 0){
                            let value = accountComboBox.model.GetData("Id", accountComboBox.currentIndex);
                            container.collectionFilter.setAccountFilter(value);
                        }
                        else{
                            container.collectionFilter.setAccountFilter("");
                        }
                    }
                }
            }

            FilterPanelDecorator {
                id: filtermenu

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;

                baseElement: mainItem.baseElement;

                width: 325;
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


