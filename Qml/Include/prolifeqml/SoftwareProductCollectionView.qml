import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtdocgui 1.0
import prolifeLicensesSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

RemoteCollectionView {
	id: container;
	
	collectionId: "SoftwareProducts";
	
	filterMenu.decorator: licenseCollectionFilterComp;
	
	additionalFieldIds: [
		SoftwareProductItemTypeMetaInfo.s_orderUuid,
		SoftwareProductItemTypeMetaInfo.s_macAddress,
		SoftwareProductItemTypeMetaInfo.s_inUse,
		SoftwareProductItemTypeMetaInfo.s_productUuid,
		SoftwareProductItemTypeMetaInfo.s_customerId,
	]
	
	commandsDelegateComp: Component {SoftwareProductsCollectionViewCommandsDelegate {
			collectionView: container;
		}
	}
	
	visibleMetaInfo: false;
	
	onHeadersChanged: {
		container.table.setColumnContentById(SoftwareProductItemTypeMetaInfo.s_status, pairComp);
	}
	
	Component.onCompleted: {
		collectionFilter.setSortingInfo(SoftwareProductItemTypeMetaInfo.s_timeStamp, "DESC");
		registerDocumentInfo();
	}
	
	function registerDocumentInfo(){
		let documentManager = MainDocumentManager.getDocumentManager(container.collectionId);
		if (documentManager){
			documentManager.registerDocumentView("SoftwareProduct", "SoftwareProductEditor", softwareEditorComp);
			documentManager.registerDocumentDataController("SoftwareProduct", documentDataControllerComp);
			documentManager.registerDocumentValidator("SoftwareProduct", licenseValidatorComp);
		}
	}
	
	Component {
		id: softwareEditorComp;
		
		SoftwareEditor {
			id: softwareEditor;
			
			commandsDelegateComp: Component {ViewCommandsDelegateBase {
					view: softwareEditor;
				}
			}
			
			commandsControllerComp:
				Component {CommandsPanelController {
					commandId: "SoftwareProduct";
					uuid: softwareEditor.viewId;
					commandsView: softwareEditor.commandsView;
				}
			}
		}
	}
	
	Component {
		id: documentDataControllerComp;
		
		GqlRequestDocumentDataController {
			id: requestDocumentDataController
			
			gqlGetCommandId: ProlifeLicensesSdlCommandIds.s_softwareProductItem;
			gqlUpdateCommandId: ProlifeLicensesSdlCommandIds.s_softwareProductUpdate;
			gqlAddCommandId: ProlifeLicensesSdlCommandIds.s_softwareProductAdd;
			
			typeId: "SoftwareProduct";
			
			documentModelComp: Component {
				SoftwareProductData {}
			}
			
			payloadModel: SoftwareProductDataPayload {
				onFinished: {
					requestDocumentDataController.documentModel = m_softwareProductData
				}
			}
		}
	}
	
	Component {
		id: licenseValidatorComp;
		
		LicenseValidator {
		}
	}
	
	Component {
		id: pairComp;
		TableCellDelegateBase {
			id: cellDelegate
			
			Image {
				id: image;
				
				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: parent.left;
				anchors.leftMargin: 5;
				
				width: 18;
				height: width;
				
				sourceSize.width: width;
				sourceSize.height: height;
			}
			
			onReused: {
				if (rowIndex < 0){
					return;
				}

				let inUse = container.table.elements.getData(SoftwareProductItemTypeMetaInfo.s_inUse, rowIndex);
				let isPaired = container.table.elements.getData(SoftwareProductItemTypeMetaInfo.s_isPaired, rowIndex);
				
				if (inUse){
					image.source = "../../../../" + Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Normal);
				}
				else if (isPaired){
					image.source = "../../../../" + Style.getIconPath("Icons/Link", Icon.State.On, Icon.Mode.Normal);
				}
				else{
					image.source = "../../../../" + Style.getIconPath("Icons/Unlink", Icon.State.On, Icon.Mode.Normal);
				}
			}
		}
	}
	
	Component {
		id: licenseCollectionFilterComp;
		
		DecoratorBase {
			id: mainItem;
			
			width: baseElement ? baseElement.width: 0;
			height: 40;
			
			property CollectionFilter complexFilter: baseElement ? baseElement.complexFilter : null;
			
			Component.onCompleted: {
				checkWidth();
			}
			
			LocalizationEvent {
				onLocalizationChanged: {
					mainItem.updateModel();
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
			
			function updateModel(){
				modelCategogy.clear();
				
				let index = modelCategogy.insertNewItem();
				modelCategogy.setData("Id", "All", index);
				modelCategogy.setData("Name", qsTr("Show all licenses"), index);
				
				index = modelCategogy.insertNewItem();
				modelCategogy.setData("Id", "Paired", index);
				modelCategogy.setData("Name", qsTr("Show only paired licenses"), index);
				
				index = modelCategogy.insertNewItem();
				modelCategogy.setData("Id", "NotPaired", index);
				modelCategogy.setData("Name", qsTr("Show only not paired licenses"), index);
				
				index = modelCategogy.insertNewItem();
				modelCategogy.setData("Id", "InUse", index);
				modelCategogy.setData("Name", qsTr("Show the licenses for which the file was created"), index);
				
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
				
				FieldFilter {
					id: pairedFilter
					m_fieldId: "IsPaired"
					m_filterValue: "true"
					m_filterValueType: "Bool"
					m_filterOperations: ["Equal"]
				}
				
				FieldFilter {
					id: unpairedFilter
					m_fieldId: "IsPaired"
					m_filterValue: "false"
					m_filterValueType: "Bool"
					m_filterOperations: ["Equal"]
				}
				
				FieldFilter {
					id: inUseFilter
					m_fieldId: "InUse"
					m_filterValue: "true"
					m_filterValueType: "Bool"
					m_filterOperations: ["Equal"]
				}
				
				ComboBox {
					id: licenseComboBox;
					
					width: 300;
					height: filtermenu.height;
					
					currentIndex: 0;
					
					radius: 3;
					
					onCurrentIndexChanged: {
						mainItem.complexFilter.removeFieldFilter(pairedFilter)
						mainItem.complexFilter.removeFieldFilter(unpairedFilter)
						mainItem.complexFilter.removeFieldFilter(inUseFilter)
						
						if (licenseComboBox.currentIndex == 1){
							mainItem.complexFilter.addFieldFilter(pairedFilter)
						}
						else if (licenseComboBox.currentIndex == 2){
							mainItem.complexFilter.addFieldFilter(unpairedFilter)
						}
						else if (licenseComboBox.currentIndex == 3){
							mainItem.complexFilter.addFieldFilter(inUseFilter)
						}
						
						mainItem.complexFilter.filterChanged()
					}
				}
				
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
				width: contentWidth;
				complexFilter: mainItem.complexFilter;
			}
		}
	}
}


