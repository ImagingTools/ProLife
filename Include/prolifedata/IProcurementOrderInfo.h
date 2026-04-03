#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>


namespace prolifedata
{


class IProcurementOrderInfo: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_PROCUREMENT_ORDER_ID = idoc::IDocumentMetaInfo::MIT_USER + 1,
		MIT_SUPPLIER_ID,
		MIT_PROCUREMENT_STATUS
	};

	enum ProcurementStatus
	{
		PS_NONE,
		PS_CREATED,
		PS_ORDERED,
		PS_PARTIALLY_RECEIVED,
		PS_RECEIVED,
		PS_IN_QC,
		PS_ON_HOLD,
		PS_CLOSED,
		PS_CANCELED
	};

	I_DECLARE_ENUM(ProcurementStatus, PS_NONE, PS_CREATED, PS_ORDERED, PS_PARTIALLY_RECEIVED, PS_RECEIVED, PS_IN_QC, PS_ON_HOLD, PS_CLOSED, PS_CANCELED);

	/**
		Get the procurement order number / identifier.
	*/
	virtual QString GetProcurementOrderNumber() const = 0;

	/**
		Set the procurement order number / identifier.
	*/
	virtual void SetProcurementOrderNumber(const QString& orderNumber) = 0;

	/**
		Get the supplier identifier.
	*/
	virtual QByteArray GetSupplierId() const = 0;

	/**
		Set the supplier identifier.
	*/
	virtual void SetSupplierId(const QByteArray& supplierId) = 0;

	/**
		Get the supplier name.
	*/
	virtual QString GetSupplierName() const = 0;

	/**
		Set the supplier name.
	*/
	virtual void SetSupplierName(const QString& supplierName) = 0;

	/**
		Get the status of this procurement order.
	*/
	virtual ProcurementStatus GetProcurementStatus() const = 0;

	/**
		Set the status of this procurement order.
	*/
	virtual void SetProcurementStatus(ProcurementStatus status) = 0;

	/**
		Get description / notes for this procurement order.
	*/
	virtual QString GetDescription() const = 0;

	/**
		Set description / notes for this procurement order.
	*/
	virtual void SetDescription(const QString& description) = 0;

	/**
		Get the optional linked order UUID (can be empty if not linked to a production order).
	*/
	virtual QByteArray GetLinkedOrderUuid() const = 0;

	/**
		Set the optional linked order UUID.
	*/
	virtual void SetLinkedOrderUuid(const QByteArray& linkedOrderUuid) = 0;

	/**
		Get expected delivery date as ISO8601 string.
	*/
	virtual QString GetExpectedDeliveryDate() const = 0;

	/**
		Set expected delivery date as ISO8601 string.
	*/
	virtual void SetExpectedDeliveryDate(const QString& expectedDeliveryDate) = 0;
};


} // namespace prolifedata
