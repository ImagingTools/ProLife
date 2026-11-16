import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import prolifeHardwareComponentsSdl 1.0

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

					GroupElementView {
						id: componentGroup;
						objectName: "ComponentGroup";
						width: parent.width;

						TextInputElementView {
							id: partNumberInput;
							objectName: "PartNumberInput";
							name: qsTr("Part Number");
							placeHolderText: qsTr("Enter the part number");

							onEditingFinished: {
								componentEditorContainer.doUpdateModel();
							}
						}

						TextInputElementView {
							id: nameInput;
							objectName: "ComponentNameInput";
							name: qsTr("Name");
							placeHolderText: qsTr("Enter the component name");

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

			function updateGui(){
				if (!componentEditorContainer.componentData){
					return
				}

				manufacturerInput.text = componentEditorContainer.componentData.m_manufacturer
				mpnInput.text = componentEditorContainer.componentData.m_mpn
				supplierInput.text = componentEditorContainer.componentData.m_supplier
			}

			function updateModel(){
				if (!componentEditorContainer.componentData){
					return
				}

				componentEditorContainer.componentData.m_manufacturer = manufacturerInput.text
				componentEditorContainer.componentData.m_mpn = mpnInput.text
				componentEditorContainer.componentData.m_supplier = supplierInput.text
			}

			function applyReadOnly(readOnly){
				let canChange = PermissionsController.checkPermission("/HardwareManagement/Edit/Change") || PermissionsController.checkPermission("/HardwareManagement/Edit/Add")
				manufacturerInput.readOnly = readOnly || !canChange
				mpnInput.readOnly = readOnly || !canChange
				supplierInput.readOnly = readOnly || !canChange
			}

			Component.onCompleted: {
				supplyPage.updateGui()
				supplyPage.applyReadOnly(componentEditorContainer.readOnly)
			}

			Column {
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.horizontalCenter: parent.horizontalCenter;
				width: Math.min(parent.width - 2 * Style.marginXL, Style.contentWidthMax);
				spacing: Style.marginXL;

				GroupHeaderView {
					width: parent.width;
					title: qsTr("Supply");
					groupView: supplyGroup;
				}

				GroupElementView {
					id: supplyGroup;
					objectName: "SupplyGroup";
					width: parent.width;

					TextInputElementView {
						id: manufacturerInput;
						objectName: "ManufacturerInput";
						name: qsTr("Manufacturer");
						placeHolderText: qsTr("Enter the manufacturer");

						onEditingFinished: {
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

					TextInputElementView {
						id: supplierInput;
						objectName: "SupplierInput";
						name: qsTr("Supplier");
						placeHolderText: qsTr("Enter the supplier");

						onEditingFinished: {
							componentEditorContainer.doUpdateModel();
						}
					}
				}
			}
		}
	}
}
