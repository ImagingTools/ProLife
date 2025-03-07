import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0

RemoteCollectionView {
	id: container;
	
	visibleMetaInfo: false;
	
	collectionId: "Orders";
	
	filterMenu.decorator: orderCollectionFilterComp;
	
	commandsDelegateComp: Component {DocumentCollectionViewDelegate {
			collectionView: container;
			
			documentTypeId: "Order";
			viewTypeId: "OrderEditor";
			
			function setupContextMenu(){
				let commandsController = collectionView.commandsController;
				if (commandsController){
					contextMenuModel.clear();
					
					let canEdit = commandsController.commandExists("Edit");
					let canRemove = commandsController.commandExists("Remove");
					
					if (canEdit){
						let index = contextMenuModel.insertNewItem();
						
						contextMenuModel.setData("Id", "Edit", index);
						contextMenuModel.setData("Name", qsTr("Edit"), index);
						contextMenuModel.setData("Icon", "Icons/Edit", index);
					}
					
					if (canRemove){
						let index = contextMenuModel.insertNewItem();
						
						contextMenuModel.setData("Id", "Remove", index);
						contextMenuModel.setData("Name", qsTr("Remove"), index);
						contextMenuModel.setData("Icon", "Icons/Delete", index);
					}
					
					let ok = PermissionsController.checkPermission("ChangeDescriptionForOrder");
					if (ok){
						let index = contextMenuModel.insertNewItem();
						
						contextMenuModel.setData("Id", "SetDescription", index);
						contextMenuModel.setData("Name", qsTr("Set Description"), index);
						contextMenuModel.setData("Icon", "", index);
					}
					
					contextMenuModel.refresh();
				}
			}
		}
	}
	
	Component.onCompleted: {
		collectionFilter.setSortingInfo("TimeStamp", "DESC");
	}
	
	Component {
		id: orderCollectionFilterComp;
		
		DecoratorBase {
			id: mainItem;
			
			width: baseElement ? baseElement.width: 0;
			height: 40;
			
			property CollectionFilter complexFilter: baseElement ? baseElement.complexFilter : null;			
			
			LocalizationEvent {
				onLocalizationChanged: {
				}
			}

			Row {
				id: content;
				
				anchors.left: parent.left;
				anchors.verticalCenter: parent.verticalCenter;
				
				spacing: Style.sizeMainMargin;
				
				AccountFilterComboBox {
					id: accountComboBox;
					width: 300;
					height: filtermenu.height;
					complexFilter: mainItem.complexFilter;
				}
			}
			
			FilterPanelDecorator {
				id: filtermenu
				
				anchors.verticalCenter: parent.verticalCenter;
				anchors.right: parent.right;
				
				baseElement: mainItem.baseElement;
				complexFilter: mainItem.complexFilter;
				
				width: 325;
			}
		}
	}
}


