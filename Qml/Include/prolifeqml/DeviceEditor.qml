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

/**
 * DeviceEditor
 *
 * MultiPageView: Device / Production / Licenses / Support / History.
 * Support and History are hidden in embedded mode.
 *
 * Embedded mode (Order product dialog): set embedded=true, optionally forcedOrderId.
 * Use loadFromOrderedProduct() / applyToOrderedProduct() to map OrderedProduct.
 */
DocumentViewBase {
	id: deviceEditorContainer

	anchors.fill: parent
	contentColor: Style.baseColor
	commandsPanelVisible: !deviceEditorContainer.embedded

	property TreeItemModel accountsModel: TreeItemModel {}
	property TreeItemModel productsModel: TreeItemModel {}

	property bool orderComboBoxEnabled: true
	property bool deviceTypeComboBoxEnabled: true

	property int radius: 3
	property int spacing: Style.marginM
	property int comboBoxHeight: 27
	property int contentMaxWidth: 900

	property DeviceData deviceData: model ? model : null
	property bool isNew: false
	property bool dialogIsShown: false
	property bool licensePageVisible: false

	// When true, editor is used inside Order product dialog (or similar host).
	property bool embedded: false
	property string forcedOrderId: ""
	property bool hideProductionPage: deviceEditorContainer.embedded

	DeviceProductionStatus {
		id: productionStatus
	}

	Component.onCompleted: {
		if (!CachedProductCollection.completed) {
			CachedProductCollection.updateModel()
		}
		if (!CachedOrderCollection.completed) {
			CachedOrderCollection.updateModel()
		}
		multiPageView.updatePages()
	}

	Connections {
		target: deviceEditorContainer.deviceData
		function onFinished() {
			deviceEditorContainer.deviceDataWasChanged()
		}
	}

	onDeviceDataChanged: {
		deviceEditorContainer.deviceDataWasChanged()
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
				productionPage.orderCB.changeable = true
			}
			devicePage.productCB.changeable = true
			devicePage.configurationCB.changeable = true
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
				productionPage.orderCB.changeable = canChangeOrder

				let canChangeProductionStatus = PermissionsController.checkPermission("ChangeProductionStatus")
				productionPage.statusCB.changeable = canChangeProductionStatus

				let canChangeProject = PermissionsController.checkPermission("ChangeProjectForSensor")
				productionPage.projectInput.readOnly = !canChangeProject
			}

			let canChangeConfiguration = PermissionsController.checkPermission("ChangeHardwareConfiguration")
			let canChangeDevice = PermissionsController.checkPermission("ChangeDeviceType")
			devicePage.configurationCB.changeable = canChangeConfiguration
			devicePage.productCB.changeable = canChangeDevice

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

		devicePage.productCB.enabled = deviceEditorContainer.deviceTypeComboBoxEnabled
		if (productionPage) {
			productionPage.orderCB.enabled = deviceEditorContainer.orderComboBoxEnabled
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
		devicePage.productCB.changeable = !readOnly
		devicePage.configurationCB.changeable = !readOnly
		if (productionPage) {
			productionPage.statusCB.changeable = !readOnly
			productionPage.orderCB.changeable = !readOnly
		}
	}

	function updateGui() {
		if (!deviceData) {
			return
		}

		if (deviceEditorContainer.forcedOrderId !== "" && deviceData.m_orderId !== deviceEditorContainer.forcedOrderId) {
			deviceData.m_orderId = deviceEditorContainer.forcedOrderId
		}

		let devicePage = deviceEditorContainer.getPageItem("Device")
		let productionPage = deviceEditorContainer.hideProductionPage ? null : deviceEditorContainer.getPageItem("Production")
		if (devicePage) {
			devicePage.updateGui()
		}
		if (productionPage) {
			productionPage.updateGui()
		}

		let licensesPage = deviceEditorContainer.getPageItem("Licenses")
		if (licensesPage) {
			licensesPage.updateGui()
		}
	}

	function updateModel() {
		if (!deviceData) {
			return
		}

		let devicePage = deviceEditorContainer.getPageItem("Device")
		let productionPage = deviceEditorContainer.hideProductionPage ? null : deviceEditorContainer.getPageItem("Production")
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
		deviceData.m_licenseName = orderedProduct.m_licenseUuid || orderedProduct.m_licenseName || ""
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
		orderedProduct.m_licenseUuid = deviceData.m_licenseName
		orderedProduct.m_licenseName = deviceData.m_licenseName
		orderedProduct.m_serialNumber = deviceData.m_serialNumber
		orderedProduct.m_macAddress = deviceData.m_macAddress
		orderedProduct.m_expiration = ""
		orderedProduct.m_isMultiple = false
		orderedProduct.m_productCount = 0
		orderedProduct.m_inUse = false

		let devicePage = deviceEditorContainer.getPageItem("Device")
		if (devicePage && devicePage.productCB.currentIndex >= 0 && devicePage.productCB.model) {
			orderedProduct.m_productName = devicePage.productCB.model.getData(ProductItemTypeMetaInfo.s_productName, devicePage.productCB.currentIndex)
		}
		if (devicePage && devicePage.configurationCB.currentIndex >= 0 && devicePage.configurationCB.model) {
			let cfgModel = devicePage.configurationCB.model
			let idx = devicePage.configurationCB.currentIndex
			orderedProduct.m_licenseId = cfgModel.getData(DeviceItemTypeMetaInfo.s_licenseId, idx)
			orderedProduct.m_licenseName = cfgModel.getData(DeviceItemTypeMetaInfo.s_licenseName, idx)
			orderedProduct.m_licenseUuid = cfgModel.getData(LicenseItemTypeMetaInfo.s_id, idx)
		}

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
				anchors.top: parent.top
				anchors.bottom: parent.bottom
				x: Math.max(0, (supportPage.width - width) / 2)
				width: Math.max(0, Math.min(deviceEditorContainer.contentMaxWidth, supportPage.width))
				entityType: "Devices"
				entityId: deviceEditorContainer.deviceData ? deviceEditorContainer.deviceData.m_id : ""
				entityDisplayName: deviceEditorContainer.deviceData ? deviceEditorContainer.deviceData.m_macAddress : ""
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
				documentId: deviceEditorContainer.deviceData ? deviceEditorContainer.deviceData.m_id : ""
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

			property alias productCB: productCB
			property alias configurationCB: configurationCB
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

				productCB.currentIndex = -1

				let productId = deviceEditorContainer.deviceData.m_deviceType
				let productModel = productCB.model
				if (productModel) {
					for (let i = 0; i < productModel.getItemsCount(); i++) {
						let id = productModel.getData(ProductItemTypeMetaInfo.s_id, i)
						if (id === productId) {
							productCB.currentIndex = i
							break
						}
					}
				}

				configurationCB.currentIndex = -1

				let licenseName = deviceEditorContainer.deviceData.m_licenseName
				let model = configurationCB.model
				if (model) {
					for (let i = 0; i < model.getItemsCount(); i++) {
						let id = model.getData(LicenseItemTypeMetaInfo.s_id, i)
						if (id === licenseName) {
							configurationCB.currentIndex = i
							break
						}
					}
				}
			}

			function updateModel() {
				if (!deviceEditorContainer.deviceData) {
					return
				}

				if (productCB.currentIndex >= 0 && productCB.model) {
					let selectedProductId = productCB.model.getData(ProductItemTypeMetaInfo.s_id, productCB.currentIndex)
					deviceEditorContainer.deviceData.m_deviceType = selectedProductId
				}
				else {
					deviceEditorContainer.deviceData.m_deviceType = ""
				}

				let configurationExists = false
				if (configurationCB.model) {
					if (configurationCB.currentIndex >= 0) {
						let configurationType = configurationCB.model.getData(LicenseItemTypeMetaInfo.s_id, configurationCB.currentIndex)
						deviceEditorContainer.deviceData.m_licenseName = configurationType
						configurationExists = true
					}
				}

				if (!configurationExists) {
					deviceEditorContainer.deviceData.m_licenseName = ""
				}

				deviceEditorContainer.deviceData.m_description = descriptionInput.text
				deviceEditorContainer.deviceData.m_serialNumber = serialNumberInput.text
				deviceEditorContainer.deviceData.m_macAddress = macAddressInput.text
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

						ComboBoxElementView {
							id: productCB
							objectName: "DeviceTypeCombo"
							name: qsTr("Device Type")
							model: CachedProductCollection.hardwareProductsModel
							nameId: ProductItemTypeMetaInfo.s_productName
							KeyNavigation.tab: configurationCB
							KeyNavigation.backtab: macAddressInput
							isSelectionRequired: true
							errorText: qsTr("Please select a device type")

							onCurrentIndexChanged: {
								// hardwareProductsModel is assembled with copyItemDataFromModel, which
								// does not carry the nested "licenses" sub-model across, so reading it
								// straight off the copy yields nothing and the configuration combo ends
								// up with no model at all - and a model-less ComboBox refuses to open,
								// which is why the field looked completely unresponsive. Fall back to
								// the source collection (where the sub-model does live), then to an
								// empty sub-model, mirroring SoftwareEditor's product combo.
								let licensesModel = null
								if (productCB.currentIndex >= 0 && productCB.model) {
									licensesModel = productCB.model.getData(ProductItemTypeMetaInfo.s_licenses, productCB.currentIndex)
									if (!licensesModel) {
										let productId = productCB.model.getData(ProductItemTypeMetaInfo.s_id, productCB.currentIndex)
										licensesModel = CachedProductCollection.getLicensesModel(productId)
									}
									if (!licensesModel) {
										licensesModel = productCB.model.addTreeModel(ProductItemTypeMetaInfo.s_licenses, productCB.currentIndex)
									}
								}

								configurationCB.model = licensesModel
								configurationCB.currentIndex = -1
								deviceEditorContainer.doUpdateModel()
							}

							onModelChanged: {
								deviceEditorContainer.doUpdateGui()
							}
						}

						ComboBoxElementView {
							id: configurationCB
							objectName: "HardwareConfigurationCombo"
							name: qsTr("Hardware Configuration")
							nameId: LicenseItemTypeMetaInfo.s_licenseName
							KeyNavigation.tab: articleText
							KeyNavigation.backtab: productCB
							isSelectionRequired: true
							errorText: qsTr("Please select a configuration")
							delegate: Component {
								FilterableComboBoxDelegate {
									width: configurationCB.width
									text: model[DeviceItemTypeMetaInfo.s_licenseName]
									comboBoxRef: configurationCB.cbRef
									description: model[DeviceItemTypeMetaInfo.s_licenseId]
								}
							}

							onCurrentIndexChanged: {
								if (currentIndex >= 0) {
									if (model) {
										articleText.text = model.getData(DeviceItemTypeMetaInfo.s_licenseId, currentIndex)
									}
									deviceEditorContainer.doUpdateModel()
								}
							}
						}

						TextInputElementView {
							id: articleText
							objectName: "ArticleInput"
							name: qsTr("Article")
							readOnly: true
							KeyNavigation.tab: descriptionInput
							KeyNavigation.backtab: configurationCB
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

							KeyNavigation.tab: productCB
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

			property alias orderCB: orderCB
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

				orderCB.currentIndex = -1

				let orderId = deviceEditorContainer.deviceData.m_orderId
				let ordersModel = orderCB.sourceModel
				if (ordersModel) {
					for (let i = 0; i < ordersModel.getItemsCount(); i++) {
						let id = ordersModel.getData(OrderItemTypeMetaInfo.s_id, i)
						if (id === orderId) {
							orderCB.currentIndex = i
							break
						}
					}
				}

				internalUseSwitchElementView.checked = deviceEditorContainer.deviceData.m_internalUse
			}

			function updateModel() {
				if (!deviceEditorContainer.deviceData) {
					return
				}

				let canChangeOrder = PermissionsController.checkPermission("ChangeOrderForSensor")
				if (canChangeOrder) {
					if (orderCB.currentIndex >= 0) {
						let selectedOrderId = orderCB.sourceModel.getData(OrderItemTypeMetaInfo.s_id, orderCB.currentIndex)
						deviceEditorContainer.deviceData.m_orderId = selectedOrderId
					}
					else {
						deviceEditorContainer.deviceData.m_orderId = ""
					}
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

						FilterableComboBoxElementView {
							id: orderCB
							objectName: "OrderCombo"
							name: qsTr("Order-ID")
							nameId: OrderItemTypeMetaInfo.s_orderId
							filteringFields: [OrderItemTypeMetaInfo.s_orderId, OrderItemTypeMetaInfo.s_customerName]
							sourceModel: CachedOrderCollection.collectionModel
							KeyNavigation.tab: statusCB
							KeyNavigation.backtab: internalUseSwitchElementView

							delegate: Component {
								FilterableComboBoxDelegate {
									width: comboBoxRef ? comboBoxRef.width : 0
									comboBoxRef: orderCB.cbRef
									description: qsTr("Customer") + ": " + model[OrderItemTypeMetaInfo.s_customerName]
								}
							}

							onFinished: {
								deviceEditorContainer.doUpdateModel()
							}

							onModelChanged: {
								deviceEditorContainer.doUpdateGui()
							}
						}

						ClearableComboBoxElementView {
							id: statusCB
							objectName: "ProductionStatusCombo"
							name: qsTr("Production Status")
							model: productionStatus.m_statusModel
							nameId: "m_name"
							KeyNavigation.tab: projectInput
							KeyNavigation.backtab: orderCB

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
							KeyNavigation.tab: orderCB
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
