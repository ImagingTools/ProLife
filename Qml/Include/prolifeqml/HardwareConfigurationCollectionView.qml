import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import prolifeHardwareConfigurationsSdl 1.0

RemoteCollectionView {
	id: container;

	collectionId: "HardwareConfigurations";

	permissionPaths: ({
		"HardwareConfigurationsList": "/HardwareManagement/View"
	})

	commandsDelegateComp: Component {DocumentCollectionViewDelegate {
			collectionView: container;

			documentTypeIds: ["HardwareConfiguration"]
			documentViewTypeIds: ["HardwareConfigurationEditor"]
			documentViewsComp: [configurationEditorComp]
			documentDataControllersComp: [dataControllerComp]
			documentValidatorsComp: [configurationValidatorComp]
		}
	}

	Component.onCompleted: {
		table.setSortingInfo(HardwareConfigurationItemTypeMetaInfo.s_name, "ASC");
	}

	Component {
		id: configurationEditorComp

		HardwareConfigurationEditor {
			id: configurationEditor;
			commandsControllerComp: Component {GqlBasedCommandsController {
					typeId: "HardwareConfiguration";
				}
			}
		}
	}

	Component {
		id: configurationValidatorComp;

		DocumentValidator {
			property HardwareConfigurationData configurationData: documentModel

			function isValid(data){
				if (!configurationData){
					return false;
				}

				if (configurationData.m_configurationId === ""){
					data.message = qsTr("Article cannot be empty");
					return false;
				}

				if (configurationData.m_name === ""){
					data.message = qsTr("Configuration name cannot be empty");
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

			property HardwareConfigurationData configurationData: documentModel;

			gqlGetCommandId: ProlifeHardwareConfigurationsSdlCommandIds.s_getHardwareConfigurationItem;
			gqlUpdateCommandId: ProlifeHardwareConfigurationsSdlCommandIds.s_updateHardwareConfiguration;
			gqlAddCommandId: ProlifeHardwareConfigurationsSdlCommandIds.s_addHardwareConfiguration;

			getPermissionPath: "/HardwareManagement/View";
			addPermissionPath: "/HardwareManagement/Edit/Add";
			updatePermissionPath: "/HardwareManagement/Edit/Change";

			typeId: "HardwareConfiguration";
			documentName: configurationData ? configurationData.m_name : "";
			documentDescription: configurationData ? configurationData.m_description : "";

			documentModelComp: Component {
				HardwareConfigurationData {}
			}
		}
	}
}
