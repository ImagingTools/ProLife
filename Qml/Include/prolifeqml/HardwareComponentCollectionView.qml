import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import prolifeHardwareComponentsSdl 1.0

RemoteCollectionView {
	id: container;

	collectionId: "HardwareComponents";

	permissionPaths: ({
		"HardwareComponentsList": "/HardwareManagement/View"
	})

	commandsDelegateComp: Component {DocumentCollectionViewDelegate {
			collectionView: container;

			documentTypeIds: ["HardwareComponent"]
			documentViewTypeIds: ["HardwareComponentEditor"]
			documentViewsComp: [componentEditorComp]
			documentDataControllersComp: [dataControllerComp]
			documentValidatorsComp: [componentValidatorComp]
		}
	}

	Component.onCompleted: {
		table.setSortingInfo(HardwareComponentItemTypeMetaInfo.s_name, "ASC");
	}

	Component {
		id: componentEditorComp

		HardwareComponentEditor {
			id: componentEditor;
			commandsControllerComp: Component {GqlBasedCommandsController {
					typeId: "HardwareComponent";
				}
			}
		}
	}

	Component {
		id: componentValidatorComp;

		DocumentValidator {
			property HardwareComponentData componentData: documentModel

			function isValid(data){
				if (!componentData){
					return false;
				}

				if (componentData.m_partNumber === ""){
					data.message = qsTr("Part number cannot be empty");
					return false;
				}

				if (componentData.m_name === ""){
					data.message = qsTr("Component name cannot be empty");
					return false;
				}

				return true;
			}
		}
	}

	Component {
		id: dataControllerComp;

		GqlRequestDocumentDataController {
			id: requestDocumentDataController

			property HardwareComponentData componentData: documentModel;

			gqlGetCommandId: ProlifeHardwareComponentsSdlCommandIds.s_getHardwareComponentItem;
			gqlUpdateCommandId: ProlifeHardwareComponentsSdlCommandIds.s_updateHardwareComponent;
			gqlAddCommandId: ProlifeHardwareComponentsSdlCommandIds.s_addHardwareComponent;

			getPermissionPath: "/HardwareManagement/View";
			addPermissionPath: "/HardwareManagement/Edit/Add";
			updatePermissionPath: "/HardwareManagement/Edit/Change";

			typeId: "HardwareComponent";
			documentName: componentData ? componentData.m_name : "";
			documentDescription: componentData ? componentData.m_description : "";

			documentModelComp: Component {
				HardwareComponentData {
					m_componentType: "Part"
					m_lifecycleStatus: "Active"
				}
			}
		}
	}
}
