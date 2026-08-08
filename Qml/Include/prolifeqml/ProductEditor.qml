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
			if (contentLoader.item) {
				contentLoader.item.softwaresModel = collectionModel
				if (contentLoader.item.doUpdateGui)
					contentLoader.item.doUpdateGui()
			}
			productEditor.syncLinkSelection()
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
			DeviceItemTypeMetaInfo.s_productUuid,
			DeviceItemTypeMetaInfo.s_licenseUuid,
			DeviceItemTypeMetaInfo.s_macAddress,
			DeviceItemTypeMetaInfo.s_serialNumber,
			DeviceItemTypeMetaInfo.s_productName,
			DeviceItemTypeMetaInfo.s_licenseId,
			DeviceItemTypeMetaInfo.s_licenseName
		]
		onCollectionModelChanged: {
			if (contentLoader.item) {
				contentLoader.item.devicesModel = collectionModel
				if (contentLoader.item.doUpdateGui)
					contentLoader.item.doUpdateGui()
			}
			productEditor.syncLinkSelection()
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
		}
		else {
			contentLoader.item.devicesModel = deviceCollection.collectionModel
		}
		if (contentLoader.item.doUpdateGui)
			contentLoader.item.doUpdateGui()
		productEditor.instanceCount = contentLoader.item.instanceCount
		productEditor.syncLinkSelection()
	}

	// --- Link picker: server-side filterable single-select over the same collection ---

	readonly property TreeItemModel linkSourceModel: productEditor.isSoftware
		? softwareCollection.collectionModel
		: deviceCollection.collectionModel

	readonly property string linkIdField: productEditor.isSoftware
		? SoftwareProductItemTypeMetaInfo.s_id
		: DeviceItemTypeMetaInfo.s_id

	readonly property bool linkSourceEmpty: !productEditor.linkSourceModel
		|| productEditor.linkSourceModel.getItemsCount() === 0

	property string linkedItemId: ""
	property string linkedItemName: ""
	property string linkedItemDetail: ""

	readonly property string emptyValueText: "—"

	// Fields shown in a picker row, and the ones the typed text is matched against.
	// They are collection field ids: the collection controller maps them onto the
	// stored ones, the same way it does for the collection views.
	readonly property var softwareLinkFields: [
		SoftwareProductItemTypeMetaInfo.s_id,
		SoftwareProductItemTypeMetaInfo.s_productName,
		SoftwareProductItemTypeMetaInfo.s_serialNumber,
		SoftwareProductItemTypeMetaInfo.s_licenseName,
		SoftwareProductItemTypeMetaInfo.s_licenseId
	]
	readonly property var hardwareLinkFields: [
		DeviceItemTypeMetaInfo.s_id,
		DeviceItemTypeMetaInfo.s_productName,
		DeviceItemTypeMetaInfo.s_licenseId,
		DeviceItemTypeMetaInfo.s_macAddress
	]
	readonly property var softwareTextFilterFields: [
		SoftwareProductItemTypeMetaInfo.s_productName,
		SoftwareProductItemTypeMetaInfo.s_serialNumber,
		SoftwareProductItemTypeMetaInfo.s_licenseName,
		SoftwareProductItemTypeMetaInfo.s_licenseId
	]
	readonly property var hardwareTextFilterFields: [
		DeviceItemTypeMetaInfo.s_productName,
		DeviceItemTypeMetaInfo.s_licenseId,
		DeviceItemTypeMetaInfo.s_macAddress
	]

	// Runs against the same list command as the collection behind the editor, with the
	// same group filters, so a picked id is always a row of linkSourceModel.
	FilterableSelectCollectionDataProvider {
		id: linkSelectProvider

		multiSelect: false
		commandId: productEditor.isSoftware
			? ProlifeLicensesSdlCommandIds.s_softwareProductsList
			: ProlifeSensorsSdlCommandIds.s_devicesList
		fields: productEditor.isSoftware
			? productEditor.softwareLinkFields
			: productEditor.hardwareLinkFields
		idField: productEditor.linkIdField
		titleField: productEditor.isSoftware
			? SoftwareProductItemTypeMetaInfo.s_productName
			: DeviceItemTypeMetaInfo.s_productName
		textFilterFieldIds: productEditor.isSoftware
			? productEditor.softwareTextFilterFields
			: productEditor.hardwareTextFilterFields
		groupFilters: [orderGroupFilter, excludesGroup]
	}

	// Popup rows are looked up by id on every repaint, so the row index is cached.
	readonly property var linkIndexById: {
		let map = ({})
		if (productEditor.linkSourceModel) {
			for (let i = 0; i < productEditor.linkSourceModel.getItemsCount(); i++) {
				map[String(productEditor.linkSourceModel.getData(productEditor.linkIdField, i))] = i
			}
		}
		return map
	}

	function indexOfLinkedId(id) {
		if (id === "") {
			return -1
		}
		let index = productEditor.linkIndexById[String(id)]
		return index === undefined ? -1 : index
	}

	// A picker row carries every requested collection field, keyed by its field id.
	function itemValue(item, fieldId) {
		if (!item || !item.values) {
			return ""
		}
		let value = item.values[fieldId]
		return value === undefined || value === null ? "" : String(value)
	}

	function itemTitle(item, fallback) {
		let productName = productEditor.itemValue(item, productEditor.isSoftware
			? SoftwareProductItemTypeMetaInfo.s_productName
			: DeviceItemTypeMetaInfo.s_productName)
		return productName !== "" ? productName : fallback
	}

	// A picker row shows the product name and two identifiers. The compact one has a
	// predictable length and shares the title line; the wide one - the article, which
	// carries a name and a number - gets a line of its own.
	readonly property string compactFieldLabel: productEditor.isSoftware ? qsTr("Software-ID") : qsTr("MAC Address")
	readonly property string wideFieldLabel: qsTr("Article")

	function itemCompactField(item) {
		let value = productEditor.itemMainId(item)
		return value !== "" ? value : productEditor.emptyValueText
	}

	function itemWideField(item) {
		let value = productEditor.isSoftware
			? productEditor.itemArticle(item)
			: productEditor.itemValue(item, DeviceItemTypeMetaInfo.s_licenseId)
		return value !== "" ? value : productEditor.emptyValueText
	}

	// Second line of the chrome chip: the identifier that tells two instances of the
	// same product apart.
	function itemMainId(item) {
		return productEditor.isSoftware
			? productEditor.itemValue(item, SoftwareProductItemTypeMetaInfo.s_serialNumber)
			: productEditor.itemValue(item, DeviceItemTypeMetaInfo.s_macAddress)
	}

	function pushDetailParam(params, name, value) {
		params.push({
			"name": name,
			"value": value !== "" ? value : productEditor.emptyValueText
		})
	}

	// A software instance carries the license name and its article number separately.
	function itemArticle(item) {
		let articleName = productEditor.itemValue(item, SoftwareProductItemTypeMetaInfo.s_licenseName)
		let articleId = productEditor.itemValue(item, SoftwareProductItemTypeMetaInfo.s_licenseId)
		if (articleId === "") {
			return articleName
		}
		return articleName !== "" ? articleName + " (" + articleId + ")" : articleId
	}

	Connections {
		target: linkSelectProvider

		function onSelectionChanged() {
			if (productEditor.linkedItemId !== "" && linkSelectProvider.getSelectedIds().length === 0) {
				productEditor.clearLinkSelection()
			}
		}
	}

	function syncLinkSelection() {
		if (!productEditor.showLinkPicker || !productItem) {
			return
		}
		let index = productEditor.indexOfLinkedId(productItem.m_id)
		if (index < 0) {
			productEditor.linkedItemId = ""
			productEditor.linkedItemName = ""
			productEditor.linkedItemDetail = ""
			return
		}
		productEditor.linkedItemId = productItem.m_id
		productEditor.linkedItemName = productItem.m_productName !== ""
			? productItem.m_productName
			: productItem.m_id
		productEditor.linkedItemDetail = productEditor.isSoftware
			? productItem.m_serialNumber
			: productItem.m_macAddress
	}

	// Opened below the control that triggered it, kept inside the view.
	function openLinkPopup(anchorItem) {
		linkSelectButton.isOpen = true
		let point = anchorItem.mapToItem(null, 0, anchorItem.height + Style.marginXS)
		let popupWidth = productEditor.linkPopupItemWidth + 2 * Style.marginL
		let maxX = ModalDialogManager.activeView.width - popupWidth - Style.marginL
		ModalDialogManager.openDialog(linkSelectComp, {
			"x": Math.max(Style.marginL, Math.min(point.x, maxX)),
			"y": point.y
		})
	}

	function clearLinkSelection() {
		productEditor.linkedItemId = ""
		productEditor.linkedItemName = ""
		productEditor.linkedItemDetail = ""
		productEditor.applyChromeLinkSelection()
	}

	function applyChromeLinkSelection() {
		if (!contentLoader.item || !productEditor.showLinkPicker)
			return
		let idx = productEditor.indexOfLinkedId(productEditor.linkedItemId)
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

	// Row geometry of the instance picker: two compact text lines, wide enough for the
	// product name over its two identifiers.
	readonly property int linkPopupItemWidth: Math.max(Style.sizeHintM,
		Math.min(Style.sizeHintXL, ModalDialogManager.activeView.width - 4 * Style.marginXL))
	readonly property int linkPopupItemHeight: 52

	Component {
		id: linkSelectComp

		FilterableSelectPopup {
			id: linkPopup

			dataProvider: linkSelectProvider
			itemWidth: productEditor.linkPopupItemWidth
			itemHeight: productEditor.linkPopupItemHeight
			maxVisibleItems: 6
			showSeparator: true
			// The picked instance is repeated as its own group above the list, so it
			// stays in sight while scrolling through the rest.
			showSelectedGroup: true
			maxSelectedGroupItems: 1
			filterPlaceholder: productEditor.isSoftware
				? qsTr("Search by product, Software-ID or article")
				: qsTr("Search by product, article or MAC address")
			preselectedIds: productEditor.linkedItemId !== "" ? [productEditor.linkedItemId] : []
			knownItems: productEditor.linkedItemId !== ""
				? [{ "id": productEditor.linkedItemId, "title": productEditor.linkedItemName }]
				: []

			delegate: Component {
				PopupMenuDelegate {
					id: linkDelegate
					width: ListView.view ? ListView.view.width : linkPopup.itemWidth
					height: linkPopup.itemHeight
					objectName: "FilterableSelectItem_" + model.index
					isSeparator: false
					text: ""

					property int rowIndex: model.index
					property var rowItem: linkPopup.getItem(linkDelegate.rowIndex)
					readonly property bool rowSelected: linkPopup.dataProvider
						? linkPopup.dataProvider.isItemSelected(linkPopup.getItemId(linkDelegate.rowIndex))
						: false

					// The decorator paints nothing: the row draws its own state below, so
					// selection and hover cannot come from two places at once.
					selected: false
					highlighted: false

					// Rows carry their own background and mouse handling: the content sits
					// above the delegate decorator, so its own hover/press never fires.
					Rectangle {
						anchors.fill: parent
						color: linkPopup.rowBackgroundColor(linkDelegate.rowIndex,
							rowMouseArea.containsMouse, linkDelegate.rowSelected)
					}

					Column {
						z: 10
						anchors.left: parent.left
						anchors.leftMargin: Style.marginM
						anchors.right: parent.right
						anchors.rightMargin: Style.marginM
						anchors.verticalCenter: parent.verticalCenter
						spacing: 2

						// Title line: the product name takes what is left of the row after
						// the short identifier, which is pinned to the right.
						Item {
							id: rowTitleLine
							width: parent.width
							height: rowTitle.height

							Row {
								id: rowCompactField
								anchors.right: parent.right
								anchors.verticalCenter: rowTitle.verticalCenter
								spacing: Style.marginXS

								Text {
									anchors.verticalCenter: parent.verticalCenter
									text: productEditor.compactFieldLabel
									color: Style.subtitleColor
									font.family: Style.fontFamily
									font.pixelSize: Style.fontSizeXS
								}

								Text {
									anchors.verticalCenter: parent.verticalCenter
									width: Math.min(implicitWidth, rowTitleLine.width * 0.4)
									text: productEditor.itemCompactField(linkDelegate.rowItem)
									color: Style.textColor
									font.family: Style.fontFamily
									font.pixelSize: Style.fontSizeS
									elide: Text.ElideRight
								}
							}

							Text {
								id: rowTitle
								anchors.left: parent.left
								anchors.right: rowCompactField.left
								anchors.rightMargin: Style.marginL
								text: productEditor.itemTitle(linkPopup.getItem(linkDelegate.rowIndex),
									linkPopup.getItemText(linkDelegate.rowIndex))
								color: Style.textColor
								font.family: Style.fontFamilyBold
								font.pixelSize: Style.fontSizeM
								elide: Text.ElideRight
							}
						}

						// The article gets the whole second line - it is the one value long
						// enough to run out of room in a shared column.
						Item {
							width: parent.width
							height: rowWideValue.height

							Text {
								id: rowWideLabel
								anchors.left: parent.left
								anchors.baseline: rowWideValue.baseline
								text: productEditor.wideFieldLabel
								color: Style.subtitleColor
								font.family: Style.fontFamily
								font.pixelSize: Style.fontSizeXS
							}

							Text {
								id: rowWideValue
								anchors.left: rowWideLabel.right
								anchors.leftMargin: Style.marginXS
								anchors.right: parent.right
								text: productEditor.itemWideField(linkDelegate.rowItem)
								color: Style.textColor
								font.family: Style.fontFamily
								font.pixelSize: Style.fontSizeS
								elide: Text.ElideRight
							}
						}
					}

					Rectangle {
						anchors.bottom: parent.bottom
						width: parent.width
						height: 1
						color: Style.borderColor
						opacity: 0.4
						visible: linkPopup.separatorVisible
					}

					MouseArea {
						id: rowMouseArea
						anchors.fill: parent
						z: 20
						hoverEnabled: true
						cursorShape: Qt.PointingHandCursor

						onEntered: {
							linkPopup.focusItem(linkDelegate.rowIndex)
						}

						onClicked: {
							linkPopup.handleItemClick(linkPopup.getItemId(linkDelegate.rowIndex), linkDelegate.rowIndex)
						}
					}
				}
			}

			onItemSelected: {
				let selectedItem = linkPopup.getItem(index)
				productEditor.linkedItemId = itemId
				productEditor.linkedItemName = productEditor.itemTitle(selectedItem,
					linkSelectProvider.getSelectedItemText(itemId))
				productEditor.linkedItemDetail = productEditor.itemMainId(selectedItem)
				productEditor.applyChromeLinkSelection()
			}

			Component.onDestruction: {
				linkSelectButton.isOpen = false
			}
		}
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
		productEditor.linkedItemId = ""
		productEditor.linkedItemName = ""
		productEditor.linkedItemDetail = ""
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

		// Instance picker on the same top bar — one drop-down trigger; picking and
		// clearing both happen inside the popup.
		Row {
			id: linkPickerSlot
			anchors.left: leftChrome.right
			anchors.leftMargin: Style.marginL
			anchors.right: parent.right
			anchors.rightMargin: Style.marginXL
			anchors.verticalCenter: parent.verticalCenter
			spacing: Style.marginS
			visible: productEditor.showLinkPicker
			height: Style.controlHeightM

			// Width left for the chip once the label had its share. The chip shrinks
			// into it instead of growing past the bar and under the segmented buttons.
			readonly property int availableWidth: Math.max(0, linkPickerSlot.width
				- (pickerLabel.visible ? pickerLabel.width + linkPickerSlot.spacing : 0))

			Text {
				id: pickerLabel
				anchors.verticalCenter: parent.verticalCenter
				visible: linkPickerSlot.width > Style.sizeHintXS
				text: productEditor.isSoftware ? qsTr("License") : qsTr("Device")
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeS
			}

			// Nothing picked yet — the same chip shape as the selected state, so the
			// control keeps its place and only its content changes.
			Rectangle {
				id: linkSelectButton
				objectName: "ProductLinkSelectButton"
				anchors.verticalCenter: parent.verticalCenter
				visible: productEditor.linkedItemId === ""
				height: Style.controlHeightM
				width: Math.min(linkPickerSlot.availableWidth,
					Style.marginM + selectIcon.width + Style.marginS + selectText.implicitWidth + Style.marginM)
				radius: Style.radiusL
				color: selectMouseArea.containsMouse ? Style.buttonHoverColor : Style.baseColor
				border.width: 1
				border.color: selectMouseArea.containsMouse ? Style.borderColor2 : Style.borderColor
				opacity: productEditor.linkSourceEmpty ? Style.opacityHigh : 1.0

				property bool isOpen: false

				MouseArea {
					id: selectMouseArea
					anchors.fill: parent
					hoverEnabled: true
					enabled: !productEditor.linkSourceEmpty
					cursorShape: Qt.PointingHandCursor

					onClicked: {
						productEditor.openLinkPopup(linkSelectButton)
					}
				}

				Image {
					id: selectIcon
					anchors.left: parent.left
					anchors.leftMargin: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					width: Style.iconSizeS
					height: Style.iconSizeS
					source: "../../../../" + Style.getIconPath("Icons/Link", Icon.State.On, Icon.Mode.Normal)
					sourceSize.width: width
					sourceSize.height: height
					opacity: productEditor.linkSourceEmpty ? Style.opacityLow : 1.0
				}

				Text {
					id: selectText
					anchors.left: selectIcon.right
					anchors.leftMargin: Style.marginS
					anchors.right: parent.right
					anchors.rightMargin: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					text: productEditor.linkSourceEmpty
						? (productEditor.isSoftware ? qsTr("No licenses available") : qsTr("No devices available"))
						: (productEditor.isSoftware ? qsTr("Select license") : qsTr("Select device"))
					color: productEditor.linkSourceEmpty ? Style.inactiveTextColor : Style.textColor
					font.family: Style.fontFamily
					font.pixelSize: Style.fontSizeM
					elide: Text.ElideRight
				}
			}

			// Picked — one chip carrying the product, its identifier and the unlink
			// action, so the selection reads as a single object instead of a button
			// with a second button next to it.
			Rectangle {
				id: linkSelectedChip
				objectName: "ProductLinkSelectedChip"
				anchors.verticalCenter: parent.verticalCenter
				visible: productEditor.linkedItemId !== ""
				height: Style.controlHeightM
				width: Math.min(linkPickerSlot.availableWidth, Style.marginM + chipName.implicitWidth
					+ (chipDetail.text !== "" ? Style.marginS + chipDetail.implicitWidth : 0)
					+ Style.marginS + chipClearButton.width + Style.marginM)
				radius: Style.radiusL
				color: chipMouseArea.containsMouse ? Style.buttonHoverColor : Style.selectedColor
				border.width: 1
				border.color: Style.borderColor

				MouseArea {
					id: chipMouseArea
					anchors.fill: parent
					hoverEnabled: true
					cursorShape: Qt.PointingHandCursor

					onClicked: {
						productEditor.openLinkPopup(linkSelectedChip)
					}
				}

				// The identifier keeps its share on the right, the name takes what is
				// left and elides - so a narrow bar shortens the name, not the id.
				Text {
					id: chipName
					anchors.left: parent.left
					anchors.leftMargin: Style.marginM
					anchors.right: chipDetail.left
					anchors.rightMargin: chipDetail.text !== "" ? Style.marginS : 0
					anchors.verticalCenter: parent.verticalCenter
					text: productEditor.linkedItemName
					color: Style.textColor
					font.family: Style.fontFamilyBold
					font.pixelSize: Style.fontSizeM
					elide: Text.ElideRight
				}

				Text {
					id: chipDetail
					anchors.right: chipClearButton.left
					anchors.rightMargin: Style.marginS
					anchors.verticalCenter: parent.verticalCenter
					width: Math.min(implicitWidth, linkSelectedChip.width * 0.45)
					text: productEditor.linkedItemDetail
					color: Style.subtitleColor
					font.family: Style.fontFamily
					font.pixelSize: Style.fontSizeS
					elide: Text.ElideRight
				}

				Button {
					id: chipClearButton
					objectName: "ProductLinkClearButton"
					anchors.right: parent.right
					anchors.rightMargin: Style.marginM
					anchors.verticalCenter: parent.verticalCenter
					width: Style.iconSizeXS
					height: Style.iconSizeXS
					decorator: Component { IconButtonDecorator {} }
					iconSource: "../../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
					tooltipText: qsTr("Unlink this instance from the order product")

					onClicked: {
						linkSelectProvider.clearSelection()
						productEditor.clearLinkSelection()
					}
				}
			}
		}

		// Starts after the segmented buttons instead of only being pinned right, so a
		// narrow bar cannot slide it under them; the label steps aside first.
		Item {
			id: qtyRow
			anchors.left: leftChrome.right
			anchors.leftMargin: Style.marginL
			anchors.right: parent.right
			anchors.rightMargin: Style.marginXL
			anchors.verticalCenter: parent.verticalCenter
			height: Style.controlHeightM
			visible: productEditor.showQuantity

			Text {
				anchors.right: qtySpin.left
				anchors.rightMargin: Style.marginM
				anchors.verticalCenter: parent.verticalCenter
				visible: qtyRow.width > Style.sizeHintXS
				text: qsTr("Quantity")
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
			}

			SpinBox {
				id: qtySpin
				objectName: "ProductQuantitySpinBox"
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
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

			// Anchored to the top control, so the popup always drops from the same place.
			onLinkRequested: {
				productEditor.openLinkPopup(linkSelectButton)
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

			// Anchored to the top control, so the popup always drops from the same place.
			onLinkRequested: {
				productEditor.openLinkPopup(linkSelectButton)
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
