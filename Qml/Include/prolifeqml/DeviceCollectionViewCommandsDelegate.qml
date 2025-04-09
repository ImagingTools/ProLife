import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0
import prolifeSensorsSdl 1.0
import imtguigql 1.0
import Qt.labs.platform 1.0
import imtbaseComplexCollectionFilterSdl 1.0

DocumentCollectionViewDelegate {
	id: container;
	
	documentTypeIds: ["Device"]
	documentViewTypeIds: ["DeviceEditor"]
	documentViewsComp: [deviceEditorComp]
	documentDataControllersComp: [dataControllerComp];
	documentValidatorsComp: [deviceValidatorComp];
	
	removeDialogTitle: qsTr("Removing the sensor");
	removeMessage: qsTr("Do you really want to remove this sensor? In case of deletion, it will disappear in all orders in which it is present.");
	
	Component {
		id: saveDialogComp;
		
		ErrorDialog {
			width: 300;
			title: qsTr("Warning message");
		}
	}
	
	// DeviceDocumentDataController.qml
	Component {
		DocumentDataController {
		}
	}
	
	Component {
		id: dataControllerComp;
		
		GqlRequestDocumentDataController {
			id: requestDocumentDataController
			
			gqlGetCommandId: ProlifeSensorsSdlCommandIds.s_deviceItem;
			gqlUpdateCommandId: ProlifeSensorsSdlCommandIds.s_deviceUpdate;
			gqlAddCommandId: ProlifeSensorsSdlCommandIds.s_deviceAdd;
			
			typeId: "Device";
			
			documentModelComp: Component {
				DeviceData {}
			}
		}
	}
	
	Component {
		id: deviceValidatorComp;
		
		DeviceValidator {
		}
	}
	
	PopupMenuModel {
		id: encryptPopupMenuModel;
		
		Component.onCompleted: {
			addItem("Encrypt", qsTr("Encrypted"), "", true);
			addItem("NotEncrypt", qsTr("Unencrypted"), "", true);
		}
	}
	
	Component {
		id: encryptPopupMenuDialog;
		
		PopupMenuDialog {
			itemWidth: 150;
			model: encryptPopupMenuModel;
			centered: true;
			hiddenBackground: false;
			
			property string hardwareId;
			
			onFinished: {
				if (commandId == ""){
					return;
				}
				
				let encrypt = true;
				if (commandId == "Encrypt"){
					encrypt = true;
				}
				else if (commandId == "NotEncrypt"){
					encrypt = false;
				}
				
				createLicenseFileInput.m_deviceId = hardwareId;
				createLicenseFileInput.m_encrypt = encrypt;
				
				createLicenseFileRequest.send(createLicenseFileInput)
			}
		}
	}
	
	MacAddressValidator {
		id: macAddressValidator;
	}
	
	Component {
		id: deviceEditorComp;
		
		DeviceEditor {
			id: deviceEditor;
			
			commandsControllerComp:
				Component { GqlBasedCommandsController {
					typeId: "Device";
				}}
			
			commandsDelegateComp: Component {ViewCommandsDelegateBase {
					view: deviceEditor;
					onCommandActivated: {
						if (commandId == "Bind"){
							let documentManager = MainDocumentManager.getDocumentManager(container.collectionId);
							if (documentManager){
								let documentData = documentManager.getDocumentDataByView(deviceEditor);
								if (!documentData){
									ModalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Unknown error")});
									
									return;
								}
								
								let documentIndex = documentData.documentIndex;
								if (documentIndex < 0){
									ModalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Unknown error")});
									
									return;
								}
								
								let isDirty = documentData.isDirty;
								let isNew = documentData.isNew;
								if (isNew || isDirty){
									ModalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Please save the document first"), "title": qsTr("Save document")});
									
									return;
								}
								
								let documentModel = documentData.documentDataController.documentModel;
								if (!documentModel){
									ModalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Unknown error")});
									
									return;
								}
								
								let macAddress = documentModel.m_macAddress;
								if (!macAddressValidator.isValid(macAddress)){
									ModalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Please enter a valid MAC-Address")});
									
									return;
								}
								
								let title = qsTr("Add license to sensor '%1'");
								title = title.replace("%1", macAddress);
								
								let documentId = documentData.documentId;
								if (documentId === ""){
									ModalDialogManager.openDialog(saveDialogComp, {"message": qsTr("Unknown error")});
									
									return;
								}
								
								ModalDialogManager.openDialog(productPairEditorDialog, {"hardwareId": documentId, "title": title});
							}
						}
					}
				}
			}
		}
	}
	
	function updateStateCustomCommands(selection, commandsController, elementsModel){
		if (!elementsModel){
			return;
		}
		
		let isEnabled = selection.length === 1;
		let macAddress = elementsModel.getData(DeviceItemTypeMetaInfo.s_macAddress, selection[0]);
		
		let isOpenOrderEnabled = isEnabled;
		if (isOpenOrderEnabled){
			let orderId = elementsModel.getData(DeviceItemTypeMetaInfo.s_deliveryId, selection[0]);
			isOpenOrderEnabled = isOpenOrderEnabled && orderId !== "";
		}
		
		let isBindEnabled = isEnabled;
		if (isBindEnabled){
			isBindEnabled = isBindEnabled && macAddress !== "";
		}
		
		let isTransferLicensesEnabled = isEnabled;
		if (isTransferLicensesEnabled){
			let status = elementsModel.getData(DeviceItemTypeMetaInfo.s_statusId, selection[0])
			
			isTransferLicensesEnabled = status === "Defected";
		}
		
		if(commandsController){
			commandsController.setCommandIsEnabled("OpenOrder", isOpenOrderEnabled);
			commandsController.setCommandIsEnabled("Bind", isBindEnabled);
			commandsController.setCommandIsEnabled("CreateLicenseFile", isEnabled);
			commandsController.setCommandIsEnabled("TransferLicenses", isTransferLicensesEnabled);
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
		let indexes = container.collectionView.table.getSelectedIndexes();
		let elementsModel = container.collectionView.table.elements;
		
		if (commandId === "Bind"){
			let hardwareId = elementsModel.getData(DeviceItemTypeMetaInfo.s_id, indexes[0]);
			let macAddress = elementsModel.getData(DeviceItemTypeMetaInfo.s_macAddress, indexes[0]);
			
			let title = qsTr("Add license to sensor '%1'");
			title = title.replace("%1", macAddress);
			
			ModalDialogManager.openDialog(productPairEditorDialog, {"hardwareId": hardwareId, "title": title});
		}
		else if (commandId === "OpenOrder"){
			let orderId = elementsModel.getData(DeviceItemTypeMetaInfo.s_orderUuid, indexes[0]);
			if (orderId !== ""){
				MainDocumentManager.openDocument("Orders", orderId, "Order", "OrderEditor")
			}
		}
		else if (commandId === "CreateLicenseFile"){
			let count = elementsModel.getData(DeviceItemTypeMetaInfo.s_softwareLinksCount, indexes[0])
			if (count <= 0){
				ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("No license is linked")})
				return;
			}
			
			let macAddress = elementsModel.getData(DeviceItemTypeMetaInfo.s_macAddress, indexes[0])
			if (macAddress === ""){
				ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("The MAC-Address is not set")})
				return;
			}
			
			let serialNumber = elementsModel.getData(DeviceItemTypeMetaInfo.s_serialNumber, indexes[0])
			if (serialNumber === ""){
				ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("The Serial Number is not set")})
				return;
			}
			
			let status = elementsModel.getData(DeviceItemTypeMetaInfo.s_status, indexes[0])
			if (status !== "Finished"){
				ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("The production status must be 'Finished'")})
				return;
			}
			
			let data = macAddress.split(':');
			let fileName = data.join('_') + "_" + licenseFileController.defaultName;
			
			let hardwareId = elementsModel.getData(DeviceItemTypeMetaInfo.s_id, indexes[0]);
			
			if (AuthorizationController.loggedUserIsSuperuser()){
				ModalDialogManager.openDialog(encryptPopupMenuDialog, {"hardwareId":hardwareId});
			}
			else{
				createLicenseFileInput.m_deviceId = hardwareId;
				createLicenseFileRequest.send(createLicenseFileInput)
			}
		}
		else if (commandId === "TransferLicenses"){
			let count = elementsModel.getData(DeviceItemTypeMetaInfo.s_softwareLinksCount, indexes[0])
			if (count <= 0){
				ModalDialogManager.openDialog(transferErrorDialogComp, {})
				return;
			}
			
			let hardwareId = elementsModel.getData(DeviceItemTypeMetaInfo.s_id, indexes[0]);
			let productId = elementsModel.getData(DeviceItemTypeMetaInfo.s_productUuid, indexes[0]);
			
			ModalDialogManager.openDialog(deviceCollectionViewComp, {"fromDeviceId": hardwareId,"productUuid": productId});
		}
		else if (commandId === "DecryptFile"){
			licenseFileDialog.open();
		}
	}
	
	FileDialog {
		id: licenseFileDialog;
		title: qsTr("Select license file")
		fileMode: FileDialog.OpenFile
		nameFilters: ["License files (*.lic)"]
		
		onAccepted: {
			let filePath;
			if (Qt.platform.os == "web"){
				filePath = licenseFileDialog.file.toString()
			}
			else{
				filePath = licenseFileDialog.file.toString()
			}
			
			filePath = filePath.replace('file:///', '')
			
			if (Qt.platform.os == "web"){
				let reader = new FileReader()
				
				reader.readAsDataURL(filePath)
				
				reader.onload = function(){
					let encodedContentWithHeader = reader.result
					let encodedContent = encodedContentWithHeader.replace(/^.{0,}base64,/, '')
					
					ModalDialogManager.openDialog(enterKeyDialog, {"encodedContent": encodedContent});
				}.bind(this)
			}
			else {
				fileIO.source = filePath
				let fileData = fileIO.read()
				let encodedData = Qt.btoa(fileData);
				
				ModalDialogManager.openDialog(enterKeyDialog, {"encodedContent": encodedData});
			}
		}
		
		FileIO {
			id: fileIO
		}
	}
	
	Component {
		id: enterKeyDialog;
		InputDialog {
			title: qsTr("Decryption key");
			message: qsTr("Enter the decryption key");
			
			property string encodedContent;
			
			onFinished: {
				if (buttonId == Enums.ok){
					decryptLicenseFileInput.m_fileData = encodedContent
					decryptLicenseFileInput.m_key = inputValue
					decryptLicenseFileRequest.send(decryptLicenseFileInput);
				}
			}
		}
	}
	
	DecryptLicenseFileInput {
		id: decryptLicenseFileInput;
	}
	
	GqlSdlRequestSender {
		id: decryptLicenseFileRequest;
		requestType: 1;
		gqlCommandId: ProlifeSensorsSdlCommandIds.s_decryptLicenseFile;
		
		sdlObjectComp: Component {
			DecryptLicenseFilePayload {
				onFinished: {
					if (Qt.platform.os == "web"){
						decryptFileIO.source = m_fileName;
					}
					
					let encodedStr = Qt.atob(m_decryptedData);
					decryptFileIO.write(encodedStr);
				}
			}
		}
		
		FileIO {
			id: decryptFileIO;
		}
	}
	
	Component {
		id: errorDialogComp;
		
		ErrorDialog {
			width: 450;
			title: qsTr("The license file could not be created");
		}
	}
	
	Component {
		id: transferErrorDialogComp;
		
		ErrorDialog {
			width: 450;
			title: qsTr("License transfer error");
			message: qsTr("No license is linked");
		}
	}
	
	LicenseFileController {
		id: licenseFileController;
	}
	
	Component {
		id: productPairEditorDialog;
		
		HardwareProductBindingDialog {
			id: dialog;
			
			onSaved: {
				container.collectionView.doUpdateGui();
			}
		}
	}
	
	Component {
		id: requestMessageDialog
		MessageDialog {
			title: qsTr("Sending a request");
			message: qsTr("Send a request to the administrator's email address requesting the transfer of licenses ?")

			onFinished: {
				if (buttonId == Enums.yes){
					transferLicensesRequest2.send(transferLicensesInput)
				}
			}
		}
	}
	
	CreateLicenseFileInput {
		id: createLicenseFileInput;
	}
	
	GqlSdlRequestSender {
		id: createLicenseFileRequest;
		requestType: 1;
		gqlCommandId: ProlifeSensorsSdlCommandIds.s_createLicenseFile;
		
		sdlObjectComp: Component {
			CreateLicenseFilePayload {
				onFinished: {
					if (Qt.platform.os == "web"){
						createLicenseFileIO.source = m_name;
					}
					
					let encodedStr = Qt.atob(m_data);
					createLicenseFileIO.write(encodedStr);
				}
			}
		}
		
		FileIO {
			id: createLicenseFileIO;
		}
	}
	
	TransferLicensesInput {
		id: transferLicensesInput;
	}
	
	GqlSdlRequestSender {
		id: transferLicensesRequest;
		gqlCommandId: ProlifeSensorsSdlCommandIds.s_transferLicenses;
		
		sdlObjectComp: Component {
			TransferLicensesPayload {
				onFinished: {
					if (m_ok){
						ModalDialogManager.showInfoDialog(qsTr("The licenses were successfully transferred"));
					}
					if (m_limit && !m_ok){
						ModalDialogManager.openDialog(requestMessageDialog, {})
					}
				}
			}
		}
	}
	
	GqlSdlRequestSender {
		id: transferLicensesRequest2;
		gqlCommandId: ProlifeSensorsSdlCommandIds.s_requestTransferLicenses;
		
		sdlObjectComp: Component {
			RequestTransferLicensesPayload {
				onFinished: {
					if (m_result){
						ModalDialogManager.showInfoDialog(qsTr("The license transfer request was successfully sent to the administrator's email address"));
					}
					else{
						ModalDialogManager.showErrorDialog(qsTr("Error when sending the request, please try again later"));
					}
				}
			}
		}
	}
	
	Component {
		id: deviceCollectionViewComp;
		
		Dialog {
			id: dialog;
			title: qsTr("Select device for license transfer");
			canMove: false;
			width: ModalDialogManager.activeView.width - 100;
			height: ModalDialogManager.activeView.height - 100;
			
			property string productUuid;
			property string fromDeviceId;
			property string toDeviceId;
			
			onFinished: {
				if (buttonId === Enums.ok){
					transferLicensesInput.m_fromDeviceId = dialog.fromDeviceId;
					transferLicensesInput.m_toDeviceId = dialog.toDeviceId;
					transferLicensesRequest.send(transferLicensesInput);
				}
			}
			
			contentComp: Component {
				Item {
					width: dialog.width;
					height: dialog.height - 100;
					
					FieldFilter {
						id: licenseFilter
						m_fieldId: "SoftwareCount"
						m_filterValue: '0';
						m_filterValueType: "Integer"
						m_filterOperations: ["Equal"]
					}
					
					FieldFilter {
						id: productFilter
						m_fieldId: "DeviceType"
						m_filterValue: dialog.productUuid;
						m_filterValueType: "String"
						m_filterOperations: ["Equal"]
					}
					
					FieldFilter {
						id: excludeFilter
						m_fieldId: "DocumentId"
						m_filterValue: dialog.fromDeviceId;
						m_filterValueType: "String"
						m_filterOperations: ["Not","Equal"]
					}
					
					Connections {
						target: dialog;
						
						function onStarted(){
							deviceCollectionView.collectionFilter.addFieldFilter(licenseFilter)
							deviceCollectionView.collectionFilter.addFieldFilter(productFilter)
							deviceCollectionView.collectionFilter.addFieldFilter(excludeFilter)
						}
					}
					
					DeviceCollectionView {
						id: deviceCollectionView;
						commandsControllerComp: null;
						visibleMetaInfo: false;
						table.isMultiSelect: false;
						commandsDelegateComp: null;
						
						onSelectionChanged: {
							dialog.setButtonEnabled(Enums.ok, selection.length > 0)
							dialog.toDeviceId = table.elements.getData(DeviceItemTypeMetaInfo.s_id, selection[0]);
						}
						
						Component.onCompleted: {
							filterMenu.decorator = filterComp;
						}
						
						Component {
							id: filterComp;
							DeviceCollectionFilterDecorator {
								licenseCb.currentIndex: 1;
								licenseCb.changeable: false;
							}
						}
					}
				}
			}
			
			Component.onCompleted: {
				addButton(Enums.ok, qsTr("Transfer"), false)
				addButton(Enums.cancel, qsTr("Close"), true)
			}
		}
	}
}
