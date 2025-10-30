import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import prolifeqml 1.0
import prolifeSensorsSdl 1.0
import prolifeOrdersSdl 1.0
import imtlicProductsSdl 1.0
import imtlicLicensesSdl 1.0

ViewBase {
	id: deviceEditorContainer;
	
	anchors.fill: parent;
	
	property TreeItemModel accountsModel: TreeItemModel {}
	property TreeItemModel productsModel: TreeItemModel {}
	
	property alias orderComboBoxEnabled: orderCB.enabled;
	property alias deviceTypeComboBoxEnabled: productCB.enabled;
	
	property int radius: 3;
	property int spacing: Style.marginM;
	
	property int comboBoxHeight: 27;
	
	property DeviceData deviceData: model ? model : null;
	property bool isNew: false

	Component.onCompleted: {
		if (!CachedProductCollection.completed){
			CachedProductCollection.updateModel();
		}
		
		if (!CachedOrderCollection.completed){
			CachedOrderCollection.updateModel();
		}
	}
	
	onDeviceDataChanged: {
		if (!deviceData){
			return;
		}

		checkPermissions();
		
		let licenseGroupVisible = false
		if (deviceData.m_softwareBindingInfos){
			licenseInformationTable.table.elements = deviceData.m_softwareBindingInfos
			licenseGroupVisible = deviceData.m_softwareBindingInfos.count > 0
		}

		licenseInformationGroup.visible = licenseGroupVisible
	}

	function checkPermissions(){
		if (!deviceData){
			return;
		}

		let canAddSensor = PermissionsController.checkPermission("AddSensor");
		if (isNew && canAddSensor){
			descriptionInput.readOnly = false;
			serialNumberInput.readOnly = false;
			macAddressInput.readOnly = false;
			projectInput.readOnly = false;
			statusCB.changeable = true;
			productCB.changeable = true;
			orderCB.changeable = true;
			configurationCB.changeable = true;
		}
		else{
			let canChangeDescription = PermissionsController.checkPermission("ChangeDescriptionForSensor");
			descriptionInput.readOnly = !canChangeDescription;
			
			let canChangeSerialNumber = PermissionsController.checkPermission("ChangeSerialNumberForSensor");
			serialNumberInput.readOnly = !canChangeSerialNumber;
			
			let canChangeMacAddress = PermissionsController.checkPermission("ChangeMacAddress");
			macAddressInput.readOnly = !canChangeMacAddress;
			
			let canChangeOrder = PermissionsController.checkPermission("ChangeOrderForSensor");
			orderCB.changeable = canChangeOrder;
			
			let canChangeProductionStatus = PermissionsController.checkPermission("ChangeProductionStatus");
			statusCB.changeable = canChangeProductionStatus;
			
			let canChangeProject = PermissionsController.checkPermission("ChangeProjectForSensor");
			projectInput.readOnly = !canChangeProject;
			
			let canChangeConfiguration = PermissionsController.checkPermission("ChangeHardwareConfiguration");
			let canChangeDevice = PermissionsController.checkPermission("ChangeDeviceType");
			configurationCB.changeable = canChangeConfiguration && canChangeDevice;
			productCB.changeable = canChangeConfiguration && canChangeDevice;
			
			let ok =
				canChangeDescription ||
				canChangeSerialNumber ||
				canChangeMacAddress ||
				canChangeOrder||
				canChangeProductionStatus ||
				canChangeProject ||
				canChangeConfiguration ||
				canChangeDevice;
			
			if (commandsController){
				commandsController.setCommandVisible("Undo", ok);
				commandsController.setCommandVisible("Redo", ok);
				commandsController.setCommandVisible("Save", ok);
			}
		}
	}
	
	property bool dialogIsShown: false
	function checkFinishedStatus(){
		if (!deviceData || dialogIsShown){
			return;
		}
		
		if (macAddressInput.acceptableInput &&
				serialNumberInput.acceptableInput &&
				!ModalDialogManager.dialogIsOpened(confirmSetFinishedStatusDialogComp) &&
				PermissionsController.checkPermission("ChangeProductionStatus")){
			
			if (deviceEditorContainer.visible && deviceData.m_macAddress !== "" && deviceData.m_serialNumber !== "" && deviceData.m_productionStatus !== "Finished"){
				ModalDialogManager.openDialog(confirmSetFinishedStatusDialogComp);
			}
		}
	}
	
	Component {
		id: confirmSetFinishedStatusDialogComp;
		MessageDialog {
			title: qsTr("Confirm status");
			message: qsTr("Do you want to set the production state of the sensor to Finished ?");
			
			onFinished: {
				if (buttonId == Enums.yes){
					let finishedStatusIndex = productionStatus.getStatusIndex("Finished");
					statusCB.currentIndex = finishedStatusIndex;
				}
			}
			
			Component.onCompleted: {
				deviceEditorContainer.dialogIsShown = true
			}
		}
	}
	
	DeviceProductionStatus {
		id: productionStatus;
	}
	
	MouseArea {
		anchors.fill: parent;
		
		onClicked: {
			deviceEditorContainer.forceActiveFocus();
		}
	}
	
	function setReadOnly(readOnly){
		descriptionInput.readOnly = readOnly;
		serialNumberInput.readOnly = readOnly;
		macAddressInput.readOnly = readOnly;
		statusCB.changeable = !readOnly;
		productCB.changeable = !readOnly;
		orderCB.changeable = !readOnly;
		configurationCB.changeable = !readOnly;
	}
	
	function updateGui(){
		if (!deviceData){
			return;
		}
		
		descriptionInput.text = deviceData.m_description;
		serialNumberInput.text = deviceData.m_serialNumber;
		macAddressInput.text = deviceData.m_macAddress;
		projectInput.text = deviceData.m_project;
		
		statusCB.currentIndex = -1;
		
		let status = deviceData.m_productionStatus;
		let statusModel = statusCB.model;
		if (statusModel){
			let index = productionStatus.getStatusIndex(status);
			if (index >= 0){
				statusCB.currentIndex = index;
			}
		}
		
		productCB.currentIndex = -1;
		
		let productId = deviceData.m_deviceType;
		let productModel = productCB.model;
		if (productModel){
			for (let i = 0; i < productModel.getItemsCount(); i++){
				let id = productModel.getData(ProductItemTypeMetaInfo.s_id, i);
				if (id === productId){
					productCB.currentIndex = i;
					
					break;
				}
			}
		}
		
		configurationCB.currentIndex = -1;
		
		let licenseName = deviceData.m_licenseName;
		let model = configurationCB.model;
		if (model){
			for (let i = 0; i < model.getItemsCount(); i++){
				let id = model.getData(LicenseItemTypeMetaInfo.s_id, i);
				if (id === licenseName){
					configurationCB.currentIndex = i;
					
					break;
				}
			}
		}
		
		orderCB.currentIndex = -1;
		
		let orderId = deviceData.m_orderId;
		let ordersModel = orderCB.sourceModel;
		if (ordersModel){
			for (let i = 0; i < ordersModel.getItemsCount(); i++){
				let id = ordersModel.getData(OrderItemTypeMetaInfo.s_id, i);
				if (id === orderId){
					orderCB.currentIndex = i;
					break;
				}
			}
		}

		internalUseSwitchElementView.checked = deviceData.m_internalUse
	}
	
	function updateModel(){
		if (!deviceData){
			return;
		}
		
		if (productCB.currentIndex >= 0 && productCB.model){
			let selectedProductId = productCB.model.getData(ProductItemTypeMetaInfo.s_id, productCB.currentIndex);
			deviceData.m_deviceType = selectedProductId;
		}
		else{
			deviceData.m_deviceType = "";
		}
		
		let configurationExists = false;
		if (configurationCB.model){
			if (configurationCB.currentIndex >= 0){
				let configurationType = configurationCB.model.getData(LicenseItemTypeMetaInfo.s_id, configurationCB.currentIndex);
				deviceData.m_licenseName = configurationType;
				configurationExists = true;
			}
		}
		
		if (!configurationExists){
			deviceData.m_licenseName = "";
		}
		
		let canChangeOrder = PermissionsController.checkPermission("ChangeOrderForSensor");
		if (canChangeOrder){
			if (orderCB.currentIndex >= 0){
				let selectedOrderId = orderCB.sourceModel.getData(OrderItemTypeMetaInfo.s_id, orderCB.currentIndex);
				deviceData.m_orderId = selectedOrderId;
			}
			else{
				deviceData.m_orderId = "";
			}
		}
		
		deviceData.m_description = descriptionInput.text;
		deviceData.m_serialNumber = serialNumberInput.text;
		deviceData.m_macAddress = macAddressInput.text;
		deviceData.m_project = projectInput.text;
		
		if (statusCB.currentIndex >= 0 && statusCB.model){
			deviceData.m_productionStatus = productionStatus.getStatusId(statusCB.currentIndex);
		}
		else{
			deviceData.m_productionStatus = "";
		}

		deviceData.m_internalUse = internalUseSwitchElementView.checked
	}
	
	DocumentHistoryPanel {
		id: historyPanel;
		documentId: deviceEditorContainer.deviceData ? deviceEditorContainer.deviceData.m_id : "";
		collectionId: "Devices";
		editorFlickable: flickable;
	}
	
	CustomScrollbar {
		id: scrollbar;
		
		z: parent.z + 1;
		
		anchors.right: parent.right;
		anchors.top: flickable.top;
		anchors.bottom: flickable.bottom;
		
		secondSize: 10;
		targetItem: flickable;
		
		radius: 2;
	}
	
	CustomScrollbar{
		id: scrollHoriz;
		
		z: parent.z + 1;
		
		anchors.left: flickable.left;
		anchors.right: flickable.right;
		anchors.bottom: flickable.bottom;
		
		secondSize: 10;
		
		vertical: false;
		targetItem: flickable;
	}
	
	Flickable {
		id: flickable;
		
		anchors.left: parent.left;
		anchors.leftMargin: Style.marginXL;
		
		anchors.top: parent.top;
		anchors.topMargin: Style.marginXL;
		
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.marginXL;
		
		anchors.right: scrollbar.left;
		anchors.rightMargin: Style.marginXL;
		
		contentWidth: bodyColumn.width;
		contentHeight: Math.max(bodyColumn.height + 2 * Style.marginXL + 100, historyPanel.contentHeight + 2 * Style.marginXL);
		
		boundsBehavior: Flickable.StopAtBounds;
		
		clip: true;
		
		Column {
			id: bodyColumn;
			
			width: 700;
			
			spacing: Style.marginXL;
			
			GroupHeaderView {
				width: parent.width;
				
				title: qsTr("Device Information");
				groupView: deviceInformationGroup;
			}
			
			GroupElementView {
				id: deviceInformationGroup;
				
				width: parent.width;
				
				ComboBoxElementView {
					id: productCB;
					
					name: qsTr("Device Type");
					
					model: CachedProductCollection.hardwareProductsModel;
					
					nameId: ProductItemTypeMetaInfo.s_productName;
					
					KeyNavigation.tab: configurationCB;
					KeyNavigation.backtab: projectInput;
					isSelectionRequired: true;
					errorText: qsTr("Please select a device type");
					
					onCurrentIndexChanged: {
						let ok = false;
						if (productCB.currentIndex >= 0){
							let model = productCB.model.getData(ProductItemTypeMetaInfo.s_licenses, productCB.currentIndex);
							if (model){
								configurationCB.model = model;
								
								ok = true;
							}
						}
						
						if (!ok){
							configurationCB.model = 0;
						}
						
						configurationCB.currentIndex = -1;
						
						deviceEditorContainer.doUpdateModel();
					}
					
					onModelChanged: {
						deviceEditorContainer.doUpdateGui();
					}
				}
				
				ComboBoxElementView {
					id: configurationCB;
					name: qsTr("Hardware Configuration");
					nameId: LicenseItemTypeMetaInfo.s_licenseName
					KeyNavigation.tab: articleText;
					KeyNavigation.backtab: productCB;
					isSelectionRequired: true;
					errorText: qsTr("Please select a configuration");
					delegate:Component { FilterableComboBoxDelegate {
						width: configurationCB.width
						text: model[DeviceItemTypeMetaInfo.s_licenseName]
						comboBoxRef: configurationCB.cbRef
						description: model[DeviceItemTypeMetaInfo.s_licenseId]
					}
					}
					
					onCurrentIndexChanged: {
						if (currentIndex >= 0){
							if (model){
								articleText.text = model.getData(DeviceItemTypeMetaInfo.s_licenseId, currentIndex)
							}
							deviceEditorContainer.doUpdateModel();
						}
					}
				}
				
				TextInputElementView {
					id: articleText
					name: qsTr("Article")
					readOnly: true
					KeyNavigation.tab: descriptionInput
					KeyNavigation.backtab: configurationCB
				}
				
				TextInputElementView {
					id: descriptionInput;
					
					name: qsTr("Description");
					placeHolderText: qsTr("Enter description");
					
					onEditingFinished: {
						deviceEditorContainer.doUpdateModel();
					}
					
					KeyNavigation.tab: serialNumberInput;
					KeyNavigation.backtab: articleText;
				}
				
				TextInputElementView {
					id: serialNumberInput;
					
					name: qsTr("Serial Number");
					
					placeHolderText: qsTr("Enter serial number");
					
					onEditingFinished: {
						if (!deviceEditorContainer.deviceData){
							return
						}

						let serialNumber = deviceEditorContainer.deviceData.m_serialNumber
						deviceEditorContainer.doUpdateModel();

						if (serialNumber !== text){
							deviceEditorContainer.checkFinishedStatus();
						}
					}
					
					KeyNavigation.tab: macAddressInput;
					KeyNavigation.backtab: descriptionInput;
				}
				
				MacAddressElementView {
					id: macAddressInput;
					
					onEditingFinished: {
						if (!deviceEditorContainer.deviceData){
							return
						}
						
						let macAddress = deviceEditorContainer.deviceData.m_macAddress
						deviceEditorContainer.doUpdateModel();
						
						if (macAddress !== text){
							deviceEditorContainer.checkFinishedStatus();
						}
					}
					
					KeyNavigation.tab: orderCB;
					KeyNavigation.backtab: serialNumberInput;
				}
			}
			
			GroupHeaderView {
				id: additionalHeaderView;
				
				width: parent.width;
				
				groupView: additionalInformationGroup;
				title: qsTr("Additional Information");
			}
			
			GroupElementView {
				id: additionalInformationGroup;
				
				width: parent.width;
				
				FilterableComboBoxElementView {
					id: orderCB;
					
					name: qsTr("Order-ID");
					
					nameId: OrderItemTypeMetaInfo.s_orderId;
					
					filteringFields: [OrderItemTypeMetaInfo.s_orderId, OrderItemTypeMetaInfo.s_customerName];
					
					sourceModel: CachedOrderCollection.collectionModel;
					
					KeyNavigation.tab: statusCB;
					KeyNavigation.backtab: macAddressInput;
					
					delegate: Component {
						FilterableComboBoxDelegate {
							width: comboBoxRef ? comboBoxRef.width : 0;
							comboBoxRef: orderCB.cbRef;
							
							description: qsTr("Customer") + ": " + model[OrderItemTypeMetaInfo.s_customerName]
						}
					}
					
					onFinished: {
						deviceEditorContainer.doUpdateModel();
					}
					
					onModelChanged: {
						deviceEditorContainer.doUpdateGui();
					}
				}
				
				ClearableComboBoxElementView {
					id: statusCB;
					
					name: qsTr("Production Status");
					model: productionStatus.m_statusModel;
					nameId: "m_name";
					
					property bool blockingIndexChanged: false;
					
					KeyNavigation.tab: projectInput;
					KeyNavigation.backtab: orderCB;
					
					onCurrentIndexChanged: {
						deviceEditorContainer.doUpdateModel();
						
						if (statusCB.currentIndex >= 0){
						}
						else{
							statusCB.model = productionStatus.m_statusModel;
						}
					}
				}
				
				TextInputElementView {
					id: projectInput;
					
					name: qsTr("Project");
					placeHolderText: qsTr("Enter the project");
					
					readOnly: deviceEditorContainer.readOnly;
					
					KeyNavigation.tab: productCB;
					KeyNavigation.backtab: statusCB;
					
					onEditingFinished: {
						deviceEditorContainer.doUpdateModel();
					}
				}
				
				SwitchElementView {
					id: internalUseSwitchElementView
					name: qsTr("Internal Use");
					description: qsTr("Activate if the sensor is for internal use");
					readOnly: deviceEditorContainer.readOnly
					onCheckedChanged: {
						deviceEditorContainer.doUpdateModel()
					}
				}
			}
		
			GroupHeaderView {
				width: parent.width;
				
				groupView: licenseInformationGroup;
				title: qsTr("License Information");
				visible: licenseInformationGroup.visible
			}
			
			GroupElementView {
				id: licenseInformationGroup
				width: parent.width
				
				TableElementView {
					id: licenseInformationTable
					
					TreeItemModel {
						id: headersModel
						
						Component.onCompleted: {
							licenseInformationTable.updateHeaders()
						}
					}

					function updateHeaders(){
						headersModel.clear();
						
						headersModel.insertNewItem();
						
						headersModel.setData("id", "softwareName");
						headersModel.setData("name", qsTr("Software Name"));

						licenseInformationTable.table.headers = headersModel;
					}
				}
			}
		}
	}
}


