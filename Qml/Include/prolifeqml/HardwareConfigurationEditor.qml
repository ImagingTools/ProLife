import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtlicgui 1.0
import prolifeHardwareConfigurationsSdl 1.0
import prolifeHardwareComponentsSdl 1.0
import imtlicProductsSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

/**
 * HardwareConfigurationEditor - variant of a hardware model that can be ordered and built.
 *
 * Pages: Configuration (article, name, hardware model) and Components (the parts it takes,
 * picked from the component catalog that is searched on the server).
 */
ViewBase {
	id: configurationEditorContainer;

	anchors.fill: parent;
	contentColor: Style.baseColor

	property HardwareConfigurationData configurationData: model

	// "part number - name" of the components, from the document and from the picker.
	property var componentTitles: null

	onConfigurationDataChanged: {
		configurationEditorContainer.loadComponentTitles()
	}

	Component.onCompleted: {
		configurationEditorContainer.loadComponentTitles()
	}

	// Adds the titles of the components [{id, partNumber, name}] in one pass.
	function addComponentTitles(components){
		let titles = {}
		let current = configurationEditorContainer.componentTitles
		for (let key in current){
			titles[key] = current[key]
		}

		for (let i = 0; i < components.length; i++){
			let componentId = String(components[i].id)
			let title = components[i].name ? String(components[i].name) : componentId
			if (components[i].partNumber && String(components[i].partNumber) !== ""){
				title = String(components[i].partNumber) + " \u2014 " + title
			}
			titles[componentId] = title
		}

		configurationEditorContainer.componentTitles = titles
	}

	function componentTitle(componentId){
		let titles = configurationEditorContainer.componentTitles
		let title = titles ? titles[String(componentId)] : null

		return title ? title : String(componentId)
	}

	// The server sends the names of the components along with their IDs.
	function loadComponentTitles(){
		let components = configurationEditorContainer.configurationData ? configurationEditorContainer.configurationData.m_components : null
		let entries = []
		for (let i = 0; components && i < components.count; i++){
			let component = components.get(i).item
			entries.push({id: component.m_id, partNumber: component.m_partNumber, name: component.m_name})
		}

		configurationEditorContainer.addComponentTitles(entries)
	}

	function updateGui(){
		configurationEditorContainer.loadComponentTitles()

		let configurationPage = multiPageView.getPageById("Configuration")
		if (configurationPage){
			configurationPage.updateGui()
		}

		let componentsPage = multiPageView.getPageById("Components")
		if (componentsPage){
			componentsPage.updateGui()
		}
	}

	function updateModel(){
		if (!configurationEditorContainer.configurationData){
			return
		}

		let configurationPage = multiPageView.getPageById("Configuration")
		if (configurationPage){
			configurationPage.updateModel()
		}
	}

	function setReadOnly(readOnly){
		let configurationPage = multiPageView.getPageById("Configuration")
		if (configurationPage){
			configurationPage.applyReadOnly(readOnly)
		}
	}

	MultiPageView {
		id: multiPageView
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS

		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("Configuration", qsTr("Configuration"), configurationPageComp, "Icons/Settings")
			multiPageView.addPage("Components", qsTr("Components"), componentsPageComp, "Icons/FeaturePackage")
			multiPageView.currentIndex = 0
		}

		Component.onCompleted: {
			multiPageView.updatePages()
		}
	}

	Component {
		id: configurationPageComp

		Item {
			id: configurationPage
			anchors.fill: parent

			function updateGui(){
				if (!configurationEditorContainer.configurationData){
					return
				}

				articleInput.text = configurationEditorContainer.configurationData.m_configurationId
				nameInput.text = configurationEditorContainer.configurationData.m_name
				descriptionInput.text = configurationEditorContainer.configurationData.m_description
				modelSelect.selectedId = configurationEditorContainer.configurationData.m_productId
				modelSelect.selectedText = configurationEditorContainer.configurationData.m_productName
			}

			function updateModel(){
				if (!configurationEditorContainer.configurationData){
					return
				}

				configurationEditorContainer.configurationData.m_configurationId = articleInput.text
				configurationEditorContainer.configurationData.m_name = nameInput.text
				configurationEditorContainer.configurationData.m_description = descriptionInput.text
				configurationEditorContainer.configurationData.m_productId = modelSelect.selectedId
				configurationEditorContainer.configurationData.m_productName = modelSelect.selectedText
			}

			function applyReadOnly(readOnly){
				let canChange = PermissionsController.checkPermission("/HardwareManagement/Edit/Change") || PermissionsController.checkPermission("/HardwareManagement/Edit/Add")
				articleInput.readOnly = readOnly || !canChange
				nameInput.readOnly = readOnly || !canChange
				descriptionInput.readOnly = readOnly || !canChange
				modelSelect.changeable = !readOnly && canChange
			}

			Component.onCompleted: {
				configurationPage.updateGui()
				configurationPage.applyReadOnly(configurationEditorContainer.readOnly)
			}

			Column {
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.horizontalCenter: parent.horizontalCenter;
				width: Math.min(parent.width - 2 * Style.marginXL, Style.contentWidthMax);
				spacing: Style.marginXL;

				GroupHeaderView {
					width: parent.width;
					title: qsTr("Configuration");
					groupView: configurationGroup;
				}

				FieldFilter {
					id: hardwareCategoryFilter
					m_fieldId: "CategoryId"
					m_filterValue: "Hardware"
					m_filterValueType: "String"
					m_filterOperations: ["Equal"]
				}

				GroupFilter {
					id: hardwareCategoryGroup
					m_logicalOperation: "And"

					Component.onCompleted: {
						hardwareCategoryGroup.emplaceFieldFilters()
						hardwareCategoryGroup.m_fieldFilters.addElement(hardwareCategoryFilter)
					}
				}

				RegularExpressionValidator {
					id: notEmptyValidator;
					regularExpression: /^(?!\s*$).+/;
				}

				GroupElementView {
					id: configurationGroup;
					objectName: "ConfigurationGroup";
					width: parent.width;

					TextInputElementView {
						id: articleInput;
						objectName: "ArticleInput";
						name: qsTr("Article");
						placeHolderText: qsTr("Enter the article");
						isRequired: true;
						showErrorWhenInvalid: true;
						deferErrorUntilEdited: true;
						errorText: qsTr("Please enter the article");
						textInputValidator: notEmptyValidator;

						onEditingFinished: {
							configurationEditorContainer.doUpdateModel();
						}
					}

					TextInputElementView {
						id: nameInput;
						objectName: "ConfigurationNameInput";
						name: qsTr("Name");
						placeHolderText: qsTr("Enter the configuration name");
						isRequired: true;
						showErrorWhenInvalid: true;
						deferErrorUntilEdited: true;
						errorText: qsTr("Please enter the configuration name");
						textInputValidator: notEmptyValidator;

						onEditingFinished: {
							configurationEditorContainer.doUpdateModel();
						}
					}

					CollectionSelectElementView {
						id: modelSelect
						objectName: "HardwareModelCombo"
						name: qsTr("Hardware Model")
						commandId: ImtlicProductsSdlCommandIds.s_productsList
						fields: [ProductItemTypeMetaInfo.s_id, ProductItemTypeMetaInfo.s_productName]
						titleField: ProductItemTypeMetaInfo.s_productName
						textFilterFieldIds: [ProductItemTypeMetaInfo.s_productName]
						sortByField: ProductItemTypeMetaInfo.s_productName
						groupFilters: [hardwareCategoryGroup]
						placeHolderText: qsTr("Select a hardware model")
						filterPlaceholder: qsTr("Search by model name")
						isSelectionRequired: true
						errorText: qsTr("Please select a hardware model")

						onItemSelected: {
							configurationEditorContainer.doUpdateModel()
						}
					}

					TextInputElementView {
						id: descriptionInput;
						objectName: "ConfigurationDescriptionInput";
						name: qsTr("Description");
						placeHolderText: qsTr("Enter the description");

						onEditingFinished: {
							configurationEditorContainer.doUpdateModel();
						}
					}
				}
			}
		}
	}

	Component {
		id: componentsPageComp

		Item {
			id: componentsPage
			anchors.fill: parent

			readonly property bool canEdit: !configurationEditorContainer.readOnly
				&& (PermissionsController.checkPermission("/HardwareManagement/Edit/Change") || PermissionsController.checkPermission("/HardwareManagement/Edit/Add"))

			function updateGui(){
				let rows = []
				let componentIds = configurationEditorContainer.configurationData ? configurationEditorContainer.configurationData.m_componentIds : null
				for (let i = 0; componentIds && i < componentIds.length; i++){
					let componentId = String(componentIds[i])
					rows.push({id: componentId, title: configurationEditorContainer.componentTitle(componentId), direct: true, paths: []})
				}

				componentsTable.rows = rows
			}

			// The table is the edited state: its rows become the component list of the configuration.
			function applyRows(){
				let componentIds = []
				let items = componentsTable.directItems()
				for (let i = 0; i < items.length; i++){
					componentIds.push(items[i].id)
				}

				configurationEditorContainer.configurationData.m_componentIds = componentIds
				configurationEditorContainer.doUpdateModel()
			}

			Component.onCompleted: {
				componentsPage.updateGui()
			}

			Column {
				id: componentsHeader
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				x: Math.max(0, (componentsPage.width - width) / 2)
				width: Math.max(0, Math.min(Style.contentWidthMax, componentsPage.width - 2 * Style.marginXL))
				spacing: Style.marginM

				SectionHeaderView {
					width: parent.width
					title: qsTr("Components") + " (" + componentsTable.itemsCount + ")"

					SectionLink {
						objectName: "RemoveComponentLink"
						visible: componentsPage.canEdit
						active: componentsTable.selectedCount > 0
						text: qsTr("Remove") + " (" + componentsTable.selectedCount + ")"
						onClicked: {
							componentsTable.removeSelected()
						}
					}

					SectionLink {
						id: addComponentLink
						objectName: "AddComponentLink"
						visible: componentsPage.canEdit
						text: "+ " + qsTr("Add Component")
						onClicked: {
							componentSelector.items = componentsTable.directItems()
							componentSelector.openSelector(addComponentLink)
						}
					}
				}

				// Picker only: the rows are requested page by page and searched on the server.
				CollectionItemSelectElementView {
					id: componentSelector
					width: parent.width
					commandId: ProlifeHardwareComponentsSdlCommandIds.s_hardwareComponentsList
					fields: ["id", "name", "partNumber"]
					titleField: "name"
					textFilterFieldIds: ["name", "partNumber"]
					sortByField: "name"
					label: qsTr("Components")
					addButtonText: qsTr("Add Component")
					filterPlaceholder: qsTr("Search by name or part number")
					visible: false

					onSelectionChanged: {
						let selectedItems = componentSelector.dataProvider.getSelectedItems()
						let entries = []
						for (let i = 0; i < selectedItems.length; i++){
							let values = selectedItems[i].values ? selectedItems[i].values : {}
							entries.push({id: selectedItems[i].id, partNumber: values["partNumber"], name: values["name"]})
						}
						configurationEditorContainer.addComponentTitles(entries)

						let items = []
						for (let k = 0; k < componentSelector.items.length; k++){
							let componentId = String(componentSelector.items[k].id)
							items.push({id: componentId, name: configurationEditorContainer.componentTitle(componentId)})
						}

						componentsTable.setDirectItems(items)
						componentsPage.applyRows()
					}
				}
			}

			AssignmentsTable {
				id: componentsTable
				anchors.top: componentsHeader.bottom
				anchors.topMargin: Style.marginM
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				x: componentsHeader.x
				width: componentsHeader.width
				editable: componentsPage.canEdit
				showSourceColumn: false
				navigationPath: "HardwareComponents/HardwareComponent/"
				nameTitle: qsTr("Component")
				emptyText: qsTr("The configuration takes no components.")
				filterPlaceholder: qsTr("Filter components...")
				removeTitle: qsTr("Remove the components?")
				removeMessage: qsTr("The selected components will be removed from the configuration.")

				onRemoved: {
					componentsPage.applyRows()
				}
			}
		}
	}
}
