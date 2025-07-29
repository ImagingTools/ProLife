import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcolgui 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtdocgui 1.0
import prolifeSensorsSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

RemoteCollectionView {
	id: container;

	anchors.fill: parent;

	collectionId: "Devices";
	additionalFieldIds: [DeviceItemTypeMetaInfo.s_orderUuid, DeviceItemTypeMetaInfo.s_statusId, DeviceItemTypeMetaInfo.s_deviceType]
	documentCollectionFilter: null
	commandsDelegateComp: Component {DeviceCollectionViewCommandsDelegate {
			collectionView: container;
		}
	}

	visibleMetaInfo: true;

	Component.onCompleted: {
		table.setSortingInfo(DeviceItemTypeMetaInfo.s_timeStamp, "DESC")
		
		registerFieldFilterDelegate("SoftwareCount", licensesDelegateFilterComp)
		registerFieldFilterDelegate("Customers", customersDelegateFilterComp)
		registerFieldFilterDelegate("SensorStatus", statusDelegateFilterComp)
	}

	Component {
		id: statusDelegateFilterComp
		FieldFilterDelegate {
			id: statusDelegateFilter
			name: qsTr("Sensor Status")
			defaultFieldFilter.m_fieldId: "Status"
			defaultFieldFilter.m_filterValueType: "Integer"
			visibleItemCount: 15
			
			Component.onCompleted: {
				createAndAddOption("0", qsTr("None"), "", true)
				createAndAddOption("1", qsTr("Accepted"), "", true)
				createAndAddOption("2", qsTr("In Progress"), "", true)
				createAndAddOption("3", qsTr("Canceled"), "", true)
				createAndAddOption("4", qsTr("On Hold"), "", true)
				createAndAddOption("5", qsTr("Finished"), "", true)
				createAndAddOption("6", qsTr("Defect"), "", true)
				createAndAddOption("7", qsTr("In Repair"), "", true)
				createAndAddOption("8", qsTr("Decommissioned"), "", true)
			}
		}
	}

	Component {
		id: licensesDelegateFilterComp
		FieldFilterDelegate {
			id: licensesDelegateFilter
			name: qsTr("License")
			
			Component.onCompleted: {
				createAndAddOption("WithoutLicense", qsTr("Sensors without a license"), "", true)
				createAndAddOption("WithLicense", qsTr("Sensors with license"), "", true)
				
				setFieldFilterForOption("WithoutLicense", withoutLicenseFilter)
				setFieldFilterForOption("WithLicense", withLicenseFilter)
			}
			
			FieldFilter {
				id: withoutLicenseFilter
				m_fieldId: "SoftwareCount"
				m_filterValue: "0"
				m_filterValueType: "Integer"
				m_filterOperations: ["Equal"]
			}
			
			FieldFilter {
				id: withLicenseFilter
				m_fieldId: "SoftwareCount"
				m_filterValue: "0"
				m_filterValueType: "Integer"
				m_filterOperations: ["Greater"]
			}
		}
	}
	
	Component {
		id: customersDelegateFilterComp
		FieldFilterDelegate {
			id: customersDelegateFilter
			name: qsTr("Customers")
			visibleItemCount: 15
			defaultFieldFilter.m_fieldId: "CustomerId"
			
			OptionsListAdapter {
				id: optionsListAdapter
				collectionModel: CachedAccountCollection.collectionModel
				
				onCollectionModelChanged: {
					customersDelegateFilter.setOptionsList(m_options)
				}
			}
		}
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
				anchors.leftMargin: Style.marginM;
				anchors.right: parent.right

				font.pixelSize: Style.fontSizeM;
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
