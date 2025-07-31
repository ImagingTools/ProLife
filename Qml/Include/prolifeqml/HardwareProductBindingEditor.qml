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
import prolifeLicensesSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

Dialog {
	id: productEditor;
	height: ModalDialogManager.activeView.height - 200
	canMove: false
	title: qsTr("Available Licenses")
	
	property int dialogDefaultWidth: 1000
	property int rootWidth: ModalDialogManager.activeView.width
	onRootWidthChanged: {
		if (rootWidth < dialogDefaultWidth){
			width = rootWidth
		}
		else{
			width = dialogDefaultWidth
		}
	}
	
	property TreeItemModel usedLicensesModel: null
	
	property string productId: ""
	property string hardwareId: "";
	
	Component.onCompleted: {
		addButton(Enums.ok, qsTr("Bind"), false)
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}
	
	onStarted: {
		contentItem.collection.updateData()
	}
	
	property TreeItemModel availableLicensesModel: null
	property var checkedIndexes: []

	contentComp: Component {
		ElementView {
			id: availableLicensesElementView
			width: productEditor.width

			property SoftwareProductCollectionView collection: null
			
			property bool hasSelectedDuplicate: false
			property string duplicateLicenseId: ""
			onHasSelectedDuplicateChanged: {
				if (hasSelectedDuplicate){
					popupContainer.addMessage("error", qsTr("Selected licenses with the same License-ID: ") + duplicateLicenseId, false, "hasSelectedDuplicate")
				}
				else{
					popupContainer.removeMessageById("hasSelectedDuplicate")
				}
			}

			function updateState(){
				hasSelectedDuplicate = false
				let isEnabled = productEditor.checkedIndexes.length > 0
				
				let licenseIds = []
				for (let i = 0; i < productEditor.checkedIndexes.length; i++){
					let licenseId = productEditor.availableLicensesModel.getData("licenseId", productEditor.checkedIndexes[i])
					if (!licenseIds.includes(licenseId)){
						licenseIds.push(licenseId)
					}
					else{
						duplicateLicenseId = licenseId
						hasSelectedDuplicate = true
						isEnabled = false
						break
					}
				}

				productEditor.setButtonEnabled(Enums.ok, isEnabled)
			}
			
			PopupContainer {
				id: popupContainer
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginM
			}
			
			bottomComp: Component {
				Rectangle {
					id: rectWrap;
					width: availableLicensesElementView.width;
					height: productEditor.height - 150;
					
					Component.onCompleted: {
						availableLicensesElementView.collection = softwareProductCollection;
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
						collectionHeadersModel.setData("id", "licenseName", index);
						collectionHeadersModel.setData("name", qsTr("Name"), index);
						
						index = collectionHeadersModel.insertNewItem();
						collectionHeadersModel.setData("id", "licenseId", index);
						collectionHeadersModel.setData("name", qsTr("Article"), index);

						index = collectionHeadersModel.insertNewItem();
						collectionHeadersModel.setData("id", "serialNumber", index);
						collectionHeadersModel.setData("name", qsTr("Software-ID"), index);

						let filteringInfoIds = []
						for (let i = 0; i < collectionHeadersModel.getItemsCount(); i++){
							let infoId = collectionHeadersModel.getData("id", i);
							filteringInfoIds.push(infoId)
						}
						
						softwareProductCollection.collectionFilter.setFilteringInfoIds(filteringInfoIds);
						
						softwareProductCollection.tableViewParamsStoredServer = false;
						softwareProductCollection.dataController.headersModel = collectionHeadersModel;
					}
					
					SoftwareProductCollectionView {
						id: softwareProductCollection;
						anchors.fill: parent
						commandsControllerComp: null;
						table.checkable: true;
						table.selectable: false;
						tableViewParamsStoredServer: false;
						canResetFilters: false

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
						
						function registerFilters(){
							registerFieldFilterDelegate("Products", productsDelegateFilterComp)
						}
						
						Component {
							id: productsDelegateFilterComp
							FieldFilterDelegate {
								id: productsDelegateFilter
								name: qsTr("Products")
								defaultFieldFilter.m_fieldId: "ProductUuid"
								readOnly: true
								
								OptionsListAdapter {
									id: optionsListAdapter
									collectionModel: CachedProductCollection.softwareProductsModel
									onCollectionModelChanged: {
										productsDelegateFilter.setOptionsList(m_options)
										
										for (let i = 0; i < m_options.count; i++){
											let optionId = productsDelegateFilter.getOptionId(i)
											if (productEditor.productId === optionId){
												productsDelegateFilter.setSelectedIndex(i)
												break
											}
										}
									}
								}
							}
						}
						
						onElementsChanged: {
							table.uncheckAll();
							productEditor.availableLicensesModel = table.elements
						}
						
						onCheckedItemsChanged: {
							productEditor.checkedIndexes = table.getCheckedItems()

							availableLicensesElementView.updateState()
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
							id: emptyLicenseIdFilter
							m_fieldId: "LicenseId"
							m_filterValueType: "String"
							m_filterValue: ""
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
							dataController.collectionId = "SoftwareProducts"
							
							softwareProductCollection.collectionFilter.removeFilterByFieldId(productFilter.m_fieldId);
							softwareProductCollection.collectionFilter.removeFilterByFieldId(emptyHardwareFilter.m_fieldId);
							softwareProductCollection.collectionFilter.removeFilterByFieldId(excludeFilter.m_fieldId);
							softwareProductCollection.collectionFilter.removeFilterByFieldId(licenseFilter.m_fieldId);
							softwareProductCollection.collectionFilter.removeFilterByFieldId(emptyLicenseIdFilter.m_fieldId);
							
							softwareProductCollection.collectionFilter.addFieldFilter(productFilter);
							softwareProductCollection.collectionFilter.addFieldFilter(emptyHardwareFilter);
							
							if (productEditor.usedLicensesModel){
								for(var i = 0; i < productEditor.usedLicensesModel.getItemsCount(); i++){
									let id = productEditor.usedLicensesModel.getData("id", i);
									let licenseUuid = productEditor.usedLicensesModel.getData("licenseUuid", i);
									
									let filter = excludeFilter.copyMe();
									filter.m_filterValue = id;
									
									let licFilter = licenseFilter.copyMe();
									licFilter.m_filterValue = licenseUuid;
									
									softwareProductCollection.collectionFilter.addFieldFilter(filter);
									softwareProductCollection.collectionFilter.addFieldFilter(licFilter);
								}
							}
							
							// Exclude licenses with empty Article
							softwareProductCollection.collectionFilter.addFieldFilter(emptyLicenseIdFilter);
							
							softwareProductCollection.collectionFilter.filterChanged()
						}
					}
				}
			}
		}
	}
}//Container


