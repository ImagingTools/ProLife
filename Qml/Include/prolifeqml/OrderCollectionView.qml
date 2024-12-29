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

    visibleMetaInfo: false;

    collectionId: "Orders";

    filterMenu.decorator: orderCollectionFilterComp;

    collectionFilter: OrderCollectionFilter {}

    commandsDelegateComp: Component {DocumentCollectionViewDelegate {
            collectionView: container;

            documentTypeId: "Order";
            viewTypeId: "OrderEditor";

            function setupContextMenu(){
                let commandsController = collectionView.commandsController;
                if (commandsController){
                    contextMenuModel.clear();

                    let canEdit = commandsController.commandExists("Edit");
                    let canRemove = commandsController.commandExists("Remove");

                    if (canEdit){
                        let index = contextMenuModel.insertNewItem();

                        contextMenuModel.setData("Id", "Edit", index);
                        contextMenuModel.setData("Name", qsTr("Edit"), index);
                        contextMenuModel.setData("Icon", "Icons/Edit", index);
                    }

                    if (canRemove){
                        let index = contextMenuModel.insertNewItem();

                        contextMenuModel.setData("Id", "Remove", index);
                        contextMenuModel.setData("Name", qsTr("Remove"), index);
                        contextMenuModel.setData("Icon", "Icons/Delete", index);
                    }

                    let ok = PermissionsController.checkPermission("ChangeDescriptionForOrder");
                    if (ok){
                        let index = contextMenuModel.insertNewItem();

                        contextMenuModel.setData("Id", "SetDescription", index);
                        contextMenuModel.setData("Name", qsTr("Set Description"), index);
                        contextMenuModel.setData("Icon", "", index);
                    }

                    contextMenuModel.refresh();
                }
            }
        }
    }

    Component.onCompleted: {
        collectionFilter.setSortingOrder("DESC");
        collectionFilter.setSortingInfoId("LastModified");
    }

    Component {
        id: orderCollectionFilterComp;

        DecoratorBase {
            id: mainItem;

            width: baseElement ? baseElement.width: 0;
            height: 40;

            Component.onCompleted: {
                Events.subscribeEvent("OnLocalizationChanged", onLocalizationChanged);
            }

            Component.onDestruction: {
                Events.unSubscribeEvent("OnLocalizationChanged", onLocalizationChanged);
            }

            CollectionDataProvider {
                id: accountsList;

                commandId: "AccountsList";
                subscriptionCommandId: "OnAccountsCollectionChanged"

                fields: ["Id", "Name"];

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ViewAllOrders")

                    accountComboBox.visible = ok;

                    if (ok){
                        accountsList.updateModel();
                    }
                }

                onCollectionModelChanged: {
                    accountsList.collectionModel.insertNewItem(0);

                    accountsList.updateComboBoxModel();
                }

                function updateComboBoxModel(){
                    accountsList.collectionModel.setData("Id", "All");
                    accountsList.collectionModel.setData("Name", qsTr("All customers"))

                    accountComboBox.model = accountsList.collectionModel;
                }
            }

            function onLocalizationChanged(language){
                accountsList.updateComboBoxModel();
            }

            Row {
                id: content;

                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;

                spacing: Style.size_mainMargin;

                ComboBox {
                    id: accountComboBox;

                    width: 300;
                    height: filtermenu.height;

                    currentIndex: 0;

                    radius: 3;

                    shownItemsCount: 15;

                    onCurrentIndexChanged: {
                        if (accountComboBox.currentIndex > 0){
                            let value = accountComboBox.model.getData("Id", accountComboBox.currentIndex);
                            container.collectionFilter.setAccountFilter(value);
                        }
                        else{
                            container.collectionFilter.setAccountFilter("");
                        }

                        container.doUpdateGui();
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
}


