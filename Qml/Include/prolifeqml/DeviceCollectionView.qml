import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import prolifeSensorsSdl 1.0

RemoteCollectionView {
	id: container;

	anchors.fill: parent;

	collectionId: "Devices";
	additionalFieldIds: [DeviceItemTypeMetaInfo.s_orderUuid, DeviceItemTypeMetaInfo.s_statusId]
	
	commandsDelegateComp: Component {DeviceCollectionViewCommandsDelegate {
			collectionView: container;
		}
	}

	visibleMetaInfo: true;

	Component.onCompleted: {
		table.setSortingInfo(DeviceItemTypeMetaInfo.s_timeStamp, "DESC")
		filterMenu.decorator = deviceCollectionFilterComp;
	}

	Component {
		id: deviceCollectionFilterComp;

		DeviceCollectionFilterDecorator {}
	}

	onHeadersChanged: {
		container.table.setColumnContentById(DeviceItemTypeMetaInfo.s_status, pairComp);
	}

	Component {
		id: productPairEditorDialog;

		HardwareProductBindingDialog {}
	}

	DeviceProductionStatus {
		id: deviceProductionStatus;
	}

	Component {
		id: pairComp;
		TableCellDelegateBase {
			id: cellDelegate

			Image {
				id: image;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: parent.left;
				anchors.leftMargin: 5;

				width: 20;
				height: width;

				sourceSize.width: width;
				sourceSize.height: height;
			}

			Text {
				id: statusLable;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: image.right
				anchors.leftMargin: Style.sizeMainMargin;
				anchors.right: parent.right

				font.pixelSize: Style.fontSizeNormal;
				font.family: Style.fontFamily;
				color: Style.textColor;

				elide: Text.ElideRight;
			}

			onReused: {
				if (rowIndex >= 0){
					let statusId = cellDelegate.rowDelegate.tableItem.elements.getData(DeviceItemTypeMetaInfo.s_statusId, rowIndex);
					image.source = deviceProductionStatus.getStatusIcon(statusId);
					statusLable.text = cellDelegate.getValue();
				}
			}
		}
	}
}
