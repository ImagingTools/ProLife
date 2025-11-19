import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtlicgui 1.0
import prolifeSensorsSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

RemoteCollectionView {
	id: container;

	anchors.fill: parent;
	collectionId: "Devices"

	additionalFieldIds: [DeviceItemTypeMetaInfo.s_statusId, DeviceItemTypeMetaInfo.s_deviceType]
	documentCollectionFilter: null
	commandsDelegateComp: Component {DeviceCollectionViewCommandsDelegate {
			collectionView: container;
		}
	}

	visibleMetaInfo: true;

	Component.onCompleted: {
		table.setSortingInfo(DeviceItemTypeMetaInfo.s_timeStamp, "DESC")
		registerFilters()
	}
	
	function registerFilters(){
		registerFieldFilterDelegate("LicenseStatus", licenseDelegateFilterComp)

		if (PermissionsController.checkPermission("ViewAccounts")){
			registerFieldFilterDelegate("Customers", customersDelegateFilterComp)
		}

		registerFieldFilterDelegate("SensorStatus", statusDelegateFilterComp)
		registerFieldFilterDelegate(DeviceItemTypeMetaInfo.s_productUuid, productsDelegateFilterComp)
		registerFieldFilterDelegate(DeviceItemTypeMetaInfo.s_licenseUuid, licensesDelegateFilterComp)
		setFilterDependency(DeviceItemTypeMetaInfo.s_licenseUuid, DeviceItemTypeMetaInfo.s_productUuid)

		registerFieldFilterDelegate("internalUse", internalUseDelegateFilterComp)
	}

	Component {
		id: statusDelegateFilterComp
		FieldFilterDelegate {
			id: statusDelegateFilter
			name: qsTr("Sensor Status")
			defaultFieldFilter.m_fieldId: "Status"
			defaultFieldFilter.m_filterValueType: "Integer"
			visibleItemCount: 15
			
			Component.onCompleted: {
				createAndAddOption("0", qsTr("None"), "", true)
				createAndAddOption("1", qsTr("Accepted"), "", true)
				createAndAddOption("2", qsTr("In Progress"), "", true)
				createAndAddOption("3", qsTr("Canceled"), "", true)
				createAndAddOption("4", qsTr("On Hold"), "", true)
				createAndAddOption("5", qsTr("Finished"), "", true)
				createAndAddOption("6", qsTr("Defect"), "", true)
				createAndAddOption("7", qsTr("In Repair"), "", true)
				createAndAddOption("8", qsTr("Decommissioned"), "", true)
			}
		}
	}

	NavigableItem {
		paths: ["<status-filter>"]
		parentSegment: container.collectionId
		onActivated: {
			container.collectionFilter.clearAllFilters()

			if (params.customerId !== ""){
				let customersFilterDelegate = container.filterMenu.getFilterDelegate("Customers")
				customersFilterDelegate.setSelectedId(params.customerId, true)
			}

			let statusFilterDelegate = container.filterMenu.getFilterDelegate("SensorStatus")
			statusFilterDelegate.setSelectedId(params.statusId, true)

			let internalUseFilterDelegate = container.filterMenu.getFilterDelegate("internalUse")
			internalUseFilterDelegate.setSelectedId("false", true)

			container.collectionFilter.filterChanged()
		}
	}

	NavigableItem {
		paths: ["<product-use-filter>"]
		parentSegment: container.collectionId
		onActivated: {
			container.collectionFilter.clearAllFilters()

			let productId = params.productId
			let inUse = params.inUse

			if (params.customerId !== ""){
				let customersFilterDelegate = container.filterMenu.getFilterDelegate("Customers")
				customersFilterDelegate.setSelectedId(params.customerId, true)
			}

			let productFilterDelegate = container.filterMenu.getFilterDelegate(DeviceItemTypeMetaInfo.s_productUuid)
			productFilterDelegate.setSelectedId(productId, true)

			if (inUse){
				let licenseStatusFilterDelegate = container.filterMenu.getFilterDelegate("LicenseStatus")
				licenseStatusFilterDelegate.setSelectedIndex(1, true)
			}

			let internalUseFilterDelegate = container.filterMenu.getFilterDelegate("internalUse")
			internalUseFilterDelegate.setSelectedId("false", true)

			container.collectionFilter.filterChanged()
		}
	}

	Component {
		id: internalUseDelegateFilterComp
		FieldFilterDelegate {
			name: qsTr("Usage")

			defaultFieldFilter.m_fieldId: "InternalUse"
			defaultFieldFilter.m_filterValueType: "Bool"

			Component.onCompleted: {
				createAndAddOption("false", qsTr("For Production"), "", true)
				createAndAddOption("true", qsTr("For Internal Purposes"), "", true)
			}
		}
	}

	Component {
		id: licenseDelegateFilterComp
		
		LicenseFilterDelegate {
		}
	}
	
	Component {
		id: customersDelegateFilterComp
		
		CustomerFilterDelegate {
		}
	}

	Component {
		id: productsDelegateFilterComp
		
		FieldFilterDelegate {
			id: productsDelegateFilter
			name: qsTr("Products")
			visibleItemCount: 15
			defaultFieldFilter.m_fieldId: DeviceItemTypeMetaInfo.s_productUuid
			
			OptionsListAdapter {
				id: optionsListAdapter
				collectionModel: CachedProductCollection.hardwareProductsModel
				
				onCollectionModelChanged: {
					productsDelegateFilter.setOptionsList(m_options)
				}
			}
			
			Connections {
				target: CachedProductCollection
				function onHardwareProductsModelReady(){
					optionsListAdapter.collectionModel = null
					optionsListAdapter.collectionModel = CachedProductCollection.hardwareProductsModel
				}
			}
		}
	}

	Component {
		id: licensesDelegateFilterComp
		
		FieldFilterDelegate {
			id: productsDelegateFilter
			name: qsTr("Licenses")
			visibleItemCount: 15
			defaultFieldFilter.m_fieldId: DeviceItemTypeMetaInfo.s_licenseUuid
			
			onFilterDependencyChanged: {
				if (filterId === DeviceItemTypeMetaInfo.s_productUuid){
					optionsListAdapter.collectionModel = CachedProductCollection.getLicensesModel(filterValue)
				}
			}
			
			OptionsListAdapter {
				id: optionsListAdapter
				onCollectionModelChanged: {
					productsDelegateFilter.setOptionsList(m_options)
				}
			}
		}
	}

	onHeadersChanged: {
		container.table.setColumnContentById(DeviceItemTypeMetaInfo.s_status, pairComp);
	}

	Component {
		id: productPairEditorDialog;

		HardwareProductBindingDialog {}
	}

	DeviceProductionStatus {
		id: deviceProductionStatus;
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

				width: 20;
				height: width;

				sourceSize.width: width;
				sourceSize.height: height;
			}

			Text {
				id: statusLable;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: image.right
				anchors.leftMargin: Style.marginM;
				anchors.right: parent.right

				font.pixelSize: Style.fontSizeM;
				font.family: Style.fontFamily;
				color: Style.textColor;

				elide: Text.ElideRight;
			}

			onReused: {
				if (rowIndex >= 0){
					let statusId = cellDelegate.rowDelegate.tableItem.elements.getData(DeviceItemTypeMetaInfo.s_statusId, rowIndex);
					image.source = deviceProductionStatus.getStatusIcon(statusId);
					statusLable.text = cellDelegate.getValue();
				}
			}
		}
	}
}
