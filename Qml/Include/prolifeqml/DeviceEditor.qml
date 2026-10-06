import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdeskgui 1.0
import prolifeqml 1.0
import prolifeSensorsSdl 1.0
import prolifeOrdersSdl 1.0
import imtlicProductsSdl 1.0
import imtlicLicensesSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

/**
 * DeviceEditor
 *
 * MultiPageView: Device / Production / Licenses / Support / History.
 * Support and History are hidden in embedded mode.
 *
 * Device type, configuration and order are picked from server-side searchable lists; the
 * names shown for the current values come with the document itself.
 *
 * Embedded mode (Order product dialog): set embedded=true, optionally forcedOrderId.
 * Use loadFromOrderedProduct() / applyToOrderedProduct() to map OrderedProduct.
 */
DocumentViewBase {
	id: deviceEditorContainer

	anchors.fill: parent
	contentColor: Style.baseColor
	commandsPanelVisible: !deviceEditorContainer.embedded

	property int radius: 3
	property int spacing: Style.marginM
	property int contentMaxWidth: 900

	property DeviceData deviceData: model ? model : null
	property bool isNew: false
	property bool dialogIsShown: false
	property bool licensePageVisible: false

	// When true, editor is used inside Order product dialog (or similar host).
	property bool embedded: false
	property string forcedOrderId: ""
	property bool hideProductionPage: deviceEditorContainer.embedded

	// Id of the stored device record. documentObjectId (DocumentViewBase) is the one
	// that is correct right after a first save; deviceData.m_id only catches up when
	// the representation is re-fetched, which is why it is the fallback and not the
	// other way round.
	readonly property string deviceObjectId: deviceEditorContainer.documentObjectId !== ""
		? deviceEditorContainer.documentObjectId
		: (deviceData ? deviceData.m_id : "")

	// A ticket is stored with a reference to the device record, so that record has to
	// exist and have an id to point at. isNewDocument comes from DocumentViewBase and
	// asks the document manager; the local `isNew` below cannot be used - it is fed
	// documentIsNew(m_id), an object id where a document id is expected, and
	// documentIsNew() reports "new" for anything it fails to find, so it reads true
	// for saved devices too.
	readonly property bool ticketsEnabled: !deviceEditorContainer.isNewDocument && deviceEditorContainer.deviceObjectId !== ""

	DeviceProductionStatus {
		id: productionStatus
	}

	Component.onCompleted: {
		multiPageView.updatePages()
	}

	Connections {
		target: deviceEditorContainer.deviceData
		function onFinished() {
			deviceEditorContainer.deviceDataWasChanged()
		}
	}

	Connections {
		target: deviceEditorContainer
		function onDeviceDataChanged() {
			deviceEditorContainer.deviceDataWasChanged()
		}
	}

	function deviceDataWasChanged() {
		if (!deviceData) {
			return
		}

		checkPermissions()

		let licenseGroupVisible = false
		if (deviceData.hasSoftwareBindingInfos()) {
			let licensesPage = deviceEditorContainer.getPageItem("Licenses")
			if (licensesPage && licensesPage.licenseInformationTable) {
				licensesPage.licenseInformationTable.table.elements = deviceData.m_softwareBindingInfos
			}
			licenseGroupVisible = deviceData.m_softwareBindingInfos.count > 0
		}

		if (deviceEditorContainer.licensePageVisible !== licenseGroupVisible) {
			deviceEditorContainer.licensePageVisible = licenseGroupVisible
			multiPageView.updatePages()
		}
	}

	function ensurePages() {
		if (multiPageView.pagesModel.count === 0) {
			multiPageView.updatePages()
		}
	}

	function getPageItem(pageId) {
		deviceEditorContainer.ensurePages()
		let idx = multiPageView.getIndexById(pageId)
		if (idx < 0) {
			return null
		}
		multiPageView.ensurePageLoaded(idx)
		return multiPageView.getPageByIndex(idx)
	}

	function getLoadedPageItem(pageId) {
		let idx = multiPageView.getIndexById(pageId)
		if (idx < 0) {
			return null
		}
		return multiPageView.getPageByIndex(idx)
	}

	function checkPermissions() {
		if (!deviceData) {
			return
		}

		// Forced read-only (e.g. linked order product) must win over field permissions.
		if (deviceEditorContainer.readOnly) {
			setReadOnly(true)
			return
		}

		let devicePage = deviceEditorContainer.getPageItem("Device")
		let productionPage = deviceEditorContainer.hideProductionPage ? null : deviceEditorContainer.getPageItem("Production")
		if (!devicePage) {
			return
		}

		let canAddSensor = PermissionsController.checkPermission("AddSensor")
		if (isNew && canAddSensor) {
			devicePage.descriptionInput.readOnly = false
			devicePage.serialNumberInput.readOnly = false
			devicePage.macAddressInput.readOnly = false
			if (productionPage) {
				productionPage.projectInput.readOnly = false
				productionPage.statusCB.changeable = true
				productionPage.orderSelect.changeable = true
			}
			devicePage.productSelect.changeable = true
			devicePage.configurationSelect.changeable = true
		}
		else {
			let canChangeDescription = PermissionsController.checkPermission("ChangeDescriptionForSensor")
			devicePage.descriptionInput.readOnly = !canChangeDescription

			let canChangeSerialNumber = PermissionsController.checkPermission("ChangeSerialNumberForSensor")
			devicePage.serialNumberInput.readOnly = !canChangeSerialNumber

			let canChangeMacAddress = PermissionsController.checkPermission("ChangeMacAddress")
			devicePage.macAddressInput.readOnly = !canChangeMacAddress

			if (productionPage) {
				let canChangeOrder = PermissionsController.checkPermission("ChangeOrderForSensor")
				productionPage.orderSelect.changeable = canChangeOrder

				let canChangeProductionStatus = PermissionsController.checkPermission("ChangeProductionStatus")
				productionPage.statusCB.changeable = canChangeProductionStatus

				let canChangeProject = PermissionsController.checkPermission("ChangeProjectForSensor")
				productionPage.projectInput.readOnly = !canChangeProject
			}

			let canChangeConfiguration = PermissionsController.checkPermission("ChangeHardwareConfiguration")
			let canChangeDevice = PermissionsController.checkPermission("ChangeDeviceType")
			devicePage.configurationSelect.changeable = canChangeConfiguration && canChangeDevice
			devicePage.productSelect.changeable = canChangeConfiguration && canChangeDevice

			let ok =
				canChangeDescription ||
				canChangeSerialNumber ||
				canChangeMacAddress ||
				canChangeConfiguration ||
				canChangeDevice ||
				PermissionsController.checkPermission("ChangeOrderForSensor") ||
				PermissionsController.checkPermission("ChangeProductionStatus") ||
				PermissionsController.checkPermission("ChangeProjectForSensor")

			if (commandsController && !deviceEditorContainer.embedded) {
				commandsController.setCommandVisible("Undo", ok)
				commandsController.setCommandVisible("Redo", ok)
				commandsController.setCommandVisible("Save", ok)
			}
		}
	}

	function checkFinishedStatus() {
		if (!deviceData || dialogIsShown) {
			return
		}

		let devicePage = deviceEditorContainer.getPageItem("Device")
		if (!devicePage) {
			return
		}

		if (devicePage.macAddressInput.acceptableInput &&
				devicePage.serialNumberInput.acceptableInput &&
				!ModalDialogManager.dialogIsOpened(confirmSetFinishedStatusDialogComp) &&
				PermissionsController.checkPermission("ChangeProductionStatus")) {

			if (deviceEditorContainer.visible && deviceData.m_macAddress !== "" && deviceData.m_serialNumber !== "" && deviceData.m_productionStatus !== "Finished") {
				ModalDialogManager.openDialog(confirmSetFinishedStatusDialogComp)
			}
		}
	}

	Component {
		id: confirmSetFinishedStatusDialogComp
		MessageDialog {
			backgroundColor: Style.baseColor
			title: qsTr("Confirm status")
			message: qsTr("Do you want to set the production state of the sensor to Finished ?")

			onFinished: {
				if (buttonId == Enums.yes) {
					let finishedStatusIndex = productionStatus.getStatusIndex("Finished")
					let productionPage = deviceEditorContainer.getPageItem("Production")
					if (productionPage) {
						productionPage.statusCB.currentIndex = finishedStatusIndex
					}
				}
			}

			Component.onCompleted: {
				deviceEditorContainer.dialogIsShown = true
			}
		}
	}

	function setReadOnly(readOnly) {
		let devicePage = deviceEditorContainer.getPageItem("Device")
		let productionPage = deviceEditorContainer.hideProductionPage ? null : deviceEditorContainer.getPageItem("Production")
		if (!devicePage) {
			return
		}

		devicePage.descriptionInput.readOnly = readOnly
		devicePage.serialNumberInput.readOnly = readOnly
		devicePage.macAddressInput.readOnly = readOnly
		devicePage.productSelect.changeable = !readOnly
		devicePage.configurationSelect.changeable = !readOnly
		if (productionPage) {
			productionPage.statusCB.changeable = !readOnly
			productionPage.orderSelect.changeable = !readOnly
		}
	}

	function updateGui() {
		if (!deviceData) {
			return
		}

		if (deviceEditorContainer.forcedOrderId !== "" && deviceData.m_orderId !== deviceEditorContainer.forcedOrderId) {
			deviceData.m_orderId = deviceEditorContainer.forcedOrderId
		}

		let devicePage = deviceEditorContainer.getLoadedPageItem("Device")
		let productionPage = deviceEditorContainer.hideProductionPage ? null : deviceEditorContainer.getLoadedPageItem("Production")
		if (devicePage) {
			devicePage.updateGui()
		}
		if (productionPage) {
			productionPage.updateGui()
		}

		let licensesPage = deviceEditorContainer.getLoadedPageItem("Licenses")
		if (licensesPage) {
			licensesPage.updateGui()
		}
	}

	function updateModel() {
		if (!deviceData) {
			return
		}

		let devicePage = deviceEditorContainer.getLoadedPageItem("Device")
		let productionPage = deviceEditorContainer.hideProductionPage ? null : deviceEditorContainer.getLoadedPageItem("Production")
		if (devicePage) {
			devicePage.updateModel()
		}
		if (productionPage) {
			productionPage.updateModel()
		}

		if (deviceEditorContainer.forcedOrderId !== "") {
			deviceData.m_orderId = deviceEditorContainer.forcedOrderId
		}
	}

	// --- OrderedProduct bridge (OrderEditor product dialog) ------------------------------------

	function loadFromOrderedProduct(orderedProduct) {
		if (!deviceData || !orderedProduct) {
			return
		}

		deviceData.m_id = orderedProduct.m_id || ""
		deviceData.m_deviceType = orderedProduct.m_productUuid || ""
		deviceData.m_productName = orderedProduct.m_productName || ""
		deviceData.m_licenseName = orderedProduct.m_licenseUuid || ""
		deviceData.m_configurationName = orderedProduct.m_licenseName || ""
		deviceData.m_configurationArticle = orderedProduct.m_licenseId || ""
		deviceData.m_serialNumber = orderedProduct.m_serialNumber || ""
		deviceData.m_macAddress = orderedProduct.m_macAddress || ""
		deviceData.m_description = orderedProduct.m_productName || ""
		if (deviceEditorContainer.forcedOrderId !== "") {
			deviceData.m_orderId = deviceEditorContainer.forcedOrderId
		}
		deviceEditorContainer.doUpdateGui()
	}

	function applyToOrderedProduct(orderedProduct) {
		if (!deviceData || !orderedProduct) {
			return false
		}

		deviceEditorContainer.doUpdateModel()

		orderedProduct.m_isNew = true
		orderedProduct.m_categoryId = "Hardware"
		orderedProduct.m_id = deviceData.m_id !== "" ? deviceData.m_id : orderedProduct.m_id
		orderedProduct.m_productUuid = deviceData.m_deviceType
		orderedProduct.m_productName = deviceData.m_productName
		orderedProduct.m_licenseUuid = deviceData.m_licenseName
		orderedProduct.m_licenseName = deviceData.m_configurationName
		orderedProduct.m_licenseId = deviceData.m_configurationArticle
		orderedProduct.m_serialNumber = deviceData.m_serialNumber
		orderedProduct.m_macAddress = deviceData.m_macAddress
		orderedProduct.m_expiration = ""
		orderedProduct.m_isMultiple = false
		orderedProduct.m_productCount = 0
		orderedProduct.m_inUse = false

		return orderedProduct.m_productUuid !== "" && orderedProduct.m_licenseUuid !== ""
	}

	function isOrderedProductAcceptable() {
		if (!deviceData) {
			return false
		}
		deviceEditorContainer.doUpdateModel()
		return deviceData.m_deviceType !== "" && deviceData.m_licenseName !== ""
	}

	MultiPageView {
		id: multiPageView
		anchors.fill: parent
		clip: true
		panelWidth: Style.sizeHintXXS

		function updatePages() {
			let currentId = ""
			if (multiPageView.currentIndex >= 0 && multiPageView.currentIndex < multiPageView.pagesModel.count) {
				currentId = multiPageView.pagesModel.get(multiPageView.currentIndex).id
			}

			multiPageView.clear()
			// Device: physical identity of the sensor (type, config, serial, MAC).
			// Production: manufacturing / order lifecycle.
			// Licenses: bound software (read-only overview, only when present).
			multiPageView.addPage("Device", qsTr("Device"), devicePageComp, "Icons/Sensor")
			if (!deviceEditorContainer.hideProductionPage) {
				multiPageView.addPage("Production", qsTr("Production"), productionPageComp, "Icons/Production")
			}
			if (deviceEditorContainer.licensePageVisible && !deviceEditorContainer.embedded) {
				multiPageView.addPage("Licenses", qsTr("Licenses"), licensesPageComp, "Icons/Key")
			}
			if (!deviceEditorContainer.embedded) {
				multiPageView.addPage("Support", qsTr("Support"), supportPageComp, "Icons/SupportDesk")
				if (PermissionsController.checkPermission("ViewRevisions")) {
					multiPageView.addPage("History", qsTr("History"), historyPageComp, "Icons/History")
				}
			}

			let targetIndex = multiPageView.getIndexById(currentId)
			multiPageView.currentIndex = targetIndex >= 0 ? targetIndex : 0
		}

		onPageLoaded: {
			if (deviceEditorContainer.readOnly)
				deviceEditorContainer.setReadOnly(true)
			else
				deviceEditorContainer.checkPermissions()
			if (pageId === "Device" || pageId === "Production" || pageId === "Licenses") {
				if (pageItem && pageItem.updateGui) {
					pageItem.updateGui()
				}
			}
			if (deviceEditorContainer.readOnly)
				deviceEditorContainer.setReadOnly(true)
		}
	}

	Component {
		id: supportPageComp

		Item {
			id: supportPage
			anchors.fill: parent

			EntityContextTicketsPanel {
				id: supportWorkspace
				visible: deviceEditorContainer.ticketsEnabled
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				x: Math.max(0, (supportPage.width - width) / 2)
				width: Math.max(0, Math.min(deviceEditorContainer.contentMaxWidth, supportPage.width))
				entityType: "Devices"
				entityId: deviceEditorContainer.ticketsEnabled ? deviceEditorContainer.deviceObjectId : ""
				entityDisplayName: deviceEditorContainer.deviceData ? deviceEditorContainer.deviceData.m_macAddress : ""
			}

			Column {
				id: supportUnavailableHint
				visible: !deviceEditorContainer.ticketsEnabled
				anchors.centerIn: parent
				width: Math.max(0, Math.min(Style.sizeHintS, supportPage.width - 2 * Style.marginXL))
				spacing: Style.marginM

				Text {
					width: parent.width
					horizontalAlignment: Text.AlignHCenter
					text: qsTr("Tickets are available after saving")
					font.pixelSize: Style.fontSizeXL
					font.family: Style.fontFamilyBold
					color: Style.textColor
					wrapMode: Text.WordWrap
				}

				Text {
					width: parent.width
					horizontalAlignment: Text.AlignHCenter
					text: qsTr("A support ticket is linked to the device record, which only exists once the device has been saved. Save this device and the Support page will let you create and track its tickets.")
					font.pixelSize: Style.fontSizeM
					font.family: Style.fontFamily
					color: Style.inactiveTextColor
					wrapMode: Text.WordWrap
				}
			}
		}
	}

	Component {
		id: historyPageComp

		Item {
			id: historyPage
			anchors.fill: parent

			Item {
				id: historyPageHeader
				anchors.top: parent.top
				anchors.topMargin: Style.marginL
				x: Math.max(0, (historyPage.width - width) / 2)
				width: Math.max(0, Math.min(deviceEditorContainer.contentMaxWidth, historyPage.width))
				height: historyTitleRow.height

				Row {
					id: historyTitleRow
					anchors.left: parent.left
					anchors.verticalCenter: parent.verticalCenter
					spacing: Style.spacingS

					Text {
						text: qsTr("History")
						font.pixelSize: Style.fontSizeXL
						font.family: Style.fontFamilyBold
						color: Style.textColor
						anchors.verticalCenter: parent.verticalCenter
					}

					Text {
						visible: historyWorkspace.revisionsCount > 0
						text: "(" + historyWorkspace.revisionsCount + ")"
						font.pixelSize: Style.fontSizeXL
						font.family: Style.fontFamilyBold
						color: Style.imaginToolsAccentColor
						anchors.verticalCenter: parent.verticalCenter
					}
				}
			}

			DocumentHistoryView {
				id: historyWorkspace
				anchors.top: historyPageHeader.bottom
				anchors.topMargin: Style.marginM
				anchors.bottom: parent.bottom
				x: Math.max(0, (historyPage.width - width) / 2)
				width: Math.max(0, Math.min(deviceEditorContainer.contentMaxWidth, historyPage.width))
				documentId: deviceEditorContainer.deviceObjectId
				collectionId: "Devices"
			}
		}
	}

	// --- Device: type, configuration, article, description, serial, MAC ---
	Component {
		id: devicePageComp

		Item {
			id: devicePage
			anchors.fill: parent

			property alias productSelect: productSelect
			property alias configurationSelect: configurationSelect
			property alias articleText: articleText
			property alias descriptionInput: descriptionInput
			property alias serialNumberInput: serialNumberInput
			property alias macAddressInput: macAddressInput

			function updateGui() {
				if (!deviceEditorContainer.deviceData) {
					return
				}

				descriptionInput.text = deviceEditorContainer.deviceData.m_description
				serialNumberInput.text = deviceEditorContainer.deviceData.m_serialNumber
				macAddressInput.text = deviceEditorContainer.deviceData.m_macAddress

				productSelect.selectedId = deviceEditorContainer.deviceData.m_deviceType
				productSelect.selectedText = deviceEditorContainer.deviceData.m_productName
				configurationSelect.selectedId = deviceEditorContainer.deviceData.m_licenseName
				configurationSelect.selectedText = deviceEditorContainer.deviceData.m_configurationName
				articleText.text = deviceEditorContainer.deviceData.m_configurationArticle
			}

			function updateModel() {
				if (!deviceEditorContainer.deviceData) {
					return
				}

				deviceEditorContainer.deviceData.m_deviceType = productSelect.selectedId
				deviceEditorContainer.deviceData.m_productName = productSelect.selectedText
				deviceEditorContainer.deviceData.m_licenseName = configurationSelect.selectedId
				deviceEditorContainer.deviceData.m_configurationName = configurationSelect.selectedText
				deviceEditorContainer.deviceData.m_configurationArticle = articleText.text
				deviceEditorContainer.deviceData.m_description = descriptionInput.text
				deviceEditorContainer.deviceData.m_serialNumber = serialNumberInput.text
				deviceEditorContainer.deviceData.m_macAddress = macAddressInput.text
			}

			FieldFilter {
				id: hardwareCategoryFilter
				m_fieldId: "CategoryId"
				m_filterValue: "Hardware"
				m_filterValueType: "String"
				m_filterOperations: ["Equal"]
			}

			GroupFilter {
				id: hardwareCategoryGroup
				m_logicalOperation: "And"

				Component.onCompleted: {
					hardwareCategoryGroup.emplaceFieldFilters()
					hardwareCategoryGroup.m_fieldFilters.addElement(hardwareCategoryFilter)
				}
			}

			FieldFilter {
				id: productConfigurationsFilter
				m_fieldId: "ProductId"
				m_filterValue: productSelect.selectedId
				m_filterValueType: "String"
				m_filterOperations: ["Equal"]
			}

			GroupFilter {
				id: productConfigurationsGroup
				m_logicalOperation: "And"

				Component.onCompleted: {
					productConfigurationsGroup.emplaceFieldFilters()
					productConfigurationsGroup.m_fieldFilters.addElement(productConfigurationsFilter)
				}
			}

			CustomScrollbar {
				id: generalScrollbar
				z: parent.z + 1
				anchors.right: parent.right
				anchors.top: generalFlickable.top
				anchors.bottom: generalFlickable.bottom
				secondSize: Style.marginM
				targetItem: generalFlickable
				alwaysVisible: false
			}

			Flickable {
				id: generalFlickable
				anchors.fill: parent
				anchors.margins: Style.marginXL
				anchors.rightMargin: Style.marginXL + generalScrollbar.secondSize
				contentWidth: width
				contentHeight: generalColumn.height + Style.marginXL * 2
				flickableDirection: Flickable.VerticalFlick
				boundsBehavior: Flickable.StopAtBounds
				clip: true
				interactive: true

				Column {
					id: generalColumn
					x: Math.max(0, (generalFlickable.width - width) / 2)
					width: Math.max(0, Math.min(deviceEditorContainer.contentMaxWidth, generalFlickable.width - 2 * Style.marginXL))
					spacing: Style.marginXL

					GroupHeaderView {
						width: parent.width
						objectName: "DeviceInformationHeader"
						title: qsTr("Device Information")
						groupView: deviceInformationGroup
					}

					GroupElementView {
						id: deviceInformationGroup
						objectName: "DeviceInformationGroup"
						width: parent.width

						CollectionSelectElementView {
							id: productSelect
							objectName: "DeviceTypeCombo"
							name: qsTr("Device Type")
							commandId: ImtlicProductsSdlCommandIds.s_productsList
							fields: [ProductItemTypeMetaInfo.s_id, ProductItemTypeMetaInfo.s_productName]
							titleField: ProductItemTypeMetaInfo.s_productName
							textFilterFieldIds: [ProductItemTypeMetaInfo.s_productName]
							sortByField: ProductItemTypeMetaInfo.s_productName
							groupFilters: [hardwareCategoryGroup]
							placeHolderText: qsTr("Select a device type")
							filterPlaceholder: qsTr("Search by product name")
							KeyNavigation.tab: configurationSelect
							KeyNavigation.backtab: macAddressInput
							isSelectionRequired: true
							errorText: qsTr("Please select a device type")

							onItemSelected: {
								configurationSelect.selectedId = ""
								configurationSelect.selectedText = ""
								articleText.text = ""
								deviceEditorContainer.doUpdateModel()
							}
						}

						CollectionSelectElementView {
							id: configurationSelect
							objectName: "HardwareConfigurationCombo"
							name: qsTr("Hardware Configuration")
							commandId: ImtlicLicensesSdlCommandIds.s_licensesList
							fields: [LicenseItemTypeMetaInfo.s_id, LicenseItemTypeMetaInfo.s_licenseName, LicenseItemTypeMetaInfo.s_licenseId]
							titleField: LicenseItemTypeMetaInfo.s_licenseName
							descriptionField: LicenseItemTypeMetaInfo.s_licenseId
							textFilterFieldIds: [LicenseItemTypeMetaInfo.s_licenseName, LicenseItemTypeMetaInfo.s_licenseId]
							sortByField: LicenseItemTypeMetaInfo.s_licenseName
							groupFilters: [productConfigurationsGroup]
							placeHolderText: productSelect.selectedId !== "" ? qsTr("Select a configuration") : qsTr("Select a device type first")
							filterPlaceholder: qsTr("Search by configuration name or article")
							KeyNavigation.tab: articleText
							KeyNavigation.backtab: productSelect
							isSelectionRequired: true
							errorText: qsTr("Please select a configuration")

							onItemSelected: {
								articleText.text = configurationSelect.itemValue(item, LicenseItemTypeMetaInfo.s_licenseId)
								deviceEditorContainer.doUpdateModel()
							}
						}

						TextInputElementView {
							id: articleText
							objectName: "ArticleInput"
							name: qsTr("Article")
							readOnly: true
							KeyNavigation.tab: descriptionInput
							KeyNavigation.backtab: configurationSelect
						}

						TextInputElementView {
							id: descriptionInput
							objectName: "DescriptionInput"
							name: qsTr("Description")
							placeHolderText: qsTr("Enter description")

							onEditingFinished: {
								deviceEditorContainer.doUpdateModel()
							}

							KeyNavigation.tab: serialNumberInput
							KeyNavigation.backtab: articleText
						}

						TextInputElementView {
							id: serialNumberInput
							objectName: "SerialNumberInput"
							name: qsTr("Serial Number")
							placeHolderText: qsTr("Enter serial number")

							onEditingFinished: {
								if (!deviceEditorContainer.deviceData) {
									return
								}

								let serialNumber = deviceEditorContainer.deviceData.m_serialNumber
								deviceEditorContainer.doUpdateModel()

								if (serialNumber !== text) {
									deviceEditorContainer.checkFinishedStatus()
								}
							}

							KeyNavigation.tab: macAddressInput
							KeyNavigation.backtab: descriptionInput
						}

						MacAddressElementView {
							id: macAddressInput
							objectName: "MacAddressInput"

							onEditingFinished: {
								if (!deviceEditorContainer.deviceData) {
									return
								}

								let macAddress = deviceEditorContainer.deviceData.m_macAddress
								deviceEditorContainer.doUpdateModel()

								if (macAddress !== text) {
									deviceEditorContainer.checkFinishedStatus()
								}
							}

							KeyNavigation.tab: productSelect
							KeyNavigation.backtab: serialNumberInput
						}
					}
				}
			}
		}
	}

	// --- Production: order, production status, project, internal use ---
	Component {
		id: productionPageComp

		Item {
			id: productionPage
			anchors.fill: parent

			property alias orderSelect: orderSelect
			property alias statusCB: statusCB
			property alias projectInput: projectInput
			property alias internalUseSwitchElementView: internalUseSwitchElementView

			function updateGui() {
				if (!deviceEditorContainer.deviceData) {
					return
				}

				projectInput.text = deviceEditorContainer.deviceData.m_project

				statusCB.currentIndex = -1

				let status = deviceEditorContainer.deviceData.m_productionStatus
				let statusModel = statusCB.model
				if (statusModel) {
					let index = productionStatus.getStatusIndex(status)
					if (index >= 0) {
						statusCB.currentIndex = index
					}
				}

				orderSelect.selectedId = deviceEditorContainer.deviceData.m_orderId
				orderSelect.selectedText = deviceEditorContainer.deviceData.m_orderName

				internalUseSwitchElementView.checked = deviceEditorContainer.deviceData.m_internalUse
			}

			function updateModel() {
				if (!deviceEditorContainer.deviceData) {
					return
				}

				if (PermissionsController.checkPermission("ChangeOrderForSensor")) {
					deviceEditorContainer.deviceData.m_orderId = orderSelect.selectedId
					deviceEditorContainer.deviceData.m_orderName = orderSelect.selectedText
				}

				deviceEditorContainer.deviceData.m_project = projectInput.text

				if (statusCB.currentIndex >= 0 && statusCB.model) {
					deviceEditorContainer.deviceData.m_productionStatus = productionStatus.getStatusId(statusCB.currentIndex)
				}
				else {
					deviceEditorContainer.deviceData.m_productionStatus = ""
				}

				deviceEditorContainer.deviceData.m_internalUse = internalUseSwitchElementView.checked
			}

			CustomScrollbar {
				id: productionScrollbar
				z: parent.z + 1
				anchors.right: parent.right
				anchors.top: productionFlickable.top
				anchors.bottom: productionFlickable.bottom
				secondSize: Style.marginM
				targetItem: productionFlickable
				alwaysVisible: false
			}

			Flickable {
				id: productionFlickable
				anchors.fill: parent
				anchors.margins: Style.marginXL
				anchors.rightMargin: Style.marginXL + productionScrollbar.secondSize
				contentWidth: width
				contentHeight: productionColumn.height + Style.marginXL * 2
				flickableDirection: Flickable.VerticalFlick
				boundsBehavior: Flickable.StopAtBounds
				clip: true
				interactive: true

				Column {
					id: productionColumn
					x: Math.max(0, (productionFlickable.width - width) / 2)
					width: Math.max(0, Math.min(deviceEditorContainer.contentMaxWidth, productionFlickable.width - 2 * Style.marginXL))
					spacing: Style.marginXL

					GroupHeaderView {
						id: productionHeaderView
						objectName: "ProductionInformationHeader"
						width: parent.width
						groupView: productionInformationGroup
						title: qsTr("Production Information")
					}

					GroupElementView {
						id: productionInformationGroup
						objectName: "productionInformationGroup"
						width: parent.width

						CollectionSelectElementView {
							id: orderSelect
							objectName: "OrderCombo"
							name: qsTr("Order-ID")
							commandId: ProlifeOrdersSdlCommandIds.s_ordersList
							fields: [OrderItemTypeMetaInfo.s_id, OrderItemTypeMetaInfo.s_orderId, OrderItemTypeMetaInfo.s_customerName]
							titleField: OrderItemTypeMetaInfo.s_orderId
							descriptionField: OrderItemTypeMetaInfo.s_customerName
							textFilterFieldIds: [OrderItemTypeMetaInfo.s_orderId, OrderItemTypeMetaInfo.s_customerLink]
							placeHolderText: qsTr("Select an order")
							filterPlaceholder: qsTr("Search by order or customer")
							clearable: true
							KeyNavigation.tab: statusCB
							KeyNavigation.backtab: internalUseSwitchElementView

							onItemSelected: {
								deviceEditorContainer.doUpdateModel()
							}
						}

						ClearableComboBoxElementView {
							id: statusCB
							objectName: "ProductionStatusCombo"
							name: qsTr("Production Status")
							model: productionStatus.m_statusModel
							nameId: "m_name"
							KeyNavigation.tab: projectInput
							KeyNavigation.backtab: orderSelect

							onCurrentIndexChanged: {
								deviceEditorContainer.doUpdateModel()

								if (statusCB.currentIndex < 0) {
									statusCB.model = productionStatus.m_statusModel
								}
							}
						}

						TextInputElementView {
							id: projectInput
							objectName: "ProjectInput"
							name: qsTr("Project")
							placeHolderText: qsTr("Enter the project")
							readOnly: deviceEditorContainer.readOnly
							KeyNavigation.tab: internalUseSwitchElementView
							KeyNavigation.backtab: statusCB

							onEditingFinished: {
								deviceEditorContainer.doUpdateModel()
							}
						}

						SwitchElementView {
							id: internalUseSwitchElementView
							objectName: "InternalUseSwitch"
							name: qsTr("Internal Use")
							description: qsTr("Activate if the sensor is for internal use")
							readOnly: deviceEditorContainer.readOnly
							KeyNavigation.tab: orderSelect
							KeyNavigation.backtab: projectInput
							onCheckedChanged: {
								deviceEditorContainer.doUpdateModel()
							}
						}
					}
				}
			}
		}
	}

	// --- Licenses: software bindings table ---
	Component {
		id: licensesPageComp

		Item {
			id: licensesPage
			anchors.fill: parent

			property alias licenseInformationTable: licenseInformationTable

			function updateGui() {
				if (!deviceEditorContainer.deviceData) {
					return
				}
				if (deviceEditorContainer.deviceData.hasSoftwareBindingInfos()) {
					licenseInformationTable.table.elements = deviceEditorContainer.deviceData.m_softwareBindingInfos
				}
			}

			CustomScrollbar {
				id: licensesScrollbar
				z: parent.z + 1
				anchors.right: parent.right
				anchors.top: licensesFlickable.top
				anchors.bottom: licensesFlickable.bottom
				secondSize: Style.marginM
				targetItem: licensesFlickable
				alwaysVisible: false
			}

			Flickable {
				id: licensesFlickable
				anchors.fill: parent
				anchors.margins: Style.marginXL
				anchors.rightMargin: Style.marginXL + licensesScrollbar.secondSize
				contentWidth: width
				contentHeight: licensesColumn.height + Style.marginXL * 2
				flickableDirection: Flickable.VerticalFlick
				boundsBehavior: Flickable.StopAtBounds
				clip: true
				interactive: true

				Column {
					id: licensesColumn
					x: Math.max(0, (licensesFlickable.width - width) / 2)
					width: Math.max(0, Math.min(deviceEditorContainer.contentMaxWidth, licensesFlickable.width - 2 * Style.marginXL))
					spacing: Style.marginXL

					GroupHeaderView {
						width: parent.width
						objectName: "LicenseInformationHeader"
						groupView: licenseInformationGroup
						title: qsTr("License Information")
					}

					GroupElementView {
						id: licenseInformationGroup
						objectName: "LicenseInformationGroup"
						width: parent.width

						TableElementView {
							id: licenseInformationTable
							objectName: "LicenseInformationTable"

							TreeItemModel {
								id: headersModel

								Component.onCompleted: {
									licenseInformationTable.updateHeaders()
								}
							}

							Connections {
								target: licenseInformationTable.table
								function onHeadersChanged() {
									target.setColumnContentById("softwareId", softwareIdLinkDelegateComp)
								}
							}

							function updateHeaders() {
								headersModel.clear()

								let index = headersModel.insertNewItem()
								headersModel.setData("id", "softwareId", index)
								headersModel.setData("name", qsTr("Software-ID"), index)

								index = headersModel.insertNewItem()
								headersModel.setData("id", "softwareName", index)
								headersModel.setData("name", qsTr("Software Name"), index)

								licenseInformationTable.table.headers = headersModel
							}

							Component {
								id: softwareIdLinkDelegateComp

								TextLinkCellDelegate {
									id: objectLinkDelegate
									onLinkActivated: {
										let softwareId = table.elements.getData("m_id", rowIndex)
										NavigationController.navigate("SoftwareProducts/SoftwareProduct/" + softwareId)
									}

									onReused: {
										if (table) {
											text = table.elements.getData("m_softwareId", rowIndex)
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
