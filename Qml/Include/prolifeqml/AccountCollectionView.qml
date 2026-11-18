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
import imtbaseComplexCollectionFilterSdl 1.0

RemoteCollectionView {
	id: container;
	
	// The vendors of the hardware (manufacturers and suppliers) are the same companies as the customers
	property bool isVendorPage: false
	property string permissionArea: isVendorPage ? "/HardwareManagement" : "/AccountManagement"
	property string documentTypeId: isVendorPage ? "Vendor" : "Account"

	collectionId: isVendorPage ? "HardwareVendors" : "Accounts";
	gqlGetListCommandId: ProlifeAccountsSdlCommandIds.s_accountsList;

	permissionPaths: ({
		"AccountsList": permissionArea + "/View"
	})

	commandsDelegateComp: Component {DocumentCollectionViewDelegate {
			revisionsPermissionPath: container.permissionArea + "/Revisions";
			collectionView: container;
			
			documentTypeIds: [container.documentTypeId]
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
					
					let ok = PermissionsController.checkPermission(container.isVendorPage ? "/HardwareManagement/Edit/Change" : "/AccountManagement/Edit/Change/Description");
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
	
	documentCollectionFilter: null

	Component.onCompleted: {
		table.setSortingInfo(AccountItemTypeMetaInfo.s_name, "ASC");
		if (container.isVendorPage){
			registerFieldFilterDelegate("VendorRoles", vendorRolesDelegateFilterComp)
		}
	}

	FieldFilter {
		id: manufacturerRoleFilter
		m_fieldId: "IsManufacturer"
		m_filterValue: "true"
		m_filterValueType: "Bool"
		m_filterOperations: ["Equal"]
	}

	FieldFilter {
		id: supplierRoleFilter
		m_fieldId: "IsSupplier"
		m_filterValue: "true"
		m_filterValueType: "Bool"
		m_filterOperations: ["Equal"]
	}

	// Each role is a flag of its own; both picked keep the vendors that are both
	Component {
		id: vendorRolesDelegateFilterComp

		FieldFilterDelegate {
			objectName: "VendorRolesFilter"
			name: qsTr("Roles")

			Component.onCompleted: {
				createAndAddOption("Manufacturer", qsTr("Manufacturer"), "", true)
				createAndAddOption("Supplier", qsTr("Supplier"), "", true)
				setFieldFilterForOption("Manufacturer", manufacturerRoleFilter)
				setFieldFilterForOption("Supplier", supplierRoleFilter)
			}
		}
	}
	
	Component {
		id: accountEditorComp
		
		AccountEditor {
			id: accountEditor;
			isVendorPage: container.isVendorPage;
			commandsControllerComp: Component {GqlBasedCommandsController {
					typeId: container.documentTypeId;
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

				if (container.isVendorPage && !accountData.m_isManufacturer && !accountData.m_isSupplier){
					data.message = qsTr("Select the role of the vendor: manufacturer or supplier");
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
			
			getPermissionPath: container.permissionArea + "/View";
			addPermissionPath: container.permissionArea + "/Edit/Add";
			updatePermissionPath: container.permissionArea + "/Edit/Change";
			
			typeId: container.documentTypeId;
			collectionId: container.collectionId;
			documentName: accountData ? accountData.m_name: "";
			documentDescription: accountData ? accountData.m_description: "";
			
			documentModelComp: Component {
				AccountData {}
			}
		}
	}
}
