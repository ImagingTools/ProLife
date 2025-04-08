#include <prolifegql/CCustomerMetaInfoDelegateComp.h>


// ProLife includes
#include <prolifedata/ICustomerInfo.h>


namespace prolifegql
{


// protected methods

bool CCustomerMetaInfoDelegateComp::FillRepresentation(
	sdl::prolife::Accounts::CAccountData::V1_0& metaInfoRepresentation,
	const idoc::IDocumentMetaInfo& metaInfo) const
{
	QByteArray customerId = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_CUSTOMER_ID).toByteArray();
	metaInfoRepresentation.customerId = customerId;
	
	QString name = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_CONTACT_NAME).toString();
	metaInfoRepresentation.name = name;
	
	QString description = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_CONTACT_DESCRIPTION).toString();
	metaInfoRepresentation.description = description;
	
	QString email = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_EMAIL).toString();
	metaInfoRepresentation.email = email;
	
	QByteArray groupIds = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_GROUPS).toByteArray();
	metaInfoRepresentation.groups = groupIds;

	return true;
}


bool CCustomerMetaInfoDelegateComp::FillMetaInfo(
	idoc::IDocumentMetaInfo& metaInfo,
	const sdl::prolife::Accounts::CAccountData::V1_0& metaInfoRepresentation) const
{
	if (metaInfoRepresentation.customerId){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_CUSTOMER_ID, *metaInfoRepresentation.customerId);
	}
	
	if (metaInfoRepresentation.name){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_CONTACT_NAME, *metaInfoRepresentation.name);
	}
	
	if (metaInfoRepresentation.description){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_CONTACT_DESCRIPTION, *metaInfoRepresentation.description);
	}
	
	if (metaInfoRepresentation.email){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_EMAIL, *metaInfoRepresentation.email);
	}
	
	if (metaInfoRepresentation.groups){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_GROUPS, *metaInfoRepresentation.groups);
	}
	
	return true;
}


} // namespace prolifegql


