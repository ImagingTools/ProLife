import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcolgui 1.0
import prolifeAccountsSdl 1.0

CollectionFieldFilterDelegate {
	name: qsTr("Customers")
	defaultFieldFilter.m_fieldId: "CustomerId"
	commandId: ProlifeAccountsSdlCommandIds.s_accountsList
	fields: [AccountItemTypeMetaInfo.s_id, AccountItemTypeMetaInfo.s_name]
	titleField: AccountItemTypeMetaInfo.s_name
	textFilterFieldIds: [AccountItemTypeMetaInfo.s_name]
	sortByField: AccountItemTypeMetaInfo.s_name
	filterPlaceholder: qsTr("Search by customer name")
}
