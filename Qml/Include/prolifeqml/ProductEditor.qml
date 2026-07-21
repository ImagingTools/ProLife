import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import prolifeOrdersSdl 1.0
import prolifeLicensesSdl 1.0
import prolifeSensorsSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

/**
 * ProductEditor — order product dialog body.
 *
 * Chrome (always visible; read-only when editing an existing line):
 *   [Software|Hardware]  |  [Link existing|Create new]  |  Qty
 *
 * Stage:
 *   Create  → SoftwareEditor / DeviceEditor (editable)
 *   Link    → same editors, read-only, filled from the linked instance
 *             (+ compact instance picker while adding a new linked line)
 */
Item {
	id: productEditor

	property TreeItemModel licensesModel: TreeItemModel {}
	property TreeItemModel productsModel: TreeItemModel {}
	property BaseModel orderProductsModel: BaseModel {}

	property TreeItemModel devicesModel: TreeItemModel {}
	property TreeItemModel softwaresModel: TreeItemModel {}

	property string orderId
	property string orderUuid

	property OrderedProduct productItem
	property int index: -1
	property int instanceCount: 1

	property string softwareCategoryId: "Software"
	property string hardwareCategoryId: "Hardware"

	property bool isSoftware: true
	property bool isCreateMode: false
	property int batchQuantity: 1
	property bool canCreateNew: true

	readonly property bool isEditing: productEditor.index >= 0
	readonly property bool chromeLocked: productEditor.isEditing
	// Quantity is relevant for create; still shown (disabled) when editing a created line.
	readonly property bool showQuantity: productEditor.isCreateMode

	CollectionDataProvider {
		id: softwareCollection
		commandId: ProlifeLicensesSdlCommandIds.s_softwareProductsList
		fields: [
			SoftwareProductItemTypeMetaInfo.s_id,
			SoftwareProductItemTypeMetaInfo.s_name,
			SoftwareProductItemTypeMetaInfo.s_productName,
			SoftwareProductItemTypeMetaInfo.s_licenseUuid,
			SoftwareProductItemTypeMetaInfo.s_licenseId,
			SoftwareProductItemTypeMetaInfo.s_licenseName,
			SoftwareProductItemTypeMetaInfo.s_serialNumber,
			SoftwareProductItemTypeMetaInfo.s_productUuid,
			SoftwareProductItemTypeMetaInfo.s_expiration,
			SoftwareProductItemTypeMetaInfo.s_inUse
		]
		onCollectionModelChanged: {
			softwareLinkCombo.model = collectionModel
			productEditor.updateLinkComboPlaceholders()
			if (contentLoader.item) {
				contentLoader.item.softwaresModel = collectionModel
				if (contentLoader.item.doUpdateGui)
					contentLoader.item.doUpdateGui()
			}
			productEditor.syncLinkComboSelection()
		}
		onStateChanged: {
			loading.visible = state === "Loading"
		}
	}

	CollectionDataProvider {
		id: deviceCollection
		commandId: ProlifeSensorsSdlCommandIds.s_devicesList
		fields: [
			DeviceItemTypeMetaInfo.s_id,
			DeviceItemTypeMetaInfo.s_name,
			DeviceItemTypeMetaInfo.s_deviceType,
			DeviceItemTypeMetaInfo.s_orderUuid,
			DeviceItemTypeMetaInfo.s_productUuid,
			DeviceItemTypeMetaInfo.s_licenseUuid,
			DeviceItemTypeMetaInfo.s_macAddress,
			DeviceItemTypeMetaInfo.s_serialNumber,
			DeviceItemTypeMetaInfo.s_productName,
			DeviceItemTypeMetaInfo.s_licenseId,
			DeviceItemTypeMetaInfo.s_licenseName
		]
		onCollectionModelChanged: {
			deviceLinkCombo.model = collectionModel
			productEditor.updateLinkComboPlaceholders()
			if (contentLoader.item) {
				contentLoader.item.devicesModel = collectionModel
				if (contentLoader.item.doUpdateGui)
					contentLoader.item.doUpdateGui()
			}
			productEditor.syncLinkComboSelection()
		}
		onStateChanged: {
			loading.visible = state === "Loading"
		}
	}

	FieldFilter {
		id: excludeDocumentIdFilter
		m_fieldId: "DocumentId"
		m_filterValueType: "String"
		m_filterOperations: ["Not", "Equal"]
	}

	GroupFilter {
		id: excludesGroup
		m_logicalOperation: deviceCollection.filter.logicalOperation.AND
		Component.onCompleted: {
			if (!hasFieldFilters()) {
				emplaceFieldFilters()
			}
		}
	}

	GroupFilter {
		id: orderGroupFilter
		m_logicalOperation: deviceCollection.filter.logicalOperation.OR

		Component.onCompleted: {
			if (!hasFieldFilters()) {
				emplaceFieldFilters()
			}
			m_fieldFilters.addElement(orderUuidFilter)
			m_fieldFilters.addElement(emptyOrderFilter)
			deviceCollection.filter.addGroupFilter(orderGroupFilter)
			softwareCollection.filter.addGroupFilter(orderGroupFilter)
		}
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

	Component.onCompleted: {
		productEditor.canCreateNew = PermissionsController.checkPermission("AddLicense")
			|| PermissionsController.checkPermission("AddSensor")
	}

	function applyCategoryToProduct() {
		if (!productItem)
			return
		productItem.m_categoryId = productEditor.isSoftware
			? productEditor.softwareCategoryId
			: productEditor.hardwareCategoryId
	}

	function setSoftware() {
		productEditor.isSoftware = true
		productEditor.applyCategoryToProduct()
		contentLoader.sourceComponent = undefined
		contentLoader.sourceComponent = softwareProductComponent
		productEditor.refreshLinkModels()
		productEditor.syncChromeToContent()
	}

	function setHardware() {
		productEditor.isSoftware = false
		productEditor.applyCategoryToProduct()
		contentLoader.sourceComponent = undefined
		contentLoader.sourceComponent = hardwareProductComponent
		productEditor.refreshLinkModels()
		productEditor.syncChromeToContent()
	}

	function refreshLinkModels() {
		deviceCollection.filter.removeGroupFilter(excludesGroup)
		softwareCollection.filter.removeGroupFilter(excludesGroup)

		if (!excludesGroup.hasFieldFilters()) {
			excludesGroup.emplaceFieldFilters()
		}

		while (excludesGroup.m_fieldFilters && excludesGroup.m_fieldFilters.count > 0) {
			excludesGroup.m_fieldFilters.remove(0)
		}

		if (orderProductsModel) {
			let categoryId = productEditor.isSoftware
				? productEditor.softwareCategoryId
				: productEditor.hardwareCategoryId
			for (let i = 0; i < orderProductsModel.count; i++) {
				let item = orderProductsModel.get(i).item
				if (item.m_categoryId === categoryId) {
					let id = item.m_id
					if (id !== "" && (!productItem || productItem.m_id !== id)) {
						excludeDocumentIdFilter.m_filterValue = id
						excludesGroup.m_fieldFilters.addElement(excludeDocumentIdFilter.copyMe())
					}
				}
			}
		}

		if (productEditor.isSoftware) {
			softwareCollection.filter.addGroupFilter(excludesGroup)
			softwareCollection.updateModel()
		}
		else {
			deviceCollection.filter.addGroupFilter(excludesGroup)
			deviceCollection.updateModel()
		}
	}

	readonly property bool showLinkPicker: !productEditor.isCreateMode && !productEditor.chromeLocked
	property bool hasLinkedSelection: false

	function syncChromeToContent() {
		if (!contentLoader.item)
			return
		contentLoader.item.createMode = productEditor.isCreateMode
		contentLoader.item.batchQuantity = productEditor.batchQuantity
		contentLoader.item.orderUuid = productEditor.orderUuid
		contentLoader.item.orderProductsModel = productEditor.orderProductsModel
		contentLoader.item.productIndex = productEditor.index
		contentLoader.item.chromeLocked = productEditor.chromeLocked
		contentLoader.item.hasLinkedSelection = productEditor.hasLinkedSelection
			|| productEditor.chromeLocked
		if (productEditor.isSoftware) {
			contentLoader.item.softwaresModel = softwareCollection.collectionModel
			softwareLinkCombo.model = softwareCollection.collectionModel
		}
		else {
			contentLoader.item.devicesModel = deviceCollection.collectionModel
			deviceLinkCombo.model = deviceCollection.collectionModel
		}
		if (contentLoader.item.doUpdateGui)
			contentLoader.item.doUpdateGui()
		productEditor.instanceCount = contentLoader.item.instanceCount
		productEditor.syncLinkComboSelection()
	}

	function linkModelIsEmpty(combo) {
		if (!combo || !combo.model)
			return true
		if (combo.model.getItemsCount !== undefined)
			return combo.model.getItemsCount() === 0
		if (combo.model.count !== undefined)
			return combo.model.count === 0
		return true
	}

	function updateLinkComboPlaceholders() {
		let softwareEmpty = productEditor.linkModelIsEmpty(softwareLinkCombo)
		softwareLinkCombo.placeHolderText = softwareEmpty
			? qsTr("No licenses available")
			: qsTr("Select…")
		softwareLinkCombo.changeable = productEditor.showLinkPicker && !softwareEmpty
		softwareLinkCombo.enabled = productEditor.showLinkPicker && !softwareEmpty

		let deviceEmpty = productEditor.linkModelIsEmpty(deviceLinkCombo)
		deviceLinkCombo.placeHolderText = deviceEmpty
			? qsTr("No devices available")
			: qsTr("Select…")
		deviceLinkCombo.changeable = productEditor.showLinkPicker && !deviceEmpty
		deviceLinkCombo.enabled = productEditor.showLinkPicker && !deviceEmpty
	}

	function syncLinkComboSelection() {
		productEditor.updateLinkComboPlaceholders()
		if (!productEditor.showLinkPicker || !productItem)
			return
		if (productEditor.isSoftware) {
			softwareLinkCombo.currentIndex = -1
			if (!softwareLinkCombo.model)
				return
			for (let i = 0; i < softwareLinkCombo.model.getItemsCount(); i++) {
				let id = softwareLinkCombo.model.getData(SoftwareProductItemTypeMetaInfo.s_id, i)
				if (id === productItem.m_id) {
					softwareLinkCombo.currentIndex = i
					break
				}
			}
		}
		else {
			deviceLinkCombo.currentIndex = -1
			if (!deviceLinkCombo.model)
				return
			for (let i = 0; i < deviceLinkCombo.model.getItemsCount(); i++) {
				let id = deviceLinkCombo.model.getData(DeviceItemTypeMetaInfo.s_id, i)
				if (id === productItem.m_id) {
					deviceLinkCombo.currentIndex = i
					break
				}
			}
		}
	}

	function applyChromeLinkSelection() {
		if (!contentLoader.item || !productEditor.showLinkPicker)
			return
		let idx = productEditor.isSoftware ? softwareLinkCombo.currentIndex : deviceLinkCombo.currentIndex
		if (idx < 0) {
			productEditor.hasLinkedSelection = false
			if (contentLoader.item.clearLinkedSelection)
				contentLoader.item.clearLinkedSelection()
			contentLoader.item.hasLinkedSelection = false
			return
		}
		productEditor.hasLinkedSelection = true
		contentLoader.item.hasLinkedSelection = true
		if (contentLoader.item.selectLinkedIndex)
			contentLoader.item.selectLinkedIndex(idx)
		productEditor.instanceCount = contentLoader.item.instanceCount
	}

	function clearProductFields() {
		if (!productItem)
			return
		if (productEditor.index < 0) {
			productItem.m_id = UuidGenerator.generateUUID()
		}
		productItem.m_productUuid = ""
		productItem.m_licenseUuid = ""
		productItem.m_licenseId = ""
		productItem.m_productName = ""
		productItem.m_licenseName = ""
		productItem.m_serialNumber = ""
		productItem.m_inUse = false
		productItem.m_expiration = ""
		productItem.m_macAddress = ""
		productItem.m_isMultiple = false
		productItem.m_productCount = 0
		productItem.m_isNew = productEditor.isCreateMode
		productEditor.hasLinkedSelection = false
		productEditor.applyCategoryToProduct()
	}

	function clearProduct() {
		productEditor.clearProductFields()
	}

	function started() {
		if (!productItem)
			return

		let category = productItem.m_categoryId
		if (category === productEditor.hardwareCategoryId) {
			productEditor.isSoftware = false
		}
		else {
			productEditor.isSoftware = true
			if (category !== productEditor.softwareCategoryId) {
				console.warn("Unknown product type, fallback to Software:", category)
			}
		}

		if (productEditor.index >= 0) {
			// Editing: keep original category / source; chrome is locked.
			productEditor.isCreateMode = productItem.m_isNew === true
			// Existing line always has data to show in the editor stage.
			productEditor.hasLinkedSelection = true
		}
		else {
			let canAdd = productEditor.isSoftware
				? PermissionsController.checkPermission("AddLicense")
				: PermissionsController.checkPermission("AddSensor")
			productEditor.isCreateMode = canAdd
			productItem.m_isNew = productEditor.isCreateMode
			productEditor.hasLinkedSelection = false
			if (productItem.m_id === "")
				productItem.m_id = UuidGenerator.generateUUID()
		}

		productEditor.applyCategoryToProduct()

		if (productEditor.isSoftware)
			productEditor.setSoftware()
		else
			productEditor.setHardware()
	}

	// --- Chrome: category | source | [link combo] | Quantity ---------------------------------

	Rectangle {
		id: chromeBar
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Style.marginL
		height: Style.controlHeightM + Style.marginXL * 2
		radius: Style.radiusL
		color: Style.baseColor
		border.width: 1
		border.color: Style.borderColor

		Row {
			id: leftChrome
			anchors.left: parent.left
			anchors.leftMargin: Style.marginXL
			anchors.verticalCenter: parent.verticalCenter
			spacing: Style.marginXL

			SegmentedButton {
				id: categorySegmented
				anchors.verticalCenter: parent.verticalCenter
				height: Style.controlHeightM
				isExclusive: true
				selectedIndex: productEditor.isSoftware ? 0 : 1
				objectName: "ProductCategorySegmented"
				enabled: !productEditor.chromeLocked
				opacity: enabled ? 1.0 : 0.55

				onSelectedIndexChanged: {
					if (productEditor.chromeLocked)
						return
					if ((selectedIndex === 0) === productEditor.isSoftware)
						return
					productEditor.clearProductFields()
					softwareLinkCombo.currentIndex = -1
					deviceLinkCombo.currentIndex = -1
					productEditor.hasLinkedSelection = false
					if (selectedIndex === 0)
						productEditor.setSoftware()
					else
						productEditor.setHardware()
				}

				Button {
					objectName: "SoftwareButton"
					anchors.verticalCenter: parent.verticalCenter
					checkable: true
					checked: productEditor.isSoftware
					enabled: !productEditor.chromeLocked
					iconSource: "../../../../" + Style.getIconPath("Icons/Key", Icon.State.On, Icon.Mode.Normal)
					text: qsTr("Software")
				}

				Button {
					objectName: "HardwareButton"
					anchors.verticalCenter: parent.verticalCenter
					checkable: true
					checked: !productEditor.isSoftware
					enabled: !productEditor.chromeLocked
					iconSource: "../../../../" + Style.getIconPath("Icons/Sensor", Icon.State.On, Icon.Mode.Normal)
					text: qsTr("Hardware")
				}
			}

			Rectangle {
				anchors.verticalCenter: parent.verticalCenter
				width: 1
				height: Style.controlHeightM
				color: Style.borderColor
			}

			SegmentedButton {
				id: sourceSegmented
				anchors.verticalCenter: parent.verticalCenter
				height: Style.controlHeightM
				isExclusive: true
				selectedIndex: productEditor.isCreateMode ? 1 : 0
				objectName: "ProductSourceSegmented"
				visible: productEditor.canCreateNew || productEditor.isEditing
				enabled: !productEditor.chromeLocked && productEditor.canCreateNew
				opacity: enabled ? 1.0 : 0.55

				onSelectedIndexChanged: {
					if (productEditor.chromeLocked)
						return
					let create = selectedIndex === 1
					if (productEditor.isCreateMode === create)
						return
					productEditor.isCreateMode = create
					productEditor.clearProductFields()
					softwareLinkCombo.currentIndex = -1
					deviceLinkCombo.currentIndex = -1
					productEditor.hasLinkedSelection = false
					productEditor.syncChromeToContent()
					if (!create)
						productEditor.refreshLinkModels()
				}

				Button {
					anchors.verticalCenter: parent.verticalCenter
					checkable: true
					checked: !productEditor.isCreateMode
					enabled: !productEditor.chromeLocked
					text: qsTr("Link existing")
				}

				Button {
					anchors.verticalCenter: parent.verticalCenter
					checkable: true
					checked: productEditor.isCreateMode
					enabled: !productEditor.chromeLocked
					text: qsTr("Create new")
				}
			}
		}

		// Link combo on the same top bar — compact, with a small label.
		Row {
			id: linkPickerSlot
			anchors.left: leftChrome.right
			anchors.leftMargin: Style.marginXL
			anchors.verticalCenter: parent.verticalCenter
			spacing: Style.marginS
			visible: productEditor.showLinkPicker
			height: Style.controlHeightM

			Text {
				anchors.verticalCenter: parent.verticalCenter
				text: productEditor.isSoftware ? qsTr("License") : qsTr("Device")
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeS
			}

			ComboBox {
				id: softwareLinkCombo
				objectName: "ProductLicenseCombo"
				width: 300
				height: Style.controlHeightM
				anchors.verticalCenter: parent.verticalCenter
				visible: productEditor.isSoftware
				enabled: productEditor.showLinkPicker
				nameId: SoftwareProductItemTypeMetaInfo.s_productName
				placeHolderText: qsTr("Select…")

				onModelChanged: {
					productEditor.updateLinkComboPlaceholders()
				}

				onCurrentIndexChanged: {
					if (!productEditor.showLinkPicker || !productEditor.isSoftware)
						return
					productEditor.applyChromeLinkSelection()
				}
			}

			ComboBox {
				id: deviceLinkCombo
				objectName: "ProductDeviceCombo"
				width: 300
				height: Style.controlHeightM
				anchors.verticalCenter: parent.verticalCenter
				visible: !productEditor.isSoftware
				enabled: productEditor.showLinkPicker
				nameId: DeviceItemTypeMetaInfo.s_productName
				placeHolderText: qsTr("Select…")

				onModelChanged: {
					productEditor.updateLinkComboPlaceholders()
				}

				onCurrentIndexChanged: {
					if (!productEditor.showLinkPicker || productEditor.isSoftware)
						return
					productEditor.applyChromeLinkSelection()
				}
			}
		}

		Row {
			id: qtyRow
			anchors.right: parent.right
			anchors.rightMargin: Style.marginXL
			anchors.verticalCenter: parent.verticalCenter
			spacing: Style.marginM
			visible: productEditor.showQuantity

			Text {
				anchors.verticalCenter: parent.verticalCenter
				text: qsTr("Quantity")
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
			}

			SpinBox {
				id: qtySpin
				objectName: "ProductQuantitySpinBox"
				width: 96
				from: 1
				to: 10
				stepSize: 1
				startValue: 1
				enabled: !productEditor.chromeLocked
				opacity: enabled ? 1.0 : 0.55
				onValueChanged: {
					productEditor.batchQuantity = value
					productEditor.syncChromeToContent()
					if (contentLoader.item)
						productEditor.instanceCount = contentLoader.item.instanceCount
				}
			}
		}
	}

	// --- Stage -------------------------------------------------------------------------------

	Item {
		id: stage
		anchors.top: chromeBar.bottom
		anchors.topMargin: Style.marginL
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.leftMargin: Style.marginL
		anchors.rightMargin: Style.marginL
		anchors.bottomMargin: Style.marginL
		clip: true

		// Background fill sits behind the content.
		Rectangle {
			anchors.fill: parent
			radius: Style.radiusL
			color: Style.baseColor
		}

		Loader {
			id: contentLoader
			anchors.fill: parent
			anchors.margins: 1
			onLoaded: {
				productEditor.syncChromeToContent()
			}
		}

		// Frame is drawn ON TOP of the content: the page fills a square 1px inset that
		// does not follow the rounded corners, so if the border were behind it the
		// content would cover the top/corner border. A transparent, border-only
		// rectangle keeps the frame visible without blocking input to the content.
		Rectangle {
			anchors.fill: parent
			radius: Style.radiusL
			color: "transparent"
			border.width: 1
			border.color: Style.borderColor
		}
	}

	Component {
		id: hardwareProductComponent
		HardwareProductEditor {
			anchors.fill: parent
			productIndex: productEditor.index
			model: productEditor.productItem
			orderProductsModel: productEditor.orderProductsModel
			orderUuid: productEditor.orderUuid
			createMode: productEditor.isCreateMode
			batchQuantity: productEditor.batchQuantity
			chromeLocked: productEditor.chromeLocked
			hasLinkedSelection: productEditor.hasLinkedSelection || productEditor.chromeLocked
			onInstanceCountChanged: {
				productEditor.instanceCount = instanceCount
			}
		}
	}

	Component {
		id: softwareProductComponent
		SoftwareProductEditor {
			anchors.fill: parent
			productIndex: productEditor.index
			model: productEditor.productItem
			orderProductsModel: productEditor.orderProductsModel
			orderUuid: productEditor.orderUuid
			createMode: productEditor.isCreateMode
			batchQuantity: productEditor.batchQuantity
			chromeLocked: productEditor.chromeLocked
			hasLinkedSelection: productEditor.hasLinkedSelection || productEditor.chromeLocked
			onInstanceCountChanged: {
				productEditor.instanceCount = instanceCount
			}
		}
	}

	Loading {
		id: loading
		anchors.fill: parent
		visible: false
		background.color: Style.baseColor
	}
}
