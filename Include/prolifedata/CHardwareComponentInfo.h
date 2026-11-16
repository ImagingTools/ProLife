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
	virtual QString GetManufacturer() const override;
	virtual void SetManufacturer(const QString& manufacturer) override;
	virtual QString GetSupplier() const override;
	virtual void SetSupplier(const QString& supplier) override;
	virtual QString GetMpn() const override;
	virtual void SetMpn(const QString& mpn) override;
	virtual QByteArray GetLifecycleStatus() const override;
	virtual void SetLifecycleStatus(const QByteArray& lifecycleStatus) override;
	virtual bool IsSerialized() const override;
	virtual void SetSerialized(bool serialized) override;
	virtual QByteArrayList GetRequirements() const override;
	virtual void SetRequirements(const QByteArrayList& componentIds) override;
	virtual QByteArrayList GetIncompatibilities() const override;
	virtual void SetIncompatibilities(const QByteArrayList& componentIds) override;
	virtual Parameters GetParameters() const override;
	virtual void SetParameters(const Parameters& parameters) override;
	virtual Bom GetBom() const override;
	virtual void SetBom(const Bom& bom) override;

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
	bool SerializeParameters(iser::IArchive& archive);
	bool SerializeBom(iser::IArchive& archive);

private:
	QByteArray m_partNumber;
	QString m_name;
	QString m_description;
	QByteArray m_componentType = QByteArrayLiteral("Part");
	QString m_revision;
	QString m_manufacturer;
	QString m_supplier;
	QString m_mpn;
	QByteArray m_lifecycleStatus = QByteArrayLiteral("Active");
	bool m_serialized = false;
	QByteArrayList m_requirements;
	QByteArrayList m_incompatibilities;
	Parameters m_parameters;
	Bom m_bom;
};


} // namespace prolifedata


