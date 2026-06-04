import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0
import prolifeLicensesSdl 1.0

DocumentCollectionViewDelegate {
	id: container;

	removeDialogTitle: qsTr("Removing the software instance");
	removeMessage: qsTr("Do you really want to remove this product? In case of deletion, it will disappear in all orders in which it is present.");

	function updateStateCustomCommands(selection, commandsController, elementsModel){
		let isEnabled = selection.length === 1;
		
		let createLicenseFileIsEnabled = isEnabled;
		if (createLicenseFileIsEnabled){
			// TODO: deviceId invalid!!
			let deviceId = elementsModel.getData(SoftwareProductItemTypeMetaInfo.s_hardwareLink, selection[0]);
			let licenseNumber = elementsModel.getData(SoftwareProductItemTypeMetaInfo.s_serialNumber, selection[0]);
			
			createLicenseFileIsEnabled = deviceId !== "" && licenseNumber !== "";
		}

		let splitEnabled = isEnabled;
		if (splitEnabled){
			let isMultiple = elementsModel.getData(SoftwareProductItemTypeMetaInfo.s_isMultiple, selection[0]);
			let productCount = elementsModel.getData(SoftwareProductItemTypeMetaInfo.s_productCount, selection[0]);
			
			// Can only split if it's a multi-product license with at least 2 licenses
			splitEnabled = isMultiple && productCount > 1;
		}
		
		// Enable Revoke if this is a parent license (has child licenses)
		// We'll enable it optimistically - the dialog will show if there are no children
		let revokeEnabled = isEnabled;
		
		if(commandsController){
			commandsController.setCommandIsEnabled("CreateLicenseFile", createLicenseFileIsEnabled);
			commandsController.setCommandIsEnabled("Split", splitEnabled);
			commandsController.setCommandIsEnabled("Revoke", revokeEnabled);
		}
	}
	
	function setupContextMenu(){
		let commandsController = collectionView.commandsController;
		if (commandsController){
			container.contextMenuModel.clear();
			
			let canEdit = commandsController.commandExists("Edit");
			let canRemove = commandsController.commandExists("Remove");
			
			if (canEdit){
				let index = container.contextMenuModel.insertNewItem();
				
				container.contextMenuModel.setData("id", "Edit", index);
				container.contextMenuModel.setData("name", qsTr("Edit"), index);
				container.contextMenuModel.setData("icon", "Icons/Edit", index);
			}
			
			if (canRemove){
				let index = container.contextMenuModel.insertNewItem();
				
				container.contextMenuModel.setData("id", "Remove", index);
				container.contextMenuModel.setData("name", qsTr("Remove"), index);
				container.contextMenuModel.setData("icon", "Icons/Delete", index);
			}
			
			container.contextMenuModel.refresh();
		}
	}
	
	onCommandActivated: {
		if (commandId === "Split"){
			let indexes = container.collectionView.table.getSelectedIndexes();
			if (indexes.length === 0){
				return;
			}
			
			let elementsModel = container.collectionView.table.elements;
			let licenseId = elementsModel.getData(SoftwareProductItemTypeMetaInfo.s_id, indexes[0]);
			let productCount = elementsModel.getData(SoftwareProductItemTypeMetaInfo.s_productCount, indexes[0]);
			
			ModalDialogManager.openDialog(splitLicenseDialogComp, {
				"licenseId": licenseId,
				"maxAvailableCount": productCount
			});
		}
		else if (commandId === "Revoke"){
			let indexes = container.collectionView.table.getSelectedIndexes();
			if (indexes.length === 0){
				return;
			}
			
			let elementsModel = container.collectionView.table.elements;
			let licenseId = elementsModel.getData(SoftwareProductItemTypeMetaInfo.s_id, indexes[0]);
			
			ModalDialogManager.openDialog(revokeLicenseDialogComp, {
				"parentLicenseId": licenseId
			});
		}
	}

	Component {
		id: splitLicenseDialogComp
		SplitLicenseDialog {
		}
	}

	Component {
		id: revokeLicenseDialogComp
		RevokeLicenseDialog {
		}
	}
}
