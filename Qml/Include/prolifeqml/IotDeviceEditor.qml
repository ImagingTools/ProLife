import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtdocgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import prolifeqml 1.0
import prolifeSensorsSdl 1.0

ViewBase {
	id: iotDeviceEditorContainer;

	property IotDeviceData iotDeviceData: model;

	function updateGui(){
		if (!iotDeviceData){
			return;
		}
	}

	function updateModel(){
		if (!iotDeviceData){
			return;
		}
	}

	Flickable {
		id: flickable;

		anchors.left: parent.left;
		anchors.leftMargin: Style.marginXL;

		anchors.top: parent.top;
		anchors.topMargin: Style.marginXL;

		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.marginXL;

		anchors.right: parent.right;
		anchors.rightMargin: Style.marginXL;

		contentWidth: bodyColumn.width;
		contentHeight: bodyColumn.height + 2 * Style.marginXL;

		boundsBehavior: Flickable.StopAtBounds;

		clip: true;

		Column {
			id: bodyColumn;

			width: 700;

			spacing: Style.marginXL;

			GroupHeaderView {
				width: parent.width;

				title: qsTr("Basic Information");
				groupView: basicInformationGroup;
			}

			GroupElementView {
				id: basicInformationGroup;

				width: parent.width;

				TextInputElementView {
					id: factoryNumberInput;

					name: qsTr("Serial Number");
					placeHolderText: qsTr("Enter serial number");

					onEditingFinished: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}

				ComboBoxElementView {
					id: modelIdInput;
					name: qsTr("Model");

					onCurrentIndexChanged: {
						iotDeviceEditorContainer.doUpdateModel();
					}
				}
			}
		}
	}
}
