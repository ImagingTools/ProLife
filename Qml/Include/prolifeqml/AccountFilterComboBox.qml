// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
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
	
	fields: ["id", "name"];
	
	property CollectionFilter complexFilter;
		
	onCurrentIndexChanged: {
		if (!complexFilter){
			return;
		}

		complexFilter.removeFieldFilter(accountFilter)
		if (currentIndex > 0){
			let value = model.getData("id", currentIndex);
			accountFilter.m_filterValue = value;
			complexFilter.addFieldFilter(accountFilter)
		}
		
		complexFilter.filterChanged()
	}
	
	onModelChanged: {
		currentIndex = -1;
		model.insertNewItem(0)
		
		model.setData("id", "All");
		model.setData("name", qsTr("All customers"))
		
		currentIndex = 0;
	}
	
	FieldFilter {
		id: accountFilter
		m_fieldId: "CustomerId"
		m_filterValueType: "String"
		m_filterOperations: ["Equal"]
	}
}

