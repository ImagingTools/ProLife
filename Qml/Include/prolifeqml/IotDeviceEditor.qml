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
	
	anchors.fill: parent;
	
	property int radius: 3;
	property int spacing: Style.marginM;
	
	property IotDeviceData iotDeviceData: model ? model : null;
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
	
	function doUpdateModel(){
		if (!iotDeviceData){
			return;
		}
		
		iotDeviceData.m_name = nameInput.value;
		iotDeviceData.m_description = descriptionInput.value;
		iotDeviceData.m_factoryNumber = factoryNumberInput.value;
		iotDeviceData.m_modemNumber = modemNumberInput.value;
		iotDeviceData.m_manufacturer = manufacturerInput.value;
		iotDeviceData.m_brandModel = brandModelInput.value;
		iotDeviceData.m_installationLocation = installationLocationInput.value;
		iotDeviceData.m_connectionType = connectionTypeInput.value;
		iotDeviceData.m_resourceType = resourceTypeInput.value;
		iotDeviceData.m_holeDiameter = holeDiameterInput.value;
		iotDeviceData.m_deviceCategory = deviceCategoryInput.value;
		iotDeviceData.m_calibrationDate = calibrationDateInput.value;
		iotDeviceData.m_commissionDate = commissionDateInput.value;
	}
	
	function doUpdateGui(){
		if (!iotDeviceData){
			return;
		}
		
		nameInput.value = iotDeviceData.m_name ? iotDeviceData.m_name : "";
		descriptionInput.value = iotDeviceData.m_description ? iotDeviceData.m_description : "";
		factoryNumberInput.value = iotDeviceData.m_factoryNumber ? iotDeviceData.m_factoryNumber : "";
		modemNumberInput.value = iotDeviceData.m_modemNumber ? iotDeviceData.m_modemNumber : "";
		manufacturerInput.value = iotDeviceData.m_manufacturer ? iotDeviceData.m_manufacturer : "";
		brandModelInput.value = iotDeviceData.m_brandModel ? iotDeviceData.m_brandModel : "";
		installationLocationInput.value = iotDeviceData.m_installationLocation ? iotDeviceData.m_installationLocation : "";
		connectionTypeInput.value = iotDeviceData.m_connectionType ? iotDeviceData.m_connectionType : "";
		resourceTypeInput.value = iotDeviceData.m_resourceType ? iotDeviceData.m_resourceType : "";
		holeDiameterInput.value = iotDeviceData.m_holeDiameter ? iotDeviceData.m_holeDiameter : "";
		deviceCategoryInput.value = iotDeviceData.m_deviceCategory ? iotDeviceData.m_deviceCategory : "";
		calibrationDateInput.value = iotDeviceData.m_calibrationDate ? iotDeviceData.m_calibrationDate : "";
		commissionDateInput.value = iotDeviceData.m_commissionDate ? iotDeviceData.m_commissionDate : "";
	}
	
	function updateGui(){
		doUpdateGui();
	}
	
	function updateModel(){
		doUpdateModel();
	}
	
	ScrollView {
		id: scrollView;
		
		anchors.fill: parent;
		anchors.margins: Style.marginM;
		
		Column {
			id: column;
			
			anchors.left: parent.left;
			anchors.right: parent.right;
			
			spacing: iotDeviceEditorContainer.spacing;
			
			GroupElementView {
				id: basicInformationGroup;
				
				anchors.left: parent.left;
				anchors.right: parent.right;
				
				title: qsTr("Basic Information");
				radius: iotDeviceEditorContainer.radius;
				
				Column {
					id: basicInfoColumn;
					
					anchors.left: parent.left;
					anchors.right: parent.right;
					anchors.leftMargin: Style.marginM;
					anchors.rightMargin: Style.marginM;
					
					spacing: Style.marginS;
					
					TextInputView {
						id: nameInput;
						
						anchors.left: parent.left;
						anchors.right: parent.right;
						
						label: qsTr("Name");
						value: "";
						onValueChanged: {
							if (iotDeviceData){
								iotDeviceData.m_name = value;
							}
						}
					}
					
					TextInputView {
						id: descriptionInput;
						
						anchors.left: parent.left;
						anchors.right: parent.right;
						
						label: qsTr("Description");
						value: "";
						onValueChanged: {
							if (iotDeviceData){
								iotDeviceData.m_description = value;
							}
						}
					}
					
					TextInputView {
						id: factoryNumberInput;
						
						anchors.left: parent.left;
						anchors.right: parent.right;
						
						label: qsTr("Factory Number");
						value: "";
						onValueChanged: {
							if (iotDeviceData){
								iotDeviceData.m_factoryNumber = value;
							}
						}
					}
					
					TextInputView {
						id: modemNumberInput;
						
						anchors.left: parent.left;
						anchors.right: parent.right;
						
						label: qsTr("Modem Number");
						value: "";
						onValueChanged: {
							if (iotDeviceData){
								iotDeviceData.m_modemNumber = value;
							}
						}
					}
				}
			}
			
			GroupElementView {
				id: deviceDetailsGroup;
				
				anchors.left: parent.left;
				anchors.right: parent.right;
				
				title: qsTr("Device Details");
				radius: iotDeviceEditorContainer.radius;
				
				Column {
					id: deviceDetailsColumn;
					
					anchors.left: parent.left;
					anchors.right: parent.right;
					anchors.leftMargin: Style.marginM;
					anchors.rightMargin: Style.marginM;
					
					spacing: Style.marginS;
					
					TextInputView {
						id: manufacturerInput;
						
						anchors.left: parent.left;
						anchors.right: parent.right;
						
						label: qsTr("Manufacturer");
						value: "";
						onValueChanged: {
							if (iotDeviceData){
								iotDeviceData.m_manufacturer = value;
							}
						}
					}
					
					TextInputView {
						id: brandModelInput;
						
						anchors.left: parent.left;
						anchors.right: parent.right;
						
						label: qsTr("Brand/Model");
						value: "";
						onValueChanged: {
							if (iotDeviceData){
								iotDeviceData.m_brandModel = value;
							}
						}
					}
					
					TextInputView {
						id: installationLocationInput;
						
						anchors.left: parent.left;
						anchors.right: parent.right;
						
						label: qsTr("Installation Location");
						value: "";
						onValueChanged: {
							if (iotDeviceData){
								iotDeviceData.m_installationLocation = value;
							}
						}
					}
					
					TextInputView {
						id: connectionTypeInput;
						
						anchors.left: parent.left;
						anchors.right: parent.right;
						
						label: qsTr("Connection Type");
						value: "";
						onValueChanged: {
							if (iotDeviceData){
								iotDeviceData.m_connectionType = value;
							}
						}
					}
					
					TextInputView {
						id: resourceTypeInput;
						
						anchors.left: parent.left;
						anchors.right: parent.right;
						
						label: qsTr("Resource Type");
						value: "";
						onValueChanged: {
							if (iotDeviceData){
								iotDeviceData.m_resourceType = value;
							}
						}
					}
					
					TextInputView {
						id: holeDiameterInput;
						
						anchors.left: parent.left;
						anchors.right: parent.right;
						
						label: qsTr("Hole Diameter");
						value: "";
						onValueChanged: {
							if (iotDeviceData){
								iotDeviceData.m_holeDiameter = value;
							}
						}
					}
					
					TextInputView {
						id: deviceCategoryInput;
						
						anchors.left: parent.left;
						anchors.right: parent.right;
						
						label: qsTr("Device Category (ОДПУ/ИПУ)");
						value: "";
						onValueChanged: {
							if (iotDeviceData){
								iotDeviceData.m_deviceCategory = value;
							}
						}
					}
				}
			}
			
			GroupElementView {
				id: datesGroup;
				
				anchors.left: parent.left;
				anchors.right: parent.right;
				
				title: qsTr("Dates");
				radius: iotDeviceEditorContainer.radius;
				
				Column {
					id: datesColumn;
					
					anchors.left: parent.left;
					anchors.right: parent.right;
					anchors.leftMargin: Style.marginM;
					anchors.rightMargin: Style.marginM;
					
					spacing: Style.marginS;
					
					TextInputView {
						id: calibrationDateInput;
						
						anchors.left: parent.left;
						anchors.right: parent.right;
						
						label: qsTr("Calibration Date");
						value: "";
						onValueChanged: {
							if (iotDeviceData){
								iotDeviceData.m_calibrationDate = value;
							}
						}
					}
					
					TextInputView {
						id: commissionDateInput;
						
						anchors.left: parent.left;
						anchors.right: parent.right;
						
						label: qsTr("Commission Date");
						value: "";
						onValueChanged: {
							if (iotDeviceData){
								iotDeviceData.m_commissionDate = value;
							}
						}
					}
				}
			}
		}
	}
}
