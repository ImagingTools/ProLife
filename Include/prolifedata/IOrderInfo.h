// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

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
		MIT_ORDER_ID = idoc::IDocumentMetaInfo::MIT_USER + 1,
		MIT_CUSTOMER_ID,
		MIT_CUSTOMER_NAME,
		MIT_ORDER_STATUS,
		MIT_PURCHASE_ORDER_ID
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
		Get ordered products.
	*/
	virtual imtbase::IObjectCollection* GetProducts() = 0;
};


} // namespace prolifedata


