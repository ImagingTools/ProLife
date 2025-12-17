import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtbaseComplexCollectionFilterSdl 1.0

FieldFilterDelegate {
	id: licensesDelegateFilter
	name: qsTr("License Status")
	
	Component.onCompleted: {
		createAndAddOption("WithoutLicense", qsTr("Sensors without a license"), "", true)
		createAndAddOption("WithLicense", qsTr("Sensors with license"), "", true)
		
		setFieldFilterForOption("WithoutLicense", withoutLicenseFilter)
		setFieldFilterForOption("WithLicense", withLicenseFilter)
	}
	
	FieldFilter {
		id: withoutLicenseFilter
		m_fieldId: "InUse"
		m_filterValue: "false"
		m_filterValueType: "Bool"
		m_filterOperations: ["Equal"]
	}
	
	FieldFilter {
		id: withLicenseFilter
		m_fieldId: "InUse"
		m_filterValue: "true"
		m_filterValueType: "Bool"
		m_filterOperations: ["Equal"]
	}
}
