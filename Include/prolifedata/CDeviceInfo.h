#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>
#include <imtlic/CProductInstanceCollection.h>

// ProLife includes
#include <prolifedata/IDeviceInfo.h>
#include <prolifedata/TOrderedWrap.h>


namespace prolifedata
{


class CDeviceInfo: virtual public IDeviceInfo
{
public:
	CDeviceInfo();

	// reimplemented (IDeviceInfo)
	virtual QByteArray GetSerialNumber() const override;
	virtual void SetSerialNumber(const QByteArray& serialNumber) override;
	virtual QByteArray GetMacAddress() const override;
	virtual void SetMacAddress(const QByteArray& macAddress) override;
	virtual QByteArray GetDeviceType() const override;
	virtual void SetDeviceType(const QByteArray& deviceType) override;
	virtual QByteArray GetConfigurationType() const override;
	virtual void SetConfigurationType(const QByteArray& configurationType) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;
	virtual DeviceProductionStatus GetDeviceProductionStatus() const override;
	virtual void SetDeviceProductionStatus(DeviceProductionStatus status) override;
	virtual QByteArray GetProject() const override;
	virtual void SetProject(const QByteArray& project) override;
	virtual bool IsInternalUse() const override;
	virtual void SetInternalUse(bool internalUse) override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_serialNumber;
	QByteArray m_macAddress;
	QByteArray m_deviceType;
	QByteArray m_configurationType;
	QString m_description;
	QByteArray m_project;
	DeviceProductionStatus m_status;
	bool m_internalUse;
};


typedef imtbase::TIdentifiableWrap<CDeviceInfo> CIdentifiableDeviceInfo;
typedef prolifedata::TOrderedWrap<CIdentifiableDeviceInfo> COrderedIdentifiableDeviceInfo;


} // namespace prolifedata


