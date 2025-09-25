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
