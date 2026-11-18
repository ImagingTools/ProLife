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
import prolifeHardwareComponentsSdl 1.0
import prolifeHardwareComponentCollectionDocumentServiceSdl 1.0

RemoteCollectionView {
	id: container;

	collectionId: "HardwareComponents";

	permissionPaths: ({
		"HardwareComponentsList": "/HardwareManagement/View"
	})

	commandsDelegateComp: Component {DocCollectionViewDelegate {
			collectionView: container;

			Component.onCompleted: {
				registerDocumentType("HardwareComponent", qsTr("Hardware Component"))
				addDocumentView("HardwareComponent", "HardwareComponentEditor", componentEditorComp, dataControllerComp)
			}
		}
	}

	documentCollectionFilter: null

	Component.onCompleted: {
		table.setSortingInfo(HardwareComponentItemTypeMetaInfo.s_name, "ASC");
		registerFieldFilterDelegate("ManufacturerId", manufacturersDelegateFilterComp)
	}

	Component {
		id: manufacturersDelegateFilterComp

		ManufacturerFilterDelegate {
			objectName: "ManufacturersFilter"
		}
	}

	Component {
		id: componentEditorComp

		HardwareComponentEditor {
			commandsControllerComp: Component {GqlBasedCommandsController {
					typeId: "HardwareComponent";
				}
			}
		}
	}

	Component {
		id: dataControllerComp

		DocumentRepresentationController {
			id: dataController

			representationModel: HardwareComponentData {}
			// A new component gets its type and lifecycle defaults on the server.
			requestRepresentationOnCreate: true

			function updateRepresentationFromDocument(){
				startUpdateRepresentation(documentId, representationModel)

				documentIdInput.m_id = documentId
				documentIdInput.m_collectionId = container.collectionId
				getRepresentationRequest.send(documentIdInput)
			}

			function updateDocumentFromRepresentation(){
				startUpdateDocument(documentId)

				updateInput.m_documentId = documentId
				updateInput.m_componentData = representationModel

				return updateRequest.send(updateInput)
			}

			property DocumentId documentIdInput: DocumentId {}
			property GqlSdlRequestSender getRepresentationRequest: GqlSdlRequestSender {
				gqlCommandId: ProlifeHardwareComponentCollectionDocumentServiceSdlCommandIds.s_getHardwareComponentRepresentation
				permissionPath: "/HardwareManagement/View"
				sdlObjectComp: Component {
					HardwareComponentData {
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

			property UpdateHardwareComponentInput updateInput: UpdateHardwareComponentInput {}
			property GqlSdlRequestSender updateRequest: GqlSdlRequestSender {
				gqlCommandId: ProlifeHardwareComponentCollectionDocumentServiceSdlCommandIds.s_updateHardwareComponentFromRepresentation
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
