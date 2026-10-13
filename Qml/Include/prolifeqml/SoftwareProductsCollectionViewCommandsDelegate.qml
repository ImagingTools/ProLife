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

		if(commandsController){
			commandsController.setCommandIsEnabled("CreateLicenseFile", createLicenseFileIsEnabled);
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
}
