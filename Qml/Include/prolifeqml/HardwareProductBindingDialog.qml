import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import prolifeSensorsSdl 1.0
import prolifeLicensesSdl 1.0
import imtbaseImtCollectionSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0
import imtlicgui 1.0
import imtcolgui 1.0
import imtauthgui 1.0

Dialog {
	id: bindingDialog

	title: qsTr("License Binding")
	backgroundColor: Style.baseColor
	height: ModalDialogManager.activeView.height - 120
	canMove: false
	notClosingButtons: Enums.ok

	property int dialogDefaultWidth: 1000
	property int rootWidth: ModalDialogManager.activeView.width

	property string hardwareId: ""
	property DeviceBindingData bindingModel: null

	signal saved()

	onRootWidthChanged: {
		if (bindingDialog.rootWidth < bindingDialog.dialogDefaultWidth){
			bindingDialog.width = bindingDialog.rootWidth
		}
		else{
			bindingDialog.width = bindingDialog.dialogDefaultWidth
		}
	}

	onHardwareIdChanged: {
		getDeviceBindingRequest.send()
	}

	Component.onCompleted: {
		bindingDialog.fillButtons()
	}

	onLocalizationChanged: {
		bindingDialog.fillButtons()
	}

	function fillButtons(){
		bindingDialog.clearButtons()
		bindingDialog.addButton(Enums.ok, qsTr("Save"), false)
		bindingDialog.addButton(Enums.cancel, qsTr("Close"), true)
	}

	onFinished: {
		if (buttonId == Enums.ok){
			if (!bindingDialog.bindingModel){
				return
			}

			ModalDialogManager.openDialog(projectDialogComp, {"inputValue": bindingDialog.bindingModel.m_project})
		}
	}

	contentComp: Component {
		Item {
			id: content

			width: bindingDialog.width
			height: bindingDialog.height - 100
			clip: true

			property DeviceBindingData workingModel: null
			property DeviceBindingData originalModel: bindingDialog.bindingModel

			property int boundCount: boundTable.elementsCount
			property var checkedIndexes: []
			property string bindIssue: ""

			property string productUuid: content.workingModel ? content.workingModel.m_productUuid : ""

			property int pageIndex: 0

			onPageIndexChanged: {
				pagesAnimation.stop()
				pagesAnimation.from = pagesRow.x
				pagesAnimation.to = -content.pageIndex * content.width
				pagesAnimation.start()

				content.updateSaveState()
			}

			onWidthChanged: {
				pagesAnimation.stop()
				pagesRow.x = -content.pageIndex * content.width
			}

			onOriginalModelChanged: {
				if (content.originalModel){
					content.workingModel = content.originalModel.copyMe()
					boundLicensesProvider.updateModel()
					content.updateGui()
				}
			}

			onBoundCountChanged: {
				content.updateGui()
				availableUpdateTimer.restart()
			}

			onBindIssueChanged: {
				if (content.bindIssue !== ""){
					popupContainer.addMessage("error", content.bindIssue, false, "bindIssue")
				}
				else{
					popupContainer.removeMessageById("bindIssue")
				}
			}

			Connections {
				target: content.workingModel

				function onModelChanged(){
					content.updateGui()
					content.updateSaveState()
				}
			}

			function updateGui(){
				if (!content.workingModel){
					return
				}

				productComboBox.changeable = content.boundCount === 0

				productComboBox.currentIndex = -1
				if (productComboBox.model){
					for (let i = 0; i < productComboBox.model.getItemsCount(); i++){
						let id = productComboBox.model.getData("id", i)
						if (id === content.workingModel.m_productUuid){
							productComboBox.currentIndex = i
							break
						}
					}
				}
			}

			function updateSaveState(){
				if (!content.workingModel || !content.originalModel){
					return
				}

				let isEnabled = content.pageIndex === 0 && content.workingModel.m_productUuid !== "" && !content.workingModel.isEqualWithModel(content.originalModel)
				bindingDialog.setButtonEnabled(Enums.ok, isEnabled)
			}

			function boundArticles(){
				let result = []

				if (!boundTable.elements){
					return result
				}

				for (let i = 0; i < boundTable.elements.getItemsCount(); i++){
					result.push(boundTable.elements.getData("licenseId", i))
				}

				return result
			}

			function findCheckedIssue(){
				let elements = availableCollection.table.elements

				if (!elements || !content.workingModel){
					return qsTr("Licenses are not loaded yet")
				}

				let articles = content.boundArticles()

				for (let i = 0; i < content.checkedIndexes.length; i++){
					let index = content.checkedIndexes[i]
					let article = elements.getData("licenseId", index)
					let itemProductUuid = elements.getData("productUuid", index)

					if (itemProductUuid !== content.workingModel.m_productUuid){
						return qsTr("A device can hold licenses of one product only")
					}

					if (articles.indexOf(article) >= 0){
						return qsTr("This article is already bound or selected: ") + article
					}

					articles.push(article)
				}

				return ""
			}

			function updateBindState(){
				if (!PermissionsController.checkPermission("BindSensor") || content.checkedIndexes.length === 0){
					content.bindIssue = ""
					confirmBindButton.enabled = false
					return
				}

				let issue = content.findCheckedIssue()

				content.bindIssue = issue
				confirmBindButton.enabled = issue === ""
			}

			function isNewBinding(softwareId){
				if (!content.originalModel || softwareId === ""){
					return false
				}

				return content.originalModel.m_softwareIds.indexOf(softwareId) < 0
			}

			function bindChecked(){
				if (!content.workingModel || !availableCollection.table.elements || !boundTable.elements){
					return
				}

				let issue = content.findCheckedIssue()

				if (issue !== ""){
					content.bindIssue = issue
					return
				}

				for (let i = 0; i < content.checkedIndexes.length; i++){
					let index = content.checkedIndexes[i]
					let softwareId = availableCollection.table.elements.getData("id", index)

					content.workingModel.m_softwareIds.push(softwareId)

					let newIndex = boundTable.elements.insertNewItem()
					boundTable.elements.copyItemDataFromModel(newIndex, availableCollection.table.elements, index)
				}

				content.workingModel.modelChanged()
				boundTable.elements.refresh()

				content.checkedIndexes = []
				availableCollection.table.uncheckAll()
				availableUpdateTimer.restart()

				content.pageIndex = 0
			}

			function unbindAt(index){
				if (!content.workingModel || !boundTable.elements || index < 0){
					return
				}

				let softwareId = boundTable.elements.getData("id", index)
				let softwareIds = content.workingModel.m_softwareIds

				for (let i = 0; i < softwareIds.length; i++){
					if (softwareIds[i] === softwareId){
						softwareIds.splice(i, 1)
						break
					}
				}

				content.workingModel.modelChanged()

				boundTable.elements.removeItem(index)

				availableUpdateTimer.restart()
			}

			Timer {
				id: availableUpdateTimer

				interval: 100

				onTriggered: {
					availableCollection.updateData()
				}
			}

			PopupContainer {
				id: popupContainer

				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				anchors.bottom: parent.bottom
				anchors.bottomMargin: Style.marginM

				z: 100
			}

			NumberAnimation {
				id: pagesAnimation

				target: pagesRow
				property: "x"
				duration: 200
			}

			Item {
				id: pagesRow

				width: content.width * 2
				height: content.height

				Item {
					id: boundPage

					width: content.width
					height: content.height

					GroupElementView {
						id: headerGroup

						anchors.top: parent.top
						anchors.topMargin: Style.marginL
						anchors.left: parent.left
						anchors.leftMargin: Style.marginL
						anchors.right: parent.right
						anchors.rightMargin: Style.marginL

						ComboBoxElementView {
							id: productComboBox

							width: parent.width
							name: qsTr("Product")
							nameId: "productName"
							model: CachedProductCollection.softwareProductsModel
							bottomComp: productComboBox.currentIndex >= 0 ? undefined : productErrorComp
							controlWidth: Style.sizeHintXS
							changeable: false

							onCurrentIndexChanged: {
								if (productComboBox.currentIndex < 0 || !content.workingModel){
									return
								}

								content.workingModel.m_productUuid = productComboBox.model.getData("id", productComboBox.currentIndex)
								content.updateSaveState()
								availableUpdateTimer.restart()
							}

							Component {
								id: productErrorComp

								BaseText {
									color: Style.errorTextColor
									text: qsTr("Please select a product")
								}
							}
						}
					}

					Rectangle {
						id: boundPane

						anchors.top: headerGroup.bottom
						anchors.topMargin: Style.marginL
						anchors.left: parent.left
						anchors.leftMargin: Style.marginL
						anchors.right: parent.right
						anchors.rightMargin: Style.marginL
						anchors.bottom: parent.bottom
						anchors.bottomMargin: Style.marginL

						radius: Style.radiusM
						color: Style.baseColor
						border.width: 1
						border.color: Style.borderColor
						clip: true

						Item {
							id: boundHeader

							anchors.top: parent.top
							anchors.left: parent.left
							anchors.leftMargin: Style.marginL
							anchors.right: parent.right
							anchors.rightMargin: Style.marginL

							height: Style.controlHeightL + Style.marginS

							BaseText {
								id: boundTitle

								anchors.left: parent.left
								anchors.verticalCenter: parent.verticalCenter

								text: qsTr("Bound Licenses") + " (" + content.boundCount + ")"
								font.family: Style.fontFamilyBold
								font.pixelSize: Style.fontSizeL
								color: Style.titleColor
							}

							Button {
								id: openAvailableButton

								anchors.right: parent.right
								anchors.verticalCenter: parent.verticalCenter

								text: qsTr("Bind Licenses")
								enabled: content.productUuid !== "" && PermissionsController.checkPermission("BindSensor")
								tooltipText: qsTr("Pick licenses to bind to this device")
								iconSource: openAvailableButton.enabled ? "../../../" + Style.getIconPath("Icons/Link", Icon.State.On, Icon.Mode.Normal) :
																	  "../../../" + Style.getIconPath("Icons/Link", Icon.State.On, Icon.Mode.Disabled)

								onClicked: {
									content.pageIndex = 1
								}
							}
						}

						Table {
							id: boundTable

							anchors.top: boundHeader.bottom
							anchors.left: parent.left
							anchors.right: parent.right
							anchors.bottom: parent.bottom
							anchors.margins: Style.marginS

							selectable: false

							TableHeaders {
								id: boundHeaders

								Component.onCompleted: {
									boundHeaders.addHeader("licenseName", qsTr("Name"))
									boundHeaders.addHeader("licenseId", qsTr("Article"))
									boundHeaders.addHeader("serialNumber", qsTr("Software-ID"))
									boundHeaders.addHeader("unbind", "")

									boundTable.headers = boundHeaders

									boundTable.setColumnContentById("licenseName", boundNameCellComp)
									boundTable.setColumnContentById("unbind", unbindCellComp)
								}
							}
						}

						BaseText {
							id: boundEmptyHint

							anchors.centerIn: boundTable

							visible: content.boundCount === 0
							text: qsTr("Nothing is bound yet. Press Bind Licenses to add one.")
							color: Style.inactiveTextColor
							font.pixelSize: Style.fontSizeM
						}
					}
				}

				Item {
					id: availablePage

					x: content.width
					width: content.width
					height: content.height

					Item {
						id: availableTopBar

						anchors.top: parent.top
						anchors.topMargin: Style.marginL
						anchors.left: parent.left
						anchors.leftMargin: Style.marginL
						anchors.right: parent.right
						anchors.rightMargin: Style.marginL

						height: Style.controlHeightL

						ToolButton {
							id: backButton

							anchors.left: parent.left
							anchors.verticalCenter: parent.verticalCenter

							width: Style.buttonWidthL
							height: width
							tooltipText: qsTr("Back to bound licenses")
							iconSource: "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal)

							onClicked: {
								content.pageIndex = 0
							}
						}

						BaseText {
							id: availableTitle

							anchors.left: backButton.right
							anchors.leftMargin: Style.marginM
							anchors.verticalCenter: parent.verticalCenter

							text: qsTr("Select Licenses")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeL
							color: Style.titleColor
						}

						Button {
							id: confirmBindButton

							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter

							text: content.checkedIndexes.length > 0 ? qsTr("Bind") + " (" + content.checkedIndexes.length + ")" : qsTr("Bind")
							enabled: false
							tooltipText: qsTr("Bind the checked licenses to this device")
							iconSource: confirmBindButton.enabled ? "../../../" + Style.getIconPath("Icons/Link", Icon.State.On, Icon.Mode.Normal) :
																"../../../" + Style.getIconPath("Icons/Link", Icon.State.On, Icon.Mode.Disabled)

							onClicked: {
								content.bindChecked()
							}
						}
					}

					Rectangle {
						id: availablePane

						anchors.top: availableTopBar.bottom
						anchors.topMargin: Style.marginM
						anchors.left: parent.left
						anchors.leftMargin: Style.marginL
						anchors.right: parent.right
						anchors.rightMargin: Style.marginL
						anchors.bottom: parent.bottom
						anchors.bottomMargin: Style.marginL

						radius: Style.radiusM
						color: Style.baseColor
						border.width: 1
						border.color: Style.borderColor
						clip: true

						TableHeaders {
							id: availableHeaders

							Component.onCompleted: {
								availableHeaders.addHeader("licenseName", qsTr("Name"))
								availableHeaders.addHeader("licenseId", qsTr("Article"))
								availableHeaders.addHeader("serialNumber", qsTr("Software-ID"))
								availableHeaders.addHeader("isMultiple", qsTr("Is Multiple"))
								availableHeaders.addHeader("productCount", qsTr("Available"))

								let filteringInfoIds = ["licenseName", "licenseId", "serialNumber"]

								availableCollection.collectionFilter.setFilteringInfoIds(filteringInfoIds)
								availableCollection.tableViewParamsStoredServer = false
								availableCollection.dataController.headersReceived(availableHeaders)
							}
				}

				SoftwareProductCollectionView {
					id: availableCollection

					anchors.top: parent.top
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.bottom: parent.bottom
					anchors.margins: Style.marginS

					commandsControllerComp: null
					commandsDelegateComp: null
					table.checkable: true
					table.selectable: false
					tableViewParamsStoredServer: false
					canResetFilters: false

					dataControllerComp: Component {
						CollectionRepresentation {
							id: availableDataController

							Component.onCompleted: {
								availableDataController.additionalFieldIds.push("orderUuid")
								availableDataController.additionalFieldIds.push("hardwareId")
								availableDataController.additionalFieldIds.push("inUse")
								availableDataController.additionalFieldIds.push("productUuid")
								availableDataController.additionalFieldIds.push("licenseUuid")
								availableDataController.additionalFieldIds.push("customerId")
							}

							function updateModel(){}
						}
					}

					function registerFilters(){
						availableCollection.registerFieldFilterDelegate("Products", productsDelegateFilterComp)
					}

					Component {
						id: productsDelegateFilterComp

						FieldFilterDelegate {
							id: productsDelegateFilter

							name: qsTr("Products")
							defaultFieldFilter.m_fieldId: "ProductUuid"
							readOnly: true

							function syncSelection(){
								for (let i = 0; i < optionsListAdapter.m_options.count; i++){
									if (productsDelegateFilter.getOptionId(i) === content.productUuid){
										productsDelegateFilter.setSelectedIndex(i)
										return
									}
								}
							}

							Component.onCompleted: {
								productsDelegateFilter.setOptionsList(optionsListAdapter.m_options)
								productsDelegateFilter.syncSelection()
							}

							Connections {
								target: content

								function onProductUuidChanged(){
									productsDelegateFilter.syncSelection()
								}
							}

							OptionsListAdapter {
								id: optionsListAdapter

								collectionModel: CachedProductCollection.softwareProductsModel

								onCollectionModelChanged: {
									productsDelegateFilter.setOptionsList(optionsListAdapter.m_options)
									productsDelegateFilter.syncSelection()
								}
							}
						}
					}

					Connections {
						target: availableCollection

						function onCheckedItemsChanged(){
							content.checkedIndexes = availableCollection.table.getCheckedItems()
							content.updateBindState()
						}
					}

					onElementsChanged: {
						availableCollection.table.uncheckAll()
						content.checkedIndexes = []
						content.updateBindState()
					}

					FieldFilter {
						id: productFilter

						m_fieldId: "ProductUuid"
						m_filterValue: content.workingModel ? content.workingModel.m_productUuid : ""
						m_filterValueType: "String"
						m_filterOperations: ["Equal"]
					}

					FieldFilter {
						id: licenseFilter

						m_fieldId: "LicenseUuid"
						m_filterValueType: "String"
						m_filterOperations: ["Not", "Equal"]
					}

					FieldFilter {
						id: emptyLicenseIdFilter

						m_fieldId: "LicenseId"
						m_filterValueType: "String"
						m_filterValue: ""
						m_filterOperations: ["Not", "Equal"]
					}

					FieldFilter {
						id: excludeFilter

						m_fieldId: "DocumentId"
						m_filterValueType: "String"
						m_filterOperations: ["Not", "Equal"]
					}

					GroupFilter {
						id: notIsMultiSoftwareFilter

						m_logicalOperation: "And"
					}

					FieldFilter {
						id: isMultipleFilter

						m_fieldId: "IsMultiProduct"
						m_filterValue: "false"
						m_filterValueType: "Bool"
						m_filterOperations: ["Equal"]
					}

					ArrayFieldFilter {
						id: emptyHardwareFilter

						m_fieldId: "HardwareId"
						m_filterValueType: "String"
						m_filterOperations: ["ArrayIsEmpty"]
					}

					GroupFilter {
						id: isMultiSoftwareFilter

						m_logicalOperation: "And"
					}

					FieldFilter {
						id: isMultipleFilter2

						m_fieldId: "IsMultiProduct"
						m_filterValue: "true"
						m_filterValueType: "Bool"
						m_filterOperations: ["Equal"]
					}

					FieldFilter {
						id: productCountFilter

						m_fieldId: "ProductCount"
						m_filterValue: "0"
						m_filterValueType: "Integer"
						m_filterOperations: ["Greater"]
					}

					GroupFilter {
						id: softwareFilter

						m_logicalOperation: "Or"
					}

					function updateData(){
						if (!content.workingModel || content.workingModel.m_productUuid === ""){
							return
						}

						availableCollection.dataController.collectionId = "SoftwareProducts"

						availableCollection.collectionFilter.removeFilterByFieldId(productFilter.m_fieldId)
						availableCollection.collectionFilter.removeFilterByFieldId(excludeFilter.m_fieldId)
						availableCollection.collectionFilter.removeFilterByFieldId(licenseFilter.m_fieldId)
						availableCollection.collectionFilter.removeFilterByFieldId(emptyLicenseIdFilter.m_fieldId)
						availableCollection.collectionFilter.removeGroupFilter(softwareFilter)

						availableCollection.collectionFilter.addFieldFilter(productFilter)

						notIsMultiSoftwareFilter.emplaceFieldFilters()
						notIsMultiSoftwareFilter.m_fieldFilters.addElement(isMultipleFilter)
						notIsMultiSoftwareFilter.m_fieldFilters.addElement(emptyHardwareFilter)

						isMultiSoftwareFilter.emplaceFieldFilters()
						isMultiSoftwareFilter.m_fieldFilters.addElement(isMultipleFilter2)
						isMultiSoftwareFilter.m_fieldFilters.addElement(productCountFilter)

						softwareFilter.emplaceGroupFilters()
						softwareFilter.m_groupFilters.addElement(notIsMultiSoftwareFilter)
						softwareFilter.m_groupFilters.addElement(isMultiSoftwareFilter)

						availableCollection.collectionFilter.addGroupFilter(softwareFilter)

						if (boundTable.elements){
							for (let i = 0; i < boundTable.elements.getItemsCount(); i++){
								let id = boundTable.elements.getData("id", i)
								let licenseUuid = boundTable.elements.getData("licenseUuid", i)
								let licenseId = boundTable.elements.getData("licenseId", i)

								if (id !== ""){
									let filter = excludeFilter.copyMe()
									filter.m_filterValue = id
									availableCollection.collectionFilter.addFieldFilter(filter)
								}

								if (licenseUuid !== ""){
									let licFilter = licenseFilter.copyMe()
									licFilter.m_filterValue = licenseUuid
									availableCollection.collectionFilter.addFieldFilter(licFilter)
								}

								if (licenseId !== ""){
									let articleFilter = emptyLicenseIdFilter.copyMe()
									articleFilter.m_filterValue = licenseId
									availableCollection.collectionFilter.addFieldFilter(articleFilter)
								}
							}
						}

						availableCollection.collectionFilter.addFieldFilter(emptyLicenseIdFilter)

						availableCollection.collectionFilter.filterChanged()
					}
				}
					}
				}
			}

			Component {
				id: unbindCellComp

				TableCellDelegateBase {
					id: unbindCellDelegate

					ToolButton {
						id: unbindRowButton

						anchors.verticalCenter: unbindCellDelegate.verticalCenter
						anchors.horizontalCenter: unbindCellDelegate.horizontalCenter

						width: Style.buttonWidthM
						height: width
						tooltipText: qsTr("Unbind")
						iconSource: "../../../" + Style.getIconPath("Icons/Unlink", Icon.State.On, Icon.Mode.Normal)

						onClicked: {
							content.unbindAt(unbindCellDelegate.rowIndex)
						}
					}

					onReused: {
						if (unbindCellDelegate.rowIndex >= 0){
							let inUse = boundTable.elements.getData("inUse", unbindCellDelegate.rowIndex)
							unbindRowButton.visible = !inUse || PermissionsController.checkPermission("UnbindSensor")
						}
					}
				}
			}

			Component {
				id: boundNameCellComp

				TableCellDelegateBase {
					id: nameCellDelegate

					ToolButton {
						id: lockButton

						anchors.verticalCenter: nameCellDelegate.verticalCenter
						anchors.left: nameCellDelegate.left
						anchors.leftMargin: Style.marginM

						width: Style.iconSizeM
						height: width
						tooltipText: qsTr("Already in use")
						iconSource: "../../../" + Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Normal)
					}

					BaseText {
						id: licenseNameText

						anchors.verticalCenter: nameCellDelegate.verticalCenter
						anchors.left: lockButton.right
						anchors.leftMargin: Style.marginM
					}

					Rectangle {
						id: newBadge

						anchors.verticalCenter: nameCellDelegate.verticalCenter
						anchors.left: licenseNameText.right
						anchors.leftMargin: Style.marginS

						width: newBadgeText.width + 2 * Style.marginXS
						height: Style.controlHeightS
						radius: Style.radiusL
						color: Style.selectedColor

						BaseText {
							id: newBadgeText

							anchors.centerIn: parent

							text: qsTr("New")
							font.family: Style.fontFamilyBold
							font.pixelSize: Style.fontSizeS
							color: Style.textColor
						}
					}

					onReused: {
						if (rowIndex >= 0){
							licenseNameText.text = boundTable.elements.getData("licenseName", rowIndex)
							lockButton.visible = boundTable.elements.getData("inUse", rowIndex)
							newBadge.visible = content.isNewBinding(boundTable.elements.getData("id", rowIndex))
						}
					}
				}
			}

			ArrayFieldFilter {
				id: boundHardwareFilter

				m_fieldId: "HardwareId"
				m_filterValues: [bindingDialog.hardwareId]
				m_filterValueType: "String"
				m_filterOperations: ["ArrayHasAny"]
			}

			CollectionDataProvider {
				id: boundLicensesProvider

				commandId: ProlifeLicensesSdlCommandIds.s_softwareProductsList
				sortByField: SoftwareProductItemTypeMetaInfo.s_name
				fields: [
					SoftwareProductItemTypeMetaInfo.s_id,
					SoftwareProductItemTypeMetaInfo.s_name,
					SoftwareProductItemTypeMetaInfo.s_productName,
					SoftwareProductItemTypeMetaInfo.s_licenseUuid,
					SoftwareProductItemTypeMetaInfo.s_licenseId,
					SoftwareProductItemTypeMetaInfo.s_licenseName,
					SoftwareProductItemTypeMetaInfo.s_serialNumber,
					SoftwareProductItemTypeMetaInfo.s_productUuid,
					SoftwareProductItemTypeMetaInfo.s_inUse,
					SoftwareProductItemTypeMetaInfo.s_isMultiple
				]

				onStateChanged: {
					loading.visible = boundLicensesProvider.state === "" || boundLicensesProvider.state === "Loading"
				}

				Component.onCompleted: {
					boundLicensesProvider.filter.addFieldFilter(boundHardwareFilter)
				}

				onCollectionModelChanged: {
					if (boundLicensesProvider.collectionModel){
						boundTable.elements = boundLicensesProvider.collectionModel
						availableUpdateTimer.restart()
					}
				}
			}
		}
	}

	Component {
		id: projectDialogComp

		InputDialog {
			backgroundColor: Style.baseColor
			title: qsTr("Apply changes")
			message: qsTr("Please check the data before saving. Save changes ?")
			placeHolderText: qsTr("Please enter the project")

			onFinished: {
				if (buttonId == Enums.ok){
					let workingModel = bindingDialog.contentItem.workingModel
					workingModel.m_id = bindingDialog.hardwareId

					deviceBindingInput.m_deviceId = bindingDialog.hardwareId
					deviceBindingInput.m_project = inputValue
					deviceBindingInput.m_item = workingModel

					updateDeviceBindingRequest.send(deviceBindingInput)
				}
			}
		}
	}

	Loading {
		id: loading

		anchors.fill: bindingDialog
		background.color: Style.baseColor
	}

	GqlSdlRequestSender {
		id: getDeviceBindingRequest

		gqlCommandId: ProlifeSensorsSdlCommandIds.s_getDeviceBinding

		inputObjectComp: Component {
			GetDeviceBindingInput {
				m_id: bindingDialog.hardwareId
			}
		}

		sdlObjectComp: Component {
			DeviceBindingData {
				onFinished: {
					bindingDialog.bindingModel = this
				}
			}
		}
	}

	DeviceBindingInput {
		id: deviceBindingInput
	}

	GqlSdlRequestSender {
		id: updateDeviceBindingRequest

		requestType: 1
		gqlCommandId: ProlifeSensorsSdlCommandIds.s_updateDeviceBinding

		sdlObjectComp: Component {
			UpdatedNotificationPayload {
				onFinished: {
					if (m_id !== ''){
						bindingDialog.saved()
						bindingDialog.finished(Enums.cancel)
					}
				}
			}
		}
	}
}
