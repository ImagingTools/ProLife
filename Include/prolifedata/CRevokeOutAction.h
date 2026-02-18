#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace prolifedata
{


class CRevokeOutAction: virtual public iser::ISerializable
{
public:
	CRevokeOutAction();
	CRevokeOutAction(const QByteArray& parentLicenseId, int initialCount, int revokedCount);

	QByteArray GetParentLicenseId() const;
	void SetParentLicenseId(const QByteArray& parentLicenseId);
	int GetInitialCount() const;
	void SetInitialCount(int initialCount);
	int GetRevokedCount() const;
	void SetRevokedCount(int revokedCount);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_parentLicenseId;
	int m_initialCount;
	int m_revokedCount;
};


} // namespace prolifedata


