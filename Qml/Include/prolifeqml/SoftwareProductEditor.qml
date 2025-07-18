import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtlicgui 1.0
import imtcontrols 1.0
import prolifeOrdersSdl 1.0
import prolifeLicensesSdl 1.0

ViewBase {
	id: root;
	
	// height: content.height;
	
	property var productLicensesModel: TreeItemModel{}
	property BaseModel orderProductsModel: BaseModel {}
	
	property TreeItemModel softwaresModel: TreeItemModel{}
	
	//    property alias tableElements: licensesTable.elements;
	property alias tableElements: licenseCB.sourceModel;
	property bool readOnly: false;
	
	property bool isNewSoftware: switchNewLicense.checked;
	property int productIndex: -1;
	
	property OrderedProduct productItem: model ? model : null;
	
	function setReadOnly(readOnly){
		serialNumberInput.readOnly = readOnly;
		expirationElementView.datePicker.readOnly = readOnly;
		licenseCB.changeable = !readOnly
	}

	function updateGui(){
		if (productItem.m_isNew){
			switchNewLicense.setChecked(true)
			
			licenseCB.currentIndex = -1;
			if (licenseCB.sourceModel){
				
				for (let i = 0; i < licenseCB.sourceModel.getItemsCount(); i++){
					let id = licenseCB.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_id, i);
					if (id === productItem.m_licenseUuid){
						licenseCB.currentIndex = i;
						
						break;
					}
				}
			}
			
			serialNumberInput.text = productItem.m_serialNumber;
			
			let expiration = productItem.m_expiration;
			
			if (expiration && expiration !== "" ){
				expirationElementView.checkBox.checkState = Qt.Checked;
			}
			else{
				expirationElementView.checkBox.checkState = Qt.Unchecked;
			}
			
			if (expiration){
				let currentDate = expirationElementView.datePicker.getDateAsString();
				
				if (expiration !== "" && expiration !== currentDate){
					let date = expiration;
					let data = date.split("-");
					expirationElementView.datePicker.setDate(Number(data[0]), Number(data[1]) - 1, Number(data[2]));
				}
			}
		}
		else{
			switchNewLicense.setChecked(false)
			
			createdLicenseCb.currentIndex = -1;
			
			let licenseUuid = productItem.m_id;
			
			if (createdLicenseCb.sourceModel){
				for (let i = 0; i < createdLicenseCb.sourceModel.getItemsCount(); i++){
					let id = createdLicenseCb.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_id, i);
					
					if (id === licenseUuid){
						createdLicenseCb.currentIndex = i;
						
						break;
					}
				}
			}
			
			softwareValue.text = productItem.m_serialNumber;
		}
	}
	
	function updateModel(){
		productItem.m_isNew = isNewSoftware;
		
		if (isNewSoftware){
			if (licenseCB.currentIndex >= 0 && licenseCB.sourceModel){
				let selectedId = licenseCB.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_id, licenseCB.currentIndex);
				productItem.m_licenseUuid = selectedId;
				
				let licenseId = licenseCB.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_licenseId, licenseCB.currentIndex);
				productItem.m_licenseId = licenseId;
				
				let licenseName = licenseCB.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_licenseName, licenseCB.currentIndex);
				productItem.m_licenseName = licenseName;
			}
			else{
				productItem.m_licenseUuid = "";
				productItem.m_licenseId = "";
				productItem.m_licenseName = "";
			}
			
			productItem.m_serialNumber = serialNumberInput.text;
			
			if (expirationElementView.checkBox.checkState == Qt.Checked){
				productItem.m_expiration = expirationElementView.datePicker.getDateAsString();
			}
			else{
				productItem.m_expiration = "";
			}
		}
		else{
			productItem.m_licenseUuid = "";
			productItem.m_licenseId = "";
			productItem.m_licenseName = "";
			productItem.m_serialNumber = "";
			productItem.m_expiration = "";
			
			if (createdLicenseCb.currentIndex >= 0){
				let id = createdLicenseCb.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_id, createdLicenseCb.currentIndex);
				productItem.m_id = id;
				
				let licenseUuid = createdLicenseCb.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_licenseUuid, createdLicenseCb.currentIndex);
				productItem.m_licenseUuid = licenseUuid;
				
				let licenseID = createdLicenseCb.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_licenseId, createdLicenseCb.currentIndex);
				productItem.m_licenseId = licenseID;
				
				let licenseName = createdLicenseCb.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_licenseName, createdLicenseCb.currentIndex);
				productItem.m_licenseName = licenseName;
				
				let serialNumber = createdLicenseCb.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_serialNumber, createdLicenseCb.currentIndex);
				productItem.m_serialNumber = serialNumber;
				
				let expiration = createdLicenseCb.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_expiration, createdLicenseCb.currentIndex);
				productItem.m_expiration = expiration;
				
				productItem.m_inUse = createdLicenseCb.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_inUse, createdLicenseCb.currentIndex);
			}
		}
	}
	
	GroupElementView {
		width: parent.width
		
		SwitchElementView {
			id: switchNewLicense;
			
			width: parent.width;
			
			name: qsTr("New License");
			
			visible: root.productIndex == -1 || root.isNewSoftware;
			
			onCheckedChanged: {
				createdLicenseCb.visible = !checked;
				
				root.doUpdateModel();
			}
			
			Component.onCompleted: {
				let canAddLicense = PermissionsController.checkPermission("AddLicense");
				if (!canAddLicense){
					switchNewLicense.visible = false;
				}
			}
		}
		
		FilterableComboBoxElementView {
			id: createdLicenseCb;
			
			width: parent.width;
			controlWidth: 500;
			
			sourceModel: root.softwaresModel;
			
			changeable: !root.readOnly;
			
			name: qsTr("License")
			
			filteringFields: [
				SoftwareProductItemTypeMetaInfo.s_serialNumber,
				SoftwareProductItemTypeMetaInfo.s_productName,
				SoftwareProductItemTypeMetaInfo.s_licenseId,
				SoftwareProductItemTypeMetaInfo.s_licenseName
			];
			
			bottomComp: currentIndex < 0 ? licenseTypeErrorComp : undefined;
			
			delegate: Component {
				FilterableComboBoxDelegate {
					width: comboBoxRef ? comboBoxRef.width : 0;
					comboBoxRef: createdLicenseCb.cbRef;
					
					text: model[SoftwareProductItemTypeMetaInfo.s_serialNumber] === "" ? model[SoftwareProductItemTypeMetaInfo.s_productName] +
																						 " (" + qsTr("No software-ID") + ")" : model[SoftwareProductItemTypeMetaInfo.s_productName] +
																						 " (" + model[SoftwareProductItemTypeMetaInfo.s_serialNumber]+ ")";
					
					property string article: qsTr("Article");
					property string notSpecified: qsTr("not specified");
					
					description: model[SoftwareProductItemTypeMetaInfo.s_licenseId] !== "" ? article +
																							 ": " + model[SoftwareProductItemTypeMetaInfo.s_licenseName] +
																							 " (" + model[SoftwareProductItemTypeMetaInfo.s_licenseId] + ")": article + ": " + notSpecified;
				}
			}
			
			onCurrentIndexChanged: {
				if (currentIndex >= 0){
					if (createdLicenseCb.sourceModel.containsKey(SoftwareProductItemTypeMetaInfo.s_licenseName, currentIndex)){
						let licenseName = createdLicenseCb.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_licenseName, currentIndex)
						
						typeValue.text = licenseName;
					}
					
					if (createdLicenseCb.sourceModel.containsKey(SoftwareProductItemTypeMetaInfo.s_licenseId, currentIndex)){
						let licenseId = createdLicenseCb.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_licenseId, currentIndex)
						
						articleValue.text = licenseId;
					}
					
					if (createdLicenseCb.sourceModel.containsKey(SoftwareProductItemTypeMetaInfo.s_expiration, currentIndex)){
						let expiration = createdLicenseCb.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_expiration, currentIndex)
						if (expiration === ""){
							expirationValue.text = qsTr("Unlimited");
						}
						else{
							expirationValue.text = expiration;
						}
					}
				}
				
				root.doUpdateModel();
			}
		}
		
		GroupElementView {
			width: parent.width;
			visible: !root.isNewSoftware && createdLicenseCb.currentIndex >= 0;
			
			TextElementView {
				id: typeValue;
				width: parent.width;
				name: qsTr("License Type");
			}
			
			TextElementView {
				id: articleValue;
				width: parent.width;
				name: qsTr("Article Number");
			}
			
			TextElementView {
				id: softwareValue;
				width: parent.width;
				name: qsTr("Software-ID");
			}
			
			TextElementView {
				id: expirationValue;
				width: parent.width;
				name: qsTr("Expiration");
			}
		}
		
		GroupElementView {
			width: parent.width;
			visible: root.isNewSoftware;
			
			FilterableComboBoxElementView {
				id: licenseCB;
				
				width: parent.width;
				controlWidth: 500;
				
				name: qsTr("License Types");
				nameId: SoftwareProductItemTypeMetaInfo.s_licenseName;
				
				sourceModel: root.productLicensesModel;
				
				changeable: !root.readOnly;
				
				bottomComp: currentIndex < 0 ? licenseTypeErrorComp : undefined;
				filteringFields: [SoftwareProductItemTypeMetaInfo.s_licenseName, SoftwareProductItemTypeMetaInfo.s_licenseId];
				
				delegate: Component {
					FilterableComboBoxDelegate {
						width: licenseCB.width;
						comboBoxRef: licenseCB.cbRef;
						
						text: model[SoftwareProductItemTypeMetaInfo.s_licenseName];
						description: model[SoftwareProductItemTypeMetaInfo.s_licenseId];
					}
				}
				
				onCurrentIndexChanged: {
					if (currentIndex >= 0){
						if (licenseCB.sourceModel.containsKey(SoftwareProductItemTypeMetaInfo.s_licenseId, currentIndex)){
							let licenseId = licenseCB.sourceModel.getData(SoftwareProductItemTypeMetaInfo.s_licenseId, currentIndex)
							
							newArticleValue.text = licenseId
						}
					}
					
					root.doUpdateModel();
				}
				
				Component {
					id: licenseTypeErrorComp;
					
					Text {
						id: selectSensorText;
						
						text: qsTr("Please select a license");
						color: Style.errorTextColor;
						font.family: Style.fontFamily;
						font.pixelSize: Style.fontSizeM;
					}
				}
			}

			TextInputElementView {
				id: serialNumberInput;
				
				width: parent.width;
				controlWidth: 500;
				
				name: qsTr("Software-ID");
				placeHolderText: qsTr("Enter the software-ID");
				
				readOnly: root.readOnly;
				
				visible: licenseCB.currentIndex >= 0;
				
				onEditingFinished: {
					serialNumberInput.bottomComp = undefined
					if (root.orderProductsModel){
						for (let i = 0; i < root.orderProductsModel.count; i++){
							let productItem = root.orderProductsModel.get(i).item
							if (productItem.m_id !== root.productItem.m_id &&
								productItem.m_categoryId === "Software" &&
								productItem.m_serialNumber === text){
								serialNumberInput.bottomComp = serialNumberErrorComp
								break
							}
						}
					}

					root.doUpdateModel();
				}
				
				Component {
					id: serialNumberErrorComp;
					
					Text {
						id: selectSensorText;
						text: qsTr("Serial Number already exists");
						color: Style.errorTextColor;
						font.family: Style.fontFamily;
						font.pixelSize: Style.fontSizeM;
					}
				}
			}
			
			ElementView {
				id: expirationElementView;
				
				width: parent.width;
				
				name: qsTr("Expiration");
				
				visible: licenseCB.currentIndex >= 0;
				
				property CheckBox checkBox;
				property DatePicker datePicker;
				
				controlComp: Component {
					Item {
						width: 300;
						height: 30;
						
						CheckBox {
							id: checkBox;
							
							anchors.verticalCenter: parent.verticalCenter;
							anchors.left: parent.left;
							
							isActive: licenseCB.currentIndex >= 0 && licenseCB.changeable && !root.readOnly;
							
							onCheckStateChanged: {
								root.doUpdateModel();
							}
							
							Component.onCompleted: {
								expirationElementView.checkBox = checkBox;
							}
						}
						
						Text {
							id: textUnlimited;
							
							anchors.verticalCenter: parent.verticalCenter;
							anchors.left: checkBox.right;
							anchors.leftMargin: Style.marginM;
							
							visible: checkBox.checkState === Qt.Unchecked;
							
							font.family: Style.fontFamily;
							font.pixelSize: Style.fontSizeM;
							color: Style.textColor;
							
							text: qsTr("Unlimited");
						}
						
						DatePicker {
							id: datePicker_;
							
							anchors.verticalCenter: parent.verticalCenter;
							anchors.left: checkBox.right;
							anchors.leftMargin: Style.marginM;
							
							visible: checkBox.checkState === Qt.Checked;
							
							width: 100;
							height: parent.height;
							
							currentDayButtonVisible: false;
							startWithCurrentDay: true;
							
							readOnly: root.readOnly;
							
							hasDayCombo: false;
							hasMonthCombo: false;
							hasYearCombo: false;
							
							textFieldWidthDay: 30;
							textFieldWidthYear: 45;
							textFieldWidthMonth: 90;
							
							textFieldHeight: height;
							
							textFieldBorderColor: Style.borderColor;
							mainMargin: Style.marginM;
							
							Component.onCompleted: {
								expirationElementView.datePicker = datePicker_;
							}
							
							onDateChanged: {
								root.doUpdateModel()
							}
							
							onCompletedChanged: {
								if (completed){
									var date_ = new Date();
									
									let day = date_.getDay();
									let year = date_.getFullYear() + 1;
									let month = date_.getMonth();
									
									datePicker_.setDate(year, month, day)
								}
							}
						}
					}
				}
			}
			
			TextElementView {
				id: newArticleValue;
				width: parent.width;
				name: qsTr("Article Number");
				visible: parent.visible && licenseCB.currentIndex >= 0;
			}
		}
	}
}//Container


