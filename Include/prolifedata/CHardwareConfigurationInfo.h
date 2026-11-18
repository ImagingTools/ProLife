#pragma once


// ProLife includes
#include <prolifedata/IHardwareConfigurationInfo.h>


namespace prolifedata
{


class CHardwareConfigurationInfo: virtual public IHardwareConfigurationInfo
{
public:
	// reimplemented (IHardwareConfigurationInfo)
	virtual QByteArray GetConfigurationId() const override;
	virtual void SetConfigurationId(const QByteArray& configurationId) override;
	virtual QString GetName() const override;
	virtual void SetName(const QString& name) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual QByteArray GetProductId() const override;
	virtual void SetProductId(const QByteArray& productId) override;
	virtual QByteArrayList GetComponentIds() const override;
	virtual void SetComponentIds(const QByteArrayList& componentIds) override;

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

private:
	QByteArray m_configurationId;
	QString m_name;
	QString m_description;
	QByteArray m_productId;
	QByteArrayList m_componentIds;
};


} // namespace prolifedata


