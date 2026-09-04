import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
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

	permissionPaths: ({
		"AccountsList": "/AccountManagement/ViewAccounts"
	})

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
						
						contextMenuModel.setData("id", "Edit", index);
						contextMenuModel.setData("name", qsTr("Edit"), index);
						contextMenuModel.setData("icon", "Icons/Edit", index);
					}
					
					if (canRemove){
						let index = contextMenuModel.insertNewItem();
						
						contextMenuModel.setData("id", "Remove", index);
						contextMenuModel.setData("name", qsTr("Remove"), index);
						contextMenuModel.setData("icon", "Icons/Delete", index);
					}
					
					let ok = PermissionsController.checkPermission("/AccountManagement/EditAccount/ChangeAccount/ChangeAccountDescription");
					if (ok){
						let index = contextMenuModel.insertNewItem();
						
						contextMenuModel.setData("id", "SetDescription", index);
						contextMenuModel.setData("name", qsTr("Set Description"), index);
						contextMenuModel.setData("icon", "", index);
					}
					
					contextMenuModel.refresh();
				}
			}
		}
	}
	
	Component.onCompleted: {
		table.setSortingInfo(AccountItemTypeMetaInfo.s_name, "ASC");
	}
	
	Component {
		id: accountEditorComp
		
		AccountEditor {
			id: accountEditor;
			commandsControllerComp: Component {GqlBasedCommandsController {
					typeId: "Account";
				}
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
			
			getPermissionPath: "/AccountManagement/ViewAccounts";
			addPermissionPath: "/AccountManagement/EditAccount/AddAccount";
			updatePermissionPath: "/AccountManagement/EditAccount/ChangeAccount";
			
			typeId: "Account";
			documentName: accountData ? accountData.m_name: "";
			documentDescription: accountData ? accountData.m_description: "";
			
			documentModelComp: Component {
				AccountData {}
			}
		}
	}
}
