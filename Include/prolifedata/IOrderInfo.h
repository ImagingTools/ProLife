#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>

// ProLife includes
#include <prolifedata/IOrdered.h>
#include <prolifedata/IOrderCustomerRole.h>


namespace prolifedata
{


class IOrderInfo: virtual public iser::IObject, virtual public prolifedata::IOrdered
{
public:
	enum MetaInfoTypes
	{
		MIT_ORDER_ID = idoc::IDocumentMetaInfo::MIT_USER + 1,
		MIT_CUSTOMER_ID,
		MIT_CUSTOMER_NAME,
		MIT_ORDER_STATUS,
		MIT_PURCHASE_ORDER_ID,
		MIT_END_CUSTOMER_ID,
		MIT_END_CUSTOMER_NAME,
		MIT_INVOICE_RECIPIENT_ID,
		MIT_INVOICE_RECIPIENT_NAME,
		MIT_DELIVERY_RECIPIENT_ID,
		MIT_DELIVERY_RECIPIENT_NAME,
		MIT_RESELLER_ID,
		MIT_RESELLER_NAME,
		MIT_REFERRER_ID,
		MIT_REFERRER_NAME
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
		Get purchase ID of the order.
	*/
	virtual QByteArray GetPurchaseOrderId() const = 0;

	/**
		Set purchase order ID.
	*/
	virtual void SetPurchaseOrderId(const QByteArray& purchaseOrderId) = 0;

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
		Get ordered products.
	*/
	virtual imtbase::IObjectCollection* GetProducts() = 0;

	/**
		Get the customer roles collection (non-const).
	*/
	virtual imtbase::IObjectCollection* GetCustomerRoles() = 0;

	/**
		Get the customer roles collection (const).
	*/
	virtual const imtbase::IObjectCollection* GetCustomerRoles() const = 0;
};


} // namespace prolifedata


