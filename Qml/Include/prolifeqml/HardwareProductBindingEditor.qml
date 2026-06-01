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

					TableHeaders {
						Component.onCompleted: {
							addHeader("licenseName", qsTr("Name"))
							addHeader("licenseId", qsTr("Article"))
							addHeader("serialNumber", qsTr("Software-ID"))
							addHeader("isMultiple", qsTr("Is Multiple"))
							addHeader("productCount", qsTr("Total"))
							
							let filteringInfoIds = ["licenseName", "licenseId", "serialNumber"]

							softwareProductCollection.collectionFilter.setFilteringInfoIds(filteringInfoIds);
							softwareProductCollection.tableViewParamsStoredServer = false;
							softwareProductCollection.dataController.headersReceived(this)
						}
					}

					SoftwareProductCollectionView {
						id: softwareProductCollection;
						anchors.fill: parent
						commandsControllerComp: null;
						table.checkable: true;
						table.selectable: false;
						tableViewParamsStoredServer: false;
						canResetFilters: false
						commandsDelegateComp: null

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
						
						onHeadersChanged: {
							table.setColumnContentById("isMultiple", isMultipleColumnDelegateComp)
						}

						Component {
							id: isMultipleColumnDelegateComp;
							TableCellDelegateBase {
								id: cellDelegate
	
								Image {
									id: image;
									anchors.verticalCenter: parent.verticalCenter;
									anchors.left: parent.left;
									anchors.leftMargin: Style.marginM;
									width: Style.iconSizeM;
									height: width;
									source: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal);
									sourceSize.width: width;
									sourceSize.height: height;
								}
	
								onReused: {
									if (!rowDelegate){
										return
									}
	
									if (rowIndex >= 0){
										let isMultiple = cellDelegate.getValue();
										image.visible = isMultiple;
									}
								}
							}
						}

						onElementsChanged: {
							table.uncheckAll()
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

						GroupFilter {
							id: notIsMultiSoftwareFilter
							m_logicalOperation: "And"
						}
						
						FieldFilter {
							id: isMultipleFilter
							m_fieldId: "IsMultiProduct"
							m_filterValue: "false"
							m_filterValueType: "Bool"
							m_filterOperations: ["Equal"]
						}

						ArrayFieldFilter {
							id: emptyHardwareFilter
							m_fieldId: "HardwareId"
							m_filterValueType: "String"
							m_filterOperations: ["ArrayIsEmpty"]
						}

						GroupFilter {
							id: isMultiSoftwareFilter
							m_logicalOperation: "And"
						}

						FieldFilter {
							id: isMultipleFilter2
							m_fieldId: "IsMultiProduct"
							m_filterValue: "true"
							m_filterValueType: "Bool"
							m_filterOperations: ["Equal"]
						}

						FieldFilter {
							id: productCountFilter
							m_fieldId: "ProductCount"
							m_filterValue: "0"
							m_filterValueType: "Integer"
							m_filterOperations: ["Greater"]
						}

						GroupFilter {
							id: softwareFilter
							m_logicalOperation: "Or"
						}

						function updateData(){
							dataController.collectionId = "SoftwareProducts"
							
							softwareProductCollection.collectionFilter.removeFilterByFieldId(productFilter.m_fieldId);
							// softwareProductCollection.collectionFilter.removeFilterByFieldId(emptyHardwareFilter.m_fieldId);
							softwareProductCollection.collectionFilter.removeFilterByFieldId(excludeFilter.m_fieldId);
							softwareProductCollection.collectionFilter.removeFilterByFieldId(licenseFilter.m_fieldId);
							softwareProductCollection.collectionFilter.removeFilterByFieldId(emptyLicenseIdFilter.m_fieldId);
							softwareProductCollection.collectionFilter.removeGroupFilter(softwareFilter);
							
							softwareProductCollection.collectionFilter.addFieldFilter(productFilter);

							// Not multi software filter
							notIsMultiSoftwareFilter.emplaceFieldFilters()
							notIsMultiSoftwareFilter.m_fieldFilters.addElement(isMultipleFilter)
							notIsMultiSoftwareFilter.m_fieldFilters.addElement(emptyHardwareFilter)

							// Multi software filter
							isMultiSoftwareFilter.emplaceFieldFilters()
							isMultiSoftwareFilter.m_fieldFilters.addElement(isMultipleFilter2)
							isMultiSoftwareFilter.m_fieldFilters.addElement(productCountFilter)

							softwareFilter.emplaceGroupFilters()
							softwareFilter.m_groupFilters.addElement(notIsMultiSoftwareFilter)
							softwareFilter.m_groupFilters.addElement(isMultiSoftwareFilter)

							softwareProductCollection.collectionFilter.addGroupFilter(softwareFilter)
							
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


