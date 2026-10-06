import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import imtlicProductsSdl 1.0
import imtbaseComplexCollectionFilterSdl 1.0

CollectionFieldFilterDelegate {
	id: productFilterDelegate

	//! Product category the options are limited to, e.g. "Software" or "Hardware".
	property string categoryId

	name: qsTr("Products")
	commandId: ImtlicProductsSdlCommandIds.s_productsList
	fields: [ProductItemTypeMetaInfo.s_id, ProductItemTypeMetaInfo.s_productName]
	titleField: ProductItemTypeMetaInfo.s_productName
	textFilterFieldIds: [ProductItemTypeMetaInfo.s_productName]
	sortByField: ProductItemTypeMetaInfo.s_productName
	filterPlaceholder: qsTr("Search by product name")
	groupFilters: [categoryGroup]

	FieldFilter {
		id: categoryFilter
		m_fieldId: "CategoryId"
		m_filterValue: productFilterDelegate.categoryId
		m_filterValueType: "String"
		m_filterOperations: ["Equal"]
	}

	GroupFilter {
		id: categoryGroup
		m_logicalOperation: "And"

		Component.onCompleted: {
			categoryGroup.emplaceFieldFilters()
			categoryGroup.m_fieldFilters.addElement(categoryFilter)
		}
	}
}
