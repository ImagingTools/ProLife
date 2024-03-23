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

    dataControllerComp: Component {CollectionRepresentation {
            collectionId: "Orders";
        }
    }

    filterMenu.decorator: orderCollectionFilterComp;

    collectionFilter: OrderCollectionFilter {}

    commandsControllerComp: Component {CommandsRepresentationProvider {
            commandId: "Orders";
            uuid: container.viewId;
        }
    }

    commandsDelegateComp: Component {DocumentCollectionViewDelegate {
            collectionView: container;

            documentTypeId: "Order";
            viewTypeId: "OrderEditor";
        }
    }

    Component.onCompleted: {
        collectionFilter.setSortingOrder("DESC");
        collectionFilter.setSortingInfoId("LastModified");

        let documentManager = MainDocumentManager.getDocumentManager("Orders");
        if (documentManager){
            container.commandsDelegate.documentManager = documentManager;

            documentManager.registerDocumentView("Order", "OrderEditor", orderEditorComp);
            documentManager.registerDocumentDataController("Order", dataControllerComp);
            documentManager.registerDocumentValidator("Order", orderValidatorComp);
        }
    }

    Component {
        id: orderEditorComp;

        OrderEditor {
            id: orderEditor;

            commandsDelegateComp: Component {ViewCommandsDelegateBase {
                view: orderEditor;
            }
            }

            commandsControllerComp: Component {CommandsRepresentationProvider {
                    commandId: "Order";
                    uuid: orderEditor.viewId;
                }
            }
        }
    }

    Component {
        id: orderValidatorComp;

        OrderValidator {}
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

                commandId: "Accounts";

                fields: ["Id", "Name"];

                Component.onCompleted: {
                    let ok = PermissionsController.checkPermission("ViewAllOrders")

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
                accountsList.updateComboBoxModel();
            }

            Row {
                id: content;

                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;

                spacing: Style.size_mainMargin;

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
        id: dataControllerComp;

        GqlDocumentDataController {
            gqlGetCommandId: "OrderItem";
            gqlUpdateCommandId: "OrderUpdate";
            gqlAddCommandId: "OrderAdd";
        }
    }
}


