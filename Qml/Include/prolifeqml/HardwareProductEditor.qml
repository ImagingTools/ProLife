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

	//! Asks the host to open its instance picker.
	signal linkRequested()

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

	// \c values is a row of the instance picker, keyed by DeviceItem field ids.
	function selectLinkedItem(values) {
		if (!values || !productItem)
			return

		productItem.m_isNew = false
		productItem.m_categoryId = "Hardware"
		productItem.m_id = values[DeviceItemTypeMetaInfo.s_id]
		productItem.m_licenseUuid = values[DeviceItemTypeMetaInfo.s_licenseUuid]
		productItem.m_licenseId = values[DeviceItemTypeMetaInfo.s_licenseId]
		productItem.m_licenseName = values[DeviceItemTypeMetaInfo.s_licenseName]
		productItem.m_macAddress = values[DeviceItemTypeMetaInfo.s_macAddress]
		productItem.m_serialNumber = values[DeviceItemTypeMetaInfo.s_serialNumber]
		productItem.m_productUuid = values[DeviceItemTypeMetaInfo.s_productUuid]
		productItem.m_productName = values[DeviceItemTypeMetaInfo.s_productName]
		productItem.m_expiration = ""
		productItem.m_isMultiple = false
		productItem.m_productCount = 0

		root.hasLinkedSelection = true
		deviceEditor.readOnly = true
		deviceEditor.loadFromOrderedProduct(productItem)
		deviceEditor.setReadOnly(true)

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
				text: qsTr("Search an existing device by product name, article or MAC address.")
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				wrapMode: Text.WordWrap
			}

			// Opens the same picker as the control on the top panel, so the empty state
			// is actionable where the reader is already looking.
			Button {
				objectName: "SelectDeviceButton"
				anchors.horizontalCenter: parent.horizontalCenter
				text: qsTr("Select device")
				iconSource: "../../../../" + Style.getIconPath("Icons/Link", Icon.State.On, Icon.Mode.Normal)

				onClicked: {
					root.linkRequested()
				}
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
