import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import prolifeHardwareComponentsSdl 1.0
import prolifeAccountsSdl 1.0

/**
 * HardwareComponentEditor - catalog card of a hardware component.
 *
 * Pages: Component (identity, type, lifecycle) and Supply (manufacturer, supplier, MPN).
 */
ViewBase {
	id: componentEditorContainer;

	anchors.fill: parent;
	contentColor: Style.baseColor

	property HardwareComponentData componentData: model

	property TreeItemModel componentTypeModel: componentTypeOptions
	property TreeItemModel lifecycleModel: lifecycleOptions

	TreeItemModel {
		id: componentTypeOptions

		Component.onCompleted: {
			componentEditorContainer.addOption(componentTypeOptions, "Part", qsTr("Part"))
			componentEditorContainer.addOption(componentTypeOptions, "Module", qsTr("Module"))
			componentEditorContainer.addOption(componentTypeOptions, "Assembly", qsTr("Assembly"))
			componentEditorContainer.addOption(componentTypeOptions, "Consumable", qsTr("Consumable"))
		}
	}

	TreeItemModel {
		id: lifecycleOptions

		Component.onCompleted: {
			componentEditorContainer.addOption(lifecycleOptions, "Active", qsTr("Active"))
			componentEditorContainer.addOption(lifecycleOptions, "NRND", qsTr("Not recommended for new designs"))
			componentEditorContainer.addOption(lifecycleOptions, "EOL", qsTr("End of life"))
		}
	}

	function addOption(optionModel, id, name){
		let index = optionModel.insertNewItem()
		optionModel.setData("id", id, index)
		optionModel.setData("name", name, index)
	}

	function findOption(optionModel, id){
		for (let i = 0; i < optionModel.getItemsCount(); i++){
			if (optionModel.getData("id", i) === id){
				return i
			}
		}

		return -1
	}

	function optionId(optionModel, index){
		if (index < 0 || index >= optionModel.getItemsCount()){
			return ""
		}

		return optionModel.getData("id", index)
	}

	function updateGui(){
		let componentPage = multiPageView.getPageById("Component")
		if (componentPage){
			componentPage.updateGui()
		}

		let supplyPage = multiPageView.getPageById("Supply")
		if (supplyPage){
			supplyPage.updateGui()
		}
	}

	function updateModel(){
		if (!componentEditorContainer.componentData){
			return
		}

		let componentPage = multiPageView.getPageById("Component")
		if (componentPage){
			componentPage.updateModel()
		}

		let supplyPage = multiPageView.getPageById("Supply")
		if (supplyPage){
			supplyPage.updateModel()
		}
	}

	function setReadOnly(readOnly){
		let componentPage = multiPageView.getPageById("Component")
		if (componentPage){
			componentPage.applyReadOnly(readOnly)
		}

		let supplyPage = multiPageView.getPageById("Supply")
		if (supplyPage){
			supplyPage.applyReadOnly(readOnly)
		}
	}

	MultiPageView {
		id: multiPageView
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS

		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("Component", qsTr("Component"), componentPageComp, "Icons/Settings")
			multiPageView.addPage("Supply", qsTr("Supply"), supplyPageComp, "Icons/Organization")
			multiPageView.currentIndex = 0
		}

		Component.onCompleted: {
			multiPageView.updatePages()
		}
	}

	Component {
		id: componentPageComp

		Item {
			id: componentPage
			anchors.fill: parent

			function updateGui(){
				if (!componentEditorContainer.componentData){
					return
				}

				partNumberInput.text = componentEditorContainer.componentData.m_partNumber
				nameInput.text = componentEditorContainer.componentData.m_name
				descriptionInput.text = componentEditorContainer.componentData.m_description
				revisionInput.text = componentEditorContainer.componentData.m_revision
				typeCB.currentIndex = componentEditorContainer.findOption(componentEditorContainer.componentTypeModel, componentEditorContainer.componentData.m_componentType)
				lifecycleCB.currentIndex = componentEditorContainer.findOption(componentEditorContainer.lifecycleModel, componentEditorContainer.componentData.m_lifecycleStatus)
				serializedSwitch.checked = componentEditorContainer.componentData.m_serialized
			}

			function updateModel(){
				if (!componentEditorContainer.componentData){
					return
				}

				componentEditorContainer.componentData.m_partNumber = partNumberInput.text
				componentEditorContainer.componentData.m_name = nameInput.text
				componentEditorContainer.componentData.m_description = descriptionInput.text
				componentEditorContainer.componentData.m_revision = revisionInput.text
				componentEditorContainer.componentData.m_componentType = componentEditorContainer.optionId(componentEditorContainer.componentTypeModel, typeCB.currentIndex)
				componentEditorContainer.componentData.m_lifecycleStatus = componentEditorContainer.optionId(componentEditorContainer.lifecycleModel, lifecycleCB.currentIndex)
				componentEditorContainer.componentData.m_serialized = serializedSwitch.checked
			}

			function applyReadOnly(readOnly){
				let canChange = PermissionsController.checkPermission("/HardwareManagement/Edit/Change") || PermissionsController.checkPermission("/HardwareManagement/Edit/Add")
				partNumberInput.readOnly = readOnly || !canChange
				nameInput.readOnly = readOnly || !canChange
				descriptionInput.readOnly = readOnly || !canChange
				revisionInput.readOnly = readOnly || !canChange
				typeCB.changeable = !readOnly && canChange
				lifecycleCB.changeable = !readOnly && canChange
				serializedSwitch.readOnly = readOnly || !canChange
			}

			Component.onCompleted: {
				componentPage.updateGui()
				componentPage.applyReadOnly(componentEditorContainer.readOnly)
			}

			CustomScrollbar {
				id: scrollbar;
				z: parent.z + 1;
				anchors.right: parent.right;
				anchors.top: flickable.top;
				anchors.bottom: flickable.bottom;
				secondSize: Style.marginM;
				targetItem: flickable;
			}

			Flickable {
				id: flickable;
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.bottom: parent.bottom;
				anchors.bottomMargin: Style.marginXL;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginXL;
				anchors.right: scrollbar.left;
				anchors.rightMargin: Style.marginXL;
				contentHeight: bodyColumn.height + 2 * Style.marginXL;

				boundsBehavior: Flickable.StopAtBounds;
				clip: true;

				Column {
					id: bodyColumn;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: Math.min(parent.width, Style.contentWidthMax);
					spacing: Style.marginXL;

					GroupHeaderView {
						width: parent.width;
						title: qsTr("Component");
						groupView: componentGroup;
					}

					RegularExpressionValidator {
						id: notEmptyValidator;
						regularExpression: /^(?!\s*$).+/;
					}

					GroupElementView {
						id: componentGroup;
						objectName: "ComponentGroup";
						width: parent.width;

						TextInputElementView {
							id: partNumberInput;
							objectName: "PartNumberInput";
							name: qsTr("Part Number");
							placeHolderText: qsTr("Enter the part number");
							isRequired: true;
							showErrorWhenInvalid: true;
							deferErrorUntilEdited: true;
							errorText: qsTr("Please enter the part number");
							textInputValidator: notEmptyValidator;

							onEditingFinished: {
								componentEditorContainer.doUpdateModel();
							}
						}

						TextInputElementView {
							id: nameInput;
							objectName: "ComponentNameInput";
							name: qsTr("Name");
							placeHolderText: qsTr("Enter the component name");
							isRequired: true;
							showErrorWhenInvalid: true;
							deferErrorUntilEdited: true;
							errorText: qsTr("Please enter the component name");
							textInputValidator: notEmptyValidator;

							onEditingFinished: {
								componentEditorContainer.doUpdateModel();
							}
						}

						ComboBoxElementView {
							id: typeCB;
							objectName: "ComponentTypeCombo";
							name: qsTr("Type");
							model: componentEditorContainer.componentTypeModel;

							onCurrentIndexChanged: {
								if (componentEditorContainer.componentData && componentEditorContainer.optionId(componentEditorContainer.componentTypeModel, typeCB.currentIndex) !== componentEditorContainer.componentData.m_componentType){
									componentEditorContainer.doUpdateModel();
								}
							}
						}

						TextInputElementView {
							id: revisionInput;
							objectName: "RevisionInput";
							name: qsTr("Revision");
							placeHolderText: qsTr("Enter the revision");

							onEditingFinished: {
								componentEditorContainer.doUpdateModel();
							}
						}

						ComboBoxElementView {
							id: lifecycleCB;
							objectName: "LifecycleCombo";
							name: qsTr("Lifecycle");
							model: componentEditorContainer.lifecycleModel;

							onCurrentIndexChanged: {
								if (componentEditorContainer.componentData && componentEditorContainer.optionId(componentEditorContainer.lifecycleModel, lifecycleCB.currentIndex) !== componentEditorContainer.componentData.m_lifecycleStatus){
									componentEditorContainer.doUpdateModel();
								}
							}
						}

						SwitchElementView {
							id: serializedSwitch;
							objectName: "SerializedSwitch";
							name: qsTr("Serial Numbers");
							description: qsTr("Every installed unit carries its own serial number");

							onCheckedChanged: {
								if (componentEditorContainer.componentData && serializedSwitch.checked !== componentEditorContainer.componentData.m_serialized){
									componentEditorContainer.doUpdateModel();
								}
							}
						}

						TextInputElementView {
							id: descriptionInput;
							objectName: "ComponentDescriptionInput";
							name: qsTr("Description");
							placeHolderText: qsTr("Enter the description");

							onEditingFinished: {
								componentEditorContainer.doUpdateModel();
							}
						}
					}
				}
			}
		}
	}

	Component {
		id: supplyPageComp

		Item {
			id: supplyPage
			anchors.fill: parent

			readonly property bool canEdit: !componentEditorContainer.readOnly
				&& (PermissionsController.checkPermission("/HardwareManagement/Edit/Change") || PermissionsController.checkPermission("/HardwareManagement/Edit/Add"))

			// The suppliers of the component: {supplierId, supplierName, sku}, in the order they were added.
			property var offers: []

			function updateGui(){
				if (!componentEditorContainer.componentData){
					return
				}

				manufacturerSelect.selectedId = componentEditorContainer.componentData.m_manufacturerId
				manufacturerSelect.selectedText = componentEditorContainer.componentData.m_manufacturerName
				mpnInput.text = componentEditorContainer.componentData.m_mpn

				let rows = []
				let elements = componentEditorContainer.componentData.m_supplierOffers
				for (let i = 0; elements && i < elements.getItemsCount(); i++){
					let item = elements.get(i).item
					rows.push({supplierId: String(item.m_supplierId), supplierName: String(item.m_supplierName), sku: String(item.m_sku)})
				}

				supplyPage.offers = rows
			}

			function updateModel(){
				if (!componentEditorContainer.componentData){
					return
				}

				componentEditorContainer.componentData.m_manufacturerId = manufacturerSelect.selectedId
				componentEditorContainer.componentData.m_mpn = mpnInput.text
			}

			// Writes the suppliers to the document; the list is rewritten as a whole.
			function applyOffers(){
				let componentData = componentEditorContainer.componentData
				if (!componentData){
					return
				}

				if (!componentData.hasSupplierOffers()){
					componentData.emplaceSupplierOffers()
				}

				let elements = componentData.m_supplierOffers
				elements.clear()
				for (let i = 0; i < supplyPage.offers.length; i++){
					let element = componentData.createSupplierOffersArrayElement()
					element.m_supplierId = supplyPage.offers[i].supplierId
					element.m_supplierName = supplyPage.offers[i].supplierName
					element.m_sku = supplyPage.offers[i].sku
					element.owner = componentData
					element.connectProperties()
					elements.append({ "item": element })
				}

				// clear() and append() are silent: the rewrite is announced once.
				componentData.modelChanged([])
			}

			function setSku(index, sku){
				if (supplyPage.offers[index].sku === sku){
					return
				}

				let rows = supplyPage.offers.slice()
				rows[index] = {supplierId: rows[index].supplierId, supplierName: rows[index].supplierName, sku: sku}
				supplyPage.offers = rows
				supplyPage.applyOffers()
			}

			function removeOffer(index){
				let rows = supplyPage.offers.slice()
				rows.splice(index, 1)
				supplyPage.offers = rows
				supplyPage.applyOffers()
			}

			// The suppliers chosen in the selector; the article numbers of the kept ones stay.
			function applySelection(selectedItems){
				let rows = []
				for (let i = 0; i < selectedItems.length; i++){
					let supplierId = String(selectedItems[i].id)
					let sku = ""
					for (let k = 0; k < supplyPage.offers.length; k++){
						if (supplyPage.offers[k].supplierId === supplierId){
							sku = supplyPage.offers[k].sku
						}
					}

					rows.push({supplierId: supplierId, supplierName: String(selectedItems[i].name), sku: sku})
				}

				supplyPage.offers = rows
				supplyPage.applyOffers()
			}

			function selectorItems(){
				let items = []
				for (let i = 0; i < supplyPage.offers.length; i++){
					items.push({id: supplyPage.offers[i].supplierId, name: supplyPage.offers[i].supplierName})
				}

				return items
			}

			function applyReadOnly(readOnly){
				manufacturerSelect.changeable = supplyPage.canEdit
				mpnInput.readOnly = !supplyPage.canEdit
			}

			Component.onCompleted: {
				supplyPage.updateGui()
				supplyPage.applyReadOnly(componentEditorContainer.readOnly)
			}

			CustomScrollbar {
				id: supplyScrollbar;
				z: parent.z + 1;
				anchors.right: parent.right;
				anchors.top: supplyFlickable.top;
				anchors.bottom: supplyFlickable.bottom;
				secondSize: Style.marginM;
				targetItem: supplyFlickable;
			}

			Flickable {
				id: supplyFlickable;
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.bottom: parent.bottom;
				anchors.bottomMargin: Style.marginXL;
				anchors.left: parent.left;
				anchors.leftMargin: Style.marginXL;
				anchors.right: supplyScrollbar.left;
				anchors.rightMargin: Style.marginXL;
				contentHeight: supplyColumn.height + 2 * Style.marginXL;

				boundsBehavior: Flickable.StopAtBounds;
				clip: true;

				Column {
					id: supplyColumn;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: Math.min(parent.width, Style.contentWidthMax);
					spacing: Style.marginXL;

					GroupHeaderView {
						width: parent.width;
						title: qsTr("Manufacturer");
						groupView: manufacturerGroup;
					}

					GroupElementView {
						id: manufacturerGroup;
						objectName: "ManufacturerGroup";
						width: parent.width;

						CollectionSelectElementView {
							id: manufacturerSelect;
							objectName: "ManufacturerSelect";
							name: qsTr("Manufacturer");
							placeHolderText: qsTr("Select the manufacturer");
							commandId: ProlifeAccountsSdlCommandIds.s_accountsList;
							fields: ["id", "name", "country"];
							descriptionField: "country";
							textFilterFieldIds: ["name", "country"];
							sortByField: "name";
							clearable: true;

							function getHeaders(){
								return {"isManufacturer": "true"}
							}

							function setCustomInputParams(inputParams){
								inputParams.InsertField("collectionId", "HardwareVendors")
							}

							onItemSelected: {
								componentEditorContainer.doUpdateModel();
							}
						}

						TextInputElementView {
							id: mpnInput;
							objectName: "MpnInput";
							name: qsTr("Manufacturer Part Number");
							placeHolderText: qsTr("Enter the manufacturer part number");

							onEditingFinished: {
								componentEditorContainer.doUpdateModel();
							}
						}
					}

					SectionHeaderView {
						width: parent.width;
						title: qsTr("Suppliers") + " (" + supplyPage.offers.length + ")";

						SectionLink {
							id: addSupplierLink;
							objectName: "AddSupplierLink";
							visible: supplyPage.canEdit;
							text: "+ " + qsTr("Add Supplier");
							onClicked: {
								supplierSelector.items = supplyPage.selectorItems()
								supplierSelector.openSelector(addSupplierLink)
							}
						}
					}

					// Picker only: the suppliers are requested page by page and searched on the server.
					CollectionItemSelectElementView {
						id: supplierSelector
						width: parent.width
						commandId: ProlifeAccountsSdlCommandIds.s_accountsList
						fields: ["id", "name", "country"]
						titleField: "name"
						textFilterFieldIds: ["name", "country"]
						sortByField: "name"
						label: qsTr("Suppliers")
						addButtonText: qsTr("Add Supplier")
						filterPlaceholder: qsTr("Search by name or country")
						visible: false

						function getHeaders(){
							return {"isSupplier": "true"}
						}

						function setCustomInputParams(inputParams){
							inputParams.InsertField("collectionId", "HardwareVendors")
						}

						onSelectionChanged: {
							let selectedItems = supplierSelector.dataProvider.getSelectedItems()
							let items = []
							for (let i = 0; i < selectedItems.length; i++){
								let values = selectedItems[i].values ? selectedItems[i].values : {}
								items.push({id: selectedItems[i].id, name: values["name"] ? values["name"] : selectedItems[i].title})
							}

							supplyPage.applySelection(items)
						}
					}

					Text {
						width: parent.width;
						visible: supplyPage.offers.length === 0;
						text: qsTr("No supplier is given: where to buy the component is unknown.");
						color: Style.subtitleColor;
						font.family: Style.fontFamily;
						font.pixelSize: Style.fontSizeM;
						wrapMode: Text.WordWrap;
					}

					// A table framed like a group: the supplier, its article number for the component, removal.
					Rectangle {
						id: offersTable;
						objectName: "SupplierOffersTable";
						width: parent.width;
						height: offersColumn.height + 2;
						visible: supplyPage.offers.length > 0;
						radius: Style.radiusM;
						color: Style.baseColor;
						border.width: 1;
						border.color: Style.borderColor;

						readonly property int skuColumnX: Math.round(offersTable.width * 0.45);
						readonly property int removeColumnWidth: Style.controlHeightM + Style.marginM;

						Column {
							id: offersColumn;
							x: 1;
							y: 1;
							width: offersTable.width - 2;

							Item {
								width: parent.width;
								height: Style.controlHeightM;

								Text {
									anchors.left: parent.left;
									anchors.leftMargin: Style.marginXL;
									anchors.verticalCenter: parent.verticalCenter;
									text: qsTr("Supplier");
									color: Style.subtitleColor;
									font.family: Style.fontFamily;
									font.pixelSize: Style.fontSizeS;
								}

								Text {
									x: offersTable.skuColumnX;
									anchors.verticalCenter: parent.verticalCenter;
									text: qsTr("Supplier Article (SKU)");
									color: Style.subtitleColor;
									font.family: Style.fontFamily;
									font.pixelSize: Style.fontSizeS;
								}
							}

							Repeater {
								model: supplyPage.offers;

								delegate: Item {
									id: offerRow;
									width: offersColumn.width;
									height: Style.controlHeightL;

									property int offerIndex: index

									Rectangle {
										anchors.top: parent.top;
										width: parent.width;
										height: 1;
										color: Style.borderColor;
									}

									Text {
										objectName: "SupplierName";
										anchors.left: parent.left;
										anchors.leftMargin: Style.marginXL;
										anchors.verticalCenter: parent.verticalCenter;
										width: offersTable.skuColumnX - Style.marginXL - Style.marginM;
										text: supplyPage.offers[offerRow.offerIndex].supplierName;
										color: Style.textColor;
										font.family: Style.fontFamily;
										font.pixelSize: Style.fontSizeM;
										elide: Text.ElideRight;
									}

									CustomTextField {
										objectName: "SupplierSkuInput";
										x: offersTable.skuColumnX;
										anchors.verticalCenter: parent.verticalCenter;
										width: offerRow.width - offersTable.skuColumnX - offersTable.removeColumnWidth - Style.marginM;
										height: Style.controlHeightM;
										placeHolderText: qsTr("Enter the article number");
										readOnly: !supplyPage.canEdit;
										text: supplyPage.offers[offerRow.offerIndex].sku;

										onEditingFinished: {
											supplyPage.setSku(offerRow.offerIndex, text);
										}
									}

									Button {
										objectName: "RemoveSupplierButton";
										anchors.right: parent.right;
										anchors.rightMargin: Style.marginM;
										anchors.verticalCenter: parent.verticalCenter;
										width: Style.controlHeightM;
										height: Style.controlHeightM;
										visible: supplyPage.canEdit;
										decorator: Component { IconButtonDecorator {} }
										iconSource: "qrc:/" + Style.getIconPath("Icons/Delete", Icon.State.On, Icon.Mode.Normal);
										tooltipText: qsTr("Remove the supplier");

										onClicked: {
											supplyPage.removeOffer(offerRow.offerIndex);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
