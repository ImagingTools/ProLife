#pragma once


// ACF includes
#include <iser/IObject.h>


namespace prolifedata
{


class IDeliveryInfo: virtual public iser::IObject
{
public:
	enum DeliveryStatus
	{
		DS_ANNOUNCED,
		DS_RECEIVED,
		DS_RETURNED
	};

	I_DECLARE_ENUM(DeliveryStatus, DS_ANNOUNCED, DS_RECEIVED, DS_RETURNED);

	/**
		Get the procurement order UUID this delivery belongs to.
	*/
	virtual QByteArray GetProcurementOrderUuid() const = 0;

	/**
		Set the procurement order UUID this delivery belongs to.
	*/
	virtual void SetProcurementOrderUuid(const QByteArray& procurementOrderUuid) = 0;

	/**
		Get the delivery note number / document reference.
	*/
	virtual QString GetDeliveryNoteNumber() const = 0;

	/**
		Set the delivery note number / document reference.
	*/
	virtual void SetDeliveryNoteNumber(const QString& deliveryNoteNumber) = 0;

	/**
		Get the delivery date as ISO8601 string.
	*/
	virtual QString GetDeliveryDate() const = 0;

	/**
		Set the delivery date as ISO8601 string.
	*/
	virtual void SetDeliveryDate(const QString& deliveryDate) = 0;

	/**
		Get the name/ID of the person who received the delivery.
	*/
	virtual QString GetReceivedBy() const = 0;

	/**
		Set the name/ID of the person who received the delivery.
	*/
	virtual void SetReceivedBy(const QString& receivedBy) = 0;

	/**
		Get carrier name.
	*/
	virtual QString GetCarrier() const = 0;

	/**
		Set carrier name.
	*/
	virtual void SetCarrier(const QString& carrier) = 0;

	/**
		Get tracking number.
	*/
	virtual QString GetTrackingNumber() const = 0;

	/**
		Set tracking number.
	*/
	virtual void SetTrackingNumber(const QString& trackingNumber) = 0;

	/**
		Get freetext annotation.
	*/
	virtual QString GetAnnotation() const = 0;

	/**
		Set freetext annotation.
	*/
	virtual void SetAnnotation(const QString& annotation) = 0;

	/**
		Get delivery status.
	*/
	virtual DeliveryStatus GetDeliveryStatus() const = 0;

	/**
		Set delivery status.
	*/
	virtual void SetDeliveryStatus(DeliveryStatus status) = 0;
};


} // namespace prolifedata
