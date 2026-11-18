#pragma once


// ImtCore includes
#include <imtaccount/CCompanyInfo.h>

// ProLife includes
#include <prolifedata/ICustomerInfo.h>


namespace prolifedata
{


class CCustomerInfo: virtual public ICustomerInfo, virtual public imtaccount::CIdentifiableCompanyInfo
{
public:
	typedef imtaccount::CCompanyInfo BaseClass;

	void SetCustomerId(const QByteArray& customerId);

	// reimplemented (ICustomerInfo)
	virtual QByteArray GetCustomerId() const override;
	virtual QString GetWebsite() const override;
	virtual void SetWebsite(const QString& website) override;
	virtual bool IsCustomer() const override;
	virtual void SetCustomer(bool isCustomer) override;
	virtual bool IsManufacturer() const override;
	virtual void SetManufacturer(bool isManufacturer) override;
	virtual bool IsSupplier() const override;
	virtual void SetSupplier(bool isSupplier) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_customerId;
	QString m_website;
	bool m_isCustomer = false;
	bool m_isManufacturer = false;
	bool m_isSupplier = false;
};


} // namespace prolifedata


