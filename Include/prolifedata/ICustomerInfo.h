#pragma once


// ImtCore includes
#include <imtaccount/ICompanyInfo.h>


namespace prolifedata
{


class ICustomerInfo: virtual public imtaccount::ICompanyInfo
{
public:
	enum MetaInfoTypes
	{
		MIT_CUSTOMER_ID = IContactBaseInfo::MIT_GROUPS + 1
	};
	
	/**
		Get customer-ID.
	*/
	virtual QByteArray GetCustomerId() const = 0;
};


} // namespace prolifedata


