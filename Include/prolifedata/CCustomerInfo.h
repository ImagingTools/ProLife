#pragma once


// ImtCore includes
#include <imtauth/CCompanyInfo.h>

// ProLife includes
#include <prolifedata/ICustomerInfo.h>


namespace prolifedata
{


class CCustomerInfo: virtual public ICustomerInfo, virtual public imtauth::CIdentifiableCompanyInfo
{
public:
	typedef imtauth::CCompanyInfo BaseClass;

	void SetCustomerId(const QByteArray& customerId);

	// reimplemented (ICustomerInfo)
	virtual QByteArray GetCustomerId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_customerId;
};


} // namespace prolifedata


