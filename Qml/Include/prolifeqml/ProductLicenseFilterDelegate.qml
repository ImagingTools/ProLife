import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtlicLicensesSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

// Licenses of the product chosen in the filter this one depends on, or all licenses without it.
CollectionFieldFilterDelegate {
	id: licenseFilterDelegate

	// The product filter this one is scoped by (FilterMenu.setFilterDependency).
	readonly property string productUuid: licenseFilterDelegate.isScoped ? licenseFilterDelegate.scopeFilter.selectedId : ""

	name: qsTr("Licenses")
	commandId: ImtlicLicensesSdlCommandIds.s_licensesList
	fields: [LicenseItemTypeMetaInfo.s_id, LicenseItemTypeMetaInfo.s_licenseName, LicenseItemTypeMetaInfo.s_licenseId]
	titleField: LicenseItemTypeMetaInfo.s_licenseName
	textFilterFieldIds: [LicenseItemTypeMetaInfo.s_licenseName, LicenseItemTypeMetaInfo.s_licenseId]
	sortByField: LicenseItemTypeMetaInfo.s_licenseName
	filterPlaceholder: qsTr("Search by license name or article")
	groupFilters: licenseFilterDelegate.productUuid !== "" ? [productGroup] : []

	FieldFilter {
		id: productFilter
		m_fieldId: "ProductId"
		m_filterValue: licenseFilterDelegate.productUuid
		m_filterValueType: "String"
		m_filterOperations: ["Equal"]
	}

	GroupFilter {
		id: productGroup
		m_logicalOperation: "And"

		Component.onCompleted: {
			productGroup.emplaceFieldFilters()
			productGroup.m_fieldFilters.addElement(productFilter)
		}
	}
}
