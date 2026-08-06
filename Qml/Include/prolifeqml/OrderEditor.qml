import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0
import imtlicgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import prolifeqml 1.0
import prolifeOrdersSdl 1.0
import imtbaseImtCollectionSdl 1.0

/**
 * OrderEditor
 *
 * MultiPageView:
 *   General  — order identity / customer / status (Icons/Settings)
 *   Products — order product lines; sidebar title includes count (Icons/Product)
 */
ViewBase {
	id: orderEditorContainer

	anchors.fill: parent
	contentColor: Style.baseColor

	property TreeItemModel accountsModel: CachedAccountCollection.collectionModel
	property TreeItemModel productsModel: CachedProductCollection.collectionModel
	property TreeItemModel devicesModel: CachedDeviceCollection.collectionModel
	property TreeItemModel softwaresModel: CachedSoftwareCollection.collectionModel
	property TreeItemModel licensesModel: CachedLicenseCollection.collectionModel

	property OrderData orderData: model
	property bool isNew: false
	property int contentMaxWidth: 900

	readonly property int productsCount: {
		if (!orderEditorContainer.orderData)
			return 0
		let products = orderEditorContainer.orderData.m_orderProducts
		return products ? products.count : 0
	}

	Component.onCompleted: {
		if (!CachedAccountCollection.completed) {
			CachedAccountCollection.updateModel()
		}
		multiPageView.updatePages()
	}

	onOrderDataChanged: {
		checkPermissions()
		orderEditorContainer.refreshProductsPageTitle()
	}

	onProductsCountChanged: {
		orderEditorContainer.refreshProductsPageTitle()
	}

	OrderStatus {
		id: orderStatus
	}

	TreeItemModel {
		id: roleTypesModel

		Component.onCompleted: {
			let index = roleTypesModel.insertNewItem()
			roleTypesModel.setData("id", "EndCustomer", index)
			roleTypesModel.setData("name", qsTr("End Customer"), index)

			index = roleTypesModel.insertNewItem()
			roleTypesModel.setData("id", "InvoiceRecipient", index)
			roleTypesModel.setData("name", qsTr("Invoice Recipient"), index)

			index = roleTypesModel.insertNewItem()
			roleTypesModel.setData("id", "DeliveryRecipient", index)
			roleTypesModel.setData("name", qsTr("Delivery Recipient"), index)

			index = roleTypesModel.insertNewItem()
			roleTypesModel.setData("id", "Reseller", index)
			roleTypesModel.setData("name", qsTr("Reseller"), index)

			index = roleTypesModel.insertNewItem()
			roleTypesModel.setData("id", "Referrer", index)
			roleTypesModel.setData("name", qsTr("Referrer"), index)
		}
	}

	function ensurePages() {
		if (multiPageView.pagesModel.count === 0) {
			multiPageView.updatePages()
		}
	}

	function getPageItem(pageId) {
		orderEditorContainer.ensurePages()
		let idx = multiPageView.getIndexById(pageId)
		if (idx < 0) {
			return null
		}
		multiPageView.ensurePageLoaded(idx)
		return multiPageView.getPageByIndex(idx)
	}

	function refreshProductsPageTitle() {
		let idx = multiPageView.getIndexById("Products")
		if (idx < 0) {
			return
		}
		multiPageView.pagesModel.setProperty(idx, "name", qsTr("Products") + " (" + orderEditorContainer.productsCount + ")")
	}

	function setReadOnly(readOnly) {
		let generalPage = orderEditorContainer.getPageItem("General")
		let productsPage = orderEditorContainer.getPageItem("Products")
		if (generalPage) {
			generalPage.instanceIdInput.readOnly = readOnly
			generalPage.purchaseIdInput.readOnly = readOnly
			generalPage.descriptionInput.readOnly = readOnly
			generalPage.customerCB.changeable = !readOnly
			generalPage.orderStatusCB.changeable = !readOnly
		}
		if (productsPage) {
			productsPage.productsView.readOnly = readOnly
			productsPage.addProduct.visible = !readOnly
		}
	}

	function checkPermissions() {
		if (!orderData) {
			return
		}

		let generalPage = orderEditorContainer.getPageItem("General")
		let productsPage = orderEditorContainer.getPageItem("Products")
		if (!generalPage) {
			return
		}

		let canAddOrder = PermissionsController.checkPermission("AddOrder")

		if (isNew && canAddOrder) {
			generalPage.instanceIdInput.readOnly = false
			generalPage.purchaseIdInput.readOnly = false
			generalPage.descriptionInput.readOnly = false
			generalPage.customerCB.changeable = true
			generalPage.orderStatusCB.changeable = true
			if (productsPage) {
				productsPage.addProduct.visible = true
				productsPage.productsView.readOnly = false
			}
		}
		else {
			let canChangeDeliveryId = PermissionsController.checkPermission("ChangeDeliveryId")
			generalPage.instanceIdInput.readOnly = !canChangeDeliveryId

			let canChangePurchaseOrderId = PermissionsController.checkPermission("ChangePurchaseOrderId")
			generalPage.purchaseIdInput.readOnly = !canChangePurchaseOrderId

			let canChangeDescriptionForOrder = PermissionsController.checkPermission("ChangeDescriptionForOrder")
			generalPage.descriptionInput.readOnly = !canChangeDescriptionForOrder

			let canChangeCustomer = PermissionsController.checkPermission("ChangeCustomer")
			generalPage.customerCB.changeable = canChangeCustomer

			let canChangeOrderStatus = PermissionsController.checkPermission("ChangeOrderStatus")
			generalPage.orderStatusCB.changeable = canChangeOrderStatus

			let canChangeOrderProducts = PermissionsController.checkPermission("ChangeOrderProducts")
			if (productsPage) {
				productsPage.addProduct.visible = canChangeOrderProducts
				productsPage.productsView.readOnly = !canChangeOrderProducts
			}

			let ok = canChangeDeliveryId || canChangePurchaseOrderId || canChangeDescriptionForOrder
				|| canChangeCustomer || canChangeOrderStatus || canChangeOrderProducts

			if (commandsController) {
				commandsController.setCommandVisible("Undo", ok)
				commandsController.setCommandVisible("Redo", ok)
				commandsController.setCommandVisible("Save", ok)
			}
		}
	}

	function updateGui() {
		if (!orderData) {
			return
		}

		let generalPage = orderEditorContainer.getPageItem("General")
		let productsPage = orderEditorContainer.getPageItem("Products")
		if (generalPage) {
			generalPage.updateGui()
		}
		if (productsPage) {
			productsPage.updateGui()
		}
		orderEditorContainer.refreshProductsPageTitle()
	}

	function updateModel() {
		if (!orderData) {
			return
		}

		let generalPage = orderEditorContainer.getPageItem("General")
		if (generalPage) {
			generalPage.updateModel()
		}

		if (!orderData.hasOrderProducts()) {
			orderData.emplaceOrderProducts()
		}
	}

	function notifyOrderChanged() {
		if (orderEditorContainer.model) {
			orderEditorContainer.model.modelChanged([])
		}
		orderEditorContainer.refreshProductsPageTitle()
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
			multiPageView.addPage("General", qsTr("General"), generalPageComp, "Icons/Settings")
			multiPageView.addPage(
				"Products",
				qsTr("Products") + " (" + orderEditorContainer.productsCount + ")",
				productsPageComp,
				"Icons/Product"
			)
			if (PermissionsController.checkPermission("ViewRevisions")) {
				multiPageView.addPage("History", qsTr("History"), historyPageComp, "Icons/History")
			}

			let targetIndex = multiPageView.getIndexById(currentId)
			multiPageView.currentIndex = targetIndex >= 0 ? targetIndex : 0
		}

		onPageLoaded: {
			orderEditorContainer.checkPermissions()
			if (pageItem && pageItem.updateGui) {
				pageItem.updateGui()
			}
		}

		Component.onCompleted: {
			multiPageView.updatePages()
		}
	}

	Component {
		id: historyPageComp

		DocumentHistoryPanel {
			documentId: orderEditorContainer.orderData ? orderEditorContainer.orderData.m_id : ""
			collectionId: "Orders"
		}
	}

	// --- General: delivery / purchase / description / customer / status ---
	Component {
		id: generalPageComp

		Item {
			id: generalPage
			anchors.fill: parent

			property alias instanceIdInput: instanceIdInput
			property alias purchaseIdInput: purchaseIdInput
			property alias descriptionInput: descriptionInput
			property alias customerCB: customerCB
			property alias orderStatusCB: orderStatusCB

			function updateGui() {
				if (!orderEditorContainer.orderData) {
					return
				}

				instanceIdInput.text = orderEditorContainer.orderData.m_orderId
				purchaseIdInput.text = orderEditorContainer.orderData.m_purchaseId
				descriptionInput.text = orderEditorContainer.orderData.m_description

				customerCB.currentIndex = -1
				let customerId = orderEditorContainer.orderData.m_customerId
				let customerModel = customerCB.model
				if (customerModel) {
					for (let i = 0; i < customerModel.getItemsCount(); i++) {
						let id = customerModel.getData("id", i)
						if (id === customerId) {
							customerCB.currentIndex = i
							break
						}
					}
				}

				orderStatusCB.currentIndex = -1
				let status = orderEditorContainer.orderData.m_orderStatus
				let statusModel = orderStatus.statusModel
				if (statusModel) {
					orderStatusCB.model = statusModel
					for (let i = 0; i < statusModel.getItemsCount(); i++) {
						let id = statusModel.getData("id", i)
						if (id === status) {
							orderStatusCB.currentIndex = i
							break
						}
					}
				}
			}

			function updateModel() {
				if (!orderEditorContainer.orderData) {
					return
				}

				orderEditorContainer.orderData.m_orderId = instanceIdInput.text
				orderEditorContainer.orderData.m_purchaseId = purchaseIdInput.text
				orderEditorContainer.orderData.m_description = descriptionInput.text

				let selectedAccountId = ""
				if (customerCB.currentIndex >= 0 && customerCB.model) {
					selectedAccountId = customerCB.model.getData("id", customerCB.currentIndex)
				}
				orderEditorContainer.orderData.m_customerId = selectedAccountId

				if (orderStatusCB.currentIndex >= 0 && orderStatusCB.model) {
					orderEditorContainer.orderData.m_orderStatus = orderStatusCB.model.getData("id", orderStatusCB.currentIndex)
				}
				else {
					orderEditorContainer.orderData.m_orderStatus = ""
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
					width: Math.max(0, Math.min(orderEditorContainer.contentMaxWidth, generalFlickable.width - 2 * Style.marginXL))
					spacing: Style.marginXL

					GroupHeaderView {
						objectName: "OrderInformationHeader"
						title: qsTr("Order Information")
						width: parent.width
						groupView: group
					}

					GroupElementView {
						id: group
						objectName: "OrderInformationGroup"
						width: parent.width

						Component {
							id: errorComp
							Text {
								text: qsTr("Enter a 5-digit or 8-digit number")
								color: Style.errorTextColor
								font.family: Style.fontFamily
								font.pixelSize: Style.fontSizeM
							}
						}

						TextInputElementView {
							id: instanceIdInput
							objectName: "DeliveryIdInput"
							name: qsTr("Delivery-ID")
							placeHolderText: qsTr("Enter the delivery-ID")
							readOnly: orderEditorContainer.readOnly
							maximumLength: 8
							KeyNavigation.tab: purchaseIdInput
							KeyNavigation.backtab: orderStatusCB
							bottomComp: acceptableInput ? undefined : errorComp

							onEditingFinished: {
								orderEditorContainer.doUpdateModel()
							}

							Component.onCompleted: {
								validate()
							}

							onTextChanged: {
								validate()
							}

							function validate() {
								let len = instanceIdInput.text.length
								let ok1 = instanceIdInput.test("\\d{5}", instanceIdInput.text) && len === 5
								let ok2 = instanceIdInput.test("\\d{8}", instanceIdInput.text) && len === 8
								instanceIdInput.bottomComp = ok1 || ok2 ? undefined : errorComp
							}

							function test(regex, text) {
								let re = new RegExp(regex)
								if (re) {
									return re.test(text)
								}
								return false
							}
						}

						TextInputElementView {
							id: purchaseIdInput
							objectName: "PurchaseOrderIdInput"
							name: qsTr("Purchase Order-ID")
							placeHolderText: qsTr("Enter the Purchase-ID")
							readOnly: orderEditorContainer.readOnly
							KeyNavigation.tab: descriptionInput
							KeyNavigation.backtab: instanceIdInput

							onEditingFinished: {
								orderEditorContainer.doUpdateModel()
							}
						}

						TextInputElementView {
							id: descriptionInput
							objectName: "DescriptionInput"
							name: qsTr("Description")
							placeHolderText: qsTr("Enter the comment")
							readOnly: orderEditorContainer.readOnly
							KeyNavigation.tab: customerCB
							KeyNavigation.backtab: purchaseIdInput

							onEditingFinished: {
								orderEditorContainer.doUpdateModel()
							}
						}

						ComboBoxElementView {
							id: customerCB
							objectName: "CustomerCombo"
							name: qsTr("Customer")
							model: orderEditorContainer.accountsModel
							changeable: !orderEditorContainer.readOnly
							isSelectionRequired: true
							errorText: qsTr("Please select a customer")
							KeyNavigation.tab: orderStatusCB
							KeyNavigation.backtab: descriptionInput

							onCurrentIndexChanged: {
								orderEditorContainer.doUpdateModel()
							}

							onModelChanged: {
								orderEditorContainer.doUpdateGui()
							}
						}

						ComboBoxElementView {
							id: orderStatusCB
							objectName: "OrderStatusCombo"
							name: qsTr("Order Status")
							changeable: !orderEditorContainer.readOnly
							model: orderStatus.statusModel
							KeyNavigation.tab: instanceIdInput
							KeyNavigation.backtab: customerCB

							onCurrentIndexChanged: {
								orderEditorContainer.doUpdateModel()
								if (orderStatusCB.currentIndex < 0) {
									orderStatusCB.model = orderStatus.statusModel
								}
							}
						}
					}
				}
			}
		}
	}

	// --- Products: list of ordered software / hardware lines ---
	Component {
		id: productsPageComp

		Item {
			id: productsPage
			anchors.fill: parent

			property alias productsView: productsView
			property alias addProduct: addProduct

			function updateGui() {
				if (!orderEditorContainer.orderData) {
					return
				}
				productsView.model = 0
				if (orderEditorContainer.orderData.hasOrderProducts
						&& orderEditorContainer.orderData.hasOrderProducts()) {
					productsView.model = orderEditorContainer.orderData.m_orderProducts
				}
				orderEditorContainer.refreshProductsPageTitle()
			}

			Component {
				id: productFactory
				OrderedProduct {}
			}

			Component {
				id: productEditorDialog

				ProductEditorDialog {
					id: productsDialog

					onStarted: {
						productsDialog.bodyItem.productsModel = orderEditorContainer.productsModel
						productsDialog.bodyItem.devicesModel = orderEditorContainer.devicesModel
						productsDialog.bodyItem.softwaresModel = orderEditorContainer.softwaresModel
						productsDialog.bodyItem.licensesModel = orderEditorContainer.licensesModel
						productsDialog.bodyItem.orderUuid = orderEditorContainer.orderData.m_id
						productsDialog.activeProductIndex = productsView.activeProductIndex
						productsDialog.bodyItem.orderId = orderEditorContainer.orderData.m_orderId
						productsDialog.bodyItem.orderProductsModel = orderEditorContainer.orderData.m_orderProducts
						if (productsView.activeProductIndex >= 0) {
							let productModel = orderEditorContainer.orderData.m_orderProducts.get(productsView.activeProductIndex).item
							productsDialog.bodyItem.productItem = productModel.copyMe()
						}
						else {
							let productItem = productFactory.createObject(orderEditorContainer)
							productItem.m_id = UuidGenerator.generateUUID()
							productItem.m_categoryId = "Software"
							productsDialog.bodyItem.productItem = productItem
						}
						productsDialog.bodyItem.started()
					}

					onFinished: {
						if (buttonId == Enums.ok) {
							let actualOrderProducts = orderEditorContainer.orderData.m_orderProducts
							let index = productsView.activeProductIndex
							if (index < 0) {
								let addProductFunc = function (product) {
									if (actualOrderProducts) {
										actualOrderProducts.insert(0, { "item": product.copyMe() })
									}
								}
								let productItem = productsDialog.bodyItem.productItem
								let instanceCount = productsDialog.bodyItem.instanceCount
								for (let i = 0; i < instanceCount; ++i) {
									if (productItem.m_isNew) {
										productItem.m_id = UuidGenerator.generateUUID()
									}
									addProductFunc(productItem)
								}
							}
							else {
								let productModel = productsDialog.bodyItem.productItem.copyMe()
								if (actualOrderProducts) {
									actualOrderProducts.set(index, { "item": productModel })
								}
							}
							productsView.model = 0
							productsView.model = actualOrderProducts
							orderEditorContainer.notifyOrderChanged()
						}
					}
				}
			}

			CustomScrollbar {
				id: productsScrollbar
				z: parent.z + 1
				anchors.right: parent.right
				anchors.top: productsFlickable.top
				anchors.bottom: productsFlickable.bottom
				secondSize: Style.marginM
				targetItem: productsFlickable
			}

			Flickable {
				id: productsFlickable
				anchors.fill: parent
				anchors.margins: Style.marginXL
				anchors.rightMargin: Style.marginXL + productsScrollbar.secondSize
				contentWidth: width
				contentHeight: productsColumn.height + Style.marginXL * 2
				flickableDirection: Flickable.VerticalFlick
				boundsBehavior: Flickable.StopAtBounds
				clip: true
				interactive: true

				Column {
					id: productsColumn
					x: Math.max(0, (productsFlickable.width - width) / 2)
					width: Math.max(0, Math.min(orderEditorContainer.contentMaxWidth, productsFlickable.width - 2 * Style.marginXL))
					spacing: Style.marginXL

					Item {
						id: productsTitle
						width: parent.width
						height: Math.max(titleLicenses.height, addProduct.height, expandButton.height)

						Text {
							id: titleLicenses
							anchors.left: parent.left
							anchors.verticalCenter: parent.verticalCenter
							text: qsTr("Products") + " (" + orderEditorContainer.productsCount + ")"
							color: Style.textColor
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeXXL
						}

						Row {
							anchors.verticalCenter: parent.verticalCenter
							anchors.right: parent.right
							spacing: Style.marginM

							ToolButton {
								id: expandButton
								objectName: "ExpandProductsButton"
								anchors.verticalCenter: parent.verticalCenter
								width: Style.buttonWidthM
								height: width
								visible: orderEditorContainer.productsCount > 0
								iconSource: !productsView.expanded
									? "../../../" + Style.getIconPath("Icons/DetailedView", Icon.State.On, Icon.Mode.Normal)
									: "../../../" + Style.getIconPath("Icons/CompactView", Icon.State.On, Icon.Mode.Normal)
								tooltipText: !productsView.expanded ? qsTr("Detailed view") : qsTr("Compact view")
								onClicked: {
									productsView.expanded = !productsView.expanded
								}
							}

							Button {
								id: addProduct
								anchors.verticalCenter: parent.verticalCenter
								text: qsTr("Add Product")
								iconSource: "../../../" + Style.getIconPath("Icons/Add", Icon.State.On, Icon.Mode.Normal)

								onClicked: {
									productsView.activeProductIndex = -1
									ModalDialogManager.openDialog(productEditorDialog, {})
								}
							}
						}
					}

					Rectangle {
						id: noProductsHint
						width: parent.width
						height: orderEditorContainer.productsCount === 0 ? noProductsColumn.height + Style.marginXXL * 2 : 0
						visible: noProductsHint.height > 0
						opacity: orderEditorContainer.productsCount === 0 ? 1 : 0
						clip: true
						radius: Style.radiusM
						color: Style.backgroundColor2
						border.width: 1
						border.color: Style.borderColor

						Behavior on opacity {
							NumberAnimation {
								duration: 160
							}
						}

						Column {
							id: noProductsColumn
							anchors.centerIn: parent
							width: Math.max(0, parent.width - Style.marginXXL * 2)
							spacing: Style.marginM

							Image {
								anchors.horizontalCenter: parent.horizontalCenter
								width: Style.iconSizeXL
								height: Style.iconSizeXL
								source: "../../../" + Style.getIconPath("Icons/Product", Icon.State.On, Icon.Mode.Normal)
								sourceSize.width: width
								sourceSize.height: height
								opacity: Style.opacityLow
							}

							Text {
								width: parent.width
								horizontalAlignment: Text.AlignHCenter
								text: qsTr("No products yet")
								color: Style.textColor
								font.family: Style.fontFamilyBold
								font.pixelSize: Style.fontSizeL
								wrapMode: Text.WordWrap
							}

							Text {
								width: parent.width
								horizontalAlignment: Text.AlignHCenter
								text: qsTr("Use \"Add Product\" above to include software or hardware in this order.")
								color: Style.inactiveTextColor
								font.family: Style.fontFamily
								font.pixelSize: Style.fontSizeM
								wrapMode: Text.WordWrap
							}
						}
					}

					ListView {
						id: productsView
						objectName: "OrderProductsListView"
						width: parent.width
						height: contentHeight
						boundsBehavior: Flickable.StopAtBounds
						spacing: Style.marginL
						cacheBuffer: 1000
						interactive: false

						property int activeProductIndex: -1
						property int selectedIndex: -1
						property bool readOnly: false
						property bool expanded: true

						delegate: OrderProductDelegate {
							id: orderProductDelegate
							width: productsView.width
							readOnly: productsView.readOnly
							expanded: productsView.expanded

							onEdited: {
								productsView.activeProductIndex = model.index
								ModalDialogManager.openDialog(productEditorDialog, {})
							}

							onRemoved: {
								productsView.activeProductIndex = model.index
								let productName = orderProductDelegate.productName !== ""
									? orderProductDelegate.productName
									: (orderProductDelegate.productItem ? orderProductDelegate.productItem.m_productName : "")
								let confirmMessage = qsTr("Remove product") + " \"" + productName + "\"?"
								ModalDialogManager.openDialog(removeDialog, { "message": confirmMessage })
							}
						}

						onCountChanged: {
							orderEditorContainer.refreshProductsPageTitle()
						}
					}
				}
			}
		}
	}

	Component {
		id: removeDialog

		MessageDialog {
			backgroundColor: Style.baseColor

			onFinished: {
				if (buttonId == Enums.yes) {
					let productsPage = orderEditorContainer.getPageItem("Products")
					if (!productsPage || productsPage.productsView.activeProductIndex < 0) {
						return
					}
					let orderProducts = orderEditorContainer.orderData.m_orderProducts
					if (orderProducts) {
						orderProducts.remove(productsPage.productsView.activeProductIndex)
						orderEditorContainer.notifyOrderChanged()
						productsPage.updateGui()
					}
				}
			}
		}
	}
}
