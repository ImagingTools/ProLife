#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace prolifedata
{


/**
	Interface for describing an Product.
	\ingroup Authentification
*/
class IOrderInfo: virtual public iser::IObject
{
public:
	typedef QByteArrayList ProductIds;

	enum MetaInfoTypes
	{

		/**
			Order Id given as QString.
		*/
		MIT_ORDER_ID,

		/**
			Order customer given as QString.
		*/
		MIT_ORDER_CUSTOMER,
	};

	/**
		Get id of the order.
	*/
	virtual QByteArray GetOrderId() const = 0;

	/**
		Set id of the order.
	*/
	virtual void SetOrderId(const QByteArray& orderId) = 0;

	/**
		Get customer of the order.
	*/
	virtual QByteArray GetCustomerId() const = 0;

	/**
		Set order customer.
	*/
	virtual void SetCustomerId(const QByteArray& customerId) = 0;

	/**
		Get description of the order.
	*/
	virtual QByteArray GetDescription() const = 0;
	/**
		Set order description.
	*/
	virtual void SetDescription(const QByteArray& description) = 0;

	/**
		Get order products.
	*/
	virtual imtbase::IObjectCollection* GetProducts() = 0;


};


} // namespace prolifedata


