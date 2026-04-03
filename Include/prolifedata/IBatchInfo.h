#pragma once


// ACF includes
#include <iser/IObject.h>


namespace prolifedata
{


class IBatchInfo: virtual public iser::IObject
{
public:
	/**
		Get the delivery UUID this batch belongs to.
	*/
	virtual QByteArray GetDeliveryUuid() const = 0;

	/**
		Set the delivery UUID this batch belongs to.
	*/
	virtual void SetDeliveryUuid(const QByteArray& deliveryUuid) = 0;

	/**
		Get the procurement line UUID this batch is associated with.
	*/
	virtual QByteArray GetProcurementLineUuid() const = 0;

	/**
		Set the procurement line UUID this batch is associated with.
	*/
	virtual void SetProcurementLineUuid(const QByteArray& procurementLineUuid) = 0;

	/**
		Get the batch/lot number (Chargennummer).
	*/
	virtual QString GetBatchNumber() const = 0;

	/**
		Set the batch/lot number (Chargennummer).
	*/
	virtual void SetBatchNumber(const QString& batchNumber) = 0;

	/**
		Get the manufacturer batch number (if different from internal batch number).
	*/
	virtual QString GetManufacturerBatchNumber() const = 0;

	/**
		Set the manufacturer batch number.
	*/
	virtual void SetManufacturerBatchNumber(const QString& manufacturerBatchNumber) = 0;

	/**
		Get the received quantity.
	*/
	virtual int GetQuantityReceived() const = 0;

	/**
		Set the received quantity.
	*/
	virtual void SetQuantityReceived(int quantity) = 0;

	/**
		Get the manufacture date as ISO8601 string.
	*/
	virtual QString GetManufactureDate() const = 0;

	/**
		Set the manufacture date as ISO8601 string.
	*/
	virtual void SetManufactureDate(const QString& manufactureDate) = 0;

	/**
		Get the expiry date as ISO8601 string.
	*/
	virtual QString GetExpiryDate() const = 0;

	/**
		Set the expiry date as ISO8601 string.
	*/
	virtual void SetExpiryDate(const QString& expiryDate) = 0;

	/**
		Get the storage location.
	*/
	virtual QString GetStorageLocation() const = 0;

	/**
		Set the storage location.
	*/
	virtual void SetStorageLocation(const QString& storageLocation) = 0;
};


} // namespace prolifedata
