#pragma once


// ImtCore includes
#include <imtaccount/ICompanyInfo.h>


namespace prolifedata
{


/**
	A company ProLife works with. Its roles tell what it is: a customer, a manufacturer of hardware
	components, a supplier they are bought from - or several of these at once.
*/
class ICustomerInfo: virtual public imtaccount::ICompanyInfo
{
public:
	enum MetaInfoTypes
	{
		MIT_CUSTOMER_ID = IContactBaseInfo::MIT_GROUPS + 1,
		MIT_WEBSITE,
		MIT_IS_CUSTOMER,
		MIT_IS_MANUFACTURER,
		MIT_IS_SUPPLIER
	};

	/**
		Get customer-ID.
	*/
	virtual QByteArray GetCustomerId() const = 0;

	virtual QString GetWebsite() const = 0;
	virtual void SetWebsite(const QString& website) = 0;

	virtual bool IsCustomer() const = 0;
	virtual void SetCustomer(bool isCustomer) = 0;
	virtual bool IsManufacturer() const = 0;
	virtual void SetManufacturer(bool isManufacturer) = 0;
	virtual bool IsSupplier() const = 0;
	virtual void SetSupplier(bool isSupplier) = 0;
};


} // namespace prolifedata


