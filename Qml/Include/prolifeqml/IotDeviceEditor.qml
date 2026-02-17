import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import prolifeqml 1.0
import prolifeSensorsSdl 1.0

ViewBase {
	id: iotDeviceEditorContainer;

	property IotDeviceData iotDeviceData: model;
	property bool isNew: false

	onIotDeviceDataChanged: {
		if (!iotDeviceData){
			return;
		}

		checkPermissions();
	}

	function checkPermissions(){
		if (!iotDeviceData){
			return;
		}

		let canAddSensor = PermissionsController.checkPermission("AddSensor");
		if (isNew && canAddSensor){
			nameInput.readOnly = false;
			descriptionInput.readOnly = false;
			factoryNumberInput.readOnly = false;
			modemNumberInput.readOnly = false;
			manufacturerInput.readOnly = false;
			brandModelInput.readOnly = false;
			installationLocationInput.readOnly = false;
			connectionTypeInput.readOnly = false;
			resourceTypeInput.readOnly = false;
			holeDiameterInput.readOnly = false;
			deviceCategoryInput.readOnly = false;
			calibrationDateInput.readOnly = false;
			commissionDateInput.readOnly = false;
		}
		else{
			let canChangeDescription = PermissionsController.checkPermission("ChangeDescriptionForSensor");
			descriptionInput.readOnly = !canChangeDescription;
			nameInput.readOnly = !canChangeDescription;

			// For other fields, use generic sensor permission for now
			let canChangeSensor = PermissionsController.checkPermission("AddSensor");
			factoryNumberInput.readOnly = !canChangeSensor;
			modemNumberInput.readOnly = !canChangeSensor;
			manufacturerInput.readOnly = !canChangeSensor;
			brandModelInput.readOnly = !canChangeSensor;
			installationLocationInput.readOnly = !canChangeSensor;
			connectionTypeInput.readOnly = !canChangeSensor;
			resourceTypeInput.readOnly = !canChangeSensor;
			holeDiameterInput.readOnly = !canChangeSensor;
			deviceCategoryInput.readOnly = !canChangeSensor;
			calibrationDateInput.readOnly = !canChangeSensor;
			commissionDateInput.readOnly = !canChangeSensor;
		}
	}

	function updateGui(){
		if (!iotDeviceData){
			return;
		}

		nameInput.text= iotDeviceData.m_name ? iotDeviceData.m_name : "";
		descriptionInput.text= iotDeviceData.m_description ? iotDeviceData.m_description : "";
		factoryNumberInput.text= iotDeviceData.m_factoryNumber ? iotDeviceData.m_factoryNumber : "";
		modemNumberInput.text= iotDeviceData.m_modemNumber ? iotDeviceData.m_modemNumber : "";
		manufacturerInput.text= iotDeviceData.m_manufacturer ? iotDeviceData.m_manufacturer : "";
		brandModelInput.text= iotDeviceData.m_brandModel ? iotDeviceData.m_brandModel : "";
		installationLocationInput.text= iotDeviceData.m_installationLocation ? iotDeviceData.m_installationLocation : "";
		connectionTypeInput.text= iotDeviceData.m_connectionType ? iotDeviceData.m_connectionType : "";
		resourceTypeInput.text= iotDeviceData.m_resourceType ? iotDeviceData.m_resourceType : "";
		holeDiameterInput.text= iotDeviceData.m_holeDiameter ? iotDeviceData.m_holeDiameter : "";
		deviceCategoryInput.text= iotDeviceData.m_deviceCategory ? iotDeviceData.m_deviceCategory : "";
		calibrationDateInput.text= iotDeviceData.m_calibrationDate ? iotDeviceData.m_calibrationDate : "";
		commissionDateInput.text= iotDeviceData.m_commissionDate ? iotDeviceData.m_commissionDate : "";
	}

	function updateModel(){
		if (!iotDeviceData){
			return;
		}

		iotDeviceData.m_name = nameInput.text;
		iotDeviceData.m_description = descriptionInput.text;
		iotDeviceData.m_factoryNumber = factoryNumberInput.text;
		iotDeviceData.m_modemNumber = modemNumberInput.text;
		iotDeviceData.m_manufacturer = manufacturerInput.text;
		iotDeviceData.m_brandModel = brandModelInput.text;
		iotDeviceData.m_installationLocation = installationLocationInput.text;
		iotDeviceData.m_connectionType = connectionTypeInput.text;
		iotDeviceData.m_resourceType = resourceTypeInput.text;
		iotDeviceData.m_holeDiameter = holeDiameterInput.text;
		iotDeviceData.m_deviceCategory = deviceCategoryInput.text;
		iotDeviceData.m_calibrationDate = calibrationDateInput.text;
		iotDeviceData.m_commissionDate = commissionDateInput.text;
	}

	Flickable {
		id: flickable;

		anchors.left: parent.left;
		anchors.leftMargin: Style.marginXL;

		anchors.top: parent.top;
		anchors.topMargin: Style.marginXL;

		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.marginXL;

		anchors.right: parent.right;
		anchors.rightMargin: Style.marginXL;

		contentWidth: bodyColumn.width;
		contentHeight: bodyColumn.height + 2 * Style.marginXL;

		boundsBehavior: Flickable.StopAtBounds;

		clip: true;

		Column {
			id: bodyColumn;

			width: 700;

			spacing: Style.marginXL;

			GroupHeaderView {
				width: parent.width;

				title: qsTr("Basic Information");
				groupView: basicInformationGroup;
			}

			GroupElementView {
				id: basicInformationGroup;

				width: parent.width;

				TextInputElementView {
					id: nameInput;

					name: qsTr("Name");
					placeHolderText: qsTr("Enter name");

					onEditingFinished: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}

				TextInputElementView {
					id: descriptionInput;

					name: qsTr("Description");
					placeHolderText: qsTr("Enter description");

					onEditingFinished: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}

				TextInputElementView {
					id: factoryNumberInput;

					name: qsTr("Factory Number");
					placeHolderText: qsTr("Enter factory number");

					onEditingFinished: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}

				TextInputElementView {
					id: modemNumberInput;

					name: qsTr("Modem Number");
					placeHolderText: qsTr("Enter modem number");

					onEditingFinished: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}
			}

			GroupHeaderView {
				width: parent.width;

				title: qsTr("Device Details");
				groupView: deviceDetailsGroup;
			}

			GroupElementView {
				id: deviceDetailsGroup;

				width: parent.width;

				TextInputElementView {
					id: manufacturerInput;

					name: qsTr("Manufacturer");
					placeHolderText: qsTr("Enter manufacturer");

					onEditingFinished: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}

				TextInputElementView {
					id: brandModelInput;

					name: qsTr("Brand/Model");
					placeHolderText: qsTr("Enter brand/model");

					onEditingFinished: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}

				TextInputElementView {
					id: installationLocationInput;

					name: qsTr("Installation Location");
					placeHolderText: qsTr("Enter installation location");

					onEditingFinished: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}

				TextInputElementView {
					id: connectionTypeInput;

					name: qsTr("Connection Type");
					placeHolderText: qsTr("Enter connection type");

					onEditingFinished: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}

				TextInputElementView {
					id: resourceTypeInput;

					name: qsTr("Resource Type");
					placeHolderText: qsTr("Enter resource type (ГВС, ХВС, Тепло, газ, э/э)");

					onEditingFinished: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}

				TextInputElementView {
					id: holeDiameterInput;

					name: qsTr("Hole Diameter");
					placeHolderText: qsTr("Enter hole diameter");

					onEditingFinished: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}

				TextInputElementView {
					id: deviceCategoryInput;

					name: qsTr("Device Category (ОДПУ/ИПУ)");
					placeHolderText: qsTr("Enter device category");

					onEditingFinished: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}
			}

			GroupHeaderView {
				width: parent.width;

				title: qsTr("Dates");
				groupView: datesGroup;
			}

			GroupElementView {
				id: datesGroup;

				width: parent.width;

				TextInputElementView {
					id: calibrationDateInput;

					name: qsTr("Calibration Date");
					placeHolderText: qsTr("Enter calibration date");

					onEditingFinished: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}

				TextInputElementView {
					id: commissionDateInput;

					name: qsTr("Commission Date");
					placeHolderText: qsTr("Enter commission date");

					onEditingFinished: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}
			}
		}
	}
}
