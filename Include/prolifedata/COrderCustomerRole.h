#pragma once


// ImtCore includes
#include <imtbase/TIdentifiableWrap.h>

// ProLife includes
#include <prolifedata/IOrderCustomerRole.h>


namespace prolifedata
{


class COrderCustomerRole: virtual public IOrderCustomerRole
{
public:
	static QByteArray GetTypeId();

	COrderCustomerRole();

	// reimplemented (IOrderCustomerRole)
	virtual QByteArray GetCustomerId() const override;
	virtual void SetCustomerId(const QByteArray& customerId) override;
	virtual RoleType GetRoleType() const override;
	virtual void SetRoleType(RoleType roleType) override;

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
	QByteArray m_customerId;
	RoleType m_roleType;
};


typedef imtbase::TIdentifiableWrap<COrderCustomerRole> CIdentifiableOrderCustomerRole;


} // namespace prolifedata


