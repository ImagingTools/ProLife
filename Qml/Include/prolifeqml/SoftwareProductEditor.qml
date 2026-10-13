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
			if (root.batchQuantity > 1) {
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

	// \c values is a row of the instance picker, keyed by SoftwareProductItem field ids.
	function selectLinkedItem(values) {
		if (!values || !productItem)
			return

		productItem.m_isNew = false
		productItem.m_categoryId = "Software"
		productItem.m_id = values[SoftwareProductItemTypeMetaInfo.s_id]
		productItem.m_licenseUuid = values[SoftwareProductItemTypeMetaInfo.s_licenseUuid]
		productItem.m_licenseId = values[SoftwareProductItemTypeMetaInfo.s_licenseId]
		productItem.m_licenseName = values[SoftwareProductItemTypeMetaInfo.s_licenseName]
		productItem.m_serialNumber = values[SoftwareProductItemTypeMetaInfo.s_serialNumber]
		productItem.m_expiration = values[SoftwareProductItemTypeMetaInfo.s_expiration]
		productItem.m_inUse = values[SoftwareProductItemTypeMetaInfo.s_inUse] === "true"
		productItem.m_productUuid = values[SoftwareProductItemTypeMetaInfo.s_productUuid]
		productItem.m_productName = values[SoftwareProductItemTypeMetaInfo.s_productName]
		productItem.m_macAddress = ""

		root.hasLinkedSelection = true
		softwareEditor.readOnly = true
		softwareEditor.loadFromOrderedProduct(productItem)
		softwareEditor.setReadOnly(true)

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
				text: qsTr("Search an existing software license by name, software-ID or article.")
				color: Style.inactiveTextColor
				font.family: Style.fontFamily
				font.pixelSize: Style.fontSizeM
				wrapMode: Text.WordWrap
			}

			// Opens the same picker as the control on the top panel, so the empty state
			// is actionable where the reader is already looking.
			Button {
				objectName: "SelectLicenseButton"
				anchors.horizontalCenter: parent.horizontalCenter
				text: qsTr("Select license")
				iconSource: "../../../../" + Style.getIconPath("Icons/Link", Icon.State.On, Icon.Mode.Normal)

				onClicked: {
					root.linkRequested()
				}
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
