import QtQuick 2.0
import imtgui 1.0
import imtcontrols 1.0
import Acf 1.0
import com.imtcore.imtqml 1.0

Dialog {
	id: productEditorDialog

	backgroundColor: Style.baseColor

	property int rootWidth: root ? root.width : 0
	property int rootHeight: root ? root.height : 0

	property int activeProductIndex: -1

	property string orderId
	property string orderUuid

	title: activeProductIndex < 0 ? qsTr("Add Product") : qsTr("Edit Product")

	Component.onCompleted: {
		// Primary action label depends on Add vs Edit of an order product line.
		let primaryLabel = productEditorDialog.activeProductIndex < 0
			? qsTr("Add")
			: qsTr("Save")
		addButton(Enums.ok, primaryLabel, false)
		addButton(Enums.cancel, qsTr("Cancel"), true)
	}

	contentComp: Component {
		ProductEditor {
			id: productEditor

			// Wide stage for MultiPageView editors + chrome link picker.
			width: {
				if (productEditorDialog.rootWidth > 0)
					return Math.max(1200, Math.min(1520, Math.floor(productEditorDialog.rootWidth * 0.94)))
				return 1320
			}
			height: {
				if (productEditorDialog.rootHeight > 0)
					return Math.max(780, Math.min(1000, Math.floor(productEditorDialog.rootHeight * 0.92)))
				return 900
			}

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
