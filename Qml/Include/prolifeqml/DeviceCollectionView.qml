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
	collectionFilter: DeviceCollectionFilter {}
	commandsViewComp: Component {
		id: commandsDecoratorComp;

		CommandsPanel {
			commandId: container.viewId;

			onCommandActivated: {
				if (container.commandsDelegate){
					container.commandsDelegate.commandHandle(commandId);
				}
			}
		}
	}

	commandsDelegateComp: Component {DeviceCollectionViewCommandsDelegate {
			collectionView: container;
		}
	}

	Component {
		id: tableDelegate;
		TablePainterRowDelegateBase{
			id: tableRowDelegateBase
			tableItem: container.table
			width: container.table.width
			minHeight: container.table.itemHeight
			readOnly: container.table.readOnly;
		}
	}

	function drawStatusColumnDelegate(ctx, x, y, cellWidth, cellHeight, columnIndex, canvas){
		let statusId = canvas.rowDelegate.tableItem.elements.getData(DeviceItemTypeMetaInfo.s_statusId, canvas.rowDelegate.rowIndex);
		let source = deviceProductionStatus.getStatusIcon(statusId);

		let ratio = 0.5;
		ctx.save();
		ctx.scale(ratio, ratio);
		ctx.drawImage(source, x, y, 20 *1/ratio, 20*1/ratio);
		// ctx.drawImage(source, x, y, 20, 20);
		ctx.restore();
	}

	visibleMetaInfo: true;

	Component.onCompleted: {
		collectionFilter.setSortingOrder("DESC");
		collectionFilter.setSortingInfoId(DeviceItemTypeMetaInfo.s_lastModified);

		filterMenu.decorator = deviceCollectionFilterComp;
	}

	Component {
		id: textFilterComp;
		CustomTextField {
			id: tfc;
			textFieldRightMargin: iconClear.width + 2 * margin;
			width: 270;
			height: 30;
			placeHolderText: qsTr("Enter some text to filter the item list");
			ToolButton {
				id: iconClear;

				z: 999;

				anchors.verticalCenter: tfc.verticalCenter;
				anchors.right: tfc.right;
				anchors.rightMargin: Style.margin;

				width: Style.buttonWidthSmall;
				height: width;

				visible: tfc.text != "";

				iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);
				decorator: Component {
					ToolButtonDecorator {
						color: "transparent";
						icon.width: 16;
					}
				}

				onClicked: {
					tfc.text = "";
				}
			}
		}
	}

	Component {
		id: accountFilterComp;

		CustomTextField {
			id: tfc;
			width: 270;
			height: 30;
			placeHolderText: qsTr("Account");
		}
	}

	Component {
		id: licenseFilterComp;

		CustomTextField {
			id: tfc;
			width: 270;
			height: 30;
			placeHolderText: qsTr("License");
		}
	}

	Component {
		id: deviceCollectionFilterComp;

		DeviceCollectionFilterDecorator {}
	}

	function onFilterChanged(filterId, filterValue){
		if (filterId == "AccountFilter"){
			container.collectionFilter.setAccountFilter(filterValue);
		}
		else if (filterId == "LicenseFilter"){
			container.collectionFilter.setLicenseFilter(filterValue);
		}
		else if (filterId == "StatusFilter"){
			container.collectionFilter.setDeviceStatusFilter(filterValue);
		}

		container.doUpdateGui();
	}

	onHeadersChanged: {
		container.table.setColumnContentById(DeviceItemTypeMetaInfo.s_status, pairComp);
	}

	Component {
		id: productPairEditorDialog;

		HardwareProductBindingDialog {
		}
	}

	MetaInfoProvider {
		id: metaInfoProvider;

		onMetaInfoModelChanged: {
			container.setMetaInfoModel(metaInfoModel);
		}
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
				anchors.leftMargin: Style.size_mainMargin;
				anchors.right: parent.right

				font.pixelSize: Style.fontSize_common;
				font.family: Style.fontFamily;
				color: Style.textColor;

				elide: Text.ElideRight;
			}

			onDraw: {
				if (!rowDelegate){
					return
				}

				if (rowIndex >= 0){
					let statusId = cellDelegate.rowDelegate.tableItem.elements.getData(DeviceItemTypeMetaInfo.s_statusId, rowIndex);
					image.source = deviceProductionStatus.getStatusIcon(statusId);
					statusLable.text = cellDelegate.getValue();
				}
			}
		}
	}
}
