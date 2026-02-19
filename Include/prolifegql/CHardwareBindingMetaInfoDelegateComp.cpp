// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#include <prolifegql/CHardwareBindingMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>

// ProLife includes
#include <prolifedata/IHardwareProductBinding.h>


namespace prolifegql
{


// protected methods

bool CHardwareBindingMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo, const QByteArray& /*typeId*/) const
{
	QByteArray hardwareId = metaInfo.GetMetaInfo(prolifedata::IHardwareProductBinding::MIT_HARDWARE_ID).toByteArray();
	representation["HardwareId"] = QString(hardwareId);
	
	QByteArray softwareIds = metaInfo.GetMetaInfo(prolifedata::IHardwareProductBinding::MIT_SOFTWARE_IDS).toByteArray();
	representation["SoftwareIds"] = QString(softwareIds);
	
	return true;
}


bool CHardwareBindingMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation, const QByteArray& /*typeId*/) const
{
	if (representation.contains("HardwareId")){
		metaInfo.SetMetaInfo(prolifedata::IHardwareProductBinding::MIT_HARDWARE_ID, representation.value("HardwareId"));
	}
	
	if (representation.contains("SoftwareIds")){
		metaInfo.SetMetaInfo(prolifedata::IHardwareProductBinding::MIT_SOFTWARE_IDS, representation.value("SoftwareIds"));
	}
	
	return true;
}


} // namespace prolifegql


