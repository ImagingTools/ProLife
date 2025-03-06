import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0
import imtcolgui 1.0
import imtbaseComplexCollectionFilterSdl 1.0

ComboBoxGqlSimple {
	id: root
	currentIndex: 0;
	radius: 3;
	shownItemsCount: 15;
	
	gqlCommandId: "AccountsList"
	subscriptionCommandId: "OnAccountsCollectionChanged"
	
	fields: ["Id", "Name"];
	
	property CollectionFilter complexFilter;
		
	onCurrentIndexChanged: {
		if (!complexFilter){
			return;
		}

		complexFilter.removeFieldFilter(accountFilter)
		if (currentIndex > 0){
			let value = model.getData("Id", currentIndex);
			accountFilter.m_filterValue = value;
			complexFilter.addFieldFilter(accountFilter)
		}
		
		complexFilter.filterChanged()
	}
	
	onModelChanged: {
		currentIndex = -1;
		model.insertNewItem(0)
		
		model.setData("Id", "All");
		model.setData("Name", qsTr("All customers"))
		
		currentIndex = 0;
	}
	
	FieldFilter {
		id: accountFilter
		m_fieldId: "CustomerId"
		m_filterValueType: "String"
		m_filterOperations: ["Equal"]
	}
}

