import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtbaseImtCollectionSdl 1.0
import imtlicProductsSdl 1.0
import prolifeHardwareConfigurationsSdl 1.0

/**
 * HardwareModelEditor - a hardware model (a product of the "Hardware" category).
 *
 * A model has no software features: what can be ordered and built is described by its
 * configurations (articles), and a configuration lists the components it takes.
 *
 * Pages: Model (name, ID, description) and Configurations (the articles of the model, picked
 * from the configuration catalog that is searched on the server).
 */
ViewBase {
	id: modelEditorContainer;

	anchors.fill: parent;
	contentColor: Style.baseColor

	property ProductData productData: model

	// ID of the model in the product collection; empty while a new model is not saved yet.
	property string modelId: productData ? productData.m_id : ""

	onModelIdChanged: {
		modelEditorContainer.reloadConfigurations()
	}

	// Configurations still to be moved: [{id, productId}], sent one by one.
	property var pendingAssignments: []

	CollectionDataProvider {
		id: configurationsProvider;

		commandId: ProlifeHardwareConfigurationsSdlCommandIds.s_hardwareConfigurationsList;
		subscriptionCommandId: "OnHardwareConfigurationsCollectionChanged";
		fields: ["id", "name", "configurationId", "productId"];
		sortByField: "configurationId";

		// The server returns the configurations of this model only.
		function getHeaders(){
			let headers = {}
			headers["hardwareModelId"] = modelEditorContainer.modelId

			return headers
		}

		onCollectionModelChanged: {
			let configurationsPage = multiPageView.getPageById("Configurations")
			if (configurationsPage){
				configurationsPage.updateGui()
			}
		}
	}

	function reloadConfigurations(){
		if (modelEditorContainer.modelId !== ""){
			configurationsProvider.updateModel()
		}
	}

	function configurationTitle(article, name){
		if (article && String(article) !== ""){
			return String(article) + " \u2014 " + String(name)
		}

		return String(name)
	}

	// Rows [{id, title}] of the configurations the server reports for this model.
	function assignedConfigurations(){
		let items = []
		let model = configurationsProvider.collectionModel
		for (let i = 0; model && i < model.getItemsCount(); i++){
			let configurationId = String(model.getData("id", i))
			items.push({id: configurationId, title: modelEditorContainer.configurationTitle(model.getData("configurationId", i), model.getData("name", i))})
		}

		return items
	}

	/**
		Moves the configurations to this model or takes them out of it, so that exactly
		the given IDs belong to the model. The change is saved at once, it is not part of
		the model document.
	*/
	function assignConfigurations(configurationIds){
		let current = modelEditorContainer.assignedConfigurations()
		let currentIds = []
		let pending = modelEditorContainer.pendingAssignments.slice()
		for (let i = 0; i < current.length; i++){
			currentIds.push(current[i].id)
			if (configurationIds.indexOf(current[i].id) < 0){
				pending.push({id: current[i].id, productId: ""})
			}
		}

		for (let k = 0; k < configurationIds.length; k++){
			if (currentIds.indexOf(configurationIds[k]) < 0){
				pending.push({id: configurationIds[k], productId: modelEditorContainer.modelId})
			}
		}

		let wasIdle = modelEditorContainer.pendingAssignments.length === 0
		modelEditorContainer.pendingAssignments = pending
		if (wasIdle){
			modelEditorContainer.sendNextAssignment()
		}
	}

	function sendNextAssignment(){
		if (modelEditorContainer.pendingAssignments.length === 0){
			modelEditorContainer.reloadConfigurations()
			return
		}

		let input = configurationItemInputComp.createObject(modelEditorContainer)
		input.m_id = modelEditorContainer.pendingAssignments[0].id

		getConfigurationRequest.send(input)
		input.destroy()
	}

	function finishAssignment(succeeded){
		let pending = modelEditorContainer.pendingAssignments.slice()
		pending.shift()
		if (!succeeded){
			// The rest is dropped, the list shows what the server has.
			pending = []
		}
		modelEditorContainer.pendingAssignments = pending
		modelEditorContainer.sendNextAssignment()
	}

	Component {
		id: configurationItemInputComp

		HardwareConfigurationItemInput {}
	}

	Component {
		id: configurationDataInputComp

		HardwareConfigurationDataInput {}
	}

	// The update request carries the whole configuration, so it is read first and sent back
	// with the other model.
	GqlSdlRequestSender {
		id: getConfigurationRequest

		requestType: 0
		gqlCommandId: ProlifeHardwareConfigurationsSdlCommandIds.s_getHardwareConfigurationItem

		sdlObjectComp: Component {
			HardwareConfigurationData {}
		}

		onFinished: {
			if (status < 0 || modelEditorContainer.pendingAssignments.length === 0){
				modelEditorContainer.finishAssignment(false)
				return
			}

			let assignment = modelEditorContainer.pendingAssignments[0]
			let configuration = getConfigurationRequest.sdlObject
			configuration.m_productId = assignment.productId

			let input = configurationDataInputComp.createObject(modelEditorContainer)
			input.m_id = assignment.id
			input.m_item = configuration

			updateConfigurationRequest.send(input)
			input.destroy()
		}
	}

	GqlSdlRequestSender {
		id: updateConfigurationRequest

		requestType: 1
		gqlCommandId: ProlifeHardwareConfigurationsSdlCommandIds.s_updateHardwareConfiguration
		permissionPath: "/HardwareManagement/Edit/Change"

		sdlObjectComp: Component {
			UpdatedNotificationPayload {}
		}

		onFinished: {
			modelEditorContainer.finishAssignment(status > 0)
		}
	}

	function updateGui(){
		let modelPage = multiPageView.getPageById("Model")
		if (modelPage){
			modelPage.updateGui()
		}
	}

	function updateModel(){
		if (!modelEditorContainer.productData){
			return
		}

		let modelPage = multiPageView.getPageById("Model")
		if (modelPage){
			modelPage.updateModel()
		}

		modelEditorContainer.productData.m_categoryId = "Hardware"
	}

	function setReadOnly(readOnly){
		let modelPage = multiPageView.getPageById("Model")
		if (modelPage){
			modelPage.applyReadOnly(readOnly)
		}
	}

	function openConfiguration(configurationId){
		MainDocumentService.openDocument("HardwareConfigurations", configurationId, "HardwareConfiguration")
	}

	function addConfiguration(){
		let input = addConfigurationInputComp.createObject(modelEditorContainer)
		input.m_id = ""
		input.m_item.m_productId = modelEditorContainer.modelId
		input.m_item.m_name = qsTr("New configuration of %1").arg(modelEditorContainer.productData.m_productName)

		addConfigurationRequest.send(input)
	}

	Component {
		id: addConfigurationInputComp

		HardwareConfigurationDataInput {
			m_item: HardwareConfigurationData {}
		}
	}

	GqlSdlRequestSender {
		id: addConfigurationRequest

		requestType: 1
		gqlCommandId: ProlifeHardwareConfigurationsSdlCommandIds.s_addHardwareConfiguration
		permissionPath: "/HardwareManagement/Edit/Add"

		sdlObjectComp: Component {
			AddedNotificationPayload {
				onFinished: {
					if (m_id !== ""){
						configurationsProvider.updateModel()
						modelEditorContainer.openConfiguration(m_id)
					}
				}
			}
		}
	}

	MultiPageView {
		id: multiPageView
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS

		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("Model", qsTr("Model"), modelPageComp, "Icons/Settings")
			multiPageView.addPage("Configurations", qsTr("Configurations"), configurationsPageComp, "Icons/FeaturePackage")
			multiPageView.currentIndex = 0
		}

		Component.onCompleted: {
			multiPageView.updatePages()
		}
	}

	Component {
		id: modelPageComp

		Item {
			id: modelPage
			anchors.fill: parent

			function updateGui(){
				if (!modelEditorContainer.productData){
					return
				}

				nameInput.text = modelEditorContainer.productData.m_productName
				modelIdInput.text = modelEditorContainer.productData.m_productId
				descriptionInput.text = modelEditorContainer.productData.m_description
			}

			function updateModel(){
				if (!modelEditorContainer.productData){
					return
				}

				modelEditorContainer.productData.m_productName = nameInput.text
				modelEditorContainer.productData.m_name = nameInput.text
				modelEditorContainer.productData.m_description = descriptionInput.text

				let modelIdText = modelIdInput.text
				if (modelIdText === ""){
					modelIdText = nameInput.text.replace(/\s+/g, '')
				}
				modelEditorContainer.productData.m_productId = modelIdText
			}

			function applyReadOnly(readOnly){
				let canChange = PermissionsController.checkPermission("/ProductManagement/Edit/Change") || PermissionsController.checkPermission("/ProductManagement/Edit/Add")
				nameInput.readOnly = readOnly || !canChange
				modelIdInput.readOnly = readOnly || !canChange
				descriptionInput.readOnly = readOnly || !canChange
			}

			Component.onCompleted: {
				modelPage.updateGui()
				modelPage.applyReadOnly(modelEditorContainer.readOnly)
			}

			Column {
				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.horizontalCenter: parent.horizontalCenter;
				width: Math.min(parent.width - 2 * Style.marginXL, Style.contentWidthMax);
				spacing: Style.marginXL;

				GroupHeaderView {
					width: parent.width;
					title: qsTr("Hardware Model");
					groupView: modelGroup;
				}

				GroupElementView {
					id: modelGroup;
					objectName: "HardwareModelGroup";
					width: parent.width;

					TextInputElementView {
						id: nameInput;
						objectName: "ModelNameInput";
						name: qsTr("Name");
						placeHolderText: qsTr("Enter the model name");

						onEditingFinished: {
							modelEditorContainer.doUpdateModel();
						}
					}

					TextInputElementView {
						id: modelIdInput;
						objectName: "ModelIdInput";
						name: qsTr("Model ID");
						description: qsTr("Taken from the name when left empty");
						placeHolderText: qsTr("Enter the model ID");

						onEditingFinished: {
							modelEditorContainer.doUpdateModel();
						}
					}

					TextInputElementView {
						id: descriptionInput;
						objectName: "ModelDescriptionInput";
						name: qsTr("Description");
						placeHolderText: qsTr("Enter the description");

						onEditingFinished: {
							modelEditorContainer.doUpdateModel();
						}
					}
				}
			}
		}
	}

	Component {
		id: configurationsPageComp

		Item {
			id: configurationsPage
			anchors.fill: parent

			readonly property bool saved: modelEditorContainer.modelId !== ""
			readonly property bool canEdit: configurationsPage.saved && !modelEditorContainer.readOnly
				&& PermissionsController.checkPermission("/HardwareManagement/Edit/Change")

			function updateGui(){
				let rows = []
				let items = modelEditorContainer.assignedConfigurations()
				for (let i = 0; i < items.length; i++){
					rows.push({id: items[i].id, title: items[i].title, direct: true, paths: []})
				}

				configurationsTable.rows = rows
			}

			function tableIds(){
				let ids = []
				let items = configurationsTable.directItems()
				for (let i = 0; i < items.length; i++){
					ids.push(items[i].id)
				}

				return ids
			}

			Component.onCompleted: {
				configurationsPage.updateGui()
			}

			Column {
				id: configurationsHeader
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				x: Math.max(0, (configurationsPage.width - width) / 2)
				width: Math.max(0, Math.min(Style.contentWidthMax, configurationsPage.width - 2 * Style.marginXL))
				spacing: Style.marginM

				SectionHeaderView {
					width: parent.width
					title: qsTr("Configurations") + " (" + configurationsTable.itemsCount + ")"
						+ (modelEditorContainer.pendingAssignments.length > 0 ? "   " + qsTr("Updating...") : "")

					SectionLink {
						objectName: "RemoveConfigurationLink"
						visible: configurationsPage.canEdit
						active: configurationsTable.selectedCount > 0
						text: qsTr("Remove") + " (" + configurationsTable.selectedCount + ")"
						onClicked: {
							configurationsTable.removeSelected()
						}
					}

					SectionLink {
						id: addConfigurationLink
						objectName: "AddConfigurationLink"
						visible: configurationsPage.canEdit
						text: "+ " + qsTr("Add Configuration")
						onClicked: {
							configurationSelector.items = configurationsTable.directItems()
							configurationSelector.openSelector(addConfigurationLink)
						}
					}

					SectionLink {
						objectName: "NewConfigurationLink"
						visible: configurationsPage.canEdit
						active: PermissionsController.checkPermission("/HardwareManagement/Edit/Add")
						text: "+ " + qsTr("New Configuration")
						onClicked: {
							modelEditorContainer.addConfiguration()
						}
					}
				}

				Text {
					width: parent.width
					visible: !configurationsPage.saved
					text: qsTr("Save the model first, then add its configurations.")
					font.pixelSize: Style.fontSizeM
					color: Style.inactiveTextColor
					wrapMode: Text.WordWrap
				}

				// Picker only: all configurations, requested page by page and searched on the server.
				// Picking one of another model moves it to this model.
				CollectionItemSelectElementView {
					id: configurationSelector
					width: parent.width
					commandId: ProlifeHardwareConfigurationsSdlCommandIds.s_hardwareConfigurationsList
					fields: ["id", "name", "configurationId"]
					titleField: "name"
					textFilterFieldIds: ["name", "configurationId"]
					sortByField: "name"
					label: qsTr("Configurations")
					addButtonText: qsTr("Add Configuration")
					filterPlaceholder: qsTr("Search by name or article")
					visible: false

					onSelectionChanged: {
						let ids = []
						for (let i = 0; i < configurationSelector.items.length; i++){
							ids.push(String(configurationSelector.items[i].id))
						}

						modelEditorContainer.assignConfigurations(ids)
					}
				}
			}

			AssignmentsTable {
				id: configurationsTable
				anchors.top: configurationsHeader.bottom
				anchors.topMargin: Style.marginM
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				x: configurationsHeader.x
				width: configurationsHeader.width
				editable: configurationsPage.canEdit
				showSourceColumn: false
				navigationPath: "HardwareConfigurations/HardwareConfiguration/"
				nameTitle: qsTr("Configuration")
				emptyText: qsTr("The model has no configurations.")
				filterPlaceholder: qsTr("Filter configurations...")
				removeTitle: qsTr("Remove from the model?")
				removeMessage: qsTr("The selected configurations stay in the catalog without a hardware model.")

				onRemoved: {
					modelEditorContainer.assignConfigurations(configurationsPage.tableIds())
				}
			}
		}
	}
}
