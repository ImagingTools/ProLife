import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import prolifeOrdersSdl 1.0

RemoteCollectionView {
	id: container;
	
	visibleMetaInfo: false;
	
	collectionId: "Orders";
	
	documentCollectionFilter: null
	
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
						
						contextMenuModel.setData("id", "Edit", index);
						contextMenuModel.setData("name", qsTr("Edit"), index);
						contextMenuModel.setData("icon", "Icons/Edit", index);
					}
					
					if (canRemove){
						let index = contextMenuModel.insertNewItem();
						
						contextMenuModel.setData("id", "Remove", index);
						contextMenuModel.setData("name", qsTr("Remove"), index);
						contextMenuModel.setData("icon", "Icons/Delete", index);
					}
					
					let ok = PermissionsController.checkPermission("ChangeDescriptionForOrder");
					if (ok){
						let index = contextMenuModel.insertNewItem();
						
						contextMenuModel.setData("id", "SetDescription", index);
						contextMenuModel.setData("name", qsTr("Set Description"), index);
						contextMenuModel.setData("icon", "", index);
					}
					
					contextMenuModel.refresh();
				}
			}
		}
	}
	
	Component.onCompleted: {
		table.setSortingInfo(OrderItemTypeMetaInfo.s_timeStamp, "DESC");
		registerFieldFilterDelegate("Customers", customersDelegateFilterComp)
	}

	Component {
		id: customersDelegateFilterComp
		FieldFilterDelegate {
			id: customersDelegateFilter
			name: qsTr("Customers")
			visibleItemCount: 15
			defaultFieldFilter.m_fieldId: "CustomerId"
			
			OptionsListAdapter {
				id: optionsListAdapter
				collectionModel: CachedAccountCollection.collectionModel
				
				onCollectionModelChanged: {
					customersDelegateFilter.setOptionsList(m_options)
				}
			}
		}
	}
}


