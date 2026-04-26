#pragma once


// ACF includes
#include <iser/IObject.h>


namespace prolifedata
{


class IIqcTemplateInfo: virtual public iser::IObject
{
public:
	enum CompletionRule
	{
		CR_ALL_REQUIRED_DONE,
		CR_ALL_ITEMS_DONE,
		CR_AT_LEAST_ONE_DONE
	};

	I_DECLARE_ENUM(CompletionRule, CR_ALL_REQUIRED_DONE, CR_ALL_ITEMS_DONE, CR_AT_LEAST_ONE_DONE);

	/**
		Get the supplier identifier this template applies to.
		An empty ID means the template applies to any supplier.
	*/
	virtual QByteArray GetSupplierId() const = 0;

	/**
		Set the supplier identifier this template applies to.
	*/
	virtual void SetSupplierId(const QByteArray& supplierId) = 0;

	/**
		Get the component identifier this template applies to.
		An empty ID means the template applies to any component.
	*/
	virtual QString GetComponentId() const = 0;

	/**
		Set the component identifier this template applies to.
	*/
	virtual void SetComponentId(const QString& componentId) = 0;

	/**
		Get the template name.
	*/
	virtual QString GetTemplateName() const = 0;

	/**
		Set the template name.
	*/
	virtual void SetTemplateName(const QString& name) = 0;

	/**
		Get the template version number.
	*/
	virtual int GetVersion() const = 0;

	/**
		Set the template version number.
	*/
	virtual void SetVersion(int version) = 0;

	/**
		Check if this template is active.
	*/
	virtual bool IsActive() const = 0;

	/**
		Set the active state of this template.
	*/
	virtual void SetActive(bool active) = 0;

	/**
		Get the completion rule for this template.
	*/
	virtual CompletionRule GetCompletionRule() const = 0;

	/**
		Set the completion rule for this template.
	*/
	virtual void SetCompletionRule(CompletionRule rule) = 0;

	/**
		Get additional pass rule description / expression.
	*/
	virtual QString GetPassRuleDescription() const = 0;

	/**
		Set additional pass rule description / expression.
	*/
	virtual void SetPassRuleDescription(const QString& passRuleDescription) = 0;

	/**
		Get checklist items as a JSON array string.
		Each element is a JSON object with the IqcChecklistItem fields.
	*/
	virtual QString GetChecklistItemsJson() const = 0;

	/**
		Set checklist items from a JSON array string.
	*/
	virtual void SetChecklistItemsJson(const QString& checklistItemsJson) = 0;
};


} // namespace prolifedata
