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

	// The roles are filtered on: the customers page and the vendors page show different companies
	representation["Website"] = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_WEBSITE).toString();
	representation["IsCustomer"] = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_IS_CUSTOMER).toBool();
	representation["IsManufacturer"] = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_IS_MANUFACTURER).toBool();
	representation["IsSupplier"] = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_IS_SUPPLIER).toBool();
	
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

	if (representation.contains("Website")){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_WEBSITE, representation.value("Website").toString());
	}

	if (representation.contains("IsCustomer")){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_IS_CUSTOMER, representation.value("IsCustomer").toBool());
	}

	if (representation.contains("IsManufacturer")){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_IS_MANUFACTURER, representation.value("IsManufacturer").toBool());
	}

	if (representation.contains("IsSupplier")){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_IS_SUPPLIER, representation.value("IsSupplier").toBool());
	}

	return true;
}


} // namespace prolifegql


