#pragma once


// ACF includes
#include <iser/IObject.h>


namespace prolifedata
{


class IIqcChecklistItemInfo: virtual public iser::IObject
{
public:
	enum ItemSeverity
	{
		IS_INFO,
		IS_MAJOR,
		IS_CRITICAL
	};

	enum ExpectedValueType
	{
		EVT_BOOLEAN,
		EVT_NUMERIC,
		EVT_TEXT,
		EVT_ENUM
	};

	I_DECLARE_ENUM(ItemSeverity, IS_INFO, IS_MAJOR, IS_CRITICAL);
	I_DECLARE_ENUM(ExpectedValueType, EVT_BOOLEAN, EVT_NUMERIC, EVT_TEXT, EVT_ENUM);

	/**
		Get the IQC template UUID this item belongs to.
	*/
	virtual QByteArray GetTemplateUuid() const = 0;

	/**
		Set the IQC template UUID this item belongs to.
	*/
	virtual void SetTemplateUuid(const QByteArray& templateUuid) = 0;

	/**
		Get the item title / name.
	*/
	virtual QString GetTitle() const = 0;

	/**
		Set the item title / name.
	*/
	virtual void SetTitle(const QString& title) = 0;

	/**
		Get the item description.
	*/
	virtual QString GetDescription() const = 0;

	/**
		Set the item description.
	*/
	virtual void SetDescription(const QString& description) = 0;

	/**
		Get the severity of this checklist item.
	*/
	virtual ItemSeverity GetSeverity() const = 0;

	/**
		Set the severity of this checklist item.
	*/
	virtual void SetSeverity(ItemSeverity severity) = 0;

	/**
		Check if this item is required to complete the run.
	*/
	virtual bool IsRequired() const = 0;

	/**
		Set whether this item is required to complete the run.
	*/
	virtual void SetRequired(bool required) = 0;

	/**
		Get the expected value type for this checklist item.
	*/
	virtual ExpectedValueType GetExpectedValueType() const = 0;

	/**
		Set the expected value type for this checklist item.
	*/
	virtual void SetExpectedValueType(ExpectedValueType valueType) = 0;

	/**
		Get optional minimum numeric bound (as string, empty if not applicable).
	*/
	virtual QString GetMinValue() const = 0;

	/**
		Set optional minimum numeric bound.
	*/
	virtual void SetMinValue(const QString& minValue) = 0;

	/**
		Get optional maximum numeric bound (as string, empty if not applicable).
	*/
	virtual QString GetMaxValue() const = 0;

	/**
		Set optional maximum numeric bound.
	*/
	virtual void SetMaxValue(const QString& maxValue) = 0;

	/**
		Get the unit of measure for numeric values.
	*/
	virtual QString GetUnit() const = 0;

	/**
		Set the unit of measure for numeric values.
	*/
	virtual void SetUnit(const QString& unit) = 0;
};


} // namespace prolifedata
