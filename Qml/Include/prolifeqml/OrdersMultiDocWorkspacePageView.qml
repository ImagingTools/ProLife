import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtgui 1.0

MultiDocWorkspacePageView {
    id: multiDocPageView;

    anchors.fill: parent;

    Component.onCompleted: {
        documentManager.registerDocumentView("Order", "OrderEditor", orderEditorComp);
        documentManager.registerDocumentDataController("Order", dataControllerComp);
        documentManager.registerDocumentValidator("Order", orderValidatorComp);
    }

    Component {
        id: orderEditorComp;

        OrderEditor {
            id: orderEditor;

            commandsDelegateComp: Component {ViewCommandsDelegateBase {
                view: orderEditor;
            }
            }

            commandsControllerComp: Component {CommandsPanelController {
                    commandId: "Order";
                    uuid: orderEditor.viewId;
                    commandsView: orderEditor.commandsView;
                }
            }
        }
    }

    Component {
        id: orderValidatorComp;

        OrderValidator {}
    }

    Component {
        id: dataControllerComp;

        GqlDocumentDataController {
            gqlGetCommandId: "OrderItem";
            gqlUpdateCommandId: "OrderUpdate";
            gqlAddCommandId: "OrderAdd";

            onSaved: {
                if (documentModel.containsKey("OrderProducts")){
                    let orderProductsModel = documentModel.getData("OrderProducts");
                    for (let i = 0; i < orderProductsModel.getItemsCount(); i++){
                        let categoryId = orderProductsModel.getData("CategoryId", i);
                        let isNew = orderProductsModel.getData("IsNew", i);
                        if (isNew){
                            orderProductsModel.setData("IsNew", false, i);
                        }
                    }
                }
            }
        }
    }
}
