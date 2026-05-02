#pragma once


// ACF includes
#include <istd/IChangeable.h>


namespace prolifedata
{


class IOrderCustomerRole: virtual public istd::IChangeable
{
public:
	enum RoleType
	{
		RT_ORDERING_PARTY,      // Auftraggeber / Besteller (mandatory)
		RT_END_CUSTOMER,        // Endkunde / Empfänger
		RT_INVOICE_RECIPIENT,   // Rechnungsempfänger
		RT_DELIVERY_RECIPIENT,  // Lieferempfänger
		RT_RESELLER,            // Vermittler / Wiederverkäufer
		RT_REFERRER             // Zuweiser / Empfehlender
	};

	I_DECLARE_ENUM(RoleType, RT_ORDERING_PARTY, RT_END_CUSTOMER,
				   RT_INVOICE_RECIPIENT, RT_DELIVERY_RECIPIENT,
				   RT_RESELLER, RT_REFERRER);

	/**
		Get the customer ID for this role.
	*/
	virtual QByteArray GetCustomerId() const = 0;

	/**
		Set the customer ID for this role.
	*/
	virtual void SetCustomerId(const QByteArray& customerId) = 0;

	/**
		Get the role type.
	*/
	virtual RoleType GetRoleType() const = 0;

	/**
		Set the role type.
	*/
	virtual void SetRoleType(RoleType roleType) = 0;
};


} // namespace prolifedata


