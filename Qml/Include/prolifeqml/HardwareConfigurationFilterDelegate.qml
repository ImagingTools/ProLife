import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import prolifeHardwareConfigurationsSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

// Configurations of the hardware model chosen in the filter this one depends on, or all configurations without it.
CollectionFieldFilterDelegate {
	id: configurationFilterDelegate

	// The hardware model filter this one is scoped by (FilterMenu.setFilterDependency).
	readonly property string modelId: configurationFilterDelegate.isScoped ? configurationFilterDelegate.scopeFilter.selectedId : ""

	name: qsTr("Configurations")
	commandId: ProlifeHardwareConfigurationsSdlCommandIds.s_hardwareConfigurationsList
	fields: [HardwareConfigurationItemTypeMetaInfo.s_id, HardwareConfigurationItemTypeMetaInfo.s_name, HardwareConfigurationItemTypeMetaInfo.s_configurationId]
	titleField: HardwareConfigurationItemTypeMetaInfo.s_name
	textFilterFieldIds: [HardwareConfigurationItemTypeMetaInfo.s_name, HardwareConfigurationItemTypeMetaInfo.s_configurationId]
	sortByField: HardwareConfigurationItemTypeMetaInfo.s_name
	filterPlaceholder: qsTr("Search by configuration name or article")
	groupFilters: configurationFilterDelegate.modelId !== "" ? [modelGroup] : []

	FieldFilter {
		id: modelFilter
		m_fieldId: "ProductId"
		m_filterValue: configurationFilterDelegate.modelId
		m_filterValueType: "String"
		m_filterOperations: ["Equal"]
	}

	GroupFilter {
		id: modelGroup
		m_logicalOperation: "And"

		Component.onCompleted: {
			modelGroup.emplaceFieldFilters()
			modelGroup.m_fieldFilters.addElement(modelFilter)
		}
	}
}
