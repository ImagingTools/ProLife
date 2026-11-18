#pragma once


// ProLife includes
#include <prolifedata/IHardwareComponentInfo.h>


namespace prolifedata
{


class CHardwareComponentInfo: virtual public IHardwareComponentInfo
{
public:
	// reimplemented (IHardwareComponentInfo)
	virtual QByteArray GetPartNumber() const override;
	virtual void SetPartNumber(const QByteArray& partNumber) override;
	virtual QString GetName() const override;
	virtual void SetName(const QString& name) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual QByteArray GetComponentType() const override;
	virtual void SetComponentType(const QByteArray& componentType) override;
	virtual QString GetRevision() const override;
	virtual void SetRevision(const QString& revision) override;
	virtual QByteArray GetManufacturerId() const override;
	virtual void SetManufacturerId(const QByteArray& manufacturerId) override;
	virtual QString GetMpn() const override;
	virtual void SetMpn(const QString& mpn) override;
	virtual SupplierOffers GetSupplierOffers() const override;
	virtual void SetSupplierOffers(const SupplierOffers& supplierOffers) override;
	virtual QByteArray GetLifecycleStatus() const override;
	virtual void SetLifecycleStatus(const QByteArray& lifecycleStatus) override;
	virtual bool IsSerialized() const override;
	virtual void SetSerialized(bool serialized) override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_partNumber;
	QString m_name;
	QString m_description;
	QByteArray m_componentType = QByteArrayLiteral("Part");
	QString m_revision;
	QByteArray m_manufacturerId;
	QString m_mpn;
	SupplierOffers m_supplierOffers;
	QByteArray m_lifecycleStatus = QByteArrayLiteral("Active");
	bool m_serialized = false;
};


} // namespace prolifedata


