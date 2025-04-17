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
		
		productEditorDialog.title = qsTr("Product editor");
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

					productEditorDialog.setButtonEnabled(Enums.ok, ok)
				}
			}
		}
	}
}//Container


