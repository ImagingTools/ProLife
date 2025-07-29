import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
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
	
	documentCollectionFilter: null
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
		table.setSortingInfo(SoftwareProductItemTypeMetaInfo.s_timeStamp, "DESC");
		registerFieldFilterDelegate("LicenseStatus", licenseDelegateFilterComp)
		registerFieldFilterDelegate("Customers", customersDelegateFilterComp)
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
				Component { GqlBasedCommandsController {
					typeId: "SoftwareProduct";
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
		id: licenseDelegateFilterComp
		FieldFilterDelegate {
			id: licenseDelegateFilter
			name: qsTr("License Status")

			Component.onCompleted: {
				createAndAddOption("Paired", qsTr("Show only paired licenses"), "", true)
				createAndAddOption("NotPaired", qsTr("Show only not paired licenses"), "", true)
				createAndAddOption("InUse", qsTr("Show the licenses for which the file was created"), "", true)
				
				setFieldFilterForOption("Paired", pairedFilter)
				setFieldFilterForOption("NotPaired", unpairedFilter)
				setFieldFilterForOption("InUse", inUseFilter)
			}
			
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
		}
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


