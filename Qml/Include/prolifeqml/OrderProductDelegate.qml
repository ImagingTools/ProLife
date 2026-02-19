// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtlicgui 1.0
import prolifeOrdersSdl 1.0

ElementView {
	id: root;
	
	property OrderedProduct productItem: model.item ? model.item : null;
	name: productItem ? productItem.m_productName : "";
	
	clip: false;
	
	property TreeItemModel activeCommandsModel: TreeItemModel {}
	
	property bool expanded: true;
	
	controlComp: Component {
		Item {
			id: item;
			
			width: commands.width + commands.anchors.rightMargin;
			height: 30;
			
			SimpleCommandsDecorator {
				id: commands;
				
				anchors.right: parent.right;
				height: 30;
				
				commandModel: root.activeCommandsModel;
				
				onCommandActivated: {
					if (commandId == "Remove"){
						root.removed();
					}
					else if (commandId == "Edit"){
						root.edited();
					}
				}
			}
		}
	}
	
	bottomComp: root.categoryId == "Software" ? softwareProductCard : hardwareProductCard;
	
	Row {
		id: row;
		
		anchors.verticalCenter: parent.top;
		anchors.left: parent.left;
		anchors.leftMargin: Style.marginM;
		
		spacing: Style.marginM;
		
		StickerView {
			anchors.verticalCenter: parent.verticalCenter;
			color: root.categoryId == "Software" ? "orange" : Style.iconColorOnSelected;
			text: root.categoryId;
		}
		
		StickerView {
			anchors.verticalCenter: parent.verticalCenter;
			color: Style.errorColor;
			text: qsTr("New");
			visible: root.isNew
		}

		StickerView {
			anchors.verticalCenter: parent.verticalCenter;
			color: "orange"
			text: qsTr("Multi: ") + root.productCount + " " + qsTr("instances")
			visible: root.isMultiple
		}
	}
	
	property bool readOnly: false;
	property bool inUse: productItem ? productItem.m_inUse: false;
	property bool isNew: productItem ? productItem.m_isNew : false;
	property string categoryId: productItem ? productItem.m_categoryId : "";
	
	property bool productInUse: softwareCommandsModel.completed && root.inUse;

	// Only for software product
	property bool isMultiple: productItem ? productItem.m_isMultiple : false
	property int productCount: productItem ? productItem.m_productCount : false
	
	signal removed();
	signal edited();
	
	onProductInUseChanged: {
		if (productInUse){
			root.setCommandValue(softwareCommandsModel, "Edit", "IsEnabled", !root.inUse);
		}
	}
	
	onReadOnlyChanged: {
		root.setIsEnabledCommand(softwareCommandsModel, "Edit", !root.readOnly);
		root.setIsEnabledCommand(softwareCommandsModel, "Remove", !root.readOnly);
	}
	
	function setIsEnabledCommand(commandsModel, commandId, isEnabled){
		for (let i = 0; i < commandsModel.getItemsCount(); i++){
			let id = commandsModel.getData("id", i);
			if (id === commandId){
				commandsModel.setData("isEnabled", isEnabled, i);
				break;
			}
		}
	}
	
	function setCommandValue(commandsModel, commandId, commandKey, commandValue){
		for (let i = 0; i < commandsModel.getItemsCount(); i++){
			let id = commandsModel.getData("id", i);
			if (id === commandId){
				commandsModel.setData(commandKey, commandValue, i);
				break;
			}
		}
	}
	
	Component {
		id: softwareProductCard;
		
		SoftwareProductCard {
			visible: root.expanded;
		}
	}
	
	Component {
		id: hardwareProductCard;
		
		HardwareProductCard {
			visible: root.expanded;
		}
	}
	
	TreeItemModel {
		id: softwareCommandsModel;
		
		property bool completed: false;
		
		Component.onCompleted: {
			let index = softwareCommandsModel.insertNewItem();
			
			softwareCommandsModel.setData("id", "Edit", index);
			softwareCommandsModel.setData("name", "Edit", index);
			softwareCommandsModel.setData("icon", "Icons/Edit", index);
			softwareCommandsModel.setData("isEnabled", !root.readOnly, index);
			softwareCommandsModel.setData("visible", true, index);
			
			index = softwareCommandsModel.insertNewItem();
			
			softwareCommandsModel.setData("id", "Remove", index);
			softwareCommandsModel.setData("name", "Remove", index);
			softwareCommandsModel.setData("icon", "Icons/Delete", index);
			softwareCommandsModel.setData("isEnabled", !root.readOnly, index);
			softwareCommandsModel.setData("visible", true, index);
			
			root.activeCommandsModel = softwareCommandsModel;
			softwareCommandsModel.completed = true;
		}
	}
} //Card


