import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0
import prolifeSensorsSdl 1.0
import imtguigql 1.0
import imtlicgui 1.0
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
	
	readonly property string createLicenseFileCommand: "CreateLicenseFile"
	readonly property string bindCommand: "Bind"
	readonly property string transferLicensesCommand: "TransferLicenses"
	
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
						if (commandId !== container.createLicenseFileCommand &&
							commandId !== container.transferLicensesCommand &&
							commandId !== container.bindCommand){
							return
						}
						
						let documentManager = MainDocumentManager.getDocumentManager(container.collectionId);
						if (!documentManager){
							ModalDialogManager.showErrorDialog(qsTr("Unable to handle command. Document manager is invalid"))
							return;
						}

						let documentModel = deviceEditor.deviceData
						if (!documentModel){
							ModalDialogManager.showInfoDialog(qsTr("Unable to handle command. Document model is invalid"));
							return;
						}

						let documentId =  deviceEditor.deviceData.m_id
						let isDirty = documentManager.documentIsDirty(documentId);
						let isNew = documentManager.documentIsNew(documentId);

						if (documentId === "" || isNew || isDirty){
							ModalDialogManager.showInfoDialog(qsTr("Please save the document first"));
							return;
						}

						if (commandId == container.createLicenseFileCommand){
							if (!documentModel.m_productionStatus){
								return;
							}

							if (documentModel.m_productionStatus !== "Finished"){
								ModalDialogManager.showInfoDialog(qsTr("The production status should be 'Finished'"))
								return;
							}
							
							container.onCreateLicenseFile(documentId)
						}
						else if (commandId == container.transferLicensesCommand){
							if (!documentModel.m_softwareBindingInfos ||
								!documentModel.m_productionStatus){
								return;
							}

							let count = documentModel.m_softwareBindingInfos.count
							if (count <= 0){
								ModalDialogManager.openDialog(transferErrorDialogComp, {})
								return;
							}
							
							if (documentModel.m_productionStatus !== "Defected"){
								ModalDialogManager.showInfoDialog(qsTr("The production status should be 'Defect'"))
								return;
							}
							
							let deviceType = documentModel.m_deviceType;
							container.onTransferLicenses(documentId, deviceType)
						}
						else if (commandId == container.bindCommand){
							let macAddress = documentModel.m_macAddress;
							if (!macAddressValidator.isValid(macAddress)){
								ModalDialogManager.showInfoDialog(qsTr("Please enter a valid MAC-Address"))
								return;
							}

							container.onBind(documentId, macAddress)
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
			commandsController.setCommandIsEnabled(bindCommand, isBindEnabled);
			commandsController.setCommandIsEnabled(createLicenseFileCommand, isEnabled);
			commandsController.setCommandIsEnabled(transferLicensesCommand, isTransferLicensesEnabled);
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
	
	function onBind(hardwareId, macAddress){
		let title = qsTr("Add license to sensor '%1'");
		title = title.replace("%1", macAddress);
		
		ModalDialogManager.openDialog(productPairEditorDialog, {"hardwareId": hardwareId, "title": title});
	}
	
	function onCreateLicenseFile(hardwareId){
		if (AuthorizationController.loggedUserIsSuperuser()){
			ModalDialogManager.openDialog(encryptPopupMenuDialog, {"hardwareId":hardwareId});
		}
		else{
			collectionView.commandsController.setCommandIsEnabled(createLicenseFileCommand, false)
			createLicenseFileInput.m_deviceId = hardwareId;
			createLicenseFileRequest.send(createLicenseFileInput)
		}
	}
	
	function onTransferLicenses(hardwareId, productId){
		ModalDialogManager.openDialog(deviceCollectionViewComp, {"fromDeviceId": hardwareId,"productUuid": productId})
	}
	
	onCommandActivated: {
		if (!collectionView){
			return
		}

		let indexes = collectionView.table.getSelectedIndexes();
		let elementsModel = collectionView.table.elements;
		
		if (commandId === bindCommand){
			let hardwareId = elementsModel.getData(DeviceItemTypeMetaInfo.s_id, indexes[0]);
			let macAddress = elementsModel.getData(DeviceItemTypeMetaInfo.s_macAddress, indexes[0]);

			onBind(hardwareId, macAddress)
		}
		else if (commandId === "OpenOrder"){
			let orderId = elementsModel.getData(DeviceItemTypeMetaInfo.s_orderUuid, indexes[0]);
			if (orderId !== ""){
				MainDocumentManager.openDocument("Orders", orderId, "Order", "OrderEditor")
			}
		}
		else if (commandId === createLicenseFileCommand){
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

			let hardwareId = elementsModel.getData(DeviceItemTypeMetaInfo.s_id, indexes[0]);

			onCreateLicenseFile(hardwareId)
		}
		else if (commandId === transferLicensesCommand){
			let count = elementsModel.getData(DeviceItemTypeMetaInfo.s_softwareLinksCount, indexes[0])
			if (count <= 0){
				ModalDialogManager.openDialog(transferErrorDialogComp, {})
				return;
			}
			
			let hardwareId = elementsModel.getData(DeviceItemTypeMetaInfo.s_id, indexes[0]);
			let productId = elementsModel.getData(DeviceItemTypeMetaInfo.s_productUuid, indexes[0]);
			
			onTransferLicenses(hardwareId, productId)
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
			if (Qt.platform.os === "web"){
				filePath = licenseFileDialog.file.toString()
			}
			else{
				filePath = licenseFileDialog.file.toString()
			}
			
			filePath = filePath.replace('file:///', '')
			
			if (Qt.platform.os === "web"){
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
					if (Qt.platform.os === "web"){
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
					if (!container.collectionView){
						return
					}
					
					if (Qt.platform.os === "web"){
						createLicenseFileIO.source = m_name;
					}
					
					let encodedStr = Qt.atob(m_data);
					createLicenseFileIO.write(encodedStr);
					
					if (container.collectionView.commandsController){
						let selectedIds = container.collectionView.getSelectedIds()
						container.collectionView.commandsController.setCommandIsEnabled(selectedIds.length === 1, true)
					}
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
						id: excludeFilter
						m_fieldId: "DocumentId"
						m_filterValue: dialog.fromDeviceId;
						m_filterValueType: "String"
						m_filterOperations: ["Not","Equal"]
					}
					
					Connections {
						target: dialog;
						
						function onStarted(){
							deviceCollectionView.collectionFilter.addFieldFilter(excludeFilter)
						}
					}
					
					Component {
						id: licensesDelegateFilterComp
						
						LicenseFilterDelegate {
							readOnly: true
							Component.onCompleted: {
								setSelectedIndex(0)
							}
						}
					}
					
					Component {
						id: customersDelegateFilterComp
						
						CustomerFilterDelegate {
						}
					}
					
					Component {
						id: productsDelegateFilterComp
						FieldFilterDelegate {
							id: productsDelegateFilter
							name: qsTr("Products")
							defaultFieldFilter.m_fieldId: "DeviceType"
							readOnly: true
							
							property string productUuid: dialog.productUuid
							onProductUuidChanged: {
								if (productsDelegateFilter.productUuid !== ""){
									optionsListAdapter.updateSelectedProduct()
								}
							}
							
							OptionsListAdapter {
								id: optionsListAdapter
								collectionModel: CachedProductCollection.hardwareProductsModel
								onCollectionModelChanged: {
									productsDelegateFilter.setOptionsList(m_options)
									
									if (productsDelegateFilter.productUuid !== ""){
										updateSelectedProduct()
									}
								}
								
								function updateSelectedProduct(){
									for (let i = 0; i < m_options.count; i++){
										let optionId = productsDelegateFilter.getOptionId(i)
										if (productsDelegateFilter.productUuid === optionId){
											productsDelegateFilter.setSelectedIndex(i)
											break
										}
									}
								}
							}
						}
					}
					
					DeviceCollectionView {
						id: deviceCollectionView;
						commandsControllerComp: null
						visibleMetaInfo: false
						table.isMultiSelect: false
						commandsDelegateComp: null
						canResetFilters: false
						
						onSelectionChanged: {
							if (selectedIds.length > 0){
								dialog.toDeviceId = selectedIds[0]
							}

							dialog.setButtonEnabled(Enums.ok, selectedIds.length > 0)
						}
						
						function registerFilters(){
							registerFieldFilterDelegate("SoftwareCount", licensesDelegateFilterComp)
							registerFieldFilterDelegate("DeviceType", productsDelegateFilterComp)
							registerFieldFilterDelegate("Customers", customersDelegateFilterComp)
							
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
