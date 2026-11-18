import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtbaseCollectionDocumentServiceSdl 1.0
import prolifeHardwareConfigurationsSdl 1.0
import prolifeHardwareConfigurationCollectionDocumentServiceSdl 1.0

RemoteCollectionView {
	id: container;

	collectionId: "HardwareConfigurations";

	permissionPaths: ({
		"HardwareConfigurationsList": "/HardwareManagement/View"
	})

	commandsDelegateComp: Component {DocCollectionViewDelegate {
			collectionView: container;

			Component.onCompleted: {
				registerDocumentType("HardwareConfiguration", qsTr("Hardware Configuration"))
				addDocumentView("HardwareConfiguration", "HardwareConfigurationEditor", configurationEditorComp, dataControllerComp)
			}
		}
	}

	documentCollectionFilter: null

	Component.onCompleted: {
		table.setSortingInfo(HardwareConfigurationItemTypeMetaInfo.s_name, "ASC");
		registerFieldFilterDelegate("ProductId", hardwareModelsDelegateFilterComp)
	}

	Component {
		id: hardwareModelsDelegateFilterComp

		ProductFilterDelegate {
			objectName: "HardwareModelsFilter"
			name: qsTr("Hardware Models")
			categoryId: "Hardware"
			filterPlaceholder: qsTr("Search by model name")
			defaultFieldFilter.m_fieldId: "ProductId"
		}
	}

	Component {
		id: configurationEditorComp

		HardwareConfigurationEditor {
			commandsControllerComp: Component {GqlBasedCommandsController {
					typeId: "HardwareConfiguration";
				}
			}
		}
	}

	Component {
		id: dataControllerComp

		DocumentRepresentationController {
			id: dataController

			representationModel: HardwareConfigurationData {}

			function updateRepresentationFromDocument(){
				startUpdateRepresentation(documentId, representationModel)

				documentIdInput.m_id = documentId
				documentIdInput.m_collectionId = container.collectionId
				getRepresentationRequest.send(documentIdInput)
			}

			function updateDocumentFromRepresentation(){
				startUpdateDocument(documentId)

				updateInput.m_documentId = documentId
				updateInput.m_configurationData = representationModel

				return updateRequest.send(updateInput)
			}

			property DocumentId documentIdInput: DocumentId {}
			property GqlSdlRequestSender getRepresentationRequest: GqlSdlRequestSender {
				gqlCommandId: ProlifeHardwareConfigurationCollectionDocumentServiceSdlCommandIds.s_getHardwareConfigurationRepresentation
				permissionPath: "/HardwareManagement/View"
				sdlObjectComp: Component {
					HardwareConfigurationData {
						onFinished: {
							dataController.representationModel.copyFrom(this)
							dataController.representationUpdated(dataController.documentId, dataController.representationModel)
						}
					}
				}

				function onError(message, type){
					dataController.updateRepresentationFailed(dataController.documentId, message)
				}
			}

			property UpdateHardwareConfigurationInput updateInput: UpdateHardwareConfigurationInput {}
			property GqlSdlRequestSender updateRequest: GqlSdlRequestSender {
				gqlCommandId: ProlifeHardwareConfigurationCollectionDocumentServiceSdlCommandIds.s_updateHardwareConfigurationFromRepresentation
				requestType: 1
				sdlObjectComp: Component {
					DocumentOperationStatus {
						onFinished: {
							if (m_status === "Success"){
								dataController.documentUpdated(dataController.documentId)
							}
							else{
								if (m_message){
									PopupManager.addErrorMessage(m_message)
								}
								dataController.updateDocumentFailed(dataController.documentId, "")
								dataController.updateRepresentationFromDocument()
							}
						}
					}
				}

				function onError(message, type){
					dataController.updateDocumentFailed(dataController.documentId, message)
					dataController.updateRepresentationFromDocument()
				}
			}
		}
	}
}
