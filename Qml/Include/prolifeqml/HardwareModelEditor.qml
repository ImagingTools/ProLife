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
import prolifeHardwareConfigurationCollectionDocumentServiceSdl 1.0

/**
 * HardwareModelEditor - a hardware model (a product of the "Hardware" category).
 *
 * A model has no software features: what can be ordered and built is described by its
 * configurations (articles), and a configuration lists the components it takes.
 *
 * Pages: Model (name, ID, description) and Configurations (the articles of the model; a
 * configuration chooses its model itself, here they are listed and new ones are created).
 */
ViewBase {
	id: modelEditorContainer;

	anchors.fill: parent;
	contentColor: Style.baseColor

	property ProductData productData: model

	// ID of the model in the product collection; empty while a new model is not saved yet.
	property string modelId: productData && productData.m_id !== "" ? productData.m_id : savedModelId

	// ID the model got on its first save: the representation read before the save has none.
	property string savedModelId: ""

	onModelIdChanged: {
		modelEditorContainer.reloadConfigurations()
	}

	// Called by the document when it is saved.
	function documentSaved(){
		let documentManager = MainDocumentService.getDocumentService("HardwareModels")
		if (documentManager){
			modelEditorContainer.savedModelId = documentManager.getDocumentObjectId(documentManager.getDocumentIdByView(modelEditorContainer))
		}
	}


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
	// The model ID is the name without white space; it is never typed in.
	function modelIdFromName(name){
		return name ? name.replace(/\s+/g, '') : ""
	}

	function assignedConfigurations(){
		let items = []
		let model = configurationsProvider.collectionModel
		for (let i = 0; model && i < model.getItemsCount(); i++){
			let configurationId = String(model.getData("id", i))
			items.push({id: configurationId, title: modelEditorContainer.configurationTitle(model.getData("configurationId", i), model.getData("name", i))})
		}

		return items
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

	// The new configuration opens in its editor with this model set; it gets its article there and is saved only when complete.
	function addConfiguration(){
		let modelId = modelEditorContainer.modelId
		MainDocumentService.insertNewDocument("HardwareConfigurations", "HardwareConfiguration", function(documentId){
			modelEditorContainer.setConfigurationModel(documentId, modelId)
		})
	}

	// Only the model is sent: the server changes the fields the request carries.
	function setConfigurationModel(documentId, modelId){
		setModelData.m_productId = modelId
		setModelInput.m_documentId = documentId
		setModelInput.m_configurationData = setModelData
		setModelRequest.send(setModelInput)
	}

	HardwareConfigurationData {
		id: setModelData
	}

	UpdateHardwareConfigurationInput {
		id: setModelInput
	}

	GqlSdlRequestSender {
		id: setModelRequest

		requestType: 1
		gqlCommandId: ProlifeHardwareConfigurationCollectionDocumentServiceSdlCommandIds.s_updateHardwareConfigurationFromRepresentation
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

				modelEditorContainer.productData.m_productId = modelEditorContainer.modelIdFromName(nameInput.text)
			}

			function applyReadOnly(readOnly){
				let canChange = PermissionsController.checkPermission("/ProductManagement/Edit/Change") || PermissionsController.checkPermission("/ProductManagement/Edit/Add")
				nameInput.readOnly = readOnly || !canChange
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
						isRequired: true;
						showErrorWhenInvalid: true;
						deferErrorUntilEdited: true;
						errorText: qsTr("Please enter the model name");
						textInputValidator: modelNameValidator;

						onTextEdited: {
							modelIdInput.text = modelEditorContainer.modelIdFromName(nameInput.text);
						}

						onEditingFinished: {
							modelEditorContainer.doUpdateModel();
						}
					}

					TextInputElementView {
						id: modelIdInput;
						objectName: "ModelIdInput";
						name: qsTr("Model ID");
						description: qsTr("Generated from the model name");
						readOnly: true;
					}

					RegularExpressionValidator {
						id: modelNameValidator;
						regularExpression: /^(?!\s*$).+/;
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
			}

			AssignmentsTable {
				id: configurationsTable
				anchors.top: configurationsHeader.bottom
				anchors.topMargin: Style.marginM
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginXL
				x: configurationsHeader.x
				width: configurationsHeader.width
				editable: false
				showSourceColumn: false
				navigationPath: "HardwareConfigurations/HardwareConfiguration/"
				nameTitle: qsTr("Configuration")
				emptyText: qsTr("The model has no configurations.")
				filterPlaceholder: qsTr("Filter configurations...")
			}
		}
	}
}
