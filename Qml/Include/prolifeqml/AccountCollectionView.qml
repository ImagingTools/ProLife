import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import prolifeAccountsSdl 1.0

RemoteCollectionView {
	id: container;
	
	collectionId: "Accounts";
	
	commandsDelegateComp: Component {DocumentCollectionViewDelegate {
			collectionView: container;
			
			documentTypeIds: ["Account"]
			documentViewTypeIds: ["AccountEditor"]
			documentViewsComp: [accountEditorComp]
			documentDataControllersComp: [dataControllerComp]
			documentValidatorsComp: [accountValidatorComp]
			
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
					
					let ok = PermissionsController.checkPermission("ChangeAccountDescription");
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
		collectionFilter.setSortingOrder("ASC");
		collectionFilter.setSortingInfoId("Name");
	}
	
	Component {
		id: accountEditorComp
		
		DocumentView {
			id: accountEditor;
			commandsControllerComp: Component {CommandsPanelController {
					commandId: "Account";
					uuid: accountEditor.viewId
					commandsView: accountEditor.commandsView
				}
			}
			
			viewComp: Component {
				AccountEditor {}
			}
		}
	}
	
	Component {
		id: accountValidatorComp;
		
		DocumentValidator {
			property AccountData accountData: documentModel
			function isValid(data){
				if (!accountData){
					return false;
				}
				
				if (accountData.m_name === ""){
					data.message = qsTr("Account name cannot be empty");
					return false;
				}
				
				return true;
			}
		}
	}
	
	Component {
		id: dataControllerComp;
		
		GqlRequestDocumentDataController {
			id: requestDocumentDataController
			
			property AccountData accountData: documentModel;
			
			gqlGetCommandId: ProlifeAccountsSdlCommandIds.s_accountItem;
			gqlUpdateCommandId: ProlifeAccountsSdlCommandIds.s_accountUpdate;
			gqlAddCommandId: ProlifeAccountsSdlCommandIds.s_accountAdd;
			
			typeId: "Order";
			documentName: accountData ? accountData.m_name: "";
			documentDescription: accountData ? accountData.m_description: "";
			
			documentModelComp: Component {
				AccountData {}
			}
			
			payloadModel: AccountDataPayload {
				onFinished: {
					requestDocumentDataController.documentModel = m_accountData
				}
			}
		}
	}
}
