import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import imtguigql 1.0
import prolifeqml 1.0
import prolifeLicensesSdl 1.0

ViewBase {
	id: root;
	
	property TreeItemModel licensesModel: TreeItemModel{}
	property TreeItemModel productsModel: TreeItemModel{}
	
	property var productLicensesModel: TreeItemModel{}

	property SoftwareProductData softwareProductData: model
	property bool isNew: false
	
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
		// License tree is now part of SoftwareProductData, no separate load needed
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

		let canAddLicense = PermissionsController.checkPermission("AddLicense");
		if (softwareProductData.m_inUse){
			setReadOnly(true)
			return
		}

		if (isNew && canAddLicense){
			setReadOnly(false)
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
			setReadOnly(true)
			setAlertPanel(alertComp);
		}
		else{
			setReadOnly(false)
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
		// internalUseSwitchElementView.readOnly = readOnly
	}
	
	function updateGui(){
		if (!softwareProductData){
			console.error("Unable to update GUI for 'SoftwareEditor'. Error: softwareProductData is invalid");
			return;
		}
		
		projectInput.text = softwareProductData.m_project;
		
		ordersCB.currentIndex = -1;
		
		let orderUuid = softwareProductData.m_orderUuid;
		if (ordersCB.sourceModel){
			for (let i = 0; i < ordersCB.sourceModel.getItemsCount(); i++){
				let id = ordersCB.sourceModel.getData("id", i);
				if (id === orderUuid){
					ordersCB.currentIndex = i;
					break;
				}
			}
		}
		
		productCB.currentIndex = -1;
		let productId = softwareProductData.m_productId;
		
		if (productCB.model){
			for (let i = 0; i < productCB.model.getItemsCount(); i++){
				let id = productCB.model.getData("id", i);
				if (id === productId){
					productCB.currentIndex = i;
					break;
				}
			}
		}
		
		internalUseSwitchElementView.checked = softwareProductData.m_internalUse
		multipleElementView.checked = softwareProductData.m_isMultiple
		productCountElementView.value = softwareProductData.m_productCount

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
					let orderUuid = ordersCB.sourceModel.getData("id", ordersCB.currentIndex);
					softwareProductData.m_orderUuid = orderUuid;
				}
				else{
					softwareProductData.m_orderUuid = "";
				}
			}
		}
		
		if (productCB.currentIndex >= 0 && productCB.model){
			let selectedId = productCB.model.getData("id", productCB.currentIndex);
			softwareProductData.m_productId = selectedId;
		}
		else{
			softwareProductData.m_productId = "";
		}
		
		softwareProductData.m_internalUse = internalUseSwitchElementView.checked
		softwareProductData.m_isMultiple = multipleElementView.checked
		softwareProductData.m_productCount = productCountElementView.value

		group2.updateModel();
	}
	
	function getProductLicensesModel(){
		for (let i = 0; i < root.licensesModel.getItemsCount(); i++){
			let productId = root.licensesModel.getData("id", i);
			if (productId === root.productId){
				if (root.licensesModel.containsKey("licenses", i)){
					return root.licensesModel.getData("licenses", i);
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
					
					nameId: "orderId";
					name: qsTr("Order");
					
					filteringFields: ["orderId", "customerName"];
					
					sourceModel: CachedOrderCollection.collectionModel;
					
					changeable: !root.readOnly;
					
					KeyNavigation.tab: productCB;
					KeyNavigation.backtab: projectInput;
					
					delegate: Component {
						FilterableComboBoxDelegate {
							width: ordersCB.cbRef ? ordersCB.cbRef.width : 0;
							description: qsTr("Customer") + ": " + model.customerName;
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
							let licenseId = licenseCB.model.getData("id", i);
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
						let selectedId = licenseCB.model.getData("id", licenseCB.currentIndex);
						root.softwareProductData.m_licenseUuid = selectedId;
					}
					else{
						root.softwareProductData.m_licenseUuid = "";
					}
				}
				
				ComboBoxElementView {
					id: productCB;
					
					name: qsTr("Product");
					nameId: "productName";
					
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
							let licensesModel = productCB.model.getData("licenses", productCB.currentIndex);
							if (!licensesModel){
								licensesModel = productCB.model.addTreeModel("licenses", productCB.currentIndex);
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
					
					nameId: SoftwareProductItemTypeMetaInfo.s_licenseName;
					name: qsTr("Licenses");
					
					model: root.productLicensesModel;
					
					KeyNavigation.tab: articleText;
					KeyNavigation.backtab: productCB;
					
					isSelectionRequired: true;
					errorText: qsTr("Please select a license");
					
					onCurrentIndexChanged: {
						if (currentIndex >= 0){
							if (model){
								articleText.text = model.getData(SoftwareProductItemTypeMetaInfo.s_licenseId, currentIndex)
							}

							root.doUpdateModel();
						}
					}
					delegate: Component {
						FilterableComboBoxDelegate {
							width: licenseCB.width
							comboBoxRef: licenseCB.cbRef
							text: model[SoftwareProductItemTypeMetaInfo.s_licenseName]
							description: model[SoftwareProductItemTypeMetaInfo.s_licenseId]
						}
					}
				}
				
				TextInputElementView {
					id: articleText
					name: qsTr("Article")
					readOnly: true
					KeyNavigation.tab: serialNumberInput
					KeyNavigation.backtab: licenseCB
				}
				
				TextInputElementView {
					id: serialNumberInput;
					
					placeHolderText: qsTr("Enter the software-ID");
					name: qsTr("Software-ID");
					
					KeyNavigation.tab: unlimitedSwitch;
					KeyNavigation.backtab: articleText;
					
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

				SwitchElementView {
					id: internalUseSwitchElementView
					name: qsTr("Internal Use");
					description: qsTr("Activate if the license is for internal use");
					readOnly: root.readOnly
					onCheckedChanged: {
						root.doUpdateModel()
					}
				}

				SwitchElementView {
					id: multipleElementView
					name: qsTr("Is Multiple");
					readOnly: root.readOnly
					onCheckedChanged: {
						root.doUpdateModel()
					}
				}

				SpinBoxElementView {
					id: productCountElementView
					name: qsTr("Product Count");
					readOnly: root.readOnly
					visible: multipleElementView.checked
					onValueChanged: {
						root.doUpdateModel()
					}
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
					readOnly: root.readOnly;
					
					KeyNavigation.tab: projectInput;
					KeyNavigation.backtab: unlimitedSwitch;
					
					onDatePickerChanged: {
						if (datePicker){
							let currentDate = new Date();
							let date = new Date(currentDate.getFullYear() + 1, currentDate.getMonth(), currentDate.getDate());
							datePicker.setDateAsString(Functions.dateToStr(date,"dd.MM.yyyy"))
						}
					}
					
					onEditingFinished: {
						root.doUpdateModel();
					}
				}
			}

			ElementView {
				name: qsTr("License Hierarchy")
				width: parent.width
				bottomComp: canvasComp
				
				controlComp: Component {
					ToolButton {
						id: expandButton
						width: Style.buttonWidthM
						height: width
						iconSource: "../../../" + Style.getIconPath(
							"Icons/Expand", 
							Icon.State.On, 
							Icon.Mode.Normal
						)
						onClicked: {
							licenseTreeDialog.treeData = root.softwareProductData ? root.softwareProductData.m_licenseTree : null;
							licenseTreeDialog.currentLicenseId = root.softwareProductData ? root.softwareProductData.m_id : "";
							licenseTreeDialog.open();
						}
					}
				}

				Component {
					id: canvasComp

					Item {
						height: canvasFlickable.height

						CustomScrollbar {
							id: scrollbar;
							z: parent.z + 1;
							anchors.right: parent.right;
							anchors.top: canvasFlickable.top;
							anchors.bottom: canvasFlickable.bottom;
							secondSize: 10;
							targetItem: canvasFlickable;
						}
						
						CustomScrollbar{
							id: scrollHoriz;
							z: parent.z + 1;
							anchors.left: canvasFlickable.left;
							anchors.right: canvasFlickable.right;
							anchors.bottom: canvasFlickable.bottom;
							secondSize: 10;
							vertical: false;
							targetItem: canvasFlickable;
						}

						Flickable {
							id: canvasFlickable
							width: parent.width
							// Compact view with 400px height limit and scrolling
							height: Math.min(contentHeight, 400)
							contentWidth: licenseTreeCanvas.treeWidth;
							contentHeight: licenseTreeCanvas.treeHeight;
							clip: true;

							LicenseTreeCanvas {
								id: licenseTreeCanvas;
								width: bodyColumn.width
								treeData: root.softwareProductData ? root.softwareProductData.m_licenseTree : null;
								currentLicenseId: root.softwareProductData ? root.softwareProductData.m_id : "";
							}
						}
					}
				}
			}
		}
	}

	// License tree full-screen dialog
	LicenseTreeDialog {
		id: licenseTreeDialog
	}
}//Container


