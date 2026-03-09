// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

FieldFilterDelegate {
	id: customersDelegateFilter
	name: qsTr("Customers")
	visibleItemCount: 20
	defaultFieldFilter.m_fieldId: "CustomerId"
	
	OptionsListAdapter {
		id: optionsListAdapter
		collectionModel: CachedAccountCollection.collectionModel
		
		onCollectionModelChanged: {
			customersDelegateFilter.setOptionsList(m_options)
		}
	}
}
