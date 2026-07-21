import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import prolifeOrdersSdl 1.0
import prolifeSensorsSdl 1.0

/**
 * HardwareProductEditor — stage body for order Hardware products.
 *
 * createMode=true  → DeviceEditor (editable)
 * createMode=false → DeviceEditor (read-only) after a device is selected;
 *                    empty prompt until selection (link picker is in ProductEditor).
 */
Item {
	id: root

	property TreeItemModel devicesModel: TreeItemModel {}
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

	property int instanceCount: root.createMode ? root.batchQuantity : 1

	readonly property bool showEditor: root.createMode || root.hasLinkedSelection || root.chromeLocked
	readonly property bool showEmptyLinkState: !root.createMode && !root.hasLinkedSelection && !root.chromeLocked

	DeviceData {
		id: deviceData
	}

	function doUpdateGui() {
		updateGui()
	}

	function updateGui() {
		if (!productItem)
			return

		let forceReadOnly = !root.createMode || root.readOnly
		deviceEditor.readOnly = forceReadOnly
		if (forceReadOnly)
			deviceEditor.setReadOnly(true)

		if (!root.showEditor)
			return

		deviceEditor.loadFromOrderedProduct(productItem)
		if (forceReadOnly)
			deviceEditor.setReadOnly(true)
	}

	function updateModel() {
		if (!productItem)
			return

		productItem.m_isNew = root.createMode
		productItem.m_categoryId = "Hardware"

		if (root.createMode) {
			deviceEditor.applyToOrderedProduct(productItem)
			if (root.batchQuantity > 1) {
				productItem.m_macAddress = ""
				productItem.m_serialNumber = ""
			}
			return
		}

		productItem.m_isNew = false
	}

	function clearLinkedSelection() {
		root.hasLinkedSelection = false
	}

	function selectLinkedIndex(index) {
		if (index < 0 || !root.devicesModel)
			return

		let src = root.devicesModel
		let idx = index

		productItem.m_isNew = false
		productItem.m_categoryId = "Hardware"
		productItem.m_id = src.getData(DeviceItemTypeMetaInfo.s_id, idx)

		if (src.containsKey(DeviceItemTypeMetaInfo.s_licenseUuid, idx))
			productItem.m_licenseUuid = src.getData(DeviceItemTypeMetaInfo.s_licenseUuid, idx)
		else if (src.containsKey(DeviceItemTypeMetaInfo.s_licenseName, idx))
			productItem.m_licenseUuid = src.getData(DeviceItemTypeMetaInfo.s_licenseName, idx)
		else
			productItem.m_licenseUuid = ""

		if (src.containsKey(DeviceItemTypeMetaInfo.s_licenseId, idx))
			productItem.m_licenseId = src.getData(DeviceItemTypeMetaInfo.s_licenseId, idx)
		else
			productItem.m_licenseId = ""

		if (src.containsKey(DeviceItemTypeMetaInfo.s_licenseName, idx))
			productItem.m_licenseName = src.getData(DeviceItemTypeMetaInfo.s_licenseName, idx)
		else
			productItem.m_licenseName = ""

		if (src.containsKey(DeviceItemTypeMetaInfo.s_macAddress, idx))
			productItem.m_macAddress = src.getData(DeviceItemTypeMetaInfo.s_macAddress, idx)
		else
			productItem.m_macAddress = ""

		if (src.containsKey(DeviceItemTypeMetaInfo.s_serialNumber, idx))
			productItem.m_serialNumber = src.getData(DeviceItemTypeMetaInfo.s_serialNumber, idx)
		else
			productItem.m_serialNumber = ""

		if (src.containsKey(DeviceItemTypeMetaInfo.s_deviceType, idx))
			productItem.m_productUuid = src.getData(DeviceItemTypeMetaInfo.s_deviceType, idx)
		else if (src.containsKey(DeviceItemTypeMetaInfo.s_productUuid, idx))
			productItem.m_productUuid = src.getData(DeviceItemTypeMetaInfo.s_productUuid, idx)
		else
			productItem.m_productUuid = ""

		if (src.containsKey(DeviceItemTypeMetaInfo.s_productName, idx))
			productItem.m_productName = src.getData(DeviceItemTypeMetaInfo.s_productName, idx)
		else
			productItem.m_productName = ""

		productItem.m_expiration = ""
		productItem.m_isMultiple = false
		productItem.m_productCount = 0

		deviceData.m_id = productItem.m_id
		deviceData.m_deviceType = productItem.m_productUuid
		deviceData.m_licenseName = productItem.m_licenseUuid !== "" ? productItem.m_licenseUuid : productItem.m_licenseName
		deviceData.m_serialNumber = productItem.m_serialNumber
		deviceData.m_macAddress = productItem.m_macAddress
		deviceData.m_description = productItem.m_productName
		if (root.orderUuid !== "")
			deviceData.m_orderId = root.orderUuid

		root.hasLinkedSelection = true
		deviceEditor.readOnly = true
		deviceEditor.doUpdateGui()
		deviceEditor.setReadOnly(true)

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
				text: qsTr("Select a device to link")
				color: Style.textColor
				font.family: Style.fontFamilyBold
				font.pixelSize: Style.fontSizeL
				font.bold: true
				wrapMode: Text.WordWrap
			}

			Text {
				width: parent.width
				horizontalAlignment: Text.AlignHCenter
				text: qsTr("Use the search field on the top panel to find an existing device by product name or MAC address.")
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				wrapMode: Text.WordWrap
			}
		}
	}

	// Pages of DeviceEditor scroll with their own Flickable + CustomScrollbar.
	DeviceEditor {
		id: deviceEditor
		anchors.fill: parent
		visible: root.showEditor
		embedded: true
		isNew: root.createMode
		model: deviceData
		forcedOrderId: root.orderUuid
		readOnly: !root.createMode || root.readOnly
		clip: true

		Component.onCompleted: {
			if (root.productItem && root.showEditor)
				root.updateGui()
		}

		onReadOnlyChanged: {
			if (readOnly)
				deviceEditor.setReadOnly(true)
		}
	}

	Timer {
		id: syncTimer
		interval: 0
		onTriggered: {
			if (root.createMode && root.productItem) {
				deviceEditor.applyToOrderedProduct(root.productItem)
				root.productItem.modelChanged([])
			}
		}
	}

	Connections {
		target: deviceData
		function onModelChanged() {
			if (root.createMode)
				syncTimer.start()
		}
	}
}
