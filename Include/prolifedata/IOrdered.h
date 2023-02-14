#pragma once


// ACF includes
#include <iser/IObject.h>
#include <istd/IPolymorphic.h>


namespace prolifedata
{


class IOrdered: virtual public istd::IPolymorphic
{
public:
	/**
		Get ID of the order.
	*/
	virtual QByteArray GetOrderId() const = 0;

	/**
		Set order ID.
	*/
	virtual void SetOrderId(const QByteArray& orderId) = 0;
};


} // namespace prolifedata


