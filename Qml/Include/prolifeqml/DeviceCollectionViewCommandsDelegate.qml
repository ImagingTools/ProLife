import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0
import prolifeSensorsSdl 1.0
import imtguigql 1.0

DocumentCollectionViewDelegate {
	id: container;

	property bool filterByNewActive: false;
	property string filterLicense: "";

	documentTypeId: "Device";
	viewTypeId: "DeviceEditor";

	removeDialogTitle: qsTr("Removing the sensor");
	removeMessage: qsTr("Do you really want to remove this sensor? In case of deletion, it will disappear in all orders in which it is present.");

	Component.onCompleted: {
		Events.subscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("OnLocalizationChanged", container.onLocalizationChanged);
	}

	onCollectionIdChanged: {
		let documentManager = MainDocumentManager.getDocumentManager(container.collectionId);
		if (documentManager){
			container.documentManager = documentManager;

			documentManager.registerDocumentView("Device", "DeviceEditor", deviceEditorComp);
			documentManager.registerDocumentDataController("Device", dataControllerComp);
			documentManager.registerDocumentValidator("Device", deviceValidatorComp);
		}
	}

	Component {
		id: saveDialogComp;

		ErrorDialog {
			width: 300;
			title: qsTr("Warning message");
		}
	}

	Component {
		id: dataControllerComp;

		GqlRequestDocumentDataController {
			id: requestDocumentDataController

			gqlGetCommandId: ProlifeSensorsSdlCommandIds.s_deviceItem;
			gqlUpdateCommandId: ProlifeSensorsSdlCommandIds.s_deviceUpdate;
			gqlAddCommandId: ProlifeSensorsSdlCommandIds.s_deviceAdd;

			documentModelComp: Component {
				DeviceData {}
			}

			payloadModel: DeviceDataPayload {
				onFinished: {
					requestDocumentDataController.documentModel = m_deviceData
				}
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
			addItem("Encrypt", "Encrypt", "", true);
			addItem("NotEncrypt", "Not Encrypt", "", true);
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

				createLicenseFileRequest.send({"m_deviceId": hardwareId, "m_encrypt":encrypt})
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
				Component {CommandsPanelController {
					commandId: "Device";
					uuid: deviceEditor.viewId;
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

	function onLocalizationChanged(languageId){
	}

	function updateStateCustomCommands(selection, commandsController, elementsModel){
		if (!elementsModel){
			return;
		}

		let isEnabled = selection.length === 1;
		let macAddress = elementsModel.getData("MacAddress", selection[0]);

		let isOpenOrderEnabled = isEnabled;
		if (isOpenOrderEnabled){
			let orderId = elementsModel.getData("OrderId", selection[0]);
			isOpenOrderEnabled = isOpenOrderEnabled && orderId !== "";
		}

		let isBindEnabled = isEnabled;
		if (isBindEnabled){
			isBindEnabled = isBindEnabled && macAddress !== "";
		}

		let isTransferLicensesEnabled = isEnabled;
		if (isTransferLicensesEnabled){
			let status = elementsModel.getData("StatusId", selection[0])

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

				container.contextMenuModel.setData("Id", "Edit", index);
				container.contextMenuModel.setData("Name", qsTr("Edit"), index);
				container.contextMenuModel.setData("Icon", "Icons/Edit", index);
			}

			if (canRemove){
				let index = container.contextMenuModel.insertNewItem();

				container.contextMenuModel.setData("Id", "Remove", index);
				container.contextMenuModel.setData("Name", qsTr("Remove"), index);
				container.contextMenuModel.setData("Icon", "Icons/Delete", index);
			}

			container.contextMenuModel.refresh();
		}
	}

	onCommandActivated: {
		let indexes = container.collectionView.table.getSelectedIndexes();
		let elementsModel = container.collectionView.table.elements;

		if (commandId === "Bind"){
			let hardwareId = elementsModel.getData("Id", indexes[0]);
			let macAddress = elementsModel.getData("MacAddress", indexes[0]);

			let title = qsTr("Add license to sensor '%1'");
			title = title.replace("%1", macAddress);

			ModalDialogManager.openDialog(productPairEditorDialog, {"hardwareId": hardwareId, "title": title});
		}
		else if (commandId === "OpenOrder"){
			let orderId = elementsModel.getData("OrderUuid", indexes[0]);
			if (orderId !== ""){
				MainDocumentManager.openDocument("Orders", orderId, "Order", "OrderEditor")
			}
		}
		else if (commandId === "CreateLicenseFile"){
			let count = elementsModel.getData("SoftwareLinksCount", indexes[0])
			if (count <= 0){
				ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("No license is linked")})
				return;
			}

			let macAddress = elementsModel.getData("MacAddress", indexes[0])
			if (macAddress === ""){
				ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("The MAC-Address is not set")})
				return;
			}

			let serialNumber = elementsModel.getData("SerialNumber", indexes[0])
			if (serialNumber === ""){
				ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("The Serial Number is not set")})
				return;
			}

			let status = elementsModel.getData("Status", indexes[0])
			if (status !== "Finished"){
				ModalDialogManager.openDialog(errorDialogComp, {"message": qsTr("The production status must be 'Finished'")})
				return;
			}

			let data = macAddress.split(':');
			let fileName = data.join('_') + "_" + licenseFileController.defaultName;

			// licenseFileController.fileName = fileName;

			let hardwareId = elementsModel.getData("Id", indexes[0]);
			// licenseFileController.createLicenseFile(hardwareId);

			if (AuthorizationController.loggedUserIsSuperuser()){
				ModalDialogManager.openDialog(encryptPopupMenuDialog, {"hardwareId":hardwareId});
			}
			else{
				createLicenseFileRequest.send({"m_deviceId": hardwareId})
			}
		}
		else if (commandId === "TransferLicenses"){
			let count = elementsModel.getData("SoftwareLinksCount", indexes[0])
			if (count <= 0){
				ModalDialogManager.openDialog(transferErrorDialogComp, {})
				return;
			}

			let hardwareId = elementsModel.getData("Id", indexes[0]);
			let productId = elementsModel.getData("ProductUuid", indexes[0]);

			console.log("TransferLicenses", hardwareId, productId);
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
					decryptLicenseFileRequest.send({"m_fileData": encodedContent, "m_key": inputValue});
				}
			}
		}
	}

	GqlSdlRequestSender {
		id: decryptLicenseFileRequest;
		requestType: 1;
		gqlCommandId: ProlifeSensorsSdlCommandIds.s_decryptLicenseFile;
		inputObjectComp: Component {
			DecryptLicenseFileInput {
			}
		}

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

	GqlSdlRequestSender {
		id: createLicenseFileRequest;
		requestType: 1;
		gqlCommandId: ProlifeSensorsSdlCommandIds.s_createLicenseFile;
		inputObjectComp: Component {
			CreateLicenseFileInput {
			}
		}

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
					transferLicensesRequest.send({"m_fromDeviceId": dialog.fromDeviceId, "m_toDeviceId": dialog.toDeviceId});
				}
			}

			GqlSdlRequestSender {
				id: transferLicensesRequest;
				gqlCommandId: ProlifeSensorsSdlCommandIds.s_transferLicenses;
				inputObjectComp: Component {
					TransferLicensesInput {
					}
				}

				sdlObjectComp: Component {
					TransferLicensesPayload {
						onFinished: {
						}
					}
				}
			}

			contentComp: Component {
				Item {
					width: dialog.width;
					height: dialog.height - 100;

					Connections {
						target: dialog;

						function onStarted(){
							deviceCollectionView.collectionFilter.addAdditionalFilter("LicenseStatus", "WithoutLicense");
							deviceCollectionView.collectionFilter.addAdditionalFilter("ProductUuid", dialog.productUuid);
							deviceCollectionView.collectionFilter.addAdditionalFilter("ExcludeIds", dialog.fromDeviceId);
							deviceCollectionView.doUpdateGui();
						}
					}

					DeviceCollectionView {
						id: deviceCollectionView;
						commandsControllerComp: null;
						visibleMetaInfo: false;
						table.isMultiSelect: false;
						commandsDelegateComp: null;

						onSelectionChanged: {
							dialog.buttons.setButtonState(Enums.ok, selection.length > 0);
							dialog.toDeviceId = table.elements.getData("Id", selection[0]);
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
				buttonsModel.append({"Id": Enums.ok, "Name": qsTr("Transfer"), "Enabled": false});
				buttonsModel.append({"Id": Enums.cancel, "Name": qsTr("Close"), "Enabled": true});
			}
		}
	}
}
