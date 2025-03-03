import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtbaseComplexCollectionFilterSdl 1.0

DecoratorBase {
	id: mainItem;
	
	width: baseElement ? baseElement.width: 0;
	height: 40;
	
	property alias licenseCb: licenseComboBox;
	property alias accountCb: accountComboBox;
	
	property CollectionFilter complexFilter: baseElement ? baseElement.complexFilter : null;	
	
	Component.onCompleted: {
		updateText();
		checkWidth();
	}

	LocalizationEvent {
		onLocalizationChanged: {
			mainItem.updateModel();
			mainItem.updateText()
		}
	}
	
	onWidthChanged: {
		checkWidth();
	}
	
	function checkWidth(){
		if (width - filtermenu.width <= content.width + 2 * content.spacing){
			content.visible = false;
		}
		else{
			content.visible = true;
		}
	}
	
	function updateText(){
		onlyNewSensorsText.text = qsTr("Only new sensors");
	}
	
	function updateModel(){
		modelCategogy.clear();
		
		let index = modelCategogy.insertNewItem();
		modelCategogy.setData("Id", "None", index);
		modelCategogy.setData("Name", qsTr("Show all sensors"), index);
		
		index = modelCategogy.insertNewItem();
		modelCategogy.setData("Id", "WithoutLicense", index);
		modelCategogy.setData("Name", qsTr("Sensors without a license"), index);
		
		index = modelCategogy.insertNewItem();
		modelCategogy.setData("Id", "WithLicense", index);
		modelCategogy.setData("Name", qsTr("Sensors with license"), index);
		
		modelCategogy.refresh();
		
		licenseComboBox.model = modelCategogy;
	}
	
	TreeItemModel {
		id: modelCategogy;
		
		Component.onCompleted: {
			mainItem.updateModel();
		}
	}
	
	Row {
		id: content;
		
		anchors.left: parent.left;
		anchors.verticalCenter: parent.verticalCenter;
		
		spacing: Style.size_mainMargin;
		
		ComboBox {
			id: licenseComboBox;
			
			height: filtermenu.height;
			width: 250;
			
			currentIndex: 0;
			
			radius: 3;
			
			onCurrentIndexChanged: {
				mainItem.complexFilter.removeFieldFilter(licenseFilter)
				
				if (licenseComboBox.currentIndex == 1){
					licenseFilter.m_filterValue = '0';
					licenseFilter.m_filterOperations = ["Equal"]
					
					mainItem.complexFilter.addFieldFilter(licenseFilter)
				}
				else if (licenseComboBox.currentIndex == 2){
					licenseFilter.m_filterValue = '0';
					licenseFilter.m_filterOperations = ["Greater"]
					
					mainItem.complexFilter.addFieldFilter(licenseFilter)
				}
				
				mainItem.complexFilter.filterChanged()
			}
		}
		
		FieldFilter {
			id: licenseFilter
			m_fieldId: "SoftwareCount"
			m_filterValueType: "Integer"
		}
		
		FieldFilter {
			id: statusFilter
			m_fieldId: "Status"
			m_filterValue: "0"
			m_filterValueType: "Integer"
			m_filterOperations: ["Equal"]
		}
		
		AccountFilterComboBox {
			id: accountComboBox;
			width: 300;
			height: filtermenu.height;
			currentIndex: 0;
			radius: 3;
			shownItemsCount: 15;
			complexFilter: mainItem.complexFilter;
		}
		
		Row {
			id: row;
			anchors.verticalCenter: parent.verticalCenter;
			height: filtermenu.height;
			spacing: Style.size_mainMargin;
			
			Text {
				id: onlyNewSensorsText;
				anchors.verticalCenter: parent.verticalCenter;
				color: Style.textColor;
				font.family: Style.fontFamily;
				font.pixelSize: Style.fontSize_common;
			}
			
			CheckBox {
				anchors.verticalCenter: parent.verticalCenter;
				widthFromDecorator: true;
				onClicked: {
					mainItem.complexFilter.removeFieldFilter(statusFilter)
					
					if (checkState == Qt.Checked){
						checkState = Qt.Unchecked;
					}
					else{
						checkState = Qt.Checked;
						mainItem.complexFilter.addFieldFilter(statusFilter)
					}

					mainItem.complexFilter.filterChanged()
				}
			}
		}
	}
	
	FilterPanelDecorator {
		id: filtermenu
		
		anchors.verticalCenter: parent.verticalCenter;
		anchors.right: parent.right;
		
		baseElement: mainItem.baseElement;
		
		width: contentWidth;
		
		complexFilter: mainItem.complexFilter;
	}
}

