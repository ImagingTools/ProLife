// SPDX-License-Identifier: LicenseRef-Commercial
// Copyright (c) 2024-2026 ImagingTools
// All rights reserved. See LICENSE file for details.

#pragma once


// ACF includes
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

	/**
		Get purchase ID of the order.
	*/
	virtual QByteArray GetPurchaseOrderId() const = 0;

	/**
		Set purchase order ID.
	*/
	virtual void SetPurchaseOrderId(const QByteArray& purchaseOrderId) = 0;
};


} // namespace prolifedata


