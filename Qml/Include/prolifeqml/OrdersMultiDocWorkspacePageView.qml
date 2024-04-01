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
        console.log("OrdersMultiDoc onCompleted")
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
        id: dataControllerComp;

        GqlDocumentDataController {
            gqlGetCommandId: "OrderItem";
            gqlUpdateCommandId: "OrderUpdate";
            gqlAddCommandId: "OrderAdd";

            onSaved: {
                if (documentModel.ContainsKey("OrderProducts")){
                    let orderProductsModel = documentModel.GetData("OrderProducts");
                    for (let i = 0; i < orderProductsModel.GetItemsCount(); i++){
                        let categoryId = orderProductsModel.GetData("CategoryId", i);
                        if (categoryId === "Hardware"){
                            if (orderProductsModel.ContainsKey("IsNewDevice", i)){
                                orderProductsModel.RemoveData("IsNewDevice", i);
                            }
                        }
                    }
                }
            }
        }
    }
}
