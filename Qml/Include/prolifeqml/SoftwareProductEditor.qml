import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtlicgui 1.0
import imtcontrols 1.0
import prolifeOrdersSdl 1.0
import prolifeLicensesSdl 1.0

/**
 * SoftwareProductEditor — stage body for order Software products.
 *
 * createMode=true  → SoftwareEditor (editable)
 * createMode=false → SoftwareEditor (read-only) after a license is selected;
 *                    empty prompt until selection (link picker is in ProductEditor).
 */
Item {
	id: root

	property TreeItemModel softwaresModel: TreeItemModel {}
	property BaseModel orderProductsModel: BaseModel {}
	property var model: null
	property OrderedProduct productItem: model

	property string orderUuid: ""
	property int productIndex: -1
	property bool readOnly: false

	property bool createMode: false
	property int batchQuantity: 1
	property bool chromeLocked: false
	property bool hasLinkedSelection: false

	property int instanceCount: {
		if (!root.createMode)
			return 1
		if (softwareEditor.softwareProductData && softwareEditor.softwareProductData.m_isMultiple)
			return 1
		return root.batchQuantity
	}

	readonly property bool showEditor: root.createMode || root.hasLinkedSelection || root.chromeLocked
	readonly property bool showEmptyLinkState: !root.createMode && !root.hasLinkedSelection && !root.chromeLocked

	SoftwareProductData {
		id: softwareData
	}

	function doUpdateGui() {
		updateGui()
	}

	function updateGui() {
		if (!productItem)
			return

		let forceReadOnly = !root.createMode || root.readOnly
		softwareEditor.readOnly = forceReadOnly
		if (forceReadOnly)
			softwareEditor.setReadOnly(true)

		if (!root.showEditor)
			return

		softwareEditor.loadFromOrderedProduct(productItem)
		if (forceReadOnly)
			softwareEditor.setReadOnly(true)
	}

	function updateModel() {
		if (!productItem)
			return

		productItem.m_isNew = root.createMode
		productItem.m_categoryId = "Software"

		if (root.createMode) {
			softwareEditor.applyToOrderedProduct(productItem)
			if (!productItem.m_isMultiple && root.batchQuantity > 1) {
				productItem.m_serialNumber = ""
				productItem.m_expiration = ""
			}
			return
		}

		productItem.m_isNew = false
	}

	function clearLinkedSelection() {
		root.hasLinkedSelection = false
	}

	function selectLinkedIndex(index) {
		if (index < 0 || !root.softwaresModel)
			return

		let src = root.softwaresModel
		let idx = index

		productItem.m_isNew = false
		productItem.m_categoryId = "Software"
		productItem.m_id = src.getData(SoftwareProductItemTypeMetaInfo.s_id, idx)
		productItem.m_licenseUuid = src.containsKey(SoftwareProductItemTypeMetaInfo.s_licenseUuid, idx)
			? src.getData(SoftwareProductItemTypeMetaInfo.s_licenseUuid, idx) : ""
		productItem.m_licenseId = src.containsKey(SoftwareProductItemTypeMetaInfo.s_licenseId, idx)
			? src.getData(SoftwareProductItemTypeMetaInfo.s_licenseId, idx) : ""
		productItem.m_licenseName = src.containsKey(SoftwareProductItemTypeMetaInfo.s_licenseName, idx)
			? src.getData(SoftwareProductItemTypeMetaInfo.s_licenseName, idx) : ""
		productItem.m_serialNumber = src.containsKey(SoftwareProductItemTypeMetaInfo.s_serialNumber, idx)
			? src.getData(SoftwareProductItemTypeMetaInfo.s_serialNumber, idx) : ""
		productItem.m_expiration = src.containsKey(SoftwareProductItemTypeMetaInfo.s_expiration, idx)
			? src.getData(SoftwareProductItemTypeMetaInfo.s_expiration, idx) : ""
		productItem.m_inUse = src.containsKey(SoftwareProductItemTypeMetaInfo.s_inUse, idx)
			? src.getData(SoftwareProductItemTypeMetaInfo.s_inUse, idx) : false
		productItem.m_productUuid = src.containsKey(SoftwareProductItemTypeMetaInfo.s_productUuid, idx)
			? src.getData(SoftwareProductItemTypeMetaInfo.s_productUuid, idx) : ""
		productItem.m_productName = src.containsKey(SoftwareProductItemTypeMetaInfo.s_productName, idx)
			? src.getData(SoftwareProductItemTypeMetaInfo.s_productName, idx) : ""
		productItem.m_isMultiple = false
		productItem.m_productCount = 1
		productItem.m_macAddress = ""

		softwareData.m_id = productItem.m_id
		softwareData.m_productId = productItem.m_productUuid
		softwareData.m_licenseUuid = productItem.m_licenseUuid
		softwareData.m_serialNumber = productItem.m_serialNumber
		softwareData.m_expiration = productItem.m_expiration
		softwareData.m_isMultiple = false
		softwareData.m_productCount = 0
		softwareData.m_inUse = productItem.m_inUse
		softwareData.m_categoryId = "Software"
		if (root.orderUuid !== "")
			softwareData.m_orderUuid = root.orderUuid

		root.hasLinkedSelection = true
		softwareEditor.readOnly = true
		softwareEditor.doUpdateGui()
		softwareEditor.setReadOnly(true)

		if (root.productItem)
			root.productItem.modelChanged([])
	}

	onCreateModeChanged: {
		if (productItem) {
			productItem.m_isNew = root.createMode
			if (root.createMode)
				root.hasLinkedSelection = false
			root.updateGui()
		}
	}

	onBatchQuantityChanged: {
		if (root.createMode && productItem)
			root.updateModel()
	}

	// Empty state before a license is chosen
	Item {
		id: emptyLinkState
		anchors.fill: parent
		visible: root.showEmptyLinkState

		Column {
			anchors.centerIn: parent
			width: Math.min(parent.width - Style.marginXL * 2, 420)
			spacing: Style.marginM

			Text {
				width: parent.width
				horizontalAlignment: Text.AlignHCenter
				text: qsTr("Select a license to link")
				color: Style.textColor
				font.family: Style.fontFamilyBold
				font.pixelSize: Style.fontSizeL
				font.bold: true
				wrapMode: Text.WordWrap
			}

			Text {
				width: parent.width
				horizontalAlignment: Text.AlignHCenter
				text: qsTr("Use the search field on the top panel to find an existing software license by name, software-ID or article.")
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				wrapMode: Text.WordWrap
			}
		}
	}

	// Pages of SoftwareEditor scroll with their own Flickable + CustomScrollbar.
	SoftwareEditor {
		id: softwareEditor
		anchors.fill: parent
		visible: root.showEditor
		embedded: true
		isNew: root.createMode
		model: softwareData
		forcedOrderUuid: root.orderUuid
		readOnly: !root.createMode || root.readOnly
		clip: true

		Component.onCompleted: {
			if (root.productItem && root.showEditor)
				root.updateGui()
		}

		onReadOnlyChanged: {
			if (readOnly)
				softwareEditor.setReadOnly(true)
		}
	}

	Timer {
		id: syncTimer
		interval: 0
		onTriggered: {
			if (root.createMode && root.productItem) {
				softwareEditor.applyToOrderedProduct(root.productItem)
				root.productItem.modelChanged([])
			}
		}
	}

	Connections {
		target: softwareData
		function onModelChanged() {
			if (root.createMode)
				syncTimer.start()
		}
	}
}
