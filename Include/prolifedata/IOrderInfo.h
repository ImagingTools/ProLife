#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>

// ProLife includes
#include <prolifedata/IOrdered.h>


namespace prolifedata
{


class IOrderInfo: virtual public iser::IObject, virtual public prolifedata::IOrdered
{
public:
	enum MetaInfoTypes
	{
		/**
			Order Id given as QString.
		*/
		MIT_ORDER_ID = idoc::IDocumentMetaInfo::MIT_USER,

		/**
			Order customer given as QString.
		*/
		MIT_ORDER_CUSTOMER,

		/**
			Order status given as OrderStatus.
		*/
		MIT_ORDER_STATUS
	};

	enum OrderStatus
	{
		OS_NONE,
		OS_CREATED,
		OS_IN_PROGRESS,
		OS_CANCELED,
		OS_ON_HOLD,
		OS_FINISHED,
		OS_CLOSED
	};

	I_DECLARE_ENUM(OrderStatus, OS_NONE, OS_CREATED, OS_IN_PROGRESS, OS_CANCELED, OS_ON_HOLD, OS_FINISHED, OS_CLOSED);

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
	virtual QString GetDescription() const = 0;
	/**
		Set order description.
	*/
	virtual void SetDescription(const QString& description) = 0;

	/**
		Get the status of this order.
	*/
	virtual OrderStatus GetOrderStatus() const = 0;

	/**
		Set the status of this order.
	*/
	virtual void SetOrderStatus(OrderStatus status) = 0;

	/**
		Get order products.
	*/
	virtual imtbase::IObjectCollection* GetProducts() = 0;
};


} // namespace prolifedata


