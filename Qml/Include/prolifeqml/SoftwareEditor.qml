import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import prolifeqml 1.0
import prolifeLicensesSdl 1.0

ViewBase {
	id: root;
	
	property TreeItemModel licensesModel: TreeItemModel{}
	property TreeItemModel productsModel: TreeItemModel{}
	
	property var productLicensesModel: TreeItemModel{}
	
	property string alertMessage: "";
	
	property SoftwareProductData softwareProductData: model ? model : null;
	
	Component.onCompleted: {
		if (!CachedProductCollection.completed){
			CachedProductCollection.updateModel();
		}
		
		if (!CachedOrderCollection.completed){
			CachedOrderCollection.updateModel();
		}
	}
	
	onVisibleChanged: {
		if (visible){
			checkInUse()
		}
		else{
			setAlertPanel(undefined);
		}
	}
	
	onSoftwareProductDataChanged: {
		checkPermissions();
		checkInUse();
	}
	
	Component {
		id: alertComp;
		AlertMessage {
			message: qsTr("The product cannot be edited as it is in use.");
		}
	}
	
	function checkPermissions(){
		if (!softwareProductData){
			return;
		}
		
		let softwareId = softwareProductData.m_id;
		
		let canAddLicense = PermissionsController.checkPermission("AddLicense");
		if (softwareId === "" && canAddLicense){
			projectInput.readOnly = false;
			
			ordersCB.changeable = true;
			productCB.changeable = true;
			licenseCB.changeable = true;
			
			serialNumberInput.readOnly = false;
			expirationEditor.readOnly = false;
			unlimitedSwitch.readOnly = false;
		}
		else{
			let canChangeProject = PermissionsController.checkPermission("ChangeProjectForLicense");
			projectInput.readOnly = !canChangeProject;
			
			let canChangeOrder = PermissionsController.checkPermission("ChangeOrderForLicense");
			ordersCB.changeable = canChangeOrder;
			
			let canChangeProduct = PermissionsController.checkPermission("ChangeProductForLicense");
			productCB.changeable = canChangeProduct;
			
			let canChangeLicense = PermissionsController.checkPermission("ChangeProductLicenses");
			licenseCB.changeable = canChangeLicense;
			
			let canChangeLicenseNumber = PermissionsController.checkPermission("ChangeLicenseNumber");
			serialNumberInput.readOnly = !canChangeLicenseNumber;
			
			let canChangeExpiration = PermissionsController.checkPermission("ChangeExpiration");
			expirationEditor.readOnly = !canChangeExpiration;
			unlimitedSwitch.readOnly = !canChangeExpiration;
			
			let ok =
				canChangeProject ||
				canChangeOrder ||
				canChangeProduct ||
				canChangeLicense ||
				canChangeLicenseNumber ||
				canChangeExpiration;
			
			if (commandsController){
				commandsController.setCommandVisible("Undo", ok);
				commandsController.setCommandVisible("Redo", ok);
				commandsController.setCommandVisible("Save", ok);
			}
		}
	}
	
	function checkInUse(){
		if (!softwareProductData){
			return;
		}
		
		if (softwareProductData.m_inUse){
			root.readOnly = true;
			setAlertPanel(alertComp);
		}
		else{
			root.readOnly = false;
			setAlertPanel(undefined);
		}
	}
	
	function setReadOnly(readOnly){
		projectInput.readOnly = readOnly;
		
		ordersCB.changeable = !readOnly;
		productCB.changeable = !readOnly;
		licenseCB.changeable = !readOnly;
		
		serialNumberInput.readOnly = readOnly;
		expirationEditor.readOnly = readOnly;
		unlimitedSwitch.readOnly = readOnly;
	}
	
	function updateGui(){
		if (!softwareProductData){
			console.error("Unable to update GUI for 'SoftwareEditor'. Error: softwareProductData is invalid");
			return;
		}
		
		projectInput.text = softwareProductData.m_project;
		
		ordersCB.currentIndex = -1;
		
		let orderUuid = softwareProductData.m_orderUuid;
		console.log("updateGui", softwareProductData);
		if (ordersCB.sourceModel){
			for (let i = 0; i < ordersCB.sourceModel.getItemsCount(); i++){
				let id = ordersCB.sourceModel.getData("Id", i);
				if (id === orderUuid){
					console.log("id ===", id);
					
					ordersCB.currentIndex = i;
					break;
				}
			}
		}
		
		productCB.currentIndex = -1;
		let productId = softwareProductData.m_productId;
		
		if (productCB.model){
			for (let i = 0; i < productCB.model.getItemsCount(); i++){
				let id = productCB.model.getData("Id", i);
				if (id === productId){
					productCB.currentIndex = i;
					break;
				}
			}
		}
		
		group2.updateGui();
	}
	
	function updateModel(){
		if (!softwareProductData){
			console.error("Unable to update model for 'SoftwareEditor'. Error: softwareProductData is invalid");
			return;
		}
		
		softwareProductData.m_project = projectInput.text;
		
		let canChangeOrder = PermissionsController.checkPermission("ChangeOrderForLicense");
		if (canChangeOrder){
			if (ordersCB.sourceModel){
				if (ordersCB.currentIndex >= 0){
					let orderUuid = ordersCB.sourceModel.getData("Id", ordersCB.currentIndex);
					softwareProductData.m_orderUuid = orderUuid;
				}
				else{
					softwareProductData.m_orderUuid = "";
				}
			}
		}
		
		if (productCB.currentIndex >= 0 && productCB.model){
			let selectedId = productCB.model.getData("Id", productCB.currentIndex);
			softwareProductData.m_productId = selectedId;
		}
		else{
			softwareProductData.m_productId = "";
			
		}
		
		group2.updateModel();
	}
	
	function getProductLicensesModel(){
		for (let i = 0; i < root.licensesModel.getItemsCount(); i++){
			let productId = root.licensesModel.getData("Id", i);
			if (productId === root.productId){
				if (root.licensesModel.containsKey("Licenses", i)){
					return root.licensesModel.getData("Licenses", i);
				}
			}
		}
		
		return null;
	}
	
	DocumentHistoryPanel {
		id: historyPanel;
		documentId: root.softwareProductData ? root.softwareProductData.m_id : "";
		collectionId: "SoftwareProducts";
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
		anchors.leftMargin: Style.sizeLargeMargin;
		
		anchors.top: parent.top;
		anchors.topMargin: Style.sizeLargeMargin;
		
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.sizeLargeMargin;
		
		anchors.right: scrollbar.left;
		anchors.rightMargin: Style.sizeLargeMargin;
		
		contentWidth: bodyColumn.width;
		contentHeight: Math.max(bodyColumn.height + 2 * Style.sizeLargeMargin + 100, historyPanel.contentHeight + 2 * Style.sizeLargeMargin);
		
		boundsBehavior: Flickable.StopAtBounds;
		clip: true;
		
		Column {
			id: bodyColumn;
			
			width: 700;
			
			spacing: Style.sizeLargeMargin;
			
			GroupHeaderView {
				width: parent.width;
				title: qsTr("Software Information");
				groupView: group;
			}
			
			GroupElementView {
				id: group;
				
				width: parent.width;
				
				TextInputElementView {
					id: projectInput;
					
					name: qsTr("Project");
					placeHolderText: qsTr("Enter the project");
					
					readOnly: root.readOnly;
					
					KeyNavigation.tab: ordersCB;
					KeyNavigation.backtab: expirationEditor;
					
					onEditingFinished: {
						root.doUpdateModel();
					}
				}
				
				FilterableComboBoxElementView {
					id: ordersCB;
					
					nameId: "OrderId";
					name: qsTr("Order");
					
					filteringFields: ["OrderId", "CustomerName"];
					
					sourceModel: CachedOrderCollection.collectionModel;
					
					changeable: !root.readOnly;
					
					KeyNavigation.tab: productCB;
					KeyNavigation.backtab: projectInput;
					
					delegate: Component {
						FilterableComboBoxDelegate {
							width: ordersCB.cbRef ? ordersCB.cbRef.width : 0;
							description: qsTr("Customer") + ": " + model.CustomerName;
							comboBoxRef: ordersCB.cbRef;
						}
					}
					
					onFinished: {
						root.doUpdateModel();
					}
					
					onModelChanged: {
						root.doUpdateGui();
					}
				}
			}
			
			GroupHeaderView {
				width: parent.width;
				
				title: qsTr("License Information");
				groupView: group2;
			}
			
			GroupElementView {
				id: group2;
				
				width: parent.width;
				
				function updateGui(){
					serialNumberInput.text = root.softwareProductData.m_serialNumber;
					
					licenseCB.currentIndex = -1;
					
					let licenseUuid = root.softwareProductData.m_licenseUuid;
					if (licenseCB.model){
						for (let i = 0; i < licenseCB.model.getItemsCount(); i++){
							let licenseId = licenseCB.model.getData("Id", i);
							if (licenseId === licenseUuid){
								licenseCB.currentIndex = i;
								
								break;
							}
						}
					}
					
					let expiration = root.softwareProductData.m_expiration;
					
					if (expiration && expiration !== "" ){
						unlimitedSwitch.switchRef.setChecked(false);
					}
					else{
						unlimitedSwitch.switchRef.setChecked(true);
					}
					
					if (expiration){
						let currentDate = expirationEditor.getDateAsString()
						
						if (expiration !== "" && expiration !== currentDate){
							expirationEditor.datePicker.setDateAsString(expiration);
						}
					}
				}
				
				function updateModel(){
					root.softwareProductData.m_serialNumber = serialNumberInput.text;
					
					if (!unlimitedSwitch.checked){
						root.softwareProductData.m_expiration = expirationEditor.getDateAsString();
					}
					else{
						root.softwareProductData.m_expiration = "";
					}
					
					if (licenseCB.currentIndex >= 0 && licenseCB.model){
						let selectedId = licenseCB.model.getData("Id", licenseCB.currentIndex);
						root.softwareProductData.m_licenseUuid = selectedId;
					}
					else{
						root.softwareProductData.m_licenseUuid = "";
					}
				}
				
				ComboBoxElementView {
					id: productCB;
					
					name: qsTr("Product");
					nameId: "ProductName";
					
					model: CachedProductCollection.softwareProductsModel;
					
					changeable: !root.readOnly
					
					KeyNavigation.tab: licenseCB;
					KeyNavigation.backtab: ordersCB;
					
					isSelectionRequired: true;
					errorText: qsTr("Please select a product");
					
					onModelChanged: {
						root.doUpdateGui();
					}
					
					onCurrentIndexChanged: {
						if (productCB.currentIndex >= 0){
							let licensesModel = productCB.model.getData("Licenses", productCB.currentIndex);
							if (!licensesModel){
								licensesModel = productCB.model.addTreeModel("Licenses", productCB.currentIndex);
							}
							
							root.productLicensesModel = licensesModel;
						}
						else{
							root.productLicensesModel = 0;
						}
						
						licenseCB.currentIndex = -1;
						
						root.doUpdateModel();
					}
				}
				
				ComboBoxElementView {
					id: licenseCB;
					
					nameId: "LicenseName";
					name: qsTr("Licenses");
					
					model: root.productLicensesModel;
					
					KeyNavigation.tab: serialNumberInput;
					KeyNavigation.backtab: productCB;
					
					isSelectionRequired: true;
					errorText: qsTr("Please select a license");
					
					onCurrentIndexChanged: {
						if (currentIndex >= 0){
							root.doUpdateModel();
						}
					}
				}
				
				TextInputElementView {
					id: serialNumberInput;
					
					placeHolderText: qsTr("Enter the software-ID");
					name: qsTr("Software-ID");
					
					KeyNavigation.tab: unlimitedSwitch;
					KeyNavigation.backtab: licenseCB;
					
					textInputValidator: serialNumberRegexp;
					showErrorWhenInvalid: true;
					errorText: qsTr("Please enter the software-ID");
					onEditingFinished: {
						root.doUpdateModel();
					}
				}
				
				RegularExpressionValidator {
					id: serialNumberRegexp;
					regularExpression: /^(?!\s*$).+/;
				}
			}
			
			GroupHeaderView {
				width: parent.width;
				
				title: qsTr("Expiration Information");
				groupView: expirationGroup;
			}
			
			GroupElementView {
				id: expirationGroup;
				
				width: parent.width;
				
				SwitchElementView {
					id: unlimitedSwitch;
					
					name: qsTr("Unlimited");
					description: unlimitedSwitch.checked ? qsTr("Deactivate it if you want to set the expiration date manually") : qsTr("Activate it if you want to set an unlimited expiration date");
					
					onCheckedChanged: {
						root.doUpdateModel();
					}
					
					readOnly: root.readOnly;
					
					KeyNavigation.tab: expirationEditor;
					KeyNavigation.backtab: serialNumberInput;
					
					onSwitchRefChanged: {
						if (switchRef){
							switchRef.readOnly = unlimitedSwitch.readOnly;
						}
					}
				}
				
				DateTimePickerElementView {
					id: expirationEditor;
					
					name: qsTr("Expiration");
					visible: !unlimitedSwitch.checked;
					
					KeyNavigation.tab: projectInput;
					KeyNavigation.backtab: unlimitedSwitch;
					
					onDatePickerChanged: {
						if (datePicker){
							let currentDate = new Date();
							datePicker.selectedDate = new Date(currentDate.getFullYear() + 1, currentDate.getMonth(), currentDate.getDate());
						}
					}
					
					onEditingFinished: {
						root.doUpdateModel();
					}
				}
			}
		}
	}
}//Container


