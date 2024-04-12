#pragma once


// ImtCore includes
#include <imtauth/ICompanyInfo.h>


namespace prolifedata
{


class ICustomerInfo: virtual public imtauth::ICompanyInfo
{
public:
	/**
		Get customer-ID.
	*/
	virtual QByteArray GetCustomerId() const = 0;
};


} // namespace prolifedata


