import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
// import prolifeSensorBindingSdl 1.0
import prolifeSensorsSdl 1.0
import imtbaseImtCollectionSdl 1.0

Dialog {
    id: productEditorDialog;

    height: ModalDialogManager.activeView.height - 100;

    property int rootWidth: root.activeView ? root.activeView.width - 100 : 0;

    canMove: false;

    onRootWidthChanged: {
        width = rootWidth;
    }

    property var softwareIds: [];
    property string hardwareId: "";

    notClosingButtons: Enums.ok;

    signal saved();

    onHardwareIdChanged: {
		getDeviceBindingRequest.send();
    }

    Component.onCompleted: {
        productEditorDialog.fillButtons();
    }

    onLocalizationChanged: {
        productEditorDialog.fillButtons();
    }

    function fillButtons(){
        productEditorDialog.buttonsModel.clear();
        productEditorDialog.buttonsModel.append({"Id": Enums.ok, "Name": qsTr("Apply"), "Enabled": false});
        productEditorDialog.buttonsModel.append({"Id": Enums.cancel, "Name": qsTr("Close"), "Enabled": true});
    }

    onFinished: {
        if (buttonId == Enums.ok){
            ModalDialogManager.openDialog(messageDialog, {});
        }
    }

    contentComp: Component {
        id: productPairEditor;

        HardwareProductBindingEditor {
            id: productBinding;

            width: productEditorDialog.width;
            height: productEditorDialog.height - 100;

            onModelChanged: {
                productEditorDialog.buttons.setButtonState(Enums.ok, true);
                productEditorDialog.buttonsModel.setProperty(1, "Name", qsTr("Cancel"));
            }
        }
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
					productEditorDialog.contentItem.bindingModel = this;
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


