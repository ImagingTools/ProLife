#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace prolifedata
{


class CSplitOutAction: virtual public iser::ISerializable
{
public:
	CSplitOutAction();
	CSplitOutAction(const QByteArray& newLicenseId, int initialCount, int movedCount);

	QByteArray GetNewLicenseId() const;
	void SetNewLicenseId(const QByteArray& newLicenseId);
	int GetInitialCount() const;
	void SetInitialCount(int initialCount);
	int GetMovedCount() const;
	void SetMovedCount(int movedCount);

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_newLicenseId;
	int m_initialCount;
	int m_movedCount;
};


} // namespace prolifedata


