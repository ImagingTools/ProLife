import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtguigql 1.0
import imtgui 1.0
import prolifeOrdersSdl 1.0

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
		
		GqlRequestDocumentDataController {
			id: requestDocumentDataController
			
			property OrderData orderData: documentModel;
			
			gqlGetCommandId: ProlifeOrdersSdlCommandIds.s_orderItem;
			gqlUpdateCommandId: ProlifeOrdersSdlCommandIds.s_orderUpdate;
			gqlAddCommandId: ProlifeOrdersSdlCommandIds.s_orderAdd;
			
			typeId: "Order";
			documentName: orderData ? orderData.m_orderId: "";
			documentDescription: orderData ? orderData.m_description: "";
			
			onSaved: {
				let orderProductsModel = documentModel.m_orderProducts;
				for (let i = 0; i < orderProductsModel.count; i++){
					let productItem = orderProductsModel.get(i).item;
					if (productItem.m_isNew){
						productItem.m_isNew = false;
						
						orderProductsModel.set(i, {item: productItem})
					}
				}
			}
			
			documentModelComp: Component {
				OrderData {}
			}
			
			payloadModel: OrderDataPayload {
				onFinished: {
					requestDocumentDataController.documentModel = m_orderData
				}
			}
		}
	}
}
