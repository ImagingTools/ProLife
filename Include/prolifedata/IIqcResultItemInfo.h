#pragma once


// ACF includes
#include <iser/IObject.h>


namespace prolifedata
{


class IIqcResultItemInfo: virtual public iser::IObject
{
public:
	enum ValueType
	{
		VT_BOOL,
		VT_NUMBER,
		VT_TEXT,
		VT_JSON
	};

	I_DECLARE_ENUM(ValueType, VT_BOOL, VT_NUMBER, VT_TEXT, VT_JSON);

	/**
		Get the IQC run UUID this result item belongs to.
	*/
	virtual QByteArray GetIqcRunUuid() const = 0;

	/**
		Set the IQC run UUID.
	*/
	virtual void SetIqcRunUuid(const QByteArray& iqcRunUuid) = 0;

	/**
		Get the checklist item UUID from the template (nullable).
	*/
	virtual QByteArray GetTemplateItemUuid() const = 0;

	/**
		Set the checklist item UUID from the template.
	*/
	virtual void SetTemplateItemUuid(const QByteArray& templateItemUuid) = 0;

	/**
		Get the measurement / item name (e.g., "camera_scratch_score").
	*/
	virtual QString GetName() const = 0;

	/**
		Set the measurement / item name.
	*/
	virtual void SetName(const QString& name) = 0;

	/**
		Get the value type for this result item.
	*/
	virtual ValueType GetValueType() const = 0;

	/**
		Set the value type.
	*/
	virtual void SetValueType(ValueType valueType) = 0;

	/**
		Get the result value as string (serialized according to valueType).
	*/
	virtual QString GetValueText() const = 0;

	/**
		Set the result value as string.
	*/
	virtual void SetValueText(const QString& value) = 0;

	/**
		Get the pass/fail result (0 = fail, 1 = pass, -1 = not evaluated).
	*/
	virtual int GetPassResult() const = 0;

	/**
		Set the pass/fail result.
	*/
	virtual void SetPassResult(int passResult) = 0;

	/**
		Get the unit of measure (for numeric values).
	*/
	virtual QString GetUnit() const = 0;

	/**
		Set the unit of measure.
	*/
	virtual void SetUnit(const QString& unit) = 0;

	/**
		Get comma-separated evidence document references (ImtCore document IDs).
	*/
	virtual QString GetEvidenceRefs() const = 0;

	/**
		Set comma-separated evidence document references.
	*/
	virtual void SetEvidenceRefs(const QString& evidenceRefs) = 0;
};


} // namespace prolifedata
