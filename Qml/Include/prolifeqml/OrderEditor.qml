import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import prolifeqml 1.0
import prolifeOrdersSdl 1.0
import imtbaseImtCollectionSdl 1.0

ViewBase {
	id: orderEditorContainer;
	
	property TreeItemModel accountsModel: CachedAccountCollection.collectionModel;
	property TreeItemModel productsModel: CachedProductCollection.collectionModel;
	property TreeItemModel devicesModel: CachedDeviceCollection.collectionModel;
	property TreeItemModel softwaresModel: CachedSoftwareCollection.collectionModel;
	property TreeItemModel licensesModel: CachedLicenseCollection.collectionModel
	
	property OrderData orderData: model ? model : null;
	property bool isNew: false

	Component.onCompleted: {
		if (!CachedAccountCollection.completed){
			CachedAccountCollection.updateModel();
		}
	}

	onOrderDataChanged: {
		checkPermissions();
	}
	
	function setReadOnly(readOnly){
		instanceIdInput.readOnly = readOnly;
		purchaseIdInput.readOnly = readOnly;
		descriptionInput.readOnly = readOnly;
		productsView.readOnly = readOnly;
		
		customerCB.changeable = !readOnly;
		orderStatusCB.changeable = !readOnly;
		additionalRolesView.readOnly = readOnly;
	}
	
	function checkPermissions(){
		if (!orderData){
			return;
		}
		
		let canAddOrder = PermissionsController.checkPermission("AddOrder");
		
		if (isNew && canAddOrder){
			instanceIdInput.readOnly = false;
			purchaseIdInput.readOnly = false;
			descriptionInput.readOnly = false;
			customerCB.changeable = true;
			orderStatusCB.changeable = true;
			addProduct.visible = true;
			productsView.readOnly = false;
			addRoleButton.visible = true;
			additionalRolesView.readOnly = false;
		}
		else{
			let canChangeDeliveryId = PermissionsController.checkPermission("ChangeDeliveryId");
			instanceIdInput.readOnly = !canChangeDeliveryId;
			
			let canChangePurchaseOrderId = PermissionsController.checkPermission("ChangePurchaseOrderId");
			purchaseIdInput.readOnly = !canChangePurchaseOrderId;
			
			let canChangeDescriptionForOrder = PermissionsController.checkPermission("ChangeDescriptionForOrder");
			descriptionInput.readOnly = !canChangeDescriptionForOrder;
			
			let canChangeCustomer = PermissionsController.checkPermission("ChangeCustomer");
			customerCB.changeable = canChangeCustomer;
			additionalRolesView.readOnly = !canChangeCustomer;
			addRoleButton.visible = canChangeCustomer;
			
			let canChangeOrderStatus = PermissionsController.checkPermission("ChangeOrderStatus");
			orderStatusCB.changeable = canChangeOrderStatus;
			
			let canChangeOrderProducts = PermissionsController.checkPermission("ChangeOrderProducts");
			addProduct.visible = canChangeOrderProducts;
			productsView.readOnly = !canChangeOrderProducts;
			
			let ok = canChangeDeliveryId || canChangePurchaseOrderId || canChangeDescriptionForOrder || canChangeCustomer || canChangeOrderStatus || canChangeOrderProducts;
			
			if (commandsController){
				commandsController.setCommandVisible("Undo", ok);
				commandsController.setCommandVisible("Redo", ok);
				commandsController.setCommandVisible("Save", ok);
			}
		}
	}
	
	function updateGui(){
		instanceIdInput.text = orderData.m_orderId;
		purchaseIdInput.text = orderData.m_purchaseId;
		descriptionInput.text = orderData.m_description;
		
		customerCB.currentIndex = -1;
		let customerId = orderData.m_customerId;
		let customerModel = customerCB.model;
		
		if (customerModel){
			for (let i = 0; i < customerModel.getItemsCount(); i++){
				let id = customerModel.getData("id", i);
				if (id === customerId){
					customerCB.currentIndex = i;
					break;
				}
			}
		}
		
		orderStatusCB.currentIndex = -1;
		let status = orderData.m_orderStatus;
		let statusModel = orderStatus.statusModel
		if (statusModel){
			orderStatusCB.model = statusModel;
			for (let i = 0; i < statusModel.getItemsCount(); i++){
				let id = statusModel.getData("id", i);
				if (id === status){
					orderStatusCB.currentIndex = i;
					break;
				}
			}
		}
		
		productsView.model = 0;
		productsView.model = orderData.m_orderProducts;

		// Restore additional customer roles (non-ordering-party roles)
		additionalRolesModel.clear();
		if (orderData.m_customerRoles){
			for (let r = 0; r < orderData.m_customerRoles.count; r++){
				let roleItem = orderData.m_customerRoles.get(r).item;
				if (roleItem && roleItem.m_roleType !== "OrderingParty"){
					let idx = additionalRolesModel.insertNewItem();
					additionalRolesModel.setData("roleType", roleItem.m_roleType || "EndCustomer", idx);
					additionalRolesModel.setData("customerId", roleItem.m_customerId || "", idx);
				}
			}
		}
	}
	
	function updateModel(){
		orderData.m_orderId = instanceIdInput.text ;
		orderData.m_purchaseId = purchaseIdInput.text;
		orderData.m_description = descriptionInput.text;
		
		let selectedAccountId = "";
		if (customerCB.currentIndex >= 0 && customerCB.model){
			selectedAccountId = customerCB.model.getData("id", customerCB.currentIndex);
		}
		
		orderData.m_customerId = selectedAccountId;
		
		if (orderStatusCB.currentIndex >= 0){
			let selectedStatus = orderStatusCB.model.getData("id", orderStatusCB.currentIndex);
			orderData.m_orderStatus = selectedStatus;
		}
		else{
			orderData.m_orderStatus = "";
		}

		if (!orderData.hasOrderProducts()){
			orderData.emplaceOrderProducts()
		}

		// Build the customer roles list: first the ordering party, then additional roles
		if (!orderData.hasCustomerRoles()){
			orderData.emplaceCustomerRoles();
		}
		let customerRoles = orderData.m_customerRoles;
		if (customerRoles){
			customerRoles.clear();

			// Ordering party role (mandatory)
			if (selectedAccountId !== ""){
				customerRoles.insert(0, {
					"roleType": "OrderingParty",
					"customerId": selectedAccountId
				});
			}

			// Additional roles
			for (let i = 0; i < additionalRolesModel.getItemsCount(); i++){
				let roleCustomerId = additionalRolesModel.getData("customerId", i);
				if (roleCustomerId !== ""){
					customerRoles.insert(customerRoles.count, {
						"roleType": additionalRolesModel.getData("roleType", i),
						"customerId": roleCustomerId
					});
				}
			}
		}
	}
	
	OrderStatus {
		id: orderStatus;
	}

	TreeItemModel {
		id: additionalRolesModel;
	}

	// Role-type labels for the dropdown
	TreeItemModel {
		id: roleTypesModel;

		Component.onCompleted: {
			let index = roleTypesModel.insertNewItem();
			roleTypesModel.setData("id", "EndCustomer", index);
			roleTypesModel.setData("name", qsTr("End Customer"), index);

			index = roleTypesModel.insertNewItem();
			roleTypesModel.setData("id", "InvoiceRecipient", index);
			roleTypesModel.setData("name", qsTr("Invoice Recipient"), index);

			index = roleTypesModel.insertNewItem();
			roleTypesModel.setData("id", "DeliveryRecipient", index);
			roleTypesModel.setData("name", qsTr("Delivery Recipient"), index);

			index = roleTypesModel.insertNewItem();
			roleTypesModel.setData("id", "Reseller", index);
			roleTypesModel.setData("name", qsTr("Reseller"), index);

			index = roleTypesModel.insertNewItem();
			roleTypesModel.setData("id", "Referrer", index);
			roleTypesModel.setData("name", qsTr("Referrer"), index);
		}
	}
	
	Rectangle {
		anchors.fill: parent;
		
		color: Style.backgroundColor2;
	}
	
	DocumentHistoryPanel {
		id: historyPanel;
		documentId: orderEditorContainer.orderData ? orderEditorContainer.orderData.m_id : "";
		collectionId: "Orders";
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
		
		contentWidth: content.width;
		contentHeight: Math.max(content.height + 2 * Style.marginXL + 100, historyPanel.contentHeight + 2 * Style.marginXL);
		
		boundsBehavior: Flickable.StopAtBounds;
		
		clip: true;
		
		Column {
			id: content;
			
			width: 700;
			
			spacing: Style.marginXL;
			
			GroupHeaderView {
				title: qsTr("Order Information");
				width: content.width;
				groupView: group;
			}
			
			GroupElementView {
				id: group;
				
				width: content.width;
				
				Component {
					id: errorComp;
					
					Text {
						id: errorInstanceId;
						
						text: qsTr("Enter a 5-digit or 8-digit number");
						
						color: Style.errorTextColor;
						font.family: Style.fontFamily;
						font.pixelSize: Style.fontSizeM;
					}
				}
				
				TextInputElementView {
					id: instanceIdInput;
					
					name: qsTr("Delivery-ID");
					placeHolderText: qsTr("Enter the delivery-ID");
					
					readOnly: orderEditorContainer.readOnly;
					maximumLength: 8;
					
					onEditingFinished: {
						orderEditorContainer.doUpdateModel();
					}
					
					KeyNavigation.tab: purchaseIdInput;
					KeyNavigation.backtab: orderStatusCB;
					
					bottomComp: acceptableInput ? undefined : errorComp;
					
					Component.onCompleted: {
						validate();
					}
					
					onTextChanged: {
						validate();
					}
					
					function validate(){
						let len = instanceIdInput.text.length;
						
						let ok1 = instanceIdInput.test("\\d{5}", instanceIdInput.text) && len === 5;
						let ok2 = instanceIdInput.test("\\d{8}", instanceIdInput.text) && len === 8;
						
						instanceIdInput.bottomComp = ok1 || ok2 ? undefined : errorComp;
					}
					
					function test(regex, text){
						let re = new RegExp(regex)
						if (re){
							return re.test(text);
						}
						
						return false;
					}
				}
				
				TextInputElementView {
					id: purchaseIdInput;
					
					name: qsTr("Purchase Order-ID");
					placeHolderText: qsTr("Enter the Purchase-ID");
					
					readOnly: orderEditorContainer.readOnly;
					
					onEditingFinished: {
						orderEditorContainer.doUpdateModel();
					}
					
					KeyNavigation.tab: descriptionInput;
					KeyNavigation.backtab: instanceIdInput;
				}
				
				TextInputElementView {
					id: descriptionInput;
					
					name: qsTr("Description");
					placeHolderText: qsTr("Enter the comment");
					
					readOnly: orderEditorContainer.readOnly;
					
					onEditingFinished: {
						orderEditorContainer.doUpdateModel();
					}
					
					KeyNavigation.tab: customerCB;
					KeyNavigation.backtab: purchaseIdInput;
				}
				
				ComboBoxElementView {
					id: customerCB;
					
					name: qsTr("Customer");
					
					model: orderEditorContainer.accountsModel;
					changeable: !orderEditorContainer.readOnly;
					isSelectionRequired: true;
					errorText: qsTr("Please select a customer");
					
					onCurrentIndexChanged: {
						orderEditorContainer.doUpdateModel();
					}
					
					KeyNavigation.tab: orderStatusCB;
					KeyNavigation.backtab: descriptionInput;
					
					onModelChanged: {
						orderEditorContainer.doUpdateGui();
					}
				}
				
				ComboBoxElementView {
					id: orderStatusCB;
					
					name: qsTr("Order Status");
					
					changeable: !orderEditorContainer.readOnly;
					
					model: orderStatus.statusModel;
					
					onCurrentIndexChanged: {
						orderEditorContainer.doUpdateModel();
						
						if (orderStatusCB.currentIndex < 0){
							orderStatusCB.model = orderStatus.statusModel;
						}
					}
					
					KeyNavigation.tab: instanceIdInput;
					KeyNavigation.backtab: customerCB;
				}

			}

			// Additional customer roles section
			Item {
				id: additionalRolesHeader;
				width: content.width;
				height: addRoleButton.height + Style.marginM;
				visible: true;

				Text {
					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: parent.left;
					anchors.leftMargin: Style.marginL;
					text: qsTr("Additional Roles");
					color: Style.textColor;
					font.family: Style.fontFamilyBold;
					font.pixelSize: Style.fontSizeL;
				}

				ToolButton {
					id: addRoleButton;
					anchors.verticalCenter: parent.verticalCenter;
					anchors.right: parent.right;
					anchors.rightMargin: Style.marginL;

					width: 22;
					height: width;

					iconSource: "../../../" + Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal);
					tooltipText: qsTr("Add Role");

					onClicked: {
						let idx = additionalRolesModel.insertNewItem();
						additionalRolesModel.setData("roleType", "EndCustomer", idx);
						additionalRolesModel.setData("customerId", "", idx);
						orderEditorContainer.doUpdateModel();
					}
				}
			}

			// List of additional roles
			Column {
				id: additionalRolesView;
				width: content.width;
				spacing: Style.marginM;

				property bool readOnly: false;

				Repeater {
					model: additionalRolesModel;

					delegate: Row {
						width: additionalRolesView.width - 2 * Style.marginL;
						x: Style.marginL;
						spacing: Style.marginM;

						ComboBox {
							id: roleTypeCB;
							width: 160;

							nameId: "name";
							model: roleTypesModel;

							changeable: !additionalRolesView.readOnly;

							Component.onCompleted: {
								let storedRoleType = additionalRolesModel.getData("roleType", index);
								for (let i = 0; i < roleTypesModel.getItemsCount(); i++){
									if (roleTypesModel.getData("id", i) === storedRoleType){
										currentIndex = i;
										break;
									}
								}
							}

							onCurrentIndexChanged: {
								if (currentIndex >= 0){
									let selectedRoleId = roleTypesModel.getData("id", currentIndex);
									additionalRolesModel.setData("roleType", selectedRoleId, index);
									orderEditorContainer.doUpdateModel();
								}
							}
						}

						ComboBox {
							id: additionalCustomerCB;
							width: parent.width - roleTypeCB.width - removeRoleButton.width - 2 * Style.marginM;

							nameId: "name";
							model: orderEditorContainer.accountsModel;

							changeable: !additionalRolesView.readOnly;

							Component.onCompleted: {
								let storedId = additionalRolesModel.getData("customerId", index);
								for (let i = 0; i < orderEditorContainer.accountsModel.getItemsCount(); i++){
									if (orderEditorContainer.accountsModel.getData("id", i) === storedId){
										currentIndex = i;
										break;
									}
								}
							}

							onCurrentIndexChanged: {
								if (currentIndex >= 0 && orderEditorContainer.accountsModel){
									let selectedId = orderEditorContainer.accountsModel.getData("id", currentIndex);
									additionalRolesModel.setData("customerId", selectedId, index);
									orderEditorContainer.doUpdateModel();
								}
							}
						}

						ToolButton {
							id: removeRoleButton;
							anchors.verticalCenter: parent.verticalCenter;

							width: 22;
							height: width;

							iconSource: "../../../" + Style.getIconPath("Icons/Remove", Icon.State.On, Icon.Mode.Normal);
							tooltipText: qsTr("Remove Role");

							visible: !additionalRolesView.readOnly;

							onClicked: {
								additionalRolesModel.removeItem(index);
								orderEditorContainer.doUpdateModel();
							}
						}
					}
				}
			}
			
			Component {
				id: productFactory;
				
				OrderedProduct {}
			}
		
			Component {
				id: productEditorDialog;
				
				ProductEditorDialog {
					id: productsDialog;
					
					onStarted: {
						productsDialog.bodyItem.productsModel = orderEditorContainer.productsModel;
						productsDialog.bodyItem.devicesModel = orderEditorContainer.devicesModel;
						productsDialog.bodyItem.softwaresModel = orderEditorContainer.softwaresModel;
						productsDialog.bodyItem.licensesModel = orderEditorContainer.licensesModel;
						productsDialog.bodyItem.orderUuid = orderEditorContainer.orderData.m_id;
						productsDialog.activeProductIndex = productsView.activeProductIndex;
						productsDialog.bodyItem.orderId = orderEditorContainer.orderData.m_orderId;
						productsDialog.bodyItem.orderProductsModel = orderEditorContainer.orderData.m_orderProducts;
						if (productsView.activeProductIndex >= 0){
							let productModel = orderEditorContainer.orderData.m_orderProducts.get(productsView.activeProductIndex).item;
							productsDialog.bodyItem.productItem = productModel.copyMe();
						}
						else{
							let productItem = productFactory.createObject(orderEditorContainer);
							productItem.m_id = UuidGenerator.generateUUID();
							productItem.m_categoryId = "Software";
							productsDialog.bodyItem.productItem = productItem;
						}
						
						productsDialog.bodyItem.started();
					}
					
					onFinished: {
						if (buttonId == Enums.ok){
							let actualOrderProducts = orderEditorContainer.orderData.m_orderProducts;

							let index = productsView.activeProductIndex;
							if (index < 0){
								let addProductFunc = function(product){
									if (actualOrderProducts){
										actualOrderProducts.insert(0, {"item": product.copyMe()})
									}
								}

								let productItem =  productsDialog.bodyItem.productItem;
								let instanceCount = productsDialog.bodyItem.instanceCount
								for (let i = 0; i < instanceCount; ++i){
									if (productItem.m_isNew){
										productItem.m_id = UuidGenerator.generateUUID();
									}
									
									addProductFunc(productItem)
								}
							}
							else{
								let productModel = productsDialog.bodyItem.productItem.copyMe();
								if (actualOrderProducts){
									actualOrderProducts.set(index, {"item": productModel})
								}
							}
							
							productsView.model = 0;
							productsView.model = actualOrderProducts;

							orderEditorContainer.model.modelChanged([]);
						}
					}
				}
			}
			
			Item {
				id: productsTitle;
				
				width: content.width;
				height: titleLicenses.height;
				
				Row {
					anchors.verticalCenter: parent.verticalCenter;
					anchors.right: parent.right;
					
					height: parent.height;
					
					spacing: Style.marginM;
					
					ToolButton {
						id: expandButton;
						
						anchors.verticalCenter: parent.verticalCenter;
						
						width: 22;
						height: width;
						
						iconSource: !productsView.expanded ? "../../../" + Style.getIconPath("Icons/DetailedView", Icon.State.On, Icon.Mode.Normal)
														   : "../../../" + Style.getIconPath("Icons/CompactView", Icon.State.On, Icon.Mode.Normal);
						
						tooltipText: !productsView.expanded ? qsTr("Detailed view") : qsTr("Compact view");
						
						onClicked: {
							productsView.expanded = !productsView.expanded;
						}
					}
					
					ToolButton {
						id: addProduct;
						
						anchors.verticalCenter: parent.verticalCenter;
						
						width: 22;
						height: width;
						
						iconSource: "../../../" + Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal);
						
						tooltipText: qsTr("Add a new product");
						
						onClicked: {
							productsView.activeProductIndex = -1;
							ModalDialogManager.openDialog(productEditorDialog, {});
						}
					}
				}
				
				Text {
					id: titleLicenses;
					
					anchors.left: parent.left;
					anchors.verticalCenter: parent.verticalCenter;
					
					text: qsTr("Products") + " (" + productsView.count + ")";
					color: Style.textColor;
					font.family: Style.fontFamilyBold;
					font.pixelSize: Style.fontSizeXXL;
				}
			}

			ListView {
				id: productsView;
				
				width: content.width;
				height: contentHeight;
				
				boundsBehavior: Flickable.StopAtBounds;
				spacing: Style.marginXL;
				
				cacheBuffer: 1000;
				
				property int activeProductIndex: -1;
				property int selectedIndex: -1;
				property bool readOnly: false;
				
				property bool expanded: true;
				
				delegate: OrderProductDelegate {
					id: orderProductDelegate;
					width: productsView.width;
					readOnly: productsView.readOnly;
					expanded: productsView.expanded;
					onEdited: {
						productsView.activeProductIndex = model.index;
						ModalDialogManager.openDialog(productEditorDialog, {});
					}
					onRemoved: {
						productsView.activeProductIndex = model.index;
						ModalDialogManager.openDialog(removeDialog, {"message": qsTr("Remove selected product ?")});
					}
				}
			}
		}
	}
	
	Component {
		id: removeDialog;
		
		MessageDialog {
			onFinished: {
				if (buttonId == Enums.yes){
					if (productsView.activeProductIndex < 0){
						return;
					}
					
					let orderProducts = orderEditorContainer.orderData.m_orderProducts
					if (orderProducts){
						orderProducts.remove(productsView.activeProductIndex);
						orderEditorContainer.model.modelChanged([]);
					}
				}
			}
		}
	}
}//Container


