#pragma once


// Qt includes
#include <QtCore/QList>

// ACF includes
#include <iser/IObject.h>


namespace prolifedata
{


/**
	Catalog card of a hardware component: a part, a module, an assembly or a consumable.
	An assembly lists its sub-components in its bill of materials (BOM).
*/
class IHardwareComponentInfo: virtual public iser::IObject
{
public:
	/**
		Technical parameter of the component, e.g. "Supply voltage = 24 V".
	*/
	struct Parameter
	{
		QByteArray key;
		QString name;
		QString value;
		QString unit;

		bool operator==(const Parameter& other) const
		{
			return key == other.key && name == other.name && value == other.value && unit == other.unit;
		}
	};

	typedef QList<Parameter> Parameters;

	/**
		Line of a bill of materials. Lines sharing an alternative group exclude each other.
	*/
	struct BomEntry
	{
		QByteArray componentId;
		int quantity = 1;
		bool optional = false;
		QString alternativeGroup;

		bool operator==(const BomEntry& other) const
		{
			return componentId == other.componentId && quantity == other.quantity && optional == other.optional && alternativeGroup == other.alternativeGroup;
		}
	};

	typedef QList<BomEntry> Bom;

	virtual QByteArray GetPartNumber() const = 0;
	virtual void SetPartNumber(const QByteArray& partNumber) = 0;
	virtual QString GetName() const = 0;
	virtual void SetName(const QString& name) = 0;
	virtual QString GetDescription() const = 0;
	virtual void SetDescription(const QString& description) = 0;

	/**
		One of "Part", "Module", "Assembly", "Consumable".
	*/
	virtual QByteArray GetComponentType() const = 0;
	virtual void SetComponentType(const QByteArray& componentType) = 0;
	virtual QString GetRevision() const = 0;
	virtual void SetRevision(const QString& revision) = 0;
	virtual QString GetManufacturer() const = 0;
	virtual void SetManufacturer(const QString& manufacturer) = 0;
	virtual QString GetSupplier() const = 0;
	virtual void SetSupplier(const QString& supplier) = 0;

	/**
		Manufacturer part number.
	*/
	virtual QString GetMpn() const = 0;
	virtual void SetMpn(const QString& mpn) = 0;

	/**
		One of "Active", "NRND" (not recommended for new designs), "EOL" (end of life).
	*/
	virtual QByteArray GetLifecycleStatus() const = 0;
	virtual void SetLifecycleStatus(const QByteArray& lifecycleStatus) = 0;

	/**
		Whether every installed unit of the component carries its own serial number.
	*/
	virtual bool IsSerialized() const = 0;
	virtual void SetSerialized(bool serialized) = 0;

	/**
		IDs of the components that must be present together with this one.
	*/
	virtual QByteArrayList GetRequirements() const = 0;
	virtual void SetRequirements(const QByteArrayList& componentIds) = 0;

	/**
		IDs of the components that cannot be present together with this one.
	*/
	virtual QByteArrayList GetIncompatibilities() const = 0;
	virtual void SetIncompatibilities(const QByteArrayList& componentIds) = 0;

	virtual Parameters GetParameters() const = 0;
	virtual void SetParameters(const Parameters& parameters) = 0;
	virtual Bom GetBom() const = 0;
	virtual void SetBom(const Bom& bom) = 0;
};


} // namespace prolifedata


