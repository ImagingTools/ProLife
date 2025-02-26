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
	metaInfoRepresentation.CustomerId = customerId;
	
	QString name = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_CONTACT_NAME).toString();
	metaInfoRepresentation.Name = name;
	
	QString description = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_CONTACT_DESCRIPTION).toString();
	metaInfoRepresentation.Description = description;
	
	QString email = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_EMAIL).toString();
	metaInfoRepresentation.Email = email;
	
	QByteArray groupIds = metaInfo.GetMetaInfo(prolifedata::ICustomerInfo::MIT_GROUPS).toByteArray();
	metaInfoRepresentation.Groups = groupIds;

	return true;
}


bool CCustomerMetaInfoDelegateComp::FillMetaInfo(
	idoc::IDocumentMetaInfo& metaInfo,
	const sdl::prolife::Accounts::CAccountData::V1_0& metaInfoRepresentation) const
{
	if (metaInfoRepresentation.CustomerId){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_CUSTOMER_ID, *metaInfoRepresentation.CustomerId);
	}
	
	if (metaInfoRepresentation.Name){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_CONTACT_NAME, *metaInfoRepresentation.Name);
	}
	
	if (metaInfoRepresentation.Description){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_CONTACT_DESCRIPTION, *metaInfoRepresentation.Description);
	}
	
	if (metaInfoRepresentation.Email){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_EMAIL, *metaInfoRepresentation.Email);
	}
	
	if (metaInfoRepresentation.Groups){
		metaInfo.SetMetaInfo(prolifedata::ICustomerInfo::MIT_GROUPS, *metaInfoRepresentation.Groups);
	}
	
	return true;
}


} // namespace prolifegql


