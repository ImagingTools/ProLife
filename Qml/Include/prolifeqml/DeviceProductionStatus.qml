// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (C) 2024-2026 ImagingTools. All rights reserved.

import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtgui 1.0
import prolifeSensorsSdl 1.0

ProductionStatusModel {
	id: productionStatusModel;

	Component.onCompleted: {
		fillModel();
	}

	property LocalizationEvent localizationEvent: LocalizationEvent{
		onLocalizationChanged: {
			productionStatusModel.fillModel();
		}
	}

	property Component productionStatusComp: Component {
		ProductionStatus {
		}
	}

	function createStatus(id, name, icon){
		let status = productionStatusComp.createObject(m_statusModel);
		status.m_id = id;
		status.m_name = name;
		status.m_icon = icon;

		return status;
	}

	function getStatusIdByName(statusName){
		for (let i = 0; i < m_statusModel.count; i++){
			let name = m_statusModel.get(i).item.m_name
			if (name === statusName){
				return m_statusModel.get(i).item.m_id
			}
		}

		return ""
	}

	function getStatusIndex(statusId){
		for (let i = 0; i < m_statusModel.count; i++){
			let id = m_statusModel.get(i).item.m_id;
			if (id === statusId){
				return i;
			}
		}

		return -1;
	}

	function getStatusId(index){
		if (index < 0 || index >= m_statusModel.count){
			return "";
		}

		return m_statusModel.get(index).item.m_id;
	}

	function getStatusIcon(statusId){
		let index = getStatusIndex(statusId);
		if (index < 0 || index >= m_statusModel.count){
			return "";
		}

		return m_statusModel.get(index).item.m_icon;
	}

	function fillModel(){
		if (!hasStatusModel()){
			emplaceStatusModel()
		}

		m_statusModel.clear();
		m_statusModel.addElement(createStatus("None", qsTr("None"), "../../../../" + Style.getIconPath("Icons/StateUnknown", Icon.State.On, Icon.Mode.Active)));
		m_statusModel.addElement(createStatus("Accepted", qsTr("Accepted"), "../../../../" + Style.getIconPath("Icons/Timeline", Icon.State.On, Icon.Mode.Active)));
		m_statusModel.addElement(createStatus("InProgress", qsTr("In Progress"), "../../../../" + Style.getIconPath("Icons/StateUnknown", Icon.State.On, Icon.Mode.Active)));
		m_statusModel.addElement(createStatus("Canceled", qsTr("Canceled"), "../../../../" + Style.getIconPath("Icons/Cancel", Icon.State.On, Icon.Mode.Active)));
		m_statusModel.addElement(createStatus("OnHold", qsTr("On Hold"), "../../../../" + Style.getIconPath("Icons/Pause", Icon.State.On, Icon.Mode.Active)));
		m_statusModel.addElement(createStatus("Finished", qsTr("Finished"), "../../../../" + Style.getIconPath("Icons/StateOk", Icon.State.On, Icon.Mode.Active)));
		m_statusModel.addElement(createStatus("Defected", qsTr("Defect"), "../../../../" + Style.getIconPath("Icons/Defect", Icon.State.On, Icon.Mode.Active)));
		m_statusModel.addElement(createStatus("InRepair", qsTr("In Repair"), "../../../../" + Style.getIconPath("Icons/Wrench", Icon.State.On, Icon.Mode.Active)));
		m_statusModel.addElement(createStatus("Decommissioned", qsTr("Decommissioned"), "../../../../" + Style.getIconPath("Icons/Garbage", Icon.State.On, Icon.Mode.Active)));
	}
}

