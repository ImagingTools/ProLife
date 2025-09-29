import QtQuick 2.0
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import imtcolgui 1.0
import prolifeOrdersSdl 1.0
import prolifeLicensesSdl 1.0
import prolifeSensorsSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

Item {
	id: productEditor;
	
	property TreeItemModel licensesModel: TreeItemModel{}
	property TreeItemModel productsModel: TreeItemModel{}
	property BaseModel orderProductsModel: BaseModel {}
	
	property TreeItemModel devicesModel: TreeItemModel {}
	property TreeItemModel softwaresModel: TreeItemModel {}
	
	property bool blockUpdatingModel: false;
	
	property string orderId;
	property string orderUuid;
	
	property OrderedProduct productItem;
	property int index: -1;
	property int instanceCount: 1
	
	property string softwareCategoryId: "Software";
	property string hardwareCategoryId: "Hardware";
	
	CollectionDataProvider {
		id: softwareCollection;
		commandId: ProlifeLicensesSdlCommandIds.s_softwareProductsList;
		fields: [
			SoftwareProductItemTypeMetaInfo.s_id,
			SoftwareProductItemTypeMetaInfo.s_name,
			SoftwareProductItemTypeMetaInfo.s_productName,
			SoftwareProductItemTypeMetaInfo.s_licenseUuid,
			SoftwareProductItemTypeMetaInfo.s_licenseId,
			SoftwareProductItemTypeMetaInfo.s_licenseName,
			SoftwareProductItemTypeMetaInfo.s_serialNumber,
			SoftwareProductItemTypeMetaInfo.s_customerName,
			SoftwareProductItemTypeMetaInfo.s_customerId,
			SoftwareProductItemTypeMetaInfo.s_productUuid,
			SoftwareProductItemTypeMetaInfo.s_orderUuid,
			SoftwareProductItemTypeMetaInfo.s_expiration
		];
		onCollectionModelChanged: {
			contentLoader.item.softwaresModel = collectionModel;
			contentLoader.item.doUpdateGui();
		}
		
		onStateChanged: {
			loading.visible = state === "Loading";
		}
	}
	
	CollectionDataProvider {
		id: deviceCollection;
		commandId: ProlifeSensorsSdlCommandIds.s_devicesList;
		fields: [
			DeviceItemTypeMetaInfo.s_id,
			DeviceItemTypeMetaInfo.s_name,
			DeviceItemTypeMetaInfo.s_deviceType,
			DeviceItemTypeMetaInfo.s_orderUuid,
			DeviceItemTypeMetaInfo.s_productUuid,
			DeviceItemTypeMetaInfo.s_licenseUuid,
			DeviceItemTypeMetaInfo.s_macAddress,
			DeviceItemTypeMetaInfo.s_serialNumber
		]
		
		onCollectionModelChanged: {
			contentLoader.item.devicesModel = collectionModel;
			contentLoader.item.doUpdateGui();
		}
		
		onStateChanged: {
			loading.visible = state === "Loading";
		}
	}
	
	FieldFilter {
		id: excludeDocumentIdFilter;
		m_fieldId: "DocumentId";
		m_filterValueType: "String";
		m_filterOperations: ["Not", "Equal"];
	}
	
	GroupFilter {
		id: excludesGroup;
		m_logicalOperation: deviceCollection.filter.logicalOperation.AND;
		Component.onCompleted: {
			if (!hasFieldFilters()){
				createFieldFilters()
			}
		}
	}
	
	GroupFilter {
		id: orderGroupFilter;
		m_logicalOperation: deviceCollection.filter.logicalOperation.OR;
		
		Component.onCompleted: {
			if (!hasFieldFilters()){
				createFieldFilters()
			}
			
			m_fieldFilters.addElement(orderUuidFilter);
			m_fieldFilters.addElement(emptyOrderFilter);
			
			deviceCollection.filter.addGroupFilter(orderGroupFilter)
			softwareCollection.filter.addGroupFilter(orderGroupFilter)
		}
	}
	
	FieldFilter {
		id: productIdFilter
		m_fieldId: "ProductUuid"
		m_filterValue: productEditor.productItem.m_productUuid
		m_filterValueType: "String"
		m_filterOperations: ["Equal"]
	}
	
	FieldFilter {
		id: deviceTypeFilter
		m_fieldId: "DeviceType"
		m_filterValue: productEditor.productItem.m_productUuid
		m_filterValueType: "String"
		m_filterOperations: ["Equal"]
	}
	
	FieldFilter {
		id: orderUuidFilter
		m_fieldId: "OrderId"
		m_filterValue: productEditor.orderUuid
		m_filterValueType: "String"
		m_filterOperations: ["Equal"]
	}
	
	FieldFilter {
		id: emptyOrderFilter
		m_fieldId: "OrderId"
		m_filterValue: ""
		m_filterValueType: "String"
		m_filterOperations: ["Equal"]
	}
	
	function setHardware(){
		segmentedElementView.softwareProductButton.checkable = false;
		segmentedElementView.softwareProductButton.checked = false;
		
		segmentedElementView.hardwareProductButton.checkable = true;
		segmentedElementView.hardwareProductButton.checked = true;
		
		productCB.model = CachedProductCollection.hardwareProductsModel;
		
		productCB.currentIndex = -1;
		
		contentLoader.sourceComponent = undefined
		contentLoader.sourceComponent = hardwareProductComponent;
	}
	
	function setSoftware(){
		segmentedElementView.softwareProductButton.checkable = true;
		segmentedElementView.softwareProductButton.checked = true;
		
		segmentedElementView.hardwareProductButton.checkable = false;
		segmentedElementView.hardwareProductButton.checked = false;
		
		productCB.model = CachedProductCollection.softwareProductsModel;
		
		productCB.currentIndex = -1;
		
		contentLoader.sourceComponent = undefined
		contentLoader.sourceComponent = softwareProductComponent;
	}
	
	function updateProductModel(){
		if (productCB.currentIndex >= 0){
			if (!productEditor.blockUpdatingModel){
				productEditor.clearProduct();
			}
			
			productItem.m_productUuid = productCB.model.getData(OrderedProductTypeMetaInfo.s_id, productCB.currentIndex);
			productItem.m_categoryId = productCB.model.getData(OrderedProductTypeMetaInfo.s_categoryId, productCB.currentIndex);
			productItem.m_productName = productCB.model.getData(OrderedProductTypeMetaInfo.s_productName, productCB.currentIndex);
			
			contentLoader.item.productLicensesModel = 0;
			
			let licensesModel = productCB.model.getData("licenses", productCB.currentIndex);
			if (licensesModel){
				contentLoader.item.productLicensesModel = licensesModel;
			}
			
			if (contentLoader.item.productLicensesModel){
				contentLoader.item.productLicensesModel.refresh()
			}
			
			
			if (productItem.m_categoryId === productEditor.hardwareCategoryId){
				deviceCollection.filter.removeFieldFilter(deviceTypeFilter);
				deviceTypeFilter.m_filterValue = productItem.m_productUuid;
				deviceCollection.filter.addFieldFilter(deviceTypeFilter)
				
				deviceCollection.filter.removeGroupFilter(excludesGroup)
				
				if (orderProductsModel){
					for (let i = 0; i < orderProductsModel.count; i++){
						let categoryId = orderProductsModel.get(i).item.m_categoryId;
						if (categoryId === hardwareCategoryId){
							let deviceID = orderProductsModel.get(i).item.m_id;
							if (deviceID !== "" && productItem.m_id != deviceID){
								excludeDocumentIdFilter.m_filterValue = deviceID;
								excludesGroup.m_fieldFilters.addElement(excludeDocumentIdFilter.copyMe());
							}
						}
					}
				}
				
				deviceCollection.filter.addGroupFilter(excludesGroup)
				
				deviceCollection.updateModel();
			}
			
			if (productItem.m_categoryId === productEditor.softwareCategoryId){
				softwareCollection.filter.removeFieldFilter(productIdFilter);
				productIdFilter.m_filterValue = productItem.m_productUuid;
				softwareCollection.filter.addFieldFilter(productIdFilter)
				
				softwareCollection.filter.removeGroupFilter(excludesGroup)
				
				if (orderProductsModel){
					for (let i = 0; i < orderProductsModel.count; i++){
						let categoryId = orderProductsModel.get(i).item.m_categoryId;
						if (categoryId === productEditor.softwareCategoryId){
							let id = orderProductsModel.get(i).item.m_id;
							if (id !== "" && productItem.m_id !== id){
								excludeDocumentIdFilter.m_filterValue = id;
								excludesGroup.m_fieldFilters.addElement(excludeDocumentIdFilter.copyMe());
							}
						}
					}
				}
				softwareCollection.filter.addGroupFilter(excludesGroup)
				
				softwareCollection.updateModel();
			}
		}
	}
	
	CustomScrollbar {
		id: scrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: flickable.top
		anchors.bottom: flickable.bottom
		secondSize: Style.marginM
		targetItem: flickable
		visible: productEditor.visible
	}
	
	Flickable {
		id: flickable

		anchors.top: parent.top
		anchors.topMargin: Style.marginM
		anchors.left: parent.left
		anchors.leftMargin: Style.marginM
		anchors.right: scrollbar.left
		anchors.rightMargin: Style.marginM
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginM

		contentWidth: width
		contentHeight: contentColumn.height

		boundsBehavior: Flickable.StopAtBounds
		clip: true
		
		Column {
			id: contentColumn
			width: flickable.width
			spacing: Style.marginM

			GroupElementView {
				width: contentColumn.width

				ElementView {
					id: segmentedElementView;
					width: parent.width;
					name: qsTr("Product Category");
					description: qsTr("Please select the product category you want to create");
					visible: productEditor.index === -1;
					
					property Button softwareProductButton;
					property Button hardwareProductButton;
					
					property int selectedIndex: 0;
					
					bottomComp: segmentedElementView.selectedIndex >= 0 ? selectedComp : undefined;
					
					Component {
						id: selectedComp;
						
						BaseText {
							font.family: Style.fontFamilyBold;
							text: qsTr("Currently selected: ") + ((segmentedElementView.selectedIndex == 0) ? "Software" : "Hardware");
						}
					}
					
					controlComp: Component {
						SegmentedButton {
							anchors.centerIn: parent;
							height: 40;
							selectedIndex: 0;
							isExclusive: true;
							
							onSelectedIndexChanged: {
								productEditor.clearProduct();
								
								if (selectedIndex == 0){
									productEditor.setSoftware();
								}
								else if (selectedIndex == 1){
									productEditor.setHardware();
								}
								
								segmentedElementView.selectedIndex = selectedIndex;
							}
							
							Button {
								id: softwareProductButton;
								
								anchors.verticalCenter: parent.verticalCenter;
								
								checkable: true
								checked: true
								
								iconSource: "../../../../" + Style.getIconPath("Icons/Key", Icon.State.On, Icon.Mode.Normal);
								text: qsTr("Software");
								
								Component.onCompleted: {
									segmentedElementView.softwareProductButton = softwareProductButton;
								}
							}
							
							Button {
								id: hardwareProductButton;
								
								anchors.verticalCenter: parent.verticalCenter;
								
								checkable: true
								checked: false;
								
								iconSource: "../../../../" + Style.getIconPath("Icons/Sensor", Icon.State.On, Icon.Mode.Normal);
								
								text: qsTr("Hardware");
								
								Component.onCompleted: {
									segmentedElementView.hardwareProductButton = hardwareProductButton;
								}
							}
						}
					}
				}
				
				ComboBoxElementView {
					id: productCB;
					
					width: parent.width;
					controlWidth: 500;
					
					name: qsTr("Product");
					nameId: OrderedProductTypeMetaInfo.s_productName;
					
					onCurrentIndexChanged: {
						productCB.bottomComp = productCB.currentIndex < 0 ? productErrorComp : undefined;
						
						productEditor.updateProductModel();
					}
					
					Component {
						id: productErrorComp;
						
						Text {
							id: selectProductText;
							
							text: qsTr("Please select a product");
							color: Style.errorTextColor;
							font.family: Style.fontFamily;
							font.pixelSize: Style.fontSizeM;
						}
					}
				}
			} // GroupElementView

			Loader {
				id: contentLoader
				width: contentColumn.width
				visible: productCB.currentIndex >= 0
				onLoaded: {
					contentLoader.height = item.height
				}
				
				Connections {
					target: contentLoader.item
					function onHeightChanged(){
						contentLoader.height = target.height
					}
				}
			}
		}
	} // Flickable
	
	function clearProduct(){
		if (productItem){
			productItem.m_productUuid = ''
			productItem.m_licenseUuid = ''
			productItem.m_categoryId = ''
			productItem.m_licenseId = ''
			productItem.m_productName = ''
			productItem.m_licenseName = ''
			productItem.m_serialNumber = ''
			productItem.m_inUse = false
			productItem.m_isNew = false
			productItem.m_expiration = ''
			productItem.m_macAddress = ''
		}
	}
	
	Component {
		id: hardwareProductComponent;
		HardwareProductEditor {
			productIndex: productEditor.index;
			model: productEditor.productItem;
			orderProductsModel: productEditor.orderProductsModel;
			onInstanceCountChanged: {
				productEditor.instanceCount = instanceCount
			}
		}
	}
	
	Component {
		id: softwareProductComponent;
		SoftwareProductEditor {
			productIndex: productEditor.index;
			model: productEditor.productItem;
			orderProductsModel: productEditor.orderProductsModel;
			onInstanceCountChanged: {
				productEditor.instanceCount = instanceCount
			}
		}
	}
	
	Component {
		id: treeItemModelComp;
		TreeItemModel {}
	}
	
	function started(){
		if (!productItem){
			return;
		}
		
		productEditor.blockUpdatingModel = true;
		
		if (productItem.m_categoryId === productEditor.softwareCategoryId){
			productEditor.setSoftware();
		}
		else if (productItem.m_categoryId  === productEditor.hardwareCategoryId){
			productEditor.setHardware();
		}
		else{
			console.error("Unknown product type:", productItem.m_categoryId);
			return;
		}
		
		for (let i = 0; i < productCB.model.getItemsCount(); i++){
			let id = productCB.model.getData(OrderedProductTypeMetaInfo.s_id, i);
			if (id === productItem.m_productUuid){
				productCB.currentIndex = i;
				break;
			}
		}
		
		if (productCB.currentIndex == -1){
			console.error("Unable to edit product. Error: This product not found!", productItem.m_categoryId);
			return;
		}
		
		productEditor.blockUpdatingModel = false;
	}
	
	Loading {
		id: loading;
		anchors.fill: parent;
		visible: false;
		color: Style.backgroundColor2;
	}
}//Container


