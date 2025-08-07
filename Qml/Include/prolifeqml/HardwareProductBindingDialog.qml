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
	id: productEditorDialog

	height: ModalDialogManager.activeView.height - 200

	canMove: false
	
	property int dialogDefaultWidth: 1000
	property int rootWidth: ModalDialogManager.activeView.width
	onRootWidthChanged: {
		if (rootWidth < dialogDefaultWidth){
			width = rootWidth
		}
		else{
			width = dialogDefaultWidth
		}
	}
	
	property var softwareIds: []
	property string hardwareId: "";
	property DeviceBindingData bindingModel: null

	notClosingButtons: Enums.ok
	
	signal saved();
	
	onHardwareIdChanged: {
		getDeviceBindingRequest.send();
	}

	Component.onCompleted: {
		productEditorDialog.fillButtons()
	}

	onLocalizationChanged: {
		productEditorDialog.fillButtons()
	}

	function fillButtons(){
		clearButtons()
		addButton(Enums.ok, qsTr("Save"), false)
		addButton(Enums.cancel, qsTr("Close"), true)
	}
	
	onFinished: {
		if (buttonId == Enums.ok){
			ModalDialogManager.openDialog(messageDialog, {});
		}
	}

	contentComp: Component {
		Item {
			id: flickableContent
			width: productEditorDialog.width
			height: productEditorDialog.height - 100
			clip: true
			// contentHeight: content.height
			
			Connections {
				target: flickableContent.bindingModel
				function onModelChanged(){
					flickableContent.updateGui()

					let isEnabled = target.m_productUuid !== "" && !target.isEqualWithModel(productEditorDialog.bindingModel)
					productEditorDialog.setButtonEnabled(Enums.ok, isEnabled)
				}
			}
			
			property int tableHeight: productComboBoxElementView.currentIndex >= 0 ? productEditorDialog.height - 300 : productEditorDialog.height - 340
			onTableHeightChanged: {
				if (!usedLicensesTableElementView.table){
					return
				}
				
				usedLicensesTableElementView.table.height = tableHeight
			}
			
			property DeviceBindingData bindingModel: null
			property DeviceBindingData originalBindingModel: productEditorDialog.bindingModel
			onOriginalBindingModelChanged: {
				if (originalBindingModel){
					bindingModel = originalBindingModel.copyMe()
					usedLicensesDataProvider.updateModel()
					updateGui()
				}
			}

			function updateGui(){
				productComboBoxElementView.changeable = bindingModel.m_softwareIds === ""
				productComboBoxElementView.currentIndex = -1
				if (productComboBoxElementView.model){
					for (let i = 0; i < productComboBoxElementView.model.getItemsCount(); i++){
						let id = productComboBoxElementView.model.getData("id", i)
						if (id === bindingModel.m_productUuid){
							productComboBoxElementView.currentIndex = i
							break;
						}
					}
				}
			}

			Column {
				id: content
				anchors.top: parent.top
				anchors.topMargin: Style.marginM
				anchors.left: parent.left
				anchors.leftMargin: Style.marginM
				anchors.right: parent.right
				anchors.rightMargin: Style.marginM
				spacing: Style.marginM

				GroupElementView {
					width: parent.width

					ComboBoxElementView {
						id: productComboBoxElementView
						width: parent.width
						name: qsTr("Product")
						nameId: "productName"
						model: CachedProductCollection.softwareProductsModel
						bottomComp: currentIndex >= 0 ? undefined : productErrorComp
						controlWidth: 300
						changeable: false
						onCurrentIndexChanged: {
							if (currentIndex > -1){
								flickableContent.bindingModel.m_productUuid = model.getData("id", currentIndex)
							}
						}
	
						Component {
							id: productErrorComp
							BaseText {
								color: Style.errorTextColor;
								text: qsTr("Please select a product");
							}
						}
					}
					
					TableElementView {
						id: usedLicensesTableElementView
						name: qsTr("Used Licenses")
						controlComp: Component {
							Row {
								height: Style.itemSizeM
								spacing: Style.marginM
								Button {
									id: unbindButton
									text: qsTr("Unbind")
									icon.source: enabled ?	"qrc:/" + Style.getIconPath("Icons/Unlink", Icon.State.On, Icon.Mode.Normal):
															"qrc:/" + Style.getIconPath("Icons/Unlink", Icon.State.Off, Icon.Mode.Disabled)
									enabled: false
									onClicked: {
										if (!usedLicensesTableElementView.table){
											return
										}

										let selection = usedLicensesTableElementView.table.getSelectedIndexes()
										if (selection.length !== 1){
											return
										}

										let softwareId = usedLicensesTableElementView.table.elements.getData("id", selection[0])
										let softwareIds = flickableContent.bindingModel.m_softwareIds.split(';')
										softwareIds.indexOf(softwareId)

										let index = softwareIds.indexOf(softwareId);
										if (index > -1) {
											softwareIds.splice(index, 1);
										}

										flickableContent.bindingModel.m_softwareIds = softwareIds.join(';')

										usedLicensesTableElementView.table.elements.removeItem(selection[0])
										usedLicensesTableElementView.table.resetSelection()
									}

									property int productIndex: productComboBoxElementView.currentIndex
									onProductIndexChanged: {
										bindButton.setEnabled(productIndex >= 0)
									}

									function setEnabled(enabled){
										let canUnbind = PermissionsController.checkPermission("UnbindSensor")

										let selection = usedLicensesTableElementView.table.getSelectedIndexes();
										if (selection.length === 0){
											unbindButton.enabled = false
											return
										}

										let inUse = usedLicensesTableElementView.table.elements.getData("inUse", selection[0])
										if (inUse && !canUnbind){
											unbindButton.enabled = false
											return
										}

										unbindButton.enabled = enabled
									}

									Connections {
										target: usedLicensesTableElementView.table
										function onSelectionChanged(selection){
											if (selection.length !== 1){
												unbindButton.setEnabled(false)
												return
											}
											unbindButton.setEnabled(selection.length > 0)
										}
									}
								}

								Button {
									id: bindButton
									text: qsTr("Bind New Licenses")
									icon.source: enabled ?	"qrc:/" + Style.getIconPath("Icons/Link", Icon.State.On, Icon.Mode.Normal) :
															"qrc:/" + Style.getIconPath("Icons/Link", Icon.State.Off, Icon.Mode.Disabled) 
									enabled: false
									onClicked: {
										ModalDialogManager.openDialog(availableLicenceCollectionComp, {});
									}

									function setEnabled(enabled){
										let canBind = PermissionsController.checkPermission("BindSensor")
										if (!canBind){
											bindButton.enabled = false
											return
										}
										bindButton.enabled = enabled
									}
								}
							}
						}
						
						onTableChanged: {
							if (table){
								table.isMultiSelect = false
							}
						}
						
						Connections {
							target: usedLicensesTableElementView.table
							function onHeadersChanged(){
								usedLicensesTableElementView.table.setColumnContentById("licenseName", unbindCellComp)
							}
						}
						
						Component {
							id: availableLicenceCollectionComp
							HardwareProductBindingEditor {
								hardwareId: productEditorDialog.hardwareId
								productId: flickableContent.bindingModel.m_productUuid
								usedLicensesModel: usedLicensesTableElementView.table.elements
								onFinished: {
									if (buttonId === Enums.ok){
										usedLicensesTableElementView.table.resetSelection()
										for (let i = 0; i < checkedIndexes.length; i++){
											let index = checkedIndexes[i]
	
											let id = availableLicensesModel.getData("id", index)
											flickableContent.bindingModel.m_softwareIds += ";" + id

											let index2 = usedLicensesTableElementView.table.elements.insertNewItem()
											usedLicensesTableElementView.table.elements.copyItemDataFromModel(index2, availableLicensesModel, index)
										}
										
										usedLicensesTableElementView.table.elements.refresh()
									}
								}
							}
						}
						
						Component {
							id: unbindCellComp
							TableCellDelegateBase {
								id: cellDelegate
								
								onReused: {
									if (rowIndex >= 0){
										let licenseName = usedLicensesTableElementView.table.elements.getData("licenseName", rowIndex)
										licenseNameText.text = licenseName

										let inUse = usedLicensesTableElementView.table.elements.getData("inUse", rowIndex)
										unbindButton2.visible = inUse
									}
								}

								ToolButton {
									id: unbindButton2
									anchors.verticalCenter: cellDelegate.verticalCenter
									anchors.left: cellDelegate.left
									anchors.leftMargin: Style.marginM
									width: 20
									height: width
									icon.source: "qrc:/" + Style.getIconPath("Icons/Lock", Icon.State.On, Icon.Mode.Normal)
								}
								
								BaseText {
									id: licenseNameText
									anchors.verticalCenter: cellDelegate.verticalCenter
									anchors.left: unbindButton2.right
									anchors.leftMargin: Style.marginM
								}
							}
						}
					
						TreeItemModel {
							id: usedLicensesTableHeadersModel
							
							Component.onCompleted: {
								updateHeaders()
							}
							
							function updateHeaders(){
								usedLicensesTableHeadersModel.clear();
								
								let index = usedLicensesTableHeadersModel.insertNewItem();
								usedLicensesTableHeadersModel.setData("id", "licenseName", index);
								usedLicensesTableHeadersModel.setData("name", qsTr("Name"), index);
								
								index = usedLicensesTableHeadersModel.insertNewItem();
								usedLicensesTableHeadersModel.setData("id", "licenseId", index);
								usedLicensesTableHeadersModel.setData("name", qsTr("Article"), index);

								index = usedLicensesTableHeadersModel.insertNewItem();
								usedLicensesTableHeadersModel.setData("id", "serialNumber", index);
								usedLicensesTableHeadersModel.setData("name", qsTr("Software-ID"), index);

								if (usedLicensesTableElementView.table){
									usedLicensesTableElementView.table.headers = usedLicensesTableHeadersModel
								}
							}
						}
	
						FieldFilter {
							id: hardwareFilter
							m_fieldId: "HardwareId"
							m_filterValue: productEditorDialog.hardwareId
							m_filterValueType: "String"
							m_filterOperations: ["Equal"]
						}
						
						CollectionDataProvider {
							id: usedLicensesDataProvider
							commandId: ProlifeLicensesSdlCommandIds.s_softwareProductsList;
							sortByField: SoftwareProductItemTypeMetaInfo.s_name;
							fields: [
								SoftwareProductItemTypeMetaInfo.s_id,
								SoftwareProductItemTypeMetaInfo.s_name,
								SoftwareProductItemTypeMetaInfo.s_productName,
								SoftwareProductItemTypeMetaInfo.s_licenseUuid,
								SoftwareProductItemTypeMetaInfo.s_licenseId,
								SoftwareProductItemTypeMetaInfo.s_licenseName,
								SoftwareProductItemTypeMetaInfo.s_serialNumber,
								SoftwareProductItemTypeMetaInfo.s_productUuid
							];
							
							onStateChanged: {
								loading.visible = state === "" || state === "Loading"
							}
							
							Component.onCompleted: {
								filter.addFieldFilter(hardwareFilter)
							}
							
							onCollectionModelChanged: {
								if (collectionModel){
									if (usedLicensesTableElementView.table){
										usedLicensesTableElementView.table.elements = collectionModel
									}
								}
							}
						}
					} // TableElementView
				}
			}
			

		}// Flickable
	}
	
	Component {
		id: messageDialog;
		
		InputDialog {
			title: qsTr("Apply changes");
			message: qsTr("Please check the data before saving. Save changes ?")
			placeHolderText: qsTr("Please enter the project");
			onFinished: {
				if (buttonId == Enums.ok){
					let bindingModel = productEditorDialog.contentItem.bindingModel;
					bindingModel.m_id = productEditorDialog.hardwareId;
					deviceBindingInput.m_deviceId = productEditorDialog.hardwareId
					deviceBindingInput.m_project = inputValue
					deviceBindingInput.m_item = bindingModel
					
					updateDeviceBindingRequest.send(deviceBindingInput)
				}
			}
		}
	}
	
	Loading {
		id: loading
		anchors.fill: productEditorDialog
		color: Style.backgroundColor2;
	}
	
	GqlSdlRequestSender {
		id: getDeviceBindingRequest;
		gqlCommandId: ProlifeSensorsSdlCommandIds.s_getDeviceBinding;
		inputObjectComp: Component {
			GetDeviceBindingInput {
				m_id: productEditorDialog.hardwareId;
			}
		}
		
		sdlObjectComp: Component {
			DeviceBindingData {
				onFinished: {
					productEditorDialog.bindingModel = this;
				}
			}
		}
	}
	
	DeviceBindingInput {
		id: deviceBindingInput;
	}
	
	GqlSdlRequestSender {
		id: updateDeviceBindingRequest
		requestType: 1;
		gqlCommandId: ProlifeSensorsSdlCommandIds.s_updateDeviceBinding;
		
		sdlObjectComp: Component {
			UpdatedNotificationPayload {
				onFinished: {
					if (m_id != ''){
						productEditorDialog.saved();
						productEditorDialog.finished(Enums.cancel);
					}
				}
			}
		}
	}
}//Container


