import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import imtguigql 1.0
import prolifeqml 1.0
import prolifeLicensesSdl 1.0
import prolifeOrdersSdl 1.0
import imtlicProductsSdl 1.0
import imtlicLicensesSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

/**
 * SoftwareEditor
 *
 * MultiPageView layout (domain-driven):
 *   General    — license identity: product/license/article/software-id + internal use.
 *                Required create/edit fields.
 *   Additional — project and order (secondary context, not the core license definition).
 *   Expiration — validity of the entitlement (unlimited / concrete date).
 *   Hierarchy  — parent/child license tree (only when the license has hierarchy).
 *
 * Product, license and order are picked from server-side searchable lists; the names
 * shown for the current values come with the document itself.
 *
 * Embedded mode (Order product dialog):
 *   Set embedded=true to hide history/commands chrome and optionally pin order via
 *   forcedOrderUuid. Use loadFromOrderedProduct() / applyToOrderedProduct() to map
 *   to/from OrderedProduct used by OrderEditor.
 */
ViewBase {
	id: root

	contentColor: Style.baseColor
	commandsPanelVisible: !root.embedded

	property SoftwareProductData softwareProductData: model
	property bool isNew: false
	property bool showLicenseHierarchy: softwareProductData && (softwareProductData.m_hasChildren || softwareProductData.m_hasParent)
	property int contentMaxWidth: 900
	property Rectangle background: null
	property bool expanded: false

	// When true, editor is used inside Order product dialog (or similar host).
	property bool embedded: false
	// When non-empty, Additional order is forced to this uuid (and Additional page can be hidden).
	property string forcedOrderUuid: ""
	property bool hideAdditionalPage: root.embedded && root.forcedOrderUuid !== ""

	onShowLicenseHierarchyChanged: {
		if (!showLicenseHierarchy && expanded) {
			expanded = false
		}
		multiPageView.updatePages()
	}

	Component.onCompleted: {
		multiPageView.updatePages()
	}

	onVisibleChanged: {
		if (visible) {
			checkInUse()
		}
		else {
			setAlertPanel(undefined)
		}
	}

	onSoftwareProductDataChanged: {
		// Page set depends on showLicenseHierarchy / hideAdditionalPage only;
		// do not rebuild MultiPageView here — it reloads pages and races with
		// ViewBase.doUpdateGui (empty controls write back into the model).
		checkPermissions()
		checkInUse()
	}

	Component {
		id: alertComp
		AlertMessage {
			message: qsTr("The product cannot be edited as it is in use.")
		}
	}

	function ensurePages() {
		if (multiPageView.pagesModel.count === 0) {
			multiPageView.updatePages()
		}
	}

	function getPageItem(pageId) {
		root.ensurePages()
		let idx = multiPageView.getIndexById(pageId)
		if (idx < 0) {
			return null
		}
		multiPageView.ensurePageLoaded(idx)
		return multiPageView.getPageByIndex(idx)
	}

	function checkPermissions() {
		if (!softwareProductData) {
			return
		}

		// Forced read-only (e.g. linked order product) must win over field permissions.
		if (root.readOnly) {
			setReadOnly(true)
			return
		}

		if (softwareProductData.m_inUse) {
			setReadOnly(true)
			return
		}

		let generalPage = root.getPageItem("General")
		let additionalPage = root.hideAdditionalPage ? null : root.getPageItem("Additional")
		let expirationPage = root.getPageItem("Expiration")
		if (!generalPage || !expirationPage) {
			return
		}

		let canAddLicense = PermissionsController.checkPermission("AddLicense")
		if (isNew && canAddLicense) {
			setReadOnly(false)
		}
		else {
			if (additionalPage) {
				let canChangeProject = PermissionsController.checkPermission("ChangeProjectForLicense")
				additionalPage.projectInput.readOnly = !canChangeProject

				let canChangeOrder = PermissionsController.checkPermission("ChangeOrderForLicense")
				additionalPage.orderSelect.changeable = canChangeOrder
			}

			let canChangeProduct = PermissionsController.checkPermission("ChangeProductForLicense")
			generalPage.productSelect.changeable = canChangeProduct

			let canChangeLicense = PermissionsController.checkPermission("ChangeProductLicenses")
			generalPage.licenseSelect.changeable = canChangeLicense

			let canChangeLicenseNumber = PermissionsController.checkPermission("ChangeLicenseNumber")
			generalPage.serialNumberInput.readOnly = !canChangeLicenseNumber

			let canChangeExpiration = PermissionsController.checkPermission("ChangeExpiration")
			expirationPage.expirationEditor.readOnly = !canChangeExpiration
			expirationPage.unlimitedSwitch.readOnly = !canChangeExpiration

			let ok =
				canChangeProduct ||
				canChangeLicense ||
				canChangeLicenseNumber ||
				canChangeExpiration ||
				PermissionsController.checkPermission("ChangeProjectForLicense") ||
				PermissionsController.checkPermission("ChangeOrderForLicense")

			if (commandsController && !root.embedded) {
				commandsController.setCommandVisible("Undo", ok)
				commandsController.setCommandVisible("Redo", ok)
				commandsController.setCommandVisible("Save", ok)
			}
		}
	}

	function checkInUse() {
		if (!softwareProductData) {
			return
		}

		if (root.readOnly) {
			setReadOnly(true)
			setAlertPanel(undefined)
			return
		}

		if (softwareProductData.m_inUse) {
			setReadOnly(true)
			setAlertPanel(alertComp)
		}
		else {
			setReadOnly(false)
			setAlertPanel(undefined)
		}
	}

	function setReadOnly(readOnly) {
		let generalPage = root.getPageItem("General")
		let additionalPage = root.hideAdditionalPage ? null : root.getPageItem("Additional")
		let expirationPage = root.getPageItem("Expiration")
		if (!generalPage || !expirationPage) {
			return
		}

		if (additionalPage) {
			additionalPage.projectInput.readOnly = readOnly
			additionalPage.orderSelect.changeable = !readOnly
		}
		generalPage.productSelect.changeable = !readOnly
		generalPage.licenseSelect.changeable = !readOnly
		generalPage.serialNumberInput.readOnly = readOnly
		generalPage.internalUseSwitchElementView.readOnly = readOnly
		expirationPage.expirationEditor.readOnly = readOnly
		expirationPage.unlimitedSwitch.readOnly = readOnly
	}

	function updateGui() {
		// No model yet is a normal state while the editor is being built - DeviceEditor and
		// OrderEditor return quietly here too. Logging it as an error meant every "New license"
		// printed one before the model arrived.
		if (!softwareProductData) {
			return
		}

		if (root.forcedOrderUuid !== "" && softwareProductData.m_orderUuid !== root.forcedOrderUuid) {
			softwareProductData.m_orderUuid = root.forcedOrderUuid
		}

		let generalPage = root.getPageItem("General")
		let additionalPage = root.hideAdditionalPage ? null : root.getPageItem("Additional")
		let expirationPage = root.getPageItem("Expiration")
		if (generalPage) {
			generalPage.updateGui()
		}
		if (additionalPage) {
			additionalPage.updateGui()
		}
		if (expirationPage) {
			expirationPage.updateGui()
		}

		let hierarchyPage = root.getPageItem("Hierarchy")
		if (hierarchyPage && hierarchyPage.updateGui) {
			hierarchyPage.updateGui()
		}
	}

	function updateModel() {
		if (!softwareProductData) {
			return
		}

		let generalPage = root.getPageItem("General")
		let additionalPage = root.hideAdditionalPage ? null : root.getPageItem("Additional")
		let expirationPage = root.getPageItem("Expiration")
		if (generalPage) {
			generalPage.updateModel()
		}
		if (additionalPage) {
			additionalPage.updateModel()
		}
		if (expirationPage) {
			expirationPage.updateModel()
		}

		if (root.forcedOrderUuid !== "") {
			softwareProductData.m_orderUuid = root.forcedOrderUuid
		}
	}

	// --- OrderedProduct bridge (OrderEditor product dialog) ------------------------------------

	function loadFromOrderedProduct(orderedProduct) {
		if (!softwareProductData || !orderedProduct) {
			return
		}

		softwareProductData.m_id = orderedProduct.m_id || ""
		softwareProductData.m_productId = orderedProduct.m_productUuid || ""
		softwareProductData.m_productName = orderedProduct.m_productName || ""
		softwareProductData.m_licenseUuid = orderedProduct.m_licenseUuid || ""
		softwareProductData.m_licenseName = orderedProduct.m_licenseName || ""
		softwareProductData.m_licenseId = orderedProduct.m_licenseId || ""
		softwareProductData.m_serialNumber = orderedProduct.m_serialNumber || ""
		softwareProductData.m_expiration = orderedProduct.m_expiration || ""
		softwareProductData.m_inUse = orderedProduct.m_inUse || false
		softwareProductData.m_categoryId = "Software"
		if (root.forcedOrderUuid !== "") {
			softwareProductData.m_orderUuid = root.forcedOrderUuid
		}
		root.doUpdateGui()
	}

	function applyToOrderedProduct(orderedProduct) {
		if (!softwareProductData || !orderedProduct) {
			return false
		}

		root.doUpdateModel()

		orderedProduct.m_isNew = true
		orderedProduct.m_categoryId = "Software"
		orderedProduct.m_id = softwareProductData.m_id !== "" ? softwareProductData.m_id : orderedProduct.m_id
		orderedProduct.m_productUuid = softwareProductData.m_productId
		orderedProduct.m_productName = softwareProductData.m_productName
		orderedProduct.m_licenseUuid = softwareProductData.m_licenseUuid
		orderedProduct.m_licenseName = softwareProductData.m_licenseName
		orderedProduct.m_licenseId = softwareProductData.m_licenseId
		orderedProduct.m_serialNumber = softwareProductData.m_serialNumber
		orderedProduct.m_expiration = softwareProductData.m_expiration
		orderedProduct.m_inUse = softwareProductData.m_inUse
		orderedProduct.m_macAddress = ""

		return orderedProduct.m_productUuid !== "" && orderedProduct.m_licenseUuid !== ""
	}

	function isOrderedProductAcceptable() {
		if (!softwareProductData) {
			return false
		}
		root.doUpdateModel()
		return softwareProductData.m_productId !== "" && softwareProductData.m_licenseUuid !== ""
	}

	MultiPageView {
		id: multiPageView
		anchors.fill: parent
		// Ensure page Flickables receive a real height and clip correctly when hosted in a dialog.
		clip: true
		panelWidth: Style.sizeHintXXS

		function updatePages() {
			let currentId = ""
			if (multiPageView.currentIndex >= 0 && multiPageView.currentIndex < multiPageView.pagesModel.count) {
				currentId = multiPageView.pagesModel.get(multiPageView.currentIndex).id
			}

			multiPageView.clear()
			// General: license identity (required create/edit fields).
			// Additional: project / order context.
			// Expiration: validity of the entitlement.
			// Hierarchy: parent/child structure when applicable.
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Key")
			if (!root.hideAdditionalPage) {
				multiPageView.addPage("Additional", qsTr("Additional"), additionalPageComp, "Icons/Assignment")
			}
			multiPageView.addPage("Expiration", qsTr("Expiration"), expirationPageComp, "Icons/Calendar")
			if (root.showLicenseHierarchy && !root.embedded) {
				multiPageView.addPage("Hierarchy", qsTr("Hierarchy"), hierarchyPageComp, "Icons/Workflow")
			}
			if (!root.embedded && PermissionsController.checkPermission("ViewRevisions")) {
				multiPageView.addPage("History", qsTr("History"), historyPageComp, "Icons/History")
			}

			let targetIndex = multiPageView.getIndexById(currentId)
			multiPageView.currentIndex = targetIndex >= 0 ? targetIndex : 0
		}

		onPageLoaded: {
			// Always go through doUpdateGui so blockingUpdateModel is set.
			// Direct pageItem.updateGui() lets ComboBox/Switch handlers call
			// doUpdateModel(), and root.updateModel() then overwrites the whole
			// document from still-empty sibling pages.
			root.doUpdateGui()
			if (root.readOnly)
				root.setReadOnly(true)
			else
				root.checkPermissions()
		}
	}

	Component {
		id: historyPageComp

		DocumentHistoryPanel {
			documentId: root.softwareProductData ? root.softwareProductData.m_id : ""
			collectionId: "SoftwareProducts"
		}
	}

	// --- General: license identity + options + assignment (project/order) ---
	Component {
		id: generalPageComp

		Item {
			id: generalPage
			anchors.fill: parent

			property alias productSelect: productSelect
			property alias licenseSelect: licenseSelect
			property alias articleText: articleText
			property alias serialNumberInput: serialNumberInput
			property alias internalUseSwitchElementView: internalUseSwitchElementView

			function updateGui() {
				if (!root.softwareProductData) {
					return
				}

				productSelect.selectedId = root.softwareProductData.m_productId
				productSelect.selectedText = root.softwareProductData.m_productName
				licenseSelect.selectedId = root.softwareProductData.m_licenseUuid
				licenseSelect.selectedText = root.softwareProductData.m_licenseName
				articleText.text = root.softwareProductData.m_licenseId
				internalUseSwitchElementView.checked = root.softwareProductData.m_internalUse
				serialNumberInput.text = root.softwareProductData.m_serialNumber
			}

			function updateModel() {
				if (!root.softwareProductData) {
					return
				}

				root.softwareProductData.m_productId = productSelect.selectedId
				root.softwareProductData.m_productName = productSelect.selectedText
				root.softwareProductData.m_licenseUuid = licenseSelect.selectedId
				root.softwareProductData.m_licenseName = licenseSelect.selectedText
				root.softwareProductData.m_licenseId = articleText.text
				root.softwareProductData.m_internalUse = internalUseSwitchElementView.checked
				root.softwareProductData.m_serialNumber = serialNumberInput.text
			}

			FieldFilter {
				id: softwareCategoryFilter
				m_fieldId: "CategoryId"
				m_filterValue: "Software"
				m_filterValueType: "String"
				m_filterOperations: ["Equal"]
			}

			GroupFilter {
				id: softwareCategoryGroup
				m_logicalOperation: "And"

				Component.onCompleted: {
					softwareCategoryGroup.emplaceFieldFilters()
					softwareCategoryGroup.m_fieldFilters.addElement(softwareCategoryFilter)
				}
			}

			FieldFilter {
				id: productLicensesFilter
				m_fieldId: "ProductId"
				m_filterValue: productSelect.selectedId
				m_filterValueType: "String"
				m_filterOperations: ["Equal"]
			}

			GroupFilter {
				id: productLicensesGroup
				m_logicalOperation: "And"

				Component.onCompleted: {
					productLicensesGroup.emplaceFieldFilters()
					productLicensesGroup.m_fieldFilters.addElement(productLicensesFilter)
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
					width: Math.max(0, Math.min(root.contentMaxWidth, generalFlickable.width - 2 * Style.marginXL))
					spacing: Style.marginXL

					// Identity of the license instance — required for create/save.
					GroupHeaderView {
						width: parent.width
						objectName: "LicenseInformationHeader"
						title: qsTr("License Information")
						groupView: licenseGroup
					}

					GroupElementView {
						id: licenseGroup
						objectName: "LicenseInformationGroup"
						width: parent.width

						CollectionSelectElementView {
							id: productSelect
							objectName: "ProductCombo"
							name: qsTr("Product")
							commandId: ImtlicProductsSdlCommandIds.s_productsList
							fields: [ProductItemTypeMetaInfo.s_id, ProductItemTypeMetaInfo.s_productName]
							titleField: ProductItemTypeMetaInfo.s_productName
							textFilterFieldIds: [ProductItemTypeMetaInfo.s_productName]
							sortByField: ProductItemTypeMetaInfo.s_productName
							groupFilters: [softwareCategoryGroup]
							placeHolderText: qsTr("Select a product")
							filterPlaceholder: qsTr("Search by product name")
							changeable: !root.readOnly
							KeyNavigation.tab: licenseSelect
							KeyNavigation.backtab: internalUseSwitchElementView
							isSelectionRequired: true
							errorText: qsTr("Please select a product")

							onItemSelected: {
								licenseSelect.selectedId = ""
								licenseSelect.selectedText = ""
								articleText.text = ""
								root.doUpdateModel()
							}
						}

						CollectionSelectElementView {
							id: licenseSelect
							objectName: "LicenseCombo"
							name: qsTr("Licenses")
							commandId: ImtlicLicensesSdlCommandIds.s_licensesList
							fields: [LicenseItemTypeMetaInfo.s_id, LicenseItemTypeMetaInfo.s_licenseName, LicenseItemTypeMetaInfo.s_licenseId]
							titleField: LicenseItemTypeMetaInfo.s_licenseName
							descriptionField: LicenseItemTypeMetaInfo.s_licenseId
							textFilterFieldIds: [LicenseItemTypeMetaInfo.s_licenseName, LicenseItemTypeMetaInfo.s_licenseId]
							sortByField: LicenseItemTypeMetaInfo.s_licenseName
							groupFilters: [productLicensesGroup]
							placeHolderText: productSelect.selectedId !== "" ? qsTr("Select a license") : qsTr("Select a product first")
							filterPlaceholder: qsTr("Search by license name or article")
							KeyNavigation.tab: articleText
							KeyNavigation.backtab: productSelect
							isSelectionRequired: true
							errorText: qsTr("Please select a license")

							onItemSelected: {
								articleText.text = licenseSelect.itemValue(item, LicenseItemTypeMetaInfo.s_licenseId)
								root.doUpdateModel()
							}
						}

						TextInputElementView {
							id: articleText
							objectName: "ArticleInput"
							name: qsTr("Article")
							readOnly: true
							KeyNavigation.tab: serialNumberInput
							KeyNavigation.backtab: licenseSelect
						}

						TextInputElementView {
							id: serialNumberInput
							objectName: "SerialNumberInput"
							placeHolderText: qsTr("Enter the software-ID")
							name: qsTr("Software-ID")
							KeyNavigation.tab: internalUseSwitchElementView
							KeyNavigation.backtab: articleText
							textInputValidator: serialNumberRegexp
							showErrorWhenInvalid: true
							errorText: qsTr("Please enter the software-ID")

							onEditingFinished: {
								root.doUpdateModel()
							}
						}

						RegularExpressionValidator {
							id: serialNumberRegexp
							regularExpression: /^(?!\s*$).+/
						}

						SwitchElementView {
							id: internalUseSwitchElementView
							objectName: "InternalUseSwitch"
							name: qsTr("Internal Use")
							description: qsTr("Activate if the license is for internal use")
							readOnly: root.readOnly
							KeyNavigation.tab: productSelect
							KeyNavigation.backtab: serialNumberInput
							onCheckedChanged: {
								root.doUpdateModel()
							}
						}
					}
				}
			}
		}
	}

	// --- Additional: project / order (secondary context) ---
	Component {
		id: additionalPageComp

		Item {
			id: additionalPage
			anchors.fill: parent

			property alias projectInput: projectInput
			property alias orderSelect: orderSelect

			function updateGui() {
				if (!root.softwareProductData) {
					return
				}

				projectInput.text = root.softwareProductData.m_project
				orderSelect.selectedId = root.softwareProductData.m_orderUuid
				orderSelect.selectedText = root.softwareProductData.m_orderName
			}

			function updateModel() {
				if (!root.softwareProductData) {
					return
				}

				root.softwareProductData.m_project = projectInput.text

				if (PermissionsController.checkPermission("ChangeOrderForLicense")) {
					root.softwareProductData.m_orderUuid = orderSelect.selectedId
					root.softwareProductData.m_orderName = orderSelect.selectedText
				}
			}

			CustomScrollbar {
				id: additionalScrollbar
				z: parent.z + 1
				anchors.right: parent.right
				anchors.top: additionalFlickable.top
				anchors.bottom: additionalFlickable.bottom
				secondSize: Style.marginM
				targetItem: additionalFlickable
				alwaysVisible: false
			}

			Flickable {
				id: additionalFlickable
				anchors.fill: parent
				anchors.margins: Style.marginXL
				anchors.rightMargin: Style.marginXL + additionalScrollbar.secondSize
				contentWidth: width
				contentHeight: additionalColumn.height + Style.marginXL * 2
				flickableDirection: Flickable.VerticalFlick
				boundsBehavior: Flickable.StopAtBounds
				clip: true
				interactive: true

				Column {
					id: additionalColumn
					x: Math.max(0, (additionalFlickable.width - width) / 2)
					width: Math.max(0, Math.min(root.contentMaxWidth, additionalFlickable.width - 2 * Style.marginXL))
					spacing: Style.marginXL

					GroupHeaderView {
						width: parent.width
						objectName: "SoftwareInformationHeader"
						title: qsTr("Additional Information")
						groupView: additionalGroup
					}

					GroupElementView {
						id: additionalGroup
						objectName: "SoftwareInformationGroup"
						width: parent.width

						TextInputElementView {
							id: projectInput
							objectName: "ProjectInput"
							name: qsTr("Project")
							placeHolderText: qsTr("Enter the project")
							readOnly: root.readOnly
							KeyNavigation.tab: orderSelect
							KeyNavigation.backtab: orderSelect

							onEditingFinished: {
								root.doUpdateModel()
							}
						}

						CollectionSelectElementView {
							id: orderSelect
							objectName: "OrderCombo"
							name: qsTr("Order")
							commandId: ProlifeOrdersSdlCommandIds.s_ordersList
							fields: [OrderItemTypeMetaInfo.s_id, OrderItemTypeMetaInfo.s_orderId, OrderItemTypeMetaInfo.s_customerName]
							titleField: OrderItemTypeMetaInfo.s_orderId
							descriptionField: OrderItemTypeMetaInfo.s_customerName
							textFilterFieldIds: [OrderItemTypeMetaInfo.s_orderId, OrderItemTypeMetaInfo.s_customerLink]
							placeHolderText: qsTr("Select an order")
							filterPlaceholder: qsTr("Search by order or customer")
							clearable: true
							changeable: !root.readOnly
							KeyNavigation.tab: projectInput
							KeyNavigation.backtab: projectInput

							onItemSelected: {
								root.doUpdateModel()
							}
						}
					}
				}
			}
		}
	}

	// --- Expiration: unlimited / date ---
	Component {
		id: expirationPageComp

		Item {
			id: expirationPage
			anchors.fill: parent

			property alias unlimitedSwitch: unlimitedSwitch
			property alias expirationEditor: expirationEditor

			function updateGui() {
				if (!root.softwareProductData) {
					return
				}

				let expiration = root.softwareProductData.m_expiration
				let isUnlimited = !expiration || expiration === ""

				if (unlimitedSwitch.switchRef) {
					unlimitedSwitch.switchRef.setChecked(isUnlimited)
				}

				if (!isUnlimited && expirationEditor.datePicker) {
					let currentDate = expirationEditor.getDateAsString()
					if (expiration !== currentDate) {
						expirationEditor.datePicker.setDateAsString(expiration)
					}
				}
			}

			function updateModel() {
				if (!root.softwareProductData) {
					return
				}

				if (!unlimitedSwitch.checked) {
					root.softwareProductData.m_expiration = expirationEditor.getDateAsString()
				}
				else {
					root.softwareProductData.m_expiration = ""
				}
			}

			CustomScrollbar {
				id: expirationScrollbar
				z: parent.z + 1
				anchors.right: parent.right
				anchors.top: expirationFlickable.top
				anchors.bottom: expirationFlickable.bottom
				secondSize: Style.marginM
				targetItem: expirationFlickable
				alwaysVisible: false
			}

			Flickable {
				id: expirationFlickable
				anchors.fill: parent
				anchors.margins: Style.marginXL
				anchors.rightMargin: Style.marginXL + expirationScrollbar.secondSize
				contentWidth: width
				contentHeight: expirationColumn.height + Style.marginXL * 2
				flickableDirection: Flickable.VerticalFlick
				boundsBehavior: Flickable.StopAtBounds
				clip: true
				interactive: true

				Column {
					id: expirationColumn
					x: Math.max(0, (expirationFlickable.width - width) / 2)
					width: Math.max(0, Math.min(root.contentMaxWidth, expirationFlickable.width - 2 * Style.marginXL))
					spacing: Style.marginXL

					GroupHeaderView {
						width: parent.width
						objectName: "ExpirationInformationHeader"
						title: qsTr("Expiration Information")
						groupView: expirationGroup
					}

					GroupElementView {
						id: expirationGroup
						objectName: "ExpirationInformationGroup"
						width: parent.width

						SwitchElementView {
							id: unlimitedSwitch
							objectName: "UnlimitedSwitch"
							name: qsTr("Unlimited")
							description: unlimitedSwitch.checked
								? qsTr("Deactivate it if you want to set the expiration date manually")
								: qsTr("Activate it if you want to set an unlimited expiration date")
							readOnly: root.readOnly
							KeyNavigation.tab: expirationEditor
							KeyNavigation.backtab: expirationEditor

							onCheckedChanged: {
								root.doUpdateModel()
							}

							onSwitchRefChanged: {
								if (switchRef) {
									switchRef.readOnly = unlimitedSwitch.readOnly
								}
							}
						}

						DateTimePickerElementView {
							id: expirationEditor
							objectName: "ExpirationDatePicker"
							name: qsTr("Expiration")
							visible: !unlimitedSwitch.checked
							readOnly: root.readOnly
							KeyNavigation.tab: unlimitedSwitch
							KeyNavigation.backtab: unlimitedSwitch

							onDatePickerChanged: {
								if (datePicker) {
									let currentDate = new Date()
									let date = new Date(currentDate.getFullYear() + 1, currentDate.getMonth(), currentDate.getDate())
									datePicker.setDateAsString(Functions.dateToStr(date, "dd.MM.yyyy"))
								}
							}

							onEditingFinished: {
								root.doUpdateModel()
							}
						}
					}
				}
			}
		}
	}

	// --- Hierarchy: license tree ---
	Component {
		id: hierarchyPageComp

		Item {
			id: hierarchyPage
			anchors.fill: parent

			function updateGui() {
			}

			CustomScrollbar {
				id: hierarchyScrollbar
				z: parent.z + 1
				anchors.right: parent.right
				anchors.top: hierarchyFlickable.top
				anchors.bottom: hierarchyFlickable.bottom
				secondSize: Style.marginM
				targetItem: hierarchyFlickable
				alwaysVisible: false
			}

			Flickable {
				id: hierarchyFlickable
				anchors.fill: parent
				anchors.margins: Style.marginXL
				anchors.rightMargin: Style.marginXL + hierarchyScrollbar.secondSize
				contentWidth: width
				contentHeight: hierarchyColumn.height + Style.marginXL * 2
				flickableDirection: Flickable.VerticalFlick
				boundsBehavior: Flickable.StopAtBounds
				clip: true
				interactive: true

				Column {
					id: hierarchyColumn
					x: Math.max(0, (hierarchyFlickable.width - width) / 2)
					width: Math.max(0, Math.min(root.contentMaxWidth, hierarchyFlickable.width - 2 * Style.marginXL))
					spacing: Style.marginXL

					ElementView {
						name: qsTr("License Hierarchy")
						width: parent.width
						visible: root.showLicenseHierarchy
						bottomComp: canvasComp

						controlComp: Component {
							ToolButton {
								id: expandButton
								width: Style.buttonWidthM
								height: width
								iconSource: "../../../" + Style.getIconPath(
									"Icons/Expand",
									Icon.State.On,
									Icon.Mode.Normal
								)
								onClicked: {
									root.expanded = !root.expanded
								}
							}
						}

						Component {
							id: canvasComp

							Item {
								height: canvasFlickable.height

								CustomScrollbar {
									id: scrollbar
									z: parent.z + 1
									anchors.right: parent.right
									anchors.top: canvasFlickable.top
									anchors.bottom: canvasFlickable.bottom
									secondSize: 10
									targetItem: canvasFlickable
								}

								CustomScrollbar {
									id: scrollHoriz
									z: parent.z + 1
									anchors.left: canvasFlickable.left
									anchors.right: canvasFlickable.right
									anchors.bottom: canvasFlickable.bottom
									secondSize: 10
									vertical: false
									targetItem: canvasFlickable
								}

								Flickable {
									id: canvasFlickable
									width: parent.width
									height: Math.min(licenseTreeCanvas.height, 300)
									contentWidth: licenseTreeCanvas.width
									contentHeight: licenseTreeCanvas.height
									clip: true

									LicenseTreeCanvas {
										id: licenseTreeCanvas
										width: treeWidth
										height: treeHeight
										treeData: root.softwareProductData ? root.softwareProductData.m_licenseTree : null
										currentLicenseId: root.softwareProductData ? root.softwareProductData.m_id : ""
									}
								}
							}
						}
					}
				}
			}
		}
	}

	onExpandedChanged: {
		if (expanded) {
			if (!showLicenseHierarchy) {
				expanded = false
				return
			}
			background = backgroundComp.createObject(root)
		}
		else {
			if (background) {
				background.destroy()
			}
		}
	}

	Component {
		id: backgroundComp
		Rectangle {
			z: parent.z + 10
			anchors.fill: parent
			color: "gray"
			visible: root.expanded
			opacity: 0.4

			ControlArea {
				id: backgroundControlArea
				anchors.fill: parent
				onClicked: {
					root.expanded = false
				}
			}

			ControlArea {
				anchors.fill: fullLicenseTreeCanvas
			}

			ElementView {
				id: fullLicenseTreeCanvas
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.top: parent.top
				anchors.topMargin: Style.sizeHintBXS
				width: parent.width - 2 * Style.sizeHintBXS

				controlComp: Component {
					ToolButton {
						id: collapseButton
						width: Style.buttonWidthM
						height: width
						iconSource: "../../../" + Style.getIconPath("Icons/Collapse", Icon.State.On, Icon.Mode.Normal)
						onClicked: {
							root.expanded = !root.expanded
						}
					}
				}
				bottomComp: Component {
					Item {
						height: licenseTreeCanvas.height
						LicenseTreeCanvas {
							id: licenseTreeCanvas
							anchors.centerIn: parent
							width: treeWidth
							height: treeHeight
							treeData: root.softwareProductData ? root.softwareProductData.m_licenseTree : null
							currentLicenseId: root.softwareProductData ? root.softwareProductData.m_id : ""
						}
					}
				}
			}
		}
	}
}
