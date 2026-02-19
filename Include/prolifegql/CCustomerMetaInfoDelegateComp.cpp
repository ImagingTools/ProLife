// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#include <prolifegql/CCustomerMetaInfoDelegateComp.h>


// Qt includes
#include <QJsonObject>

// ProLife includes
#include <prolifedata/ICustomerInfo.h>


namespace prolifegql
{


bool CCustomerMetaInfoDelegateComp::FillRepresentation(QJsonObject& representation, const idoc::IDocumentMetaInfo& metaInfo, const QByteArray& /*typeId*/) const
{
	QByteArray customerId = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_CUSTOMER_ID).toByteArray();
	representation["CustomerId"] = QString(customerId);
	
	QString name = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_CONTACT_NAME).toString();
	representation["Name"] = name;
	
	QString description = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_CONTACT_DESCRIPTION).toString();
	representation["Description"] = description;
	
	QString email = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_EMAIL).toString();
	representation["Email"] = email;
	
	QByteArray groupIds = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_GROUPS).toByteArray();
	representation["Groups"] = QString(groupIds);
	
	return true;
}


bool CCustomerMetaInfoDelegateComp::FillMetaInfo(idoc::IDocumentMetaInfo& metaInfo, const QJsonObject& representation, const QByteArray& /*typeId*/) const
{
	if (representation.contains("CustomerId")){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_CUSTOMER_ID, representation.value("CustomerId"));
	}
	
	if (representation.contains("Name")){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_CONTACT_NAME, representation.value("Name"));
	}
	
	if (representation.contains("Description")){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_CONTACT_DESCRIPTION, representation.value("Description"));
	}
	
	if (representation.contains("Email")){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_EMAIL, representation.value("Email"));
	}
	
	if (representation.contains("Groups")){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_GROUPS, representation.value("Groups"));
	}

	return true;
}


} // namespace prolifegql


