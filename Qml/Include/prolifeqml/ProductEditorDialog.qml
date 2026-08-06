import QtQuick 2.0
import imtgui 1.0
import imtcontrols 1.0
import Acf 1.0
import com.imtcore.imtqml 1.0

Dialog {
	id: productEditorDialog

	backgroundColor: Style.baseColor

	width: Math.max(Style.sizeHintXXL, Math.min(ModalDialogManager.activeView.width * 0.94, 1520))
	height: Math.max(Style.sizeHintXL, Math.min(ModalDialogManager.activeView.height * 0.92, 1000))

	property int activeProductIndex: -1

	property string orderId
	property string orderUuid

	title: activeProductIndex < 0 ? qsTr("Add Product") : qsTr("Edit Product")

	Component.onCompleted: {
		addButton(Enums.ok, qsTr("Add"), false)
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}

	onActiveProductIndexChanged: {
		let primaryLabel = productEditorDialog.activeProductIndex < 0
			? qsTr("Add")
			: qsTr("Save")
		productEditorDialog.setButtonName(Enums.ok, primaryLabel)
	}

	contentComp: Component {
		ProductEditor {
			id: productEditor

			width: productEditorDialog.width
			height: productEditorDialog.height - 120

			index: productEditorDialog.activeProductIndex

			Connections {
				target: productEditor.productItem

				function onModelChanged() {
					let ok = productEditor.productItem.m_licenseUuid !== "" &&
						productEditor.productItem.m_productUuid !== ""

					if (productEditor.orderProductsModel) {
						for (let i = 0; i < productEditor.orderProductsModel.count; i++) {
							let productItem1 = productEditor.orderProductsModel.get(i).item
							if (productItem1.m_id !== productEditor.productItem.m_id &&
									productItem1.m_categoryId === productEditor.productItem.m_categoryId &&
									(productItem1.m_serialNumber !== "" &&
									productItem1.m_serialNumber === productEditor.productItem.m_serialNumber ||
									productItem1.m_macAddress !== "" &&
									productItem1.m_macAddress === productEditor.productItem.m_macAddress)) {
								ok = false
								break
							}
						}
					}

					productEditorDialog.setButtonEnabled(Enums.ok, ok)
				}
			}
		}
	}
}
