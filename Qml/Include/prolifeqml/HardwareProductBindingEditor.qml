import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtlicgui 1.0
import prolifeSensorsSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

Item {
	id: productEditor;
	clip: true;
	
	property int contentHeight: availableLicensesColumn.height;
	
	property DeviceBindingData bindingModel: null;
	
	property string productId: ""
	property string hardwareId: "";
	
	property string productErrorMessage: qsTr("Please select a product");
	property string duplicateErrorMessage: qsTr("License with ID '%1' has already been selected");
	property string licenseErrorMessage: qsTr("License with ID '%1' has already been added");
	
	signal modelChanged();
	
	Component.onCompleted: {
		CachedProductCollection.updateModel();
	}
	
	onBindingModelChanged: {
		if (bindingModel){
			productEditor.hardwareId = bindingModel.m_id;
			productEditor.productId = bindingModel.m_productUuid;
			bindingModel.modelChanged.connect(productEditor.modelChanged);
			
			updateGui();
		}
	}
	
	property bool blockUpdatingModel: false;
	onBlockUpdatingModelChanged: {
		loading.visible = blockUpdatingModel;
	}
	
	function updateGui(){
		blockUpdatingModel = true;
		
		if (productComboBoxElementView.cbRef){
			let productsCB = productComboBoxElementView.cbRef;
			
			productsCB.currentIndex = -1;
			if (productsCB.model){
				for (let i = 0; i < productsCB.model.getItemsCount(); i++){
					let id = productsCB.model.getData("id", i);
					if (id === productEditor.productId){
						productsCB.currentIndex = i;
						break;
					}
				}
			}
		}
		
		if (usedLicensesElementView.collection){
			usedLicensesElementView.collection.updateData();
		}
		
		blockUpdatingModel = false;
	}
	
	function checkLicenseId(licenseId){
		if (!usedLicensesElementView.collection){
			return false;
		}
		
		let bindingElements = usedLicensesElementView.collection.table.elements;
		if (bindingElements){
			for (let i = 0; i < bindingElements.getItemsCount(); i++){
				let id = bindingElements.getData("licenseId", i)
				if (id === licenseId){
					return false;
				}
			}
		}
		
		return true;
	}
	
	CustomScrollbar {
		id: scrollbar;
		anchors.right: parent.right;
		anchors.top: flickable.top;
		anchors.bottom: flickable.bottom;
		secondSize: Style.sizeMainMargin;
		targetItem: flickable;
	}
	
	Flickable {
		id: flickable;
		anchors.top: parent.top;
		anchors.topMargin: Style.sizeLargeMargin;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.sizeLargeMargin;
		anchors.left: parent.left;
		anchors.leftMargin: Style.sizeLargeMargin;
		anchors.right: scrollbar.left;
		anchors.rightMargin: Style.sizeLargeMargin;
		contentWidth: width;
		contentHeight: content.height + 2 * Style.sizeLargeMargin;
		boundsBehavior: Flickable.StopAtBounds;
		clip: true;
		
		Column {
			id: content;
			width: parent.width;
			
			Item {
				width: parent.width;
				height: Math.max(availableLicensesColumn.height, bindingLicensesColumn.height, buttonsColumn.height)
				Column {
					id: availableLicensesColumn;
					anchors.top: parent.top
					anchors.topMargin: Style.sizeMainMargin;
					anchors.right: parent.horizontalCenter;
					anchors.rightMargin: buttonsColumn.width;
					anchors.left: parent.left;
					anchors.leftMargin: Style.sizeMainMargin;
					spacing: Style.sizeMainMargin;
					
					ComboBoxElementView {
						id: productComboBoxElementView;
						width: parent.width;
						name: qsTr("Product");
						nameId: "productName";
						model: CachedProductCollection.softwareProductsModel;
						changeable: usedLicensesElementView.collection && usedLicensesElementView.collection.table.elementsList.count === 0;
						bottomComp: currentIndex >= 0 ? undefined : productErrorComp
						controlWidth: 300
						onCurrentIndexChanged: {
							if (productEditor.blockUpdatingModel){
								return;
							}
							
							if (currentIndex > -1){
								productEditor.productId = model.getData("id", currentIndex);
							}
							
							if (availableLicensesElementView.collection){
								availableLicensesElementView.collection.updateData();
							}
						}
					}
					
					Component {
						id: productErrorComp;
						
						BaseText {
							color: Style.errorTextColor;
							text: qsTr("Please select a product");
						}
					}
					
					ElementView {
						id: availableLicensesElementView;
						width: parent.width;
						name: qsTr("Available licenses");
						
						property SoftwareProductCollectionView collection: null;
						
						property bool hasSelectedDuplicate: false;
						
						topComp: hasSelectedDuplicate ? licenseErrorComp : undefined;
						
						Component {
							id: licenseErrorComp;
							BaseText {
								color: Style.errorTextColor;
								text: qsTr("Selected licenses with the same License-ID");
							}
						}
						
						bottomComp: Component {
							Rectangle {
								id: rectWrap;
								width: availableLicensesColumn.width;
								height: 500;
								
								Component.onCompleted: {
									availableLicensesElementView.collection = softwareProductCollection;
								}
								
								TreeItemModel {
									id: collectionHeadersModel2;
									
									Component.onCompleted: {
										rectWrap.updateHeaders2();
									}
								}
								
								function updateHeaders2(){
									collectionHeadersModel2.clear();
									
									let index = collectionHeadersModel2.insertNewItem();
									collectionHeadersModel2.setData("id", "licenseName", index);
									collectionHeadersModel2.setData("name", qsTr("Name"), index);
									
									index = collectionHeadersModel2.insertNewItem();
									collectionHeadersModel2.setData("id", "licenseId", index);
									collectionHeadersModel2.setData("name", qsTr("Article"), index);
									
									index = collectionHeadersModel2.insertNewItem();
									collectionHeadersModel2.setData("id", "deliveryId", index);
									collectionHeadersModel2.setData("name", qsTr("Delivery-ID"), index);
									
									index = collectionHeadersModel2.insertNewItem();
									collectionHeadersModel2.setData("id", "purchaseId", index);
									collectionHeadersModel2.setData("name", qsTr("Purchase Order-ID"), index);
									
									index = collectionHeadersModel2.insertNewItem();
									collectionHeadersModel2.setData("id", "serialNumber", index);
									collectionHeadersModel2.setData("name", qsTr("Software-ID"), index);
									
									index = collectionHeadersModel2.insertNewItem();
									collectionHeadersModel2.setData("id", "customerName", index);
									collectionHeadersModel2.setData("name", qsTr("Customer"), index);
									
									let filteringInfoIds = []
									for (let i = 0; i < collectionHeadersModel2.getItemsCount(); i++){
										let infoId = collectionHeadersModel2.getData("id", i);
										filteringInfoIds.push(infoId)
									}
									
									softwareProductCollection.collectionFilter.setFilteringInfoIds(filteringInfoIds);
									
									softwareProductCollection.tableViewParamsStoredServer = false;
									softwareProductCollection.dataController.headersModel  = collectionHeadersModel2;
								}
								
								SoftwareProductCollectionView {
									id: softwareProductCollection;
									anchors.fill: parent
									filterMenu.decorator: Style.filterPanelDecorator;
									commandsControllerComp: null;
									table.checkable: true;
									table.selectable: false;
									tableViewParamsStoredServer: false;
									commandsViewComp: undefined;
									dataControllerComp:
										Component {
										CollectionRepresentation {
											id: softwareDataController;
											
											Component.onCompleted: {
												additionalFieldIds.push("orderUuid");
												additionalFieldIds.push("hardwareId");
												additionalFieldIds.push("inUse");
												additionalFieldIds.push("productUuid");
												additionalFieldIds.push("customerId");
											}
											
											function updateModel(){}
										}
									}
									
									function registerDocumentInfo(){}
									
									onCheckedItemsChanged: {
										let selection = softwareProductCollection.table.getCheckedItems();
										if (selection.length <= 0){
											availableLicensesElementView.hasSelectedDuplicate = false;
											bindButton.enabled = false
										}
										else{
											let ok = true;
											for (let i = 0; i < selection.length; i++){
												let index = selection[i];
												
												let inUse = softwareProductCollection.table.elements.getData("inUse", index);
												if (inUse && !unbindButton.userCanUnbind){
													ok = false;
													break;
												}
												
												let licenseId = softwareProductCollection.table.elements.getData("licenseId", index);
												if (!productEditor.checkLicenseId(licenseId)){
													let message = productEditor.licenseErrorMessage.replace("%1", licenseId)
													// productEditor.setError(message)
													availableLicensesElementView.hasSelectedDuplicate = true;
													ok = false;
													
													break;
												}
												
												for (let j = i + 1; j < selection.length; j++){
													let index2 = selection[j];
													
													let licenseId2 = softwareProductCollection.table.elements.getData("licenseId", index2);
													if (licenseId === licenseId2){
														let message =  productEditor.duplicateErrorMessage.replace("%1", licenseId);
														// productEditor.setError(message)
														availableLicensesElementView.hasSelectedDuplicate = true;
														
														ok = false;
														
														break;
													}
												}
												
												if (!ok){
													break;
												}
											}
											
											if (ok){
												availableLicensesElementView.hasSelectedDuplicate = false;
												// productEditor.setError("")
											}
											
											bindButton.enabled = ok;
										}
									}
									
									onElementsChanged: {
										table.uncheckAll();
									}
									
									onHeadersChanged: {
										softwareProductCollection.table.setColumnContentById("licenseName", null);
									}
									
									FieldFilter {
										id: productFilter
										m_fieldId: "ProductUuid"
										m_filterValue: productEditor.productId
										m_filterValueType: "String"
										m_filterOperations: ["Equal"]
									}
									
									FieldFilter {
										id: licenseFilter
										m_fieldId: "LicenseUuid"
										m_filterValueType: "String"
										m_filterOperations: ["Not", "Equal"]
									}
									
									FieldFilter {
										id: excludeFilter
										m_fieldId: "DocumentId"
										m_filterValueType: "String"
										m_filterOperations: ["Not", "Equal"]
									}
									
									FieldFilter {
										id: emptyHardwareFilter
										m_fieldId: "HardwareId"
										m_filterValue: ""
										m_filterValueType: "String"
										m_filterOperations: ["Equal"]
									}

									function updateData(){
										if (!dataController){
											return;
										}
										
										if (visible){
											if (productEditor.productId === ""){
												return;
											}
											
											dataController.collectionId = "SoftwareProducts"
											
											if (!usedLicensesElementView.collection){
												return;
											}
											
											softwareProductCollection.collectionFilter.clearAllFilters();
											softwareProductCollection.collectionFilter.addFieldFilter(productFilter);
											softwareProductCollection.collectionFilter.addFieldFilter(emptyHardwareFilter);
											
											for(var i = 0; i < usedLicensesElementView.collection.table.elements.getItemsCount(); i++){
												let id = usedLicensesElementView.collection.table.elements.getData("id", i);
												let licenseUuid = usedLicensesElementView.collection.table.elements.getData("licenseUuid", i);
												
												let filter = excludeFilter.copyMe();
												filter.m_filterValue = id;
												
												let licFilter = licenseFilter.copyMe();
												licFilter.m_filterValue = licenseUuid;
												
												softwareProductCollection.collectionFilter.addFieldFilter(filter);
												softwareProductCollection.collectionFilter.addFieldFilter(licFilter);
	
											}
											
											softwareProductCollection.collectionFilter.filterChanged()
										}
									}
								}
							}
						}
					}
					
					BaseText {
						id: message;
						color: Style.errorTextColor;
						
						visible: false;
					}
				}
				
				Column {
					id: bindingLicensesColumn
					
					anchors.bottom: availableLicensesColumn.bottom;
					anchors.right: parent.right;
					anchors.rightMargin: Style.sizeMainMargin;
					anchors.left: parent.horizontalCenter;
					anchors.leftMargin: buttonsColumn.width;
					
					ElementView {
						id: usedLicensesElementView;
						width: parent.width;
						name: qsTr("Used licenses");
						
						property SoftwareProductCollectionView collection: null;
						
						bottomComp: Component {
							Rectangle {
								id: rectWrap;
								width: parent.width;
								height: 500;
								
								Component.onCompleted: {
									usedLicensesElementView.collection = bindingProductsCollection;
								}
								
								Component {
									id: lockIconCellComp;
									
									TableCellDelegateBase {
										id: cellDelegate
										Image {
											id: image;
											
											anchors.verticalCenter: parent.verticalCenter;
											anchors.left: parent.left;
											anchors.leftMargin: Style.sizeSmallMargin;
											
											width: 18;
											height: width;
											
											sourceSize.width: width;
											sourceSize.height: height;
										}
										
										onRowIndexChanged: {
											if (!rowDelegate){
												return
											}
											
											let value = cellDelegate.getValue();
											if (value){
												image.source = "../../../../" + Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Normal);
											}
											else{
												image.source = "";
											}
										}
									}
								}
								
								TreeItemModel {
									id: collectionHeadersModel;
									
									Component.onCompleted: {
										rectWrap.updateHeaders();
									}
								}
								
								function updateHeaders(){
									collectionHeadersModel.clear();
									
									let index = collectionHeadersModel.insertNewItem();
									collectionHeadersModel.setData("id", "inUse", index);
									collectionHeadersModel.setData("name", "", index);
									
									index = collectionHeadersModel.insertNewItem();
									collectionHeadersModel.setData("id", "licenseName", index);
									collectionHeadersModel.setData("name", qsTr("Name"), index);
									
									index = collectionHeadersModel.insertNewItem();
									collectionHeadersModel.setData("id", "licenseId", index);
									collectionHeadersModel.setData("name", qsTr("Article"), index);
									
									index = collectionHeadersModel.insertNewItem();
									collectionHeadersModel.setData("id", "deliveryId", index);
									collectionHeadersModel.setData("name", qsTr("Delivery-ID"), index);
									
									index = collectionHeadersModel.insertNewItem();
									collectionHeadersModel.setData("id", "purchaseId", index);
									collectionHeadersModel.setData("name", qsTr("Purchase Order-ID"), index);
									
									index = collectionHeadersModel.insertNewItem();
									collectionHeadersModel.setData("id", "serialNumber", index);
									collectionHeadersModel.setData("name", qsTr("Software-ID"), index);
									
									index = collectionHeadersModel.insertNewItem();
									collectionHeadersModel.setData("id", "customerName", index);
									collectionHeadersModel.setData("name", qsTr("Customer"), index);
									
									bindingProductsCollection.tableViewParamsStoredServer = false;
									bindingProductsCollection.dataController.headersModel = collectionHeadersModel;
								}
								
								SoftwareProductCollectionView {
									id: bindingProductsCollection;
									anchors.fill: parent;
									commandsControllerComp: null;
									filterMenu.decorator: Style.filterPanelDecorator;
									hasSort: false;
									hasFilter: false;
									filterMenuVisible: false;
									hasPagination: false;
									commandsViewComp: undefined;
									tableViewParamsStoredServer: false;
									table.isMultiSelect: false;
									// additionalFieldIds: ["OrderUuid","HardwareUuid", "inUse", "ProductUuid", "CustomerUuid"]
									
									dataControllerComp:
										Component {CollectionRepresentation {
											id: bindingDataController;
											
											Component.onCompleted: {
												additionalFieldIds.push("orderUuid");
												additionalFieldIds.push("hardwareId");
												additionalFieldIds.push("inUse");
												additionalFieldIds.push("productUuid");
												additionalFieldIds.push("customerId");
											}
											
											function updateModel(){}
										}
									}
									
									function registerDocumentInfo(){}
									
									FieldFilter {
										id: hardwareFilter
										m_fieldId: "HardwareId"
										m_filterValue: productEditor.hardwareId
										m_filterValueType: "String"
										m_filterOperations: ["Equal"]
									}
									
									function updateData(){
										if (!dataController){
											return;
										}
										
										dataController.collectionId = "SoftwareProducts";
										
										bindingProductsCollection.collectionFilter.clearAllFilters()
										bindingProductsCollection.collectionFilter.addFieldFilter(hardwareFilter)
										bindingProductsCollection.collectionFilter.filterChanged()
									}
									
									onSelectionChanged: {
										if (selection.length === 0){
											unbindButton.enabled = false
										}
										else{
											if (availableLicensesElementView.collection){
												availableLicensesElementView.collection.table.resetSelection();
											}
											
											let elementId = selection[0]
											let index = bindingProductsCollection.table.getIndexByItemId(elementId);
											
											let elementsModel = bindingProductsCollection.table.elements;
											let inUse = elementsModel.getData("inUse", index);
											
											if (unbindButton.userCanUnbind){
												unbindButton.enabled = true;
											}
											else{
												unbindButton.enabled = !inUse;
											}
										}
									}
									
									onElementsChanged: {
										if (availableLicensesElementView.collection){
											availableLicensesElementView.collection.updateData();
										}
									}
									
									onHeadersChanged: {
										bindingProductsCollection.table.setColumnContentById("inUse", lockIconCellComp);
									}
								}
							}
						}
					}
				}
				
				Column {
					id: buttonsColumn;
					anchors.centerIn: parent;
					spacing: Style.sizeLargeMargin;
					width: Style.sizeLargeMargin;
					
					ToolButton {
						id: bindButton;
						
						anchors.horizontalCenter: parent.horizontalCenter;
						enabled: false;
						width: 18;
						height: 25;
						iconSource: enabled ? "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal):
											  "../../../" + Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled)
						tooltipText: qsTr("Bind to the sensor");
						property bool userCanBind: false;
						
						Component.onCompleted: {
							bindButton.userCanBind = PermissionsController.checkPermission("BindSensor");
						}
						
						onClicked: {
							if (!availableLicensesElementView.collection){
								return;
							}
							
							if (!usedLicensesElementView.collection){
								return;
							}
							
							let selectedProductIds = []
							let softwareIds = productEditor.bindingModel.m_softwareIds;
							if (softwareIds && softwareIds != ""){
								selectedProductIds = softwareIds.split(';')
							}
							
							let indexes = availableLicensesElementView.collection.table.getCheckedItems();
							if (indexes.length === 0){
								return
							}
							
							for (let index of indexes){
								let id = availableLicensesElementView.collection.table.elements.getData("id", index);
								if (!selectedProductIds.includes(id)){
									selectedProductIds.push(id)
									let newIndex = usedLicensesElementView.collection.table.elements.insertNewItem()
									usedLicensesElementView.collection.table.elements.copyItemDataFromModel(newIndex, availableLicensesElementView.collection.table.elements, index);
								}
							}
							
							let products = selectedProductIds.join(';');
							productEditor.bindingModel.m_softwareIds = products;
							
							availableLicensesElementView.collection.updateData();
							availableLicensesElementView.collection.table.resetSelection();
						}
					}
					
					ToolButton {
						id: unbindButton;
						
						anchors.horizontalCenter: parent.horizontalCenter;
						
						enabled: false;
						
						width: 18;
						height: 25;
						
						iconSource: enabled ? "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal):
											  "../../../" + Style.getIconPath("Icons/Left", Icon.State.Off, Icon.Mode.Disabled)
						
						property bool userCanUnbind: false;
						
						tooltipText: qsTr("Unbind from the sensor");
						
						Component.onCompleted: {
							unbindButton.userCanUnbind = PermissionsController.checkPermission("UnbindSensor");
						}
						
						onClicked: {
							if (!usedLicensesElementView.collection){
								return;
							}
							
							let selectedProductIds = productEditor.bindingModel.m_softwareIds.split(';')
							let indexes = usedLicensesElementView.collection.table.getSelectedIndexes()
							if (indexes.length === 0){
								return
							}
							
							let index = indexes[0];
							let elementsModel = usedLicensesElementView.collection.table.elements;
							
							if (!unbindButton.userCanUnbind){
								if (elementsModel.containsKey("inUse", index)){
									let inUse = elementsModel.getData("inUse", index);
									if (inUse){
										return;
									}
								}
							}
							
							let id = elementsModel.getData("id", index);
							if (selectedProductIds.indexOf(id) > -1){
								elementsModel.removeItem(index)
								selectedProductIds.splice(selectedProductIds.indexOf(id), 1);
							}
							
							let products = selectedProductIds.join(';');
							productEditor.bindingModel.m_softwareIds = products;
							
							usedLicensesElementView.collection.table.resetSelection();
							
							if (availableLicensesElementView.collection){
								availableLicensesElementView.collection.updateData();
							}
						}
					}
				}
			}
		}
	}
	
	Loading {
		id: loading;
		anchors.fill: parent;
		visible: false;
		color: Style.backgroundColor2;
	}
	
	TreeItemModel {
		id: tableDecoratorModel;
		
		Component.onCompleted: {
			var cellWidthModel = tableDecoratorModel.addTreeModel("CellWidth");
			
			let index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("WidthPercent", 5, index);
			
			index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("WidthPercent", -1, index);
			
			index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("WidthPercent", -1, index);
			
			index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("WidthPercent", -1, index);
			
			index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("WidthPercent", -1, index);
			
			index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("WidthPercent", -1, index);
			
			index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("WidthPercent", -1, index);
		}
	}
	
	TreeItemModel {
		id: tableDecoratorModel2;
		
		Component.onCompleted: {
			var cellWidthModel = tableDecoratorModel2.addTreeModel("CellWidth");
			
			let index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("Width", -1, index);
			
			index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("Width", -1, index);
			
			index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("Width", -1, index);
			
			index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("Width", -1, index);
			
			index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("Width", -1, index);
			
			index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("Width", -1, index);
		}
	}
}//Container


