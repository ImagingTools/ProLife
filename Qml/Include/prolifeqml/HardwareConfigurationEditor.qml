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

	// Hardware models only: the configuration belongs to one of them.
	property TreeItemModel hardwareModels: CachedProductCollection.hardwareProductsModel

	// "part number - name" of the components, from the document and from the picker.
	property var componentTitles: ({})

	onConfigurationDataChanged: {
		configurationEditorContainer.loadComponentTitles()
	}

	Component.onCompleted: {
		configurationEditorContainer.loadComponentTitles()
	}

	function findModelIndex(productId){
		if (!configurationEditorContainer.hardwareModels){
			return -1
		}

		for (let i = 0; i < configurationEditorContainer.hardwareModels.getItemsCount(); i++){
			if (configurationEditorContainer.hardwareModels.getData("id", i) === productId){
				return i
			}
		}

		return -1
	}

	function modelId(index){
		if (!configurationEditorContainer.hardwareModels || index < 0 || index >= configurationEditorContainer.hardwareModels.getItemsCount()){
			return ""
		}

		return configurationEditorContainer.hardwareModels.getData("id", index)
	}

	function setComponentTitle(componentId, partNumber, name){
		let titles = {}
		for (let key in configurationEditorContainer.componentTitles){
			titles[key] = configurationEditorContainer.componentTitles[key]
		}

		let title = name ? String(name) : String(componentId)
		if (partNumber && String(partNumber) !== ""){
			title = String(partNumber) + " \u2014 " + title
		}
		titles[String(componentId)] = title

		configurationEditorContainer.componentTitles = titles
	}

	function componentTitle(componentId){
		let title = configurationEditorContainer.componentTitles[String(componentId)]

		return title ? title : String(componentId)
	}

	// The server sends the names of the components along with their IDs.
	function loadComponentTitles(){
		let components = configurationEditorContainer.configurationData ? configurationEditorContainer.configurationData.m_components : null
		for (let i = 0; components && i < components.count; i++){
			let component = components.get(i).item
			configurationEditorContainer.setComponentTitle(component.m_id, component.m_partNumber, component.m_name)
		}
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
				modelCB.currentIndex = configurationEditorContainer.findModelIndex(configurationEditorContainer.configurationData.m_productId)
			}

			function updateModel(){
				if (!configurationEditorContainer.configurationData){
					return
				}

				configurationEditorContainer.configurationData.m_configurationId = articleInput.text
				configurationEditorContainer.configurationData.m_name = nameInput.text
				configurationEditorContainer.configurationData.m_description = descriptionInput.text
				configurationEditorContainer.configurationData.m_productId = configurationEditorContainer.modelId(modelCB.currentIndex)
			}

			function applyReadOnly(readOnly){
				let canChange = PermissionsController.checkPermission("/HardwareManagement/Edit/Change") || PermissionsController.checkPermission("/HardwareManagement/Edit/Add")
				articleInput.readOnly = readOnly || !canChange
				nameInput.readOnly = readOnly || !canChange
				descriptionInput.readOnly = readOnly || !canChange
				modelCB.changeable = !readOnly && canChange
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

				GroupElementView {
					id: configurationGroup;
					objectName: "ConfigurationGroup";
					width: parent.width;

					TextInputElementView {
						id: articleInput;
						objectName: "ArticleInput";
						name: qsTr("Article");
						placeHolderText: qsTr("Enter the article");

						onEditingFinished: {
							configurationEditorContainer.doUpdateModel();
						}
					}

					TextInputElementView {
						id: nameInput;
						objectName: "ConfigurationNameInput";
						name: qsTr("Name");
						placeHolderText: qsTr("Enter the configuration name");

						onEditingFinished: {
							configurationEditorContainer.doUpdateModel();
						}
					}

					ComboBoxElementView {
						id: modelCB;
						objectName: "HardwareModelCombo";
						name: qsTr("Hardware Model");
						model: configurationEditorContainer.hardwareModels;
						nameId: "productName";

						onCurrentIndexChanged: {
							if (configurationEditorContainer.configurationData && configurationEditorContainer.modelId(modelCB.currentIndex) !== configurationEditorContainer.configurationData.m_productId){
								configurationEditorContainer.doUpdateModel();
							}
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
						for (let i = 0; i < selectedItems.length; i++){
							let values = selectedItems[i].values ? selectedItems[i].values : {}
							configurationEditorContainer.setComponentTitle(selectedItems[i].id, values["partNumber"], values["name"])
						}

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
