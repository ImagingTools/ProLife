#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>

// ProLife includes
#include <prolifedata/IIotDeviceInfo.h>
#include <prolifedata/TOrderedWrap.h>


namespace prolifedata
{


class CIotDeviceInfo: virtual public IIotDeviceInfo
{
public:
	CIotDeviceInfo();

	// reimplemented (IIotDeviceInfo)
	virtual QByteArray GetFactoryNumber() const override;
	virtual void SetFactoryNumber(const QByteArray& factoryNumber) override;
	virtual QByteArray GetModemNumber() const override;
	virtual void SetModemNumber(const QByteArray& modemNumber) override;
	virtual QString GetManufacturer() const override;
	virtual void SetManufacturer(const QString& manufacturer) override;
	virtual QString GetBrandModel() const override;
	virtual void SetBrandModel(const QString& brandModel) override;
	virtual QString GetInstallationLocation() const override;
	virtual void SetInstallationLocation(const QString& installationLocation) override;
	virtual QString GetConnectionType() const override;
	virtual void SetConnectionType(const QString& connectionType) override;
	virtual QString GetResourceType() const override;
	virtual void SetResourceType(const QString& resourceType) override;
	virtual QString GetHoleDiameter() const override;
	virtual void SetHoleDiameter(const QString& holeDiameter) override;
	virtual QString GetDeviceCategory() const override;
	virtual void SetDeviceCategory(const QString& deviceCategory) override;
	virtual QString GetCalibrationDate() const override;
	virtual void SetCalibrationDate(const QString& calibrationDate) override;
	virtual QString GetCommissionDate() const override;
	virtual void SetCommissionDate(const QString& commissionDate) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(const QString& description) override;

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
	QByteArray m_factoryNumber;
	QByteArray m_modemNumber;
	QString m_manufacturer;
	QString m_brandModel;
	QString m_installationLocation;
	QString m_connectionType;
	QString m_resourceType;
	QString m_holeDiameter;
	QString m_deviceCategory;
	QString m_calibrationDate;
	QString m_commissionDate;
	QString m_description;
};


typedef imtbase::TIdentifiableWrap<CIotDeviceInfo> CIdentifiableIotDeviceInfo;
typedef prolifedata::TOrderedWrap<CIdentifiableIotDeviceInfo> COrderedIdentifiableIotDeviceInfo;


} // namespace prolifedata


