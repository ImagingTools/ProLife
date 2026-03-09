// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

import QtQuick 2.0
import imtgui 1.0
import imtcontrols 1.0
import Acf 1.0
import com.imtcore.imtqml 1.0

Dialog {
	id: productEditorDialog;
	
	backgroundColor: Style.backgroundColor2;
	
	property int rootWidth: root ? root.width: 0;
	property int rootHeight: root ? root.height: 0;
	
	property int activeProductIndex: -1;
	
	property string orderId;
	property string orderUuid;
	
	Component.onCompleted: {
		addButton(Enums.ok, qsTr("OK"), false)
		addButton(Enums.cancel, qsTr("Cancel"), true)
		title = qsTr("Product Editor");
	}
	
	contentComp: Component {
		ProductEditor {
			id: productEditor;
			
			width: 800;
			height: 700;
			
			index: productEditorDialog.activeProductIndex;
			
			Connections {
				target: productEditor.productItem;
				
				function onModelChanged(){
					let ok = productEditor.productItem.m_licenseUuid !== "" &&
						productEditor.productItem.m_productUuid !== "";

					// Validate Software-ID / MAC Address in this order
					if (productEditor.orderProductsModel){
						for (let i = 0; i < productEditor.orderProductsModel.count; i++){
							let productItem1 = productEditor.orderProductsModel.get(i).item
							if (productItem1.m_id !== productEditor.productItem.m_id &&
									productItem1.m_categoryId === productEditor.productItem.m_categoryId &&
									(productItem1.m_serialNumber !== "" &&
									productItem1.m_serialNumber === productEditor.productItem.m_serialNumber ||
									productItem1.m_macAddress !== "" &&
									productItem1.m_macAddress === productEditor.productItem.m_macAddress)){
								ok = false
								break;
							}
						}
					}

					productEditorDialog.setButtonEnabled(Enums.ok, ok)
				}
			}
		}
	}
}//Container


