import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0

CollectionView {
    id: container;

    visibleMetaInfo: false;

    defaultSortHeaderIndex: 6;
    defaultOrderType: "DESC";
    filterMenuVisible: true;

    documentName: qsTr("Orders");
    commandId: "Orders";

    onDocumentManagerPtrChanged: {
        if (documentManagerPtr){
            documentManagerPtr.registerDocument("Order", orderEditorComp);
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
                accountFilterBlock.updateModel();
            }

            onWidthChanged: {
                console.log("Filter onWidthChanged", width);
                if (width - filtermenu.width <= accountFilterBlock.width){
                    accountFilterBlock.visible = false;
                }
                else{
                    if (accountFilterBlock.canViewAccountFilter){
                        accountFilterBlock.visible = true;
                    }
                }
            }

            Item {
                id: accountFilterBlock;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 10;

                width: canViewAccountFilter ? accountComboBox.width : 0;
                height: canViewAccountFilter ? filtermenu.height : 0;

                property bool canViewAccountFilter: false;

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ViewAllOrders")
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

                    backgroundColor: Style.baseColor;
                    currentIndex: 0;

                    radius: 3;

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

                width: 325;

                decoratorSource: Style.filterPanelDecoratorPath;

                onTextFilterChanged: {
                    parent.textFilterChanged(index, text);
                }

                onClosed: {
                    accountComboBox.currentIndex = 0;

                    parent.closed();
                }
            }

            signal textFilterChanged(int index, string text);
            signal closed();
        }
    }

    Component {
        id: orderEditorComp;

        OrderEditor {
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
}


