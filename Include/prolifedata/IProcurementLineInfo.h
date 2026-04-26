#pragma once


// ACF includes
#include <iser/IObject.h>


namespace prolifedata
{


class IProcurementLineInfo: virtual public iser::IObject
{
public:
	enum ComponentCriticality
	{
		CC_LOW,
		CC_MEDIUM,
		CC_HIGH
	};

	I_DECLARE_ENUM(ComponentCriticality, CC_LOW, CC_MEDIUM, CC_HIGH);

	/**
		Get the procurement order UUID this line belongs to.
	*/
	virtual QByteArray GetProcurementOrderUuid() const = 0;

	/**
		Set the procurement order UUID this line belongs to.
	*/
	virtual void SetProcurementOrderUuid(const QByteArray& procurementOrderUuid) = 0;

	/**
		Get the component identifier / part number.
	*/
	virtual QString GetComponentId() const = 0;

	/**
		Set the component identifier / part number.
	*/
	virtual void SetComponentId(const QString& componentId) = 0;

	/**
		Get the component name.
	*/
	virtual QString GetComponentName() const = 0;

	/**
		Set the component name.
	*/
	virtual void SetComponentName(const QString& componentName) = 0;

	/**
		Get the manufacturer part number (MPN).
	*/
	virtual QString GetManufacturerPartNumber() const = 0;

	/**
		Set the manufacturer part number (MPN).
	*/
	virtual void SetManufacturerPartNumber(const QString& mpn) = 0;

	/**
		Get the manufacturer name.
	*/
	virtual QString GetManufacturer() const = 0;

	/**
		Set the manufacturer name.
	*/
	virtual void SetManufacturer(const QString& manufacturer) = 0;

	/**
		Get the requested quantity.
	*/
	virtual int GetRequestedQuantity() const = 0;

	/**
		Set the requested quantity.
	*/
	virtual void SetRequestedQuantity(int quantity) = 0;

	/**
		Get the unit of measure.
	*/
	virtual QString GetUnit() const = 0;

	/**
		Set the unit of measure.
	*/
	virtual void SetUnit(const QString& unit) = 0;

	/**
		Get the criticality level of this component.
	*/
	virtual ComponentCriticality GetCriticality() const = 0;

	/**
		Set the criticality level of this component.
	*/
	virtual void SetCriticality(ComponentCriticality criticality) = 0;
};


} // namespace prolifedata
