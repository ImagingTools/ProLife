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
		A supplier the component is bought from and the article number of the component there.
	*/
	struct SupplierOffer
	{
		QByteArray supplierId;
		QString sku;

		bool operator==(const SupplierOffer& other) const
		{
			return supplierId == other.supplierId && sku == other.sku;
		}

		bool operator!=(const SupplierOffer& other) const
		{
			return !(*this == other);
		}
	};
	typedef QList<SupplierOffer> SupplierOffers;

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

	/**
		ID of the manufacturer in the collection of manufacturers.
	*/
	virtual QByteArray GetManufacturerId() const = 0;
	virtual void SetManufacturerId(const QByteArray& manufacturerId) = 0;

	/**
		Manufacturer part number.
	*/
	virtual QString GetMpn() const = 0;
	virtual void SetMpn(const QString& mpn) = 0;

	/**
		Suppliers the component is bought from, by ID in the collection of suppliers.
	*/
	virtual SupplierOffers GetSupplierOffers() const = 0;
	virtual void SetSupplierOffers(const SupplierOffers& supplierOffers) = 0;

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
};


} // namespace prolifedata


