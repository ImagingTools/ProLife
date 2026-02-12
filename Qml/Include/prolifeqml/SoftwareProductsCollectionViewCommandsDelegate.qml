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
			let deviceId = elementsModel.getData(SoftwareProductItemTypeMetaInfo.s_hardwareId, selection[0]);
			let licenseNumber = elementsModel.getData(SoftwareProductItemTypeMetaInfo.s_serialNumber, selection[0]);
			
			createLicenseFileIsEnabled = deviceId !== "" && licenseNumber !== "";
		}
		
		let openOrderEnabled = isEnabled;
		if (openOrderEnabled){
			let orderUuid = elementsModel.getData(SoftwareProductItemTypeMetaInfo.s_orderUuid, selection[0]);
			if (orderUuid == "undefined" || orderUuid == ""){
				openOrderEnabled = false;
			}
		}
		
		let splitEnabled = isEnabled;
		if (splitEnabled){
			let isMultiple = elementsModel.getData(SoftwareProductItemTypeMetaInfo.s_isMultiple, selection[0]);
			let productCount = elementsModel.getData(SoftwareProductItemTypeMetaInfo.s_productCount, selection[0]);
			
			// Can only split if it's a multi-product license with at least 2 licenses
			splitEnabled = isMultiple && productCount > 1;
		}
		
		if(commandsController){
			commandsController.setCommandIsEnabled("OpenOrder", openOrderEnabled);
			commandsController.setCommandIsEnabled("CreateLicenseFile", createLicenseFileIsEnabled);
			commandsController.setCommandIsEnabled("Split", splitEnabled);
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
		if (commandId === "OpenOrder"){
			let indexes = container.collectionView.table.getSelectedIndexes();
			let elementsModel = container.collectionView.table.elements;
			let orderUuid = elementsModel.getData(SoftwareProductItemTypeMetaInfo.s_orderUuid, indexes[0]);
			if (orderUuid !== ""){
				MainDocumentManager.openDocument("Orders", orderUuid, "Order")
			}
		}
		else if (commandId === "Split"){
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
	}

	Component {
		id: splitLicenseDialogComp
		SplitLicenseDialog {
		}
	}
}
