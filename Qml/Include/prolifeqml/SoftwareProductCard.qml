import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import prolifeOrdersSdl 1.0

Rectangle {
	id: softwareCard;
	
	width: 500;
	height: visible ? softwareCard.contentHeight : 0;
	
	color: Style.baseColor;
	
	property int margin: 10;
	property int contentHeight: contentColumn.height;
	
	property OrderedProduct productItem: model.item;
	
	property bool readOnly: false;
	property bool commmandsVisible: false;
	
	signal clicked();
	signal edited();

	Component.onCompleted: {
		softwareCard.updateElements();
	}
	
	LocalizationEvent {
		onLocalizationChanged: {
			softwareCard.updateElements();
		}
	}
	
	Item {
		id: rightPanel;
		
		anchors.top: parent.top;
		anchors.right: parent.right;
		
		width: visible ? 30 : 0;
		height: parent.height;
		
		visible: editButton.visible;
		
		Button {
			id: editButton;
			
			anchors.horizontalCenter: parent.horizontalCenter;
			anchors.top: parent.top;
			anchors.topMargin: 10;
			
			width: 18;
			height: width;
			
			iconSource: enabled ? "../../../../" + Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal) :
								  "../../../../" + Style.getIconPath("Icons/Edit", Icon.State.Off, Icon.Mode.Disabled);
			visible: !softwareCard.readOnly && softwareCard.commmandsVisible;
			
			decorator: Component {
				ButtonDecorator {
					color: parent.hovered ? Style.buttonHoverColor : "transparent";
					border.width: 0;
				}
			}
			
			onClicked: {
				softwareCard.edited();
			}
		}
	}
	
	Column {
		id: contentColumn;
		
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: rightPanel.left;
		
		visible: licensesView.elementsCount !== 0

		Table {
			id: licensesView;
			
			width: contentColumn.width;
			height: contentHeight;
			
			enableAlternating: false;
			
			radius: 0;
			backgroundElementsColor: Style.baseColor;
			backgroundHeadersColor: Style.alternateBaseColor;
			
			selectable: false;
			separatorVisible: false;
			
			itemHeight: 25;
			
			clip: true;
			showHeaders: false;
			
			onHeadersChanged: {
				licensesView.tableDecorator = tableDecoratorModel;
			}
		}
	} // Column
	
	TreeItemModel {
		id: headersLicensesTable;
		
		Component.onCompleted: {
			softwareCard.updateHeaders();
		}
	}
	
	TreeItemModel {
		id: elementsTableModel;
	}
	
	function updateElements(){
		elementsTableModel.clear();
		
		let index = elementsTableModel.insertNewItem();
		elementsTableModel.setData("Key", qsTr("Software-ID"), index)
		elementsTableModel.setData("Value", softwareCard.productItem.m_serialNumber, index)
		
		let licenseId = productItem.m_licenseId;
		let licenseName = productItem.m_licenseName;
		
		let name = licenseName;
		if (licenseId !== ""){
			name += " (" + licenseId + ")";
		}
		
		index = elementsTableModel.insertNewItem();
		elementsTableModel.setData("Key", qsTr("Article"), index)
		elementsTableModel.setData("Value", name, index);
		
		let expiration = qsTr("Unlimited");
		
		if (productItem.m_expiration !== ""){
			expiration = productItem.m_expiration;
		}
		
		index = elementsTableModel.insertNewItem();
		elementsTableModel.setData("Key", qsTr("Expiration"), index)
		elementsTableModel.setData("Value", expiration, index)
		
		licensesView.elements = elementsTableModel;
	}
	
	function updateHeaders(){
		headersLicensesTable.clear();
		
		let index = headersLicensesTable.insertNewItem();
		
		headersLicensesTable.setData("id", "Key", index)
		headersLicensesTable.setData("name", qsTr("Key"), index)
		
		index = headersLicensesTable.insertNewItem();
		
		headersLicensesTable.setData("id", "Value", index)
		headersLicensesTable.setData("name", qsTr("Value"), index)
		
		licensesView.headers = headersLicensesTable;
	}
	
	TreeItemModel {
		id: tableDecoratorModel;
		
		Component.onCompleted: {
			var cellWidthModel = tableDecoratorModel.addTreeModel("CellWidth");
			
			let index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("Width", 150, index);
			
			index = cellWidthModel.insertNewItem();
			cellWidthModel.setData("Width", -1, index);
		}
	}
} //Card


