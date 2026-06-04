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

	TimeFilter {
		id: licenseCreationTimeFilter
	}

	Component.onCompleted: {
		table.setSortingInfo(DeviceItemTypeMetaInfo.s_timeStamp, "DESC")
		registerFilters()
	}
	
	function registerFilters(){
		registerFilter("LicenseCreationTimeFilter", licenseCreationTimeFilter)
	
		registerFieldFilterDelegate("LicenseStatus", licenseDelegateFilterComp)

		registerFieldFilterDelegate("Customers", customersDelegateFilterComp)

		registerFieldFilterDelegate("SensorStatus", statusDelegateFilterComp)
		registerFieldFilterDelegate(DeviceItemTypeMetaInfo.s_productUuid, productsDelegateFilterComp)
		registerFieldFilterDelegate(DeviceItemTypeMetaInfo.s_licenseUuid, licensesDelegateFilterComp)
		setFilterDependency(DeviceItemTypeMetaInfo.s_licenseUuid, DeviceItemTypeMetaInfo.s_productUuid)

		registerFieldFilterDelegate("internalUse", internalUseDelegateFilterComp)
		registerFieldFilterDelegate("LicenseCreationTimeFilter", licenseCreationTimeDelegateFilterComp)
	}

	Component {
		id: statusDelegateFilterComp
		FieldFilterDelegate {
			id: statusDelegateFilter
			objectName: "SensorStatusFilter"
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
		paths: ["<hardware-filter>"]
		parentSegment: container.collectionId
		onActivated: {
			let documentManager = MainDocumentService.getDocumentService(container.collectionId)
			if (documentManager){
				let docManagerView = documentManager.getDocumentManagerActiveView()
				if (docManagerView){
					docManagerView.setCurrentTabIndex(0)
				}
			}

			container.filterMenu.clearAllFilters(true)

			if (params.customerId !== undefined && params.customerId !== ""){
				let customersFilterDelegate = container.filterMenu.getFilterDelegate("Customers")
				customersFilterDelegate.setSelectedId(params.customerId, true)
			}

			if (params.productId !== undefined && params.productId !== ""){
				let productFilterDelegate = container.filterMenu.getFilterDelegate(DeviceItemTypeMetaInfo.s_productUuid)
				productFilterDelegate.setSelectedId(params.productId, true)
			}

			if (params.licenseId !== undefined && params.licenseId !== ""){
				let licenseFilterDelegate = container.filterMenu.getFilterDelegate(DeviceItemTypeMetaInfo.s_licenseUuid)
				licenseFilterDelegate.setSelectedId(params.licenseId, true)
			}

			if (params.statusId !== undefined && params.statusId !== ""){
				let statusFilterDelegate = container.filterMenu.getFilterDelegate("SensorStatus")
				statusFilterDelegate.setSelectedId(params.statusId, true)
			}

			if (params.inUse !== undefined){
				if (params.inUse){
					let licenseStatusFilterDelegate = container.filterMenu.getFilterDelegate("LicenseStatus")
					licenseStatusFilterDelegate.setSelectedIndex(1, true)
				}
			}

			if (params.internalUse !== undefined){
				let internalUseFilterDelegate = container.filterMenu.getFilterDelegate("internalUse")
				if (!params.internalUse){
					internalUseFilterDelegate.setSelectedId("false", true)
				}
				else{
					internalUseFilterDelegate.setSelectedId("true", true)
				}
			}

			if (params.licenseCreationTimeFilter){
				let timeFilterDelegate = container.filterMenu.getFilterDelegate("LicenseCreationTimeFilter")
				timeFilterDelegate.setTimeUnit(params.licenseCreationTimeFilter.mode, params.licenseCreationTimeFilter.unit, true)
			}

			if (params.timeFilter){
				let timeFilterDelegate = container.filterMenu.getFilterDelegate("DateFilter")
				timeFilterDelegate.setTimeUnit(params.timeFilter.mode, params.timeFilter.unit, true)
			}

			container.collectionFilter.setSortingInfo(DeviceItemTypeMetaInfo.s_timeStamp, "DESC")

			container.filterMenu.filterChanged()
		}
	}

	Component {
		id: internalUseDelegateFilterComp
		FieldFilterDelegate {
			objectName: "UsageFilter"
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
			objectName: "LicenseFilter"
		}
	}

	Component {
		id: licenseCreationTimeDelegateFilterComp
		TimeFilterDelegate {
			objectName: "LicenseCreationDateFilter"
			name: qsTr("License Creation Date")
			canTimeRangeEdit: false
			onAccepted: {
				licenseCreationTimeFilter.copyFrom(timeFilter)
			}
		}
	}
	
	Component {
		id: customersDelegateFilterComp
		
		CustomerFilterDelegate {
			objectName: "CustomersFilter"
		}
	}

	Component {
		id: productsDelegateFilterComp
		
		FieldFilterDelegate {
			id: productsDelegateFilter
			objectName: "ProductsFilter"
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
			objectName: "LicensesFilter"
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
